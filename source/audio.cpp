#include <tremor/ivorbisfile.h>
#include <ogg/ogg.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "option.h"

#define AUDIO_BUFFER_SIZE 4000
#define STACKSIZE (4 * 1000)
#define SOUND_NUMBER 4

typedef struct {
	float rate;
	u32 channels;
	u32 encoding;
	u32 nsamples;
	u32 size;
	char* data;
	bool loop;
	int audiochannel;
	float mix[12];
	ndspInterpType interp;
	OggVorbis_File ovf;
} Sound;
Sound sound[71];
ndspWaveBuf waveBuf[71];

void load_sound() {

	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1);
	char sound_address[SOUND_NUMBER][30] = {
		"romfs:/don.ogg",
		"romfs:/ka.ogg",
		"romfs:/balloonbreak.ogg",
		"romfs:/failed.ogg",
	};

	for (int i = 0; i < SOUND_NUMBER; ++i) {
		memset(&sound[i], 0, sizeof(sound[i]));
		sound[i].mix[0] = 1.0f;
		sound[i].mix[1] = 1.0f;
		FILE * file = fopen(sound_address[i], "rb");
		if (file == 0) {
			printf("no file\n");
			while (1);
		}
		if (ov_open(file, &sound[i].ovf, NULL, 0) < 0) {
			printf("ogg vorbis file error\n");
			while (1);
		}
		vorbis_info * vorbisInfo = ov_info(&sound[i].ovf, -1);
		if (vorbisInfo == NULL) {
			printf("could not retrieve ogg audio stream information\n");
			while (1);
		}
		sound[i].rate = (float)vorbisInfo->rate;
		sound[i].channels = (u32)vorbisInfo->channels;
		sound[i].encoding = NDSP_ENCODING_PCM16;
		sound[i].nsamples = (u32)ov_pcm_total(&sound[i].ovf, -1);
		sound[i].size = sound[i].nsamples * sound[i].channels * 2;
		sound[i].audiochannel = i;
		sound[i].interp = NDSP_INTERP_NONE;
		sound[i].loop = false;
		if (linearSpaceFree() < sound[i].size) {
			printf("not enough linear memory available %ld\n", sound[i].size);
		}
		sound[i].data = (char*)linearAlloc(sound[i].size);
		if (sound[i].data == 0) {
			printf("null\n");
			while (1);
		}
		int offset = 0;
		int eof = 0;
		int currentSection;
		while (!eof) {
			long ret = ov_read(&sound[i].ovf, &sound[i].data[offset], AUDIO_BUFFER_SIZE, &currentSection);
			if (ret == 0) {
				eof = 1;
			}
			else if (ret < 0) {
				ov_clear(&sound[i].ovf);
				linearFree(sound[i].data);
				printf("error in the ogg vorbis stream\n");
				while (1);
			}
			else {
				offset += ret;
			}
			//printf("%ld %d\n", ret, currentSection);
		}
		memset(&waveBuf[i], 0, sizeof(ndspWaveBuf));
		waveBuf[i].data_vaddr = sound[i].data;
		waveBuf[i].nsamples = sound[i].nsamples;
		waveBuf[i].looping = sound[i].loop;
		waveBuf[i].status = NDSP_WBUF_FREE;
		DSP_FlushDataCache(sound[i].data, sound[i].size);
		//linearFree(&sound[i].ovf);
		ov_clear(&sound[i].ovf);
		fclose(file);
	}
}

