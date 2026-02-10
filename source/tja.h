#pragma once
#include "select.h"
#include <string>

typedef struct {

	int knd,data[3],test;
	double val[3];
	std::string command_s,notes,value_s;
} COMMAND_T;

typedef struct {

	char *title,*subtitle,*wave,*bg;
	int level,balloon[4][256],songvol,sevol,scoreinit,scorediff,
		course,style,life,scoremode,subtitle_state; //total,side;
	float bgfps;
	double bpm,offset; //demostart;

}TJA_HEADER_T;

typedef struct {

	int knd;
	char* val;
}HEADER_T;

typedef struct {
	double judge_time,create_time,pop_time, //sudn_time,
		bpm,speed,measure,scroll;
	int notes,firstmeasure,start_measure_count,max_notes,notes_count,command,branch,
		original_id;	//ソート前のid
	bool flag,isDispBarLine,isDummy;
	std::string lyric;

} MEASURE_T;

void init_tja(),load_tja_notes(int course, LIST_T Song),white_tja(LIST_T Song),
tja_to_notes(int isDon, int isKatsu, int count, C2D_Sprite sprites[SPRITES_NUMER]),
load_tja_head_simple(LIST_T *List),get_tja_header(TJA_HEADER_T *TJA_Header),
get_command_value(char* buf, COMMAND_T *Command),conv_tja(LIST_T Song);
double get_FirstMeasureTime(),get_StartTime();
int get_MeasureId_From_OriginalId(int id), get_edme();
bool load_tja_head(int course, LIST_T Song), get_isBranch();
