﻿#pragma once
#include "tja.h"

typedef struct {
	int num, notes_max, knd, roll_id;
	double x_ini, create_time, judge_time, pop_time, hb_time, bpm, scroll;
	bool flag, isThrough;
	float x;
	C2D_Sprite spr;

} NOTES_T;

typedef struct {
	int measure;
	double x_ini, create_time, scroll;
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

void notes_main(
	bool isDon,
	bool isKatsu,
	char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX],
	MEASURE_T Measure[MEASURE_MAX],
	int cnt,
	C2D_Sprite  sprites[SPRITES_NUMER]);
int ctoi(char c);
int get_branch_course();
void init_notes(TJA_HEADER_T TJA_Header);
bool get_notes_finish();
void draw_title();
void draw_condition();
double sign(double A);