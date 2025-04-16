#pragma once
#include "select.h"

typedef struct {

	int knd, data[3],test;
	double val[3];
	char* command_s,*notes,*value_s;
} COMMAND_T;

typedef struct {

	char *title, *subtitle,*wave;
	int level,balloon[4][256],songvol,sevol,scoreinit,scorediff,
		course,style,life,scoremode,subtitle_state; //total,side;
	double bpm, offset; //demostart;

}TJA_HEADER_T;

typedef struct {

	int knd;
	char* val;
}HEADER_T;

typedef struct {
	double judge_time, create_time, pop_time, sudn_time,
		bpm, speed,measure,scroll;
	int notes,firstmeasure,start_measure_count,max_notes,notes_count,command,branch,
		original_id;	//ソート前のid
	bool flag,isDispBarLine;

} MEASURE_T;

void load_tja_head(int course, LIST_T Song), load_tja_notes(int course, LIST_T Song), init_tja();
void tja_to_notes(bool isDnon, bool isKatsu, int count, C2D_Sprite sprites[SPRITES_NUMER]);
void load_tja_head_simple(LIST_T *List), get_tja_header(TJA_HEADER_T *TJA_Header);
void get_command_value(char* buf, COMMAND_T *Command);
double get_FirstMeasureTime();
int get_MeasureId_From_OriginalId(int id);
bool get_isBranch();
