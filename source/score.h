#pragma once

typedef struct {
	float perfect, nice, bad, score, norma, soul;

}GAUGE_T;

enum Judge_Knd {
	PERFECT = 0,
	SPECIAL_PERFECT,
	NICE,
	SPECIAL_NICE,
	BAD,
	THROUGH,
	ROLL,
	BIG_ROLL,
	ROLL_END,
	BALLOON,
	BALLOON_BREAK,
};

void init_score(),debug_score(),update_score(int knd),send_gogotime(bool temp),set_rollmultiple(int i),init_branch_section();
int dan_condition(),round_down(int arg),start_branch(int knd, double x, double y);
void calc_base_score(MEASURE_T Measure[MEASURE_MAX], char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX]);
void draw_lane(C2D_Sprite sprites[SPRITES_NUMER], int kcnt, int dcnt),draw_gauge(C2D_Sprite sprites[SPRITES_NUMER]),draw_score(C2D_Sprite sprites[SPRITES_NUMER]),
void update_balloon_count(int arg),draw_emblem(C2D_Sprite sprites[SPRITES_NUMER]),draw_gauge_result(C2D_Sprite sprites[SPRITES_NUMER]);
