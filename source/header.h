#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#define VERSION "1.4.0"
#define DEFAULT_DIR	"sdmc:/tjafiles/"
#define SETTING_FILE "sdmc:/TJAPlayerfor3ds_setting.json"
#define PATH_DSP1 "sdmc:/3ds/dspfirm.cdc"
#define GENRE_FILE "genre.json"

#define DEFAULT_JUDGE_RANGE_PERFECT	0.042
#define DEFAULT_JUDGE_RANGE_NICE	0.108
#define DEFAULT_JUDGE_RANGE_BAD		0.125

#define NOTES_MEASURE_MAX 256	//一小節の最大ノーツ数+1
#define MEASURE_MAX 8192
#define NOTES_AREA 338.0	//ノーツ表示エリアの長さ
#define NOTES_JUDGE_X 93	//判定枠の中心のX座標
#define NOTES_JUDGE_RANGE 327.0	//判定枠の中心から小節生成位置の距離(右端+20)

#define TOP_WIDTH  400
#define TOP_HEIGHT 240
#define BOTTOM_WIDTH  320
#define BOTTOM_HEIGHT 240

#define SPRITES_NUMER 85

#define NOTES_MAX 512
#define BARLINE_MAX 512
#define ROLL_MAX 512
#define BALLOON_MAX 512

#define LIST_MAX 16384		//選曲リストの最大数
#define GENRE_MAX 512		//ジャンルの最大数

#define DEFAULT_BUFFER_SIZE 8192

enum NOTES_KND {

	NOTES_REST = 0,		//休符
	NOTES_DON,		//ドン
	NOTES_KATSU,		//カツ
	NOTES_BIGDON,		//ドン(大)
	NOTES_BIGKATSU,		//カツ(大)
	NOTES_ROLL,		//連打開始
	NOTES_BIGROLL,		//連打(大)開始
	NOTES_BALLOON,		//風船開始
	NOTES_ROLLEND,		//連打終了
	NOTES_POTATO,		//お芋音符開始
	NOTES_BIGROLLEND,	//大連打終了
	NOTES_BALLOONEND,	//風船終了
};

enum SPRITE_NOTES_KND {	//スプライト用

	SPRITE_TOP = 0,
	SPRITE_BOTTOM,
	SPRITE_DON,
	SPRITE_KATSU,
	SPRITE_BIG_DON,
	SPRITE_BIG_KATSU,
	SPRITE_ROLL_START,
	SPRITE_ROLL_INT,
	SPRITE_ROLL_END,
	SPRITE_BIG_ROLL_START,
	SPRITE_BIG_ROLL_INT,
	SPRITE_BIG_ROLL_END,
	SPRITE_BALLOON,
	SPRITE_BALLOON_1,
	SPRITE_BALLOON_2,
	SPRITE_BALLOON_3,
	SPRITE_BALLOON_4,
	SPRITE_BALLOON_5,
	SPRITE_BALLOON_6,
	SPRITE_JUDGE_PERFECT,
	SPRITE_JUDGE_NICE,
	SPRITE_JUDGE_BAD,
	SPRITE_JUDGE_CIRCLE,
	SPRITE_CHART_NORMAL,
	SPRITE_CHART_EXPERT,
	SPRITE_CHART_MASTER,
	SPRITE_LANE_EXPERT,
	SPRITE_LANE_MASTER,
	SPRITE_EFFECT_PERFECT,
	SPRITE_EFFECT_SPECIAL_PERFECT,
	SPRITE_EFFECT_NICE,
	SPRITE_EFFECT_SPECIAL_NICE,
	SPRITE_SOUL_ON,
	SPRITE_SOUL_OFF,
	SPRITE_SOUL_EFFECT,
	SPRITE_EFFECT_GOGO,
	SPRITE_SCORE_0,
	SPRITE_SCORE_1,
	SPRITE_SCORE_2,
	SPRITE_SCORE_3,
	SPRITE_SCORE_4,
	SPRITE_SCORE_5,
	SPRITE_SCORE_6,
	SPRITE_SCORE_7,
	SPRITE_SCORE_8,
	SPRITE_SCORE_9,
	SPRITE_COMBO_0,
	SPRITE_COMBO_1,
	SPRITE_COMBO_2,
	SPRITE_COMBO_3,
	SPRITE_COMBO_4,
	SPRITE_COMBO_5,
	SPRITE_COMBO_6,
	SPRITE_COMBO_7,
	SPRITE_COMBO_8,
	SPRITE_COMBO_9,
	SPRITE_COMBO_0_RED,
	SPRITE_COMBO_1_RED,
	SPRITE_COMBO_2_RED,
	SPRITE_COMBO_3_RED,
	SPRITE_COMBO_4_RED,
	SPRITE_COMBO_5_RED,
	SPRITE_COMBO_6_RED,
	SPRITE_COMBO_7_RED,
	SPRITE_COMBO_8_RED,
	SPRITE_COMBO_9_RED,
	SPRITE_ROLL_0,
	SPRITE_ROLL_1,
	SPRITE_ROLL_2,
	SPRITE_ROLL_3,
	SPRITE_ROLL_4,
	SPRITE_ROLL_5,
	SPRITE_ROLL_6,
	SPRITE_ROLL_7,
	SPRITE_ROLL_8,
	SPRITE_ROLL_9,
	SPRITE_ROLL_COUNT,
	SPRITE_BALLOON_COUNT,
	SPRITE_EMBLEM_EASY,
	SPRITE_EMBLEM_NORMAL,
	SPRITE_EMBLEM_HARD,
	SPRITE_EMBLEM_ONI,
	SPRITE_EMBLEM_EDIT,
	SPRITE_EMBLEM_TOWER,
	SPRITE_EMBLEM_DAN,
	SPRITE_TOP2,
	SPRITE_DONCHAN_1,
	SPRITE_DONCHAN_2,
};

enum COMMAND_KND {

	COMMAND_START = 1,
	COMMAND_END,
	COMMAND_BPMCHANGE,
	COMMAND_GOGOSTART,
	COMMAND_GOGOEND,
	COMMAND_MEASURE,
	COMMAND_SCROLL,
	COMMAND_DELAY,
	COMMAND_SECTION,
	COMMAND_BRANCHSTART,
	COMMAND_BRANCHEND,
	COMMAND_N,
	COMMAND_E,
	COMMAND_M,
	COMMAND_LEVELHOLD,
	COMMAND_BMSCROLL,
	COMMAND_HBSCROLL,
	COMMAND_BARLINEOFF,
	COMMAND_BARLINEON,
};

enum HEADER_KND {

	HEADER_TITLE,
	HEADER_SUBTITLE,
	HEADER_BPM,
	HEADER_WAVE,
	HEADER_OFFSET,
	HEADER_BALLOON,
	HEADER_SONGVOL,
	HEADER_SEVOL,
	HEADER_SCOREINIT,
	HEADER_SCOREDIFF,
	HEADER_COURSE,
	HEADER_STYLE,
	HEADER_LIFE,
	HEADER_DEMOSTART,
	HEADER_SIDE,
	HEADER_SCOREMODE,
	HEADER_TOTAL,
};

enum COURSE_KND {

	COURSE_EASY = 0,
	COURSE_NORMAL,
	COURSE_HARD,
	COURSE_ONI,
	COURSE_EDIT,
	COURSE_TOWER,
	COURSE_DAN,
};

enum SCENE_STATE {

	SCENE_SELECTLOAD = 0,
	SCENE_WARNING = 5,
	SCENE_SELECTSONG = 10,
	SCENE_MAINLOAD = 50,
	SCENE_MAINGAME = 100,
	SCENE_RESULT = 110,
};

enum SOUND_KND {

	SOUND_DON = 0,
	SOUND_KATSU,
	SOUND_BALLOONBREAK,
	SOUND_FAILED,
};

enum TIME_KND {

	TIME_NOTES = 0,	//ノーツが開始(最初の小節が生成)で計測開始,cntはこれに最初にcreate_time加算(マイナス用,通常は0),
	TIME_MAINGAME,	//メインゲーム,開始時には-1000,1秒後に計測開始,ノーツ・音楽開始にのみ使用
	TIME_FPS,	//fps計測用
};

enum WARNING_KND {

	WARNING_DSP1 = 0,		//DSP1未起動
	WARNING_WAVE_NO_EXIST,		//音楽ファイルが存在しない
	WARNING_WAVE_NOT_OGG,		//音楽ファイルがOGGファイルじゃない
};