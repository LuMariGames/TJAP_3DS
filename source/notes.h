#pragma once
#include "tja.h"

typedef struct {
	int num,notes_max,knd,roll_id;
	float create_time,judge_time,pop_time,bpm,scroll,x_ini;
	bool flag,isThrough,isDummy = false;
	float x;

} NOTES_T;

typedef struct {
	int measure;
	float create_time, scroll, x_ini;
	bool flag, isDisp;
	float x;

} BARLINE_T;

typedef struct {
	int id, start_id, end_id, knd;
	float start_x, end_x;
	bool flag;

}ROLL_T;

typedef struct {
	int id, start_id, end_id,
		need_hit, current_hit;
	bool flag;

}BALLOON_T;

typedef struct {
	int knd, course;
	float x, y;
	bool next, wait;

}BRANCH_T;

struct notejudge_t {
	int* donc;
	int* katsuc;
	int* count;
	int* bth;
};

void notes_main(
	int isDon, int isKatsu,
	char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX],
	MEASURE_T Measure[MEASURE_MAX],
	int cnt, C2D_Sprite sprites[SPRITES_NUMER]
),
init_notes(TJA_HEADER_T TJA_Header),draw_title(),draw_condition(),
newfont(),fontfree(),plus_measure(),min_measure(),set_measure();
int ctoi(char c), get_branch_course();
bool get_notes_finish(), get_isPttBorder();
