#pragma once
#include "result.h"

typedef struct {

	int level[7],x,y,tmp,genre;
	char title[256],path[256],tja[256],wave[256];
	bool course[7],course_exist[5];
} LIST_T;

typedef struct {

	int genre_color,font_color;
	char name[256], path[256];
	bool isOpened;
} GENRE_T;

void disp_file_list(), load_file_main(), get_SelectedId(LIST_T *TMP, int *arg), get_result(RESULT_T *Result);
static void update_cursor(int knd), select_ini();
static void draw_select_text(float x, float y, const char* text, int color = 0xffffff);
static void draw_option_text(float x, float y, const char *text, bool state, float *width, float *height,float sizex=0.7,float sizey = 0.7);
static void draw_result_text(float x, float y, float size, const char *text);
static void calc_result_text(const char *text, float *width, float *height);
static bool get_isGameStart();