void sd_load_sound() {

	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(1);
	char sound_address[SOUND_NUMBER][40] = {
		"sdmc:/tjafiles/theme/don.ogg",
		"sdmc:/tjafiles/theme/ka.ogg",
		"sdmc:/tjafiles/theme/balloonbreak.ogg",
		"sdmc:/tjafiles/theme/failed.ogg",
	};

	for (int i = 0; i < SOUND_NUMBER; ++i) {
		memset(&sound[i], 0, sizeof(sound[i]));
		sound[i].mix[0] = 1.0f;
		sound[i].mix[1] = 1.0f;
		FILE * file = fopen(sound_address[i], "rb");
		if (file == 0) {
			printf("no file\n");
			while (1);
		}
		if (ov_open(file, &sound[i].ovf, NULL, 0) < 0) {
			printf("ogg vorbis file error\n");
			while (1);
		}
		vorbis_info * vorbisInfo = ov_info(&sound[i].ovf, -1);
		if (vorbisInfo == NULL) {
			printf("could not retrieve ogg audio stream information\n");
			while (1);
		}
		sound[i].rate = (float)vorbisInfo->rate;
		sound[i].channels = (u32)vorbisInfo->channels;
		sound[i].encoding = NDSP_ENCODING_PCM16;
		sound[i].nsamples = (u32)ov_pcm_total(&sound[i].ovf, -1);
		sound[i].size = sound[i].nsamples * sound[i].channels * 2;
		sound[i].audiochannel = i;
		sound[i].interp = NDSP_INTERP_NONE;
		sound[i].loop = false;
		if (linearSpaceFree() < sound[i].size) {
			printf("not enough linear memory available %ld\n", sound[i].size);
		}
		sound[i].data = (char*)linearAlloc(sound[i].size);
		if (sound[i].data == 0) {
			printf("null\n");
			while (1);
		}
		int offset = 0;
		int eof = 0;
		int currentSection;
		while (!eof) {
			long ret = ov_read(&sound[i].ovf, &sound[i].data[offset], AUDIO_BUFFER_SIZE, &currentSection);
			if (ret == 0) {
				eof = 1;
			}
			else if (ret < 0) {
				ov_clear(&sound[i].ovf);
				linearFree(sound[i].data);
				printf("error in the ogg vorbis stream\n");
				while (1);
			}
			else {
				offset += ret;
			}
			//printf("%ld %d\n", ret, currentSection);
		}
		memset(&waveBuf[i], 0, sizeof(ndspWaveBuf));
		waveBuf[i].data_vaddr = sound[i].data;
		waveBuf[i].nsamples = sound[i].nsamples;
		waveBuf[i].looping = sound[i].loop;
		waveBuf[i].status = NDSP_WBUF_FREE;
		DSP_FlushDataCache(sound[i].data, sound[i].size);
		//linearFree(&sound[i].ovf);
		ov_clear(&sound[i].ovf);
		fclose(file);
	}
}

