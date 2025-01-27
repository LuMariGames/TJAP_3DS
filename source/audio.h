#pragma once
void load_sound();
void sd_load_sound();
//void load_combo();
int play_sound(int id) {

	if (sound[id].audiochannel == -1) {
		printf("No available audio channel\n");
		return -1;
	}
	ndspChnWaveBufClear(sound[id].audiochannel);
	/*ndspChnReset(sound[id].audiochannel);
	ndspChnInitParams(sound[id].audiochannel);
	ndspChnSetMix(sound[id].audiochannel, sound[id].mix);
	ndspChnSetInterp(sound[id].audiochannel, sound[id].interp);
	ndspChnSetRate(sound[id].audiochannel, sound[id].rate);
	ndspChnSetFormat(sound[id].audiochannel, NDSP_CHANNELS(sound[id].channels) | NDSP_ENCODING(sound[id].encoding));*/
	ndspChnWaveBufAdd(sound[id].audiochannel, &waveBuf[id]);

	return 0;
}
void exit_music();
int ogg_check();
