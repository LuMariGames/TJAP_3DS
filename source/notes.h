#pragma once
#include "tja.h"

typedef struct {
	bool flag,isThrough,isDummy = false;
	float create_time,judge_time,pop_time,x,bpm,scroll,x_ini;
	int num,notes_max,knd,roll_id;

} NOTES_T;

typedef struct {
	bool flag,isDisp;
	float create_time,scroll,x_ini,x;
	int measure;

} BARLINE_T;

typedef struct {
	bool flag;
	int id,start_id,end_id,knd;
	float start_x,end_x;

}ROLL_T;

typedef struct {
	bool flag;
	int id,start_id,end_id,need_hit,current_hit;

}BALLOON_T;

typedef struct {
	int knd,course;
	float x,y;
	bool next,wait;

}BRANCH_T;


void notes_main(
	int isDon, int isKatsu,
	char (&tja_notes)[MEASURE_MAX][NOTES_MEASURE_MAX],
	MEASURE_T Measure[MEASURE_MAX],
	int cnt, C2D_Sprite sprites[SPRITES_NUMER]
),
init_notes(TJA_HEADER_T TJA_Header),draw_title(),draw_condition(),
newfont(),fontfree(),plus_measure(),min_measure(),set_measure();
int ctoi(char c), get_branch_course();
bool get_notes_finish(), get_isPttBorder();