void load_combo() {

	char sound_address[67][30] = {
		"romfs:/combo/50combo.ogg",
		"romfs:/combo/100combo.ogg",
		"romfs:/combo/200combo.ogg",
		"romfs:/combo/300combo.ogg",
		"romfs:/combo/400combo.ogg",
		"romfs:/combo/500combo.ogg",
		"romfs:/combo/600combo.ogg",
		"romfs:/combo/700combo.ogg",
		"romfs:/combo/800combo.ogg",
		"romfs:/combo/900combo.ogg",
		"romfs:/combo/1000combo.ogg",
		"romfs:/combo/1100combo.ogg",
		"romfs:/combo/1200combo.ogg",
		"romfs:/combo/1300combo.ogg",
		"romfs:/combo/1400combo.ogg",
		"romfs:/combo/1500combo.ogg",
		"romfs:/combo/1600combo.ogg",
		"romfs:/combo/1700combo.ogg",
		"romfs:/combo/1800combo.ogg",
		"romfs:/combo/1900combo.ogg",
		"romfs:/combo/2000combo.ogg",
		"romfs:/combo/2100combo.ogg",
		"romfs:/combo/2200combo.ogg",
		"romfs:/combo/2300combo.ogg",
		"romfs:/combo/2400combo.ogg",
		"romfs:/combo/2500combo.ogg",
		"romfs:/combo/2600combo.ogg",
		"romfs:/combo/2700combo.ogg",
		"romfs:/combo/2800combo.ogg",
		"romfs:/combo/2900combo.ogg",
		"romfs:/combo/3000combo.ogg",
		"romfs:/combo/3100combo.ogg",
		"romfs:/combo/3200combo.ogg",
		"romfs:/combo/3300combo.ogg",
		"romfs:/combo/3400combo.ogg",
		"romfs:/combo/3500combo.ogg",
		"romfs:/combo/3600combo.ogg",
		"romfs:/combo/3700combo.ogg",
		"romfs:/combo/3800combo.ogg",
		"romfs:/combo/3900combo.ogg",
		"romfs:/combo/4000combo.ogg",
		"romfs:/combo/4100combo.ogg",
		"romfs:/combo/4200combo.ogg",
		"romfs:/combo/4300combo.ogg",
		"romfs:/combo/4400combo.ogg",
		"romfs:/combo/4500combo.ogg",
		"romfs:/combo/4600combo.ogg",
		"romfs:/combo/4700combo.ogg",
		"romfs:/combo/4800combo.ogg",
		"romfs:/combo/4900combo.ogg",
		"romfs:/combo/5000combo.ogg",
		"romfs:/combo/50meka.ogg",
		"romfs:/combo/100meka.ogg",
		"romfs:/combo/200meka.ogg",
		"romfs:/combo/300meka.ogg",
		"romfs:/combo/400meka.ogg",
		"romfs:/combo/500meka.ogg",
		"romfs:/combo/600meka.ogg",
		"romfs:/combo/700meka.ogg",
		"romfs:/combo/800meka.ogg",
		"romfs:/combo/900meka.ogg",
		"romfs:/combo/1000meka.ogg",
		"romfs:/combo/1100meka.ogg",
		"romfs:/combo/1200meka.ogg",
		"romfs:/combo/1300meka.ogg",
		"romfs:/combo/1400meka.ogg",
		"romfs:/combo/mekaover.ogg",
	};

	for (int i = 4; i < 71; ++i) {
		memset(&sound[i], 0, sizeof(sound[i]));
		sound[i].mix[0] = 1.0f;
		sound[i].mix[1] = 1.0f;
		FILE * file = fopen(sound_address[i-4], "rb");
		if (file == 0) {
			printf("no file\n");
			while (1);
		}
		if (ov_open(file, &sound[i].ovf, NULL, 0) < 0) {
			printf("ogg vorbis file error\n");
			while (1);
		}
		vorbis_info * vorbisInfo = ov_info(&sound[i].ovf, -1);
		if (vorbisInfo == NULL) {
			printf("could not retrieve ogg audio stream information\n");
			while (1);
		}
		sound[i].rate = (float)vorbisInfo->rate;
		sound[i].channels = (u32)vorbisInfo->channels;
		sound[i].encoding = NDSP_ENCODING_PCM16;
		sound[i].nsamples = (u32)ov_pcm_total(&sound[i].ovf, -1);
		sound[i].size = sound[i].nsamples * sound[i].channels * 2;
		sound[i].audiochannel = SOUND_NUMBER;
		sound[i].interp = NDSP_INTERP_NONE;
		sound[i].loop = false;
		if (linearSpaceFree() < sound[i].size) {
			printf("not enough linear memory available %ld\n", sound[i].size);
		}
		sound[i].data = (char*)linearAlloc(sound[i].size);
		if (sound[i].data == 0) {
			printf("null\n");
			while (1);
		}
		int offset = 0;
		int eof = 0;
		int currentSection;
		while (!eof) {
			long ret = ov_read(&sound[i].ovf, &sound[i].data[offset], AUDIO_BUFFER_SIZE, &currentSection);
			if (ret == 0) {
				eof = 1;
			}
			else if (ret < 0) {
				ov_clear(&sound[i].ovf);
				linearFree(sound[i].data);
				printf("error in the ogg vorbis stream\n");
				while (1);
			}
			else {
				offset += ret;
			}
			//printf("%ld %d\n", ret, currentSection);
		}
		memset(&waveBuf[i], 0, sizeof(ndspWaveBuf));
		waveBuf[i].data_vaddr = sound[i].data;
		waveBuf[i].nsamples = sound[i].nsamples;
		waveBuf[i].looping = sound[i].loop;
		waveBuf[i].status = NDSP_WBUF_FREE;
		DSP_FlushDataCache(sound[i].data, sound[i].size);
		//linearFree(&sound[j].ovf);
		ov_clear(&sound[i].ovf);
		fclose(file);
	}
}

int play_sound(int id) {

	if (sound[id].audiochannel == -1) {
		printf("No available audio channel\n");
		return -1;
	}
	ndspChnWaveBufClear(sound[id].audiochannel);
	ndspChnReset(sound[id].audiochannel);
	ndspChnInitParams(sound[id].audiochannel);
	ndspChnSetMix(sound[id].audiochannel, sound[id].mix);
	ndspChnSetInterp(sound[id].audiochannel, sound[id].interp);
	ndspChnSetRate(sound[id].audiochannel, sound[id].rate);
	ndspChnSetFormat(sound[id].audiochannel, NDSP_CHANNELS(sound[id].channels) | NDSP_ENCODING(sound[id].encoding));
	ndspChnWaveBufAdd(sound[id].audiochannel, &waveBuf[id]);

	return 0;
}

void exit_music() {

	ndspChnWaveBufClear(sound[0].audiochannel);
	ndspChnWaveBufClear(sound[1].audiochannel);
	ndspChnWaveBufClear(sound[2].audiochannel);
	ndspChnWaveBufClear(sound[3].audiochannel);
	ndspChnWaveBufClear(sound[4].audiochannel);
	for (int i = 0; i < 71; ++i) {
		linearFree(sound[i].data);
	}
	ndspExit();
}
