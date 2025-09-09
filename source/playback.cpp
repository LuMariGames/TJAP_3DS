#include <cstdio>

#include "header.h"
#include "vorbis.h"
#include "mp3.h"
#include "main.h"
#include "option.h"
#include "select.h"

#define delete(ptr) \
	free((void*) ptr); ptr = NULL

static int Decode_CoreID = 1;
static volatile bool stop = true;
extern float mix[12];

bool togglePlayback(void){

	bool paused = ndspChnIsPaused(CHANNEL);
	ndspChnSetPaused(CHANNEL, !paused);
	return !paused;
}

void stopPlayback(void){

	stop = true;
}

bool isPlaying(void){

	return !stop;
}

int getFileType(const char *file){

	FILE* ftest = fopen(file, "rb");
	uint32_t fileSig;
	enum file_types file_type = FILE_TYPE_ERROR;

	/* Failure opening file */
	if(ftest == NULL)
		return -1;

	if(fread(&fileSig, 4, 1, ftest) == 0)
		goto err;

	switch(fileSig){

		// "RIFF"
		case 0x46464952:
			if(fseek(ftest, 4, SEEK_CUR) != 0)
				break;

			// "WAVE"
			// Check required as AVI file format also uses "RIFF".
			if(fread(&fileSig, 4, 1, ftest) == 0)
				break;

			if(fileSig != 0x45564157)
				break;

			file_type = FILE_TYPE_WAV;
			break;

		// "fLaC"
		case 0x43614c66:
			file_type = FILE_TYPE_FLAC;
			break;

		// "OggS"
		case 0x5367674F:
			if(isVorbis(file) == 0)
				file_type = FILE_TYPE_VORBIS;

			break;

		default:

			if((fileSig << 16) == 0xFBFF0000 || (fileSig << 16) == 0xFAFF0000 || (fileSig << 8) == 0x33444900) {
				file_type = FILE_TYPE_MP3;
				break;
			}
			break;
	}

err:
	fclose(ftest);
	return file_type;
}

int testtest = 0;

void playFile(void* infoIn){

	struct decoder_fn decoder;
	struct playbackInfo_t* info = (playbackInfo_t*)infoIn;
	int16_t*	buffer[2] = {NULL};
	ndspWaveBuf	waveBuf[2];
	bool		lastbuf = false, isNdspInit = false, isMp3 = false;
	int		ret = -1;
	const char*	file = info->file;

	/* Reset previous stop command */
	stop = false;

	switch(getFileType(file))
	{
		case FILE_TYPE_VORBIS:
			setVorbis(&decoder);
			break;
		case FILE_TYPE_MP3:
			setMp3(&decoder);
			isMp3 = true;
			break;
		default:
			goto err;
	}

	if(ndspInit() < 0)
	{
		goto err;
	}

	isNdspInit = true;

	if((ret = (*decoder.init)(file)) != 0)
	{
		goto err;
	}

	if((*decoder.channels)() > 2 || (*decoder.channels)() < 1)
	{
		goto err;
	}
	testtest = 99;
	buffer[0] = (int16_t*)linearAlloc(decoder.buffSize * sizeof(int16_t));
	buffer[1] = (int16_t*)linearAlloc(decoder.buffSize * sizeof(int16_t));

	ndspChnReset(CHANNEL);
	ndspChnWaveBufClear(CHANNEL);
	ndspChnSetInterp(CHANNEL, NDSP_INTERP_LINEAR);
	ndspChnSetRate(CHANNEL, (*decoder.rate)() * mspeed());
	ndspChnSetFormat(CHANNEL, (*decoder.channels)() == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);
	ndspChnSetMix(CHANNEL, mix);

	memset(waveBuf, 0, sizeof(waveBuf));

	if 		(!isMp3 && get_ismeasure()) setVorbisTime(starttime());
	else if (isMp3 && get_ismeasure()) seekMp3(starttime());
	
	waveBuf[0].nsamples = (*decoder.decode)(&buffer[0][0]) / (*decoder.channels)();
	waveBuf[0].data_vaddr = &buffer[0][0];
	while (*info->isPlay == false) svcSleepThread(100000);

	ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);
	waveBuf[1].nsamples = (*decoder.decode)(&buffer[1][0]) / (*decoder.channels)();
	waveBuf[1].data_vaddr = &buffer[1][0];
	ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);

	while(ndspChnIsPlaying(CHANNEL) == false);

	while(stop == false){
		if (aptShouldClose()) {
			ndspChnSetPaused(CHANNEL, true);
			break;
		}
		svcSleepThread(100000);

		if(lastbuf == true && waveBuf[0].status == NDSP_WBUF_DONE &&
			waveBuf[1].status == NDSP_WBUF_DONE) break;
		if(ndspChnIsPaused(CHANNEL) == true || lastbuf == true)
			continue;

		//音声処理
		if(waveBuf[0].status == NDSP_WBUF_DONE) {
			size_t read = (*decoder.decode)(&buffer[0][0]);
			if(read <= 0) {
				lastbuf = true;
				continue;
			}
			else if(read < decoder.buffSize) waveBuf[0].nsamples = read / (*decoder.channels)();
			ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);
		}
		if(waveBuf[1].status == NDSP_WBUF_DONE) {
			size_t read = (*decoder.decode)(&buffer[1][0]);
			if(read <= 0) {
				lastbuf = true;
				continue;
			}
			else if(read < decoder.buffSize) waveBuf[1].nsamples = read / (*decoder.channels)();
			ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);
		}
	}

	(*decoder.exit)();
