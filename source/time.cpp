#include "header.h"
#include "main.h"
#include "vorbis.h"
#include "tja.h"
#include "time.h"
#include "option.h"
#include <time.h>

enum msec_status {
	MSEC_INIT,
	MSEC_CURRENT,
	MSEC_DIFF,
	MSEC_LAST_DIFF,
};
#define TIME_NUM 4

struct timespec tv;
int cnt[TIME_NUM], msec[TIME_NUM][4], sec[TIME_NUM];
int isStop[TIME_NUM];
double PreTime[TIME_NUM],Time[TIME_NUM],CurrentTime[TIME_NUM],IniVorbisTime[TIME_NUM],OffTime[TIME_NUM];

double get_current_time(int id) {

	clock_gettime(CLOCK_MONOTONIC, &tv);
	if (isStop[id] != 1) {

		if (cnt[id] != 0 &&
			(tv.tv_sec + tv.tv_nsec * 0.000000001 - OffTime[id]) < 1.0)
			Time[id] += tv.tv_sec + tv.tv_nsec * 0.000000001 - OffTime[id];
		else if (cnt[id] != 0 &&
			(tv.tv_sec + tv.tv_nsec * 0.000000001 - OffTime[id]) >= 1.0)
			Time[id] += 1.0/12.0;
		++cnt[id];
	}
	OffTime[id] = tv.tv_sec + tv.tv_nsec * 0.000000001;
	//snprintf(get_buffer(), BUFFER_SIZE, "t:%.1f", Time[id]);
	//draw_debug(0, id*10, get_buffer());
	return Time[id] * mspeed();
}

void restart_time(int id) {
	isStop[id] = 0;
}

void stop_time(int id) {

	isStop[id] = 1;
	//Time[id] += 0.0178;
	cnt[id] = 0;
}

void toggle_time(int id) {

	if (Time[id] != 0) {
		if (isStop[id] == 1) restart_time(id);
		else stop_time(id);
	}
}

int get_time_isStop(int id) {
	return isStop[id];
}

#define FPS_SAMPLE 10
double fps_time[2],fps_cnt,fps_sum,fps;	//要初期化
void draw_fps() {
	
	fps_time[0] = fps_time[1];
	fps_time[1] = get_current_time(TIME_FPS) / mspeed();

	fps_sum += fps_time[1] - fps_time[0];
	++fps_cnt;

	if (fps_cnt == FPS_SAMPLE) {
		fps_cnt = 0;
		fps = FPS_SAMPLE / fps_sum;
		fps_sum = 0;
	}
	snprintf(get_buffer(), BUFFER_SIZE, "%.2ffps", fps);
	draw_debug(0, 0, get_buffer());
}

void time_ini() {

	for (int i = 0; i < TIME_NUM; ++i) {

		for (int n = 0; n < 4; ++n) {
			msec[i][n] = 0;

		}

		sec[i] = 0;
		cnt[i] = 0;
		isStop[i] = 0;
		PreTime[i] = 0;
		Time[i] = 0;
		CurrentTime[i] = 0;
		IniVorbisTime[i] = 0;
	}
	fps_time[0] = 0;
	fps_time[1] = 0;
	fps_cnt = 0;
	fps_sum = 0;
	fps = 0;
}