out:
	if(isNdspInit == true)
	{
		ndspChnWaveBufClear(CHANNEL);
		ndspExit();
	}

	delete(info->file);
	linearFree(buffer[0]);
	linearFree(buffer[1]);

	APT_SetAppCpuTimeLimit(5);
	return;

err:
	goto out;
}

struct playbackInfo_t playbackInfo;

inline int changeFile(const char* ep_file, struct playbackInfo_t* playbackInfo, bool *p_isPlayMain){

	s32 prio;
	static Thread thread = NULL;

	if (ep_file != NULL && getFileType(ep_file) == FILE_TYPE_ERROR) return -1;

	if (getFileType(ep_file) == FILE_TYPE_VORBIS) {
		Decode_CoreID = 1;
		APT_SetAppCpuTimeLimit(50);
	}
	else if (getFileType(ep_file) == FILE_TYPE_MP3) {
		Decode_CoreID = 0;
		APT_SetAppCpuTimeLimit(5);
	}
	if (thread != NULL) {
		stopPlayback();

		threadJoin(thread, U64_MAX);
		threadFree(thread);
		thread = NULL;
	}

	if (ep_file == NULL || playbackInfo == NULL)
		return 0;

	playbackInfo->file = strdup(ep_file);
	playbackInfo->isPlay = p_isPlayMain;

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(playFile, playbackInfo, DECODE_MEM, prio - 1, Decode_CoreID, false);
	return 0;
}

void play_main_music(bool *p_isPlayMain,LIST_T Song) {

	char abs_path[512];

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.wave);
	changeFile(abs_path, &playbackInfo, p_isPlayMain);
}

void pasue_main_music() {

	if (isPlaying() == true) {
		togglePlayback();
	}
}

void stop_main_music() {

	stopPlayback();
	changeFile(NULL, &playbackInfo ,NULL);
}

void init_main_music() {

	playbackInfo.file = NULL;
}

int check_wave(LIST_T Song) { //音楽ファイルの確認

	char abs_path[512];

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.wave);
	int result = getFileType(abs_path);

	if (result == -1) return WARNING_WAVE_NO_EXIST;
	else if (result != FILE_TYPE_VORBIS) {
		if (result != FILE_TYPE_MP3) return WARNING_WAVE_NOT_OGG;
	}
	return -1;
}
