#include "header.h"
#include "notes.h"
#include "tja.h"
#include "main.h"
#include "score.h"
#include "result.h"

extern int gaugelife, redCdn[4];
extern char *exam[4][4];
bool isGOGO;
int combo,init,diff,DiffMul,scoremode,HitScore,ScoreDiff,courselife,TotalPerfectCount,TotalNiceCount,TotalBadCount,
CurrentScore,TotalScore,CurrentTotalRollCount,CurrentRollCount,TotalRollCount,TotalCount,
CurrentPerfectCount,CurrentNiceCount,CurrentBadCount,CurrentBalloonCount,CurrentDaiNotes,MaxComboCount;
volatile double BaseCeilingPoint;
double tmp, Precision, CurrentPrecision;
TJA_HEADER_T TJA_Header;
char buf_score[160];
GAUGE_T Gauge;

void init_guage_structure() {

	Gauge.perfect = 0;
	Gauge.nice = 0;
	Gauge.bad = 0;
	Gauge.score = 0;
	Gauge.norma = 0;
	Gauge.soul = 0;
}

void init_score() {

	isGOGO = false;
	combo = 0;
	get_tja_header(&TJA_Header);
	init = TJA_Header.scoreinit;
	diff = TJA_Header.scorediff;
	DiffMul = 0;
	scoremode = TJA_Header.scoremode;
	HitScore = 0;
	ScoreDiff = 0;
	BaseCeilingPoint = 0;
	CurrentScore = 0;
	TotalScore = 0;
	CurrentRollCount = 0;
	CurrentTotalRollCount = 0;
	TotalRollCount = 0;
	TotalPerfectCount = 0;
	TotalNiceCount = 0;
	TotalBadCount = 0;
	CurrentPerfectCount = 0;
	CurrentNiceCount = 0;
	CurrentBadCount = 0;
	Precision = 0;
	CurrentPrecision = 0;
	CurrentBalloonCount = 0;
	CurrentDaiNotes = 0;
	MaxComboCount = 0;
	init_guage_structure();
	TotalCount = 0;
}


void update_score(int knd) {

	int PreScore = TotalScore;
	bool isCombo = false;
	TotalCount = TotalPerfectCount + TotalNiceCount + TotalRollCount;

	//if (scoremode == 1 || scoremode == 2) {

	double GOGOMul;
	if (isGOGO == true) GOGOMul = 1.2;
	else GOGOMul = 1.0;

	if (scoremode == 0) {	//ドンダフル配点
		if (combo < 200) DiffMul = 0;
		else DiffMul = 1;
	}
	else if (scoremode == 2) {	//新配点
		if (0 <= combo && combo < 9) DiffMul = 0;
		else if (9 <= combo && combo < 29) DiffMul = 1;
		else if (29 <= combo && combo < 49) DiffMul = 2;
		else if (49 <= combo && combo < 99) DiffMul = 4;
		else if (99 <= combo) DiffMul = 8;
	}
	else if (scoremode == 1) {	//旧配点
		DiffMul = (combo + 1) / 10;
		if (combo > 100) DiffMul = 10;
	}
	else if (scoremode == 3) {	//ニジイロ配点
		DiffMul = 0;
		GOGOMul = 1.0;
	}

	HitScore = init + diff * DiffMul;

	switch (knd) {

	case PERFECT:
		TotalScore += round_down(HitScore * GOGOMul);
		CurrentScore += round_down(HitScore * GOGOMul);
		++combo;
		isCombo = true;
		++TotalPerfectCount;
		++CurrentPerfectCount;
		Gauge.score += Gauge.perfect;
		break;

	case SPECIAL_PERFECT:
		TotalScore += round_down(HitScore * GOGOMul) * (scoremode == 3 ? 1 : 2);
		CurrentScore += round_down(HitScore * GOGOMul) * (scoremode == 3 ? 1 : 2);
		++combo;
		isCombo = true;
		++TotalPerfectCount;
		++CurrentPerfectCount;
		Gauge.score += Gauge.perfect;
		++CurrentDaiNotes;
		break;

	case NICE:
		TotalScore += round_down(HitScore / 2);
		CurrentScore += round_down(HitScore / 2);
		++combo;
		isCombo = true;
		++TotalNiceCount;
		++CurrentNiceCount;
		Gauge.score += Gauge.nice;
		break;

	case SPECIAL_NICE:
		TotalScore += round_down((scoremode == 3) ? (HitScore / 2) : (HitScore - 10));
		CurrentScore += round_down((scoremode == 3) ? (HitScore / 2) : (HitScore - 10));
		++combo;
		isCombo = true;
		++TotalNiceCount;
		++CurrentNiceCount;
		Gauge.score += Gauge.nice;
		++CurrentDaiNotes;
		break;

	case BAD:
		combo = 0;
		++TotalBadCount;
		++CurrentBadCount;
		Gauge.score -= Gauge.bad;
		break;

	case THROUGH:
		combo = 0;
		++TotalBadCount;
		++CurrentBadCount;
		Gauge.score -= Gauge.bad;
		break;

	case BALLOON:
	case ROLL:
		if (scoremode == 0 || scoremode == 1) {	//旧配点
			if (isGOGO == true) {
				TotalScore += 360;
				CurrentScore += 360;
			}
			else {
				TotalScore += 300;
				CurrentScore += 300;
			}
		}
		else if (scoremode == 2) {	//新配点
			if (isGOGO == true) {
				TotalScore += 120;
				CurrentScore += 120;
			}
			else {
				TotalScore += 100;
				CurrentScore += 100;
			}
		}
		else if (scoremode == 3) {	//ニジイロ配点
			TotalScore += 100;
			CurrentScore += 100;
		}

		if (knd == ROLL) {
			++CurrentRollCount;
			++CurrentTotalRollCount;
		}
			++TotalRollCount;
		break;

	case BIG_ROLL:
		if (scoremode == 0 || scoremode == 1) {	//旧配点
			if (isGOGO == true) {
				TotalScore += 430;
				CurrentScore += 430;
			}
			else {
				TotalScore += 360;
				CurrentScore += 360;
			}
		}
		else if (scoremode == 2) {	//新配点
			if (isGOGO == true) {
				TotalScore += 240;
				CurrentScore += 240;
			}
			else {
				TotalScore += 200;
				CurrentScore += 200;
			}
		}
		else if (scoremode == 3) {	//ニジイロ配点
			TotalScore += 100;
			CurrentScore += 100;
		}
		++CurrentTotalRollCount;
		++TotalRollCount;
		++CurrentRollCount;
		break;

	case BALLOON_BREAK:
		if (scoremode != 3) {
			if (isGOGO == true) {
				TotalScore += 6000;
				CurrentScore += 6000;
			}
			else {
				TotalScore += 5000;
				CurrentScore += 5000;
			}
		}
		else if (scoremode == 3) {	//ニジイロ配点
			TotalScore += 100;
			CurrentScore += 100;
		}
		++TotalRollCount;
		break;

	case ROLL_END:
		CurrentRollCount = 0;
		break;
	}
	//}

	if (combo > MaxComboCount) MaxComboCount = combo;
	if (Gauge.score < 0) Gauge.score = 0;

	if (scoremode == 2) {	//100コンボ毎のボーナス(新配点のみ)
		if (isCombo == true && combo % 100 == 0) {
			TotalScore += 10000;
			CurrentScore += 10000;
		}
	}
	ScoreDiff = TotalScore - PreScore;
	if ((TotalPerfectCount + TotalNiceCount + TotalBadCount) != 0) Precision = (double)TotalPerfectCount / (TotalPerfectCount + TotalNiceCount + TotalBadCount) * 100.0;
	else Precision = 0;
	if ((CurrentPerfectCount + CurrentNiceCount + CurrentBadCount) != 0) CurrentPrecision = (double)CurrentPerfectCount / (CurrentPerfectCount + CurrentNiceCount + CurrentBadCount) * 100.0;
	else CurrentPrecision = 0;
}

void draw_score(C2D_Sprite  sprites[SPRITES_NUMER]) {


	//スコア
	for (int i = 0; i < 7; ++i) {

		if (TotalScore / powi(10, i) > 0) {
			int n = TotalScore / powi(10, i) % 10;
			C2D_SpriteSetPos(&sprites[SPRITE_SCORE_0 + n], 80 - i * 12, 70);
			C2D_DrawSprite(&sprites[SPRITE_SCORE_0 + n]);
		}
	}

	int j;

	//コンボ
	for (j = 0; j < 5; ++j) {
		if (combo / powi(10, j) == 0) break;
	}
	for (int i = 0; i < 5; ++i) {

		if (combo >= 10 && combo / powi(10, i) > 0) {

			int n = combo / powi(10, i) % 10;

			if (combo < 100) {
				C2D_SpriteSetPos(&sprites[SPRITE_COMBO_0 + n], 24 + j * 7 - i * 14, 110);
				C2D_DrawSprite(&sprites[SPRITE_COMBO_0 + n]);
			}
			else {
				C2D_SpriteSetPos(&sprites[SPRITE_COMBO_0_RED + n], 24 + j * 7 - i * 14, 110);
				C2D_DrawSprite(&sprites[SPRITE_COMBO_0_RED + n]);
			}
		}
	}

	//連打
	for (j = 0; j < 4; ++j) {
		if (CurrentRollCount / powi(10, j) == 0) break;
	}
	if (CurrentRollCount > 0) {
		C2D_SpriteSetPos(&sprites[SPRITE_ROLL_COUNT], 110, 35);
		C2D_DrawImage(sprites[SPRITE_ROLL_COUNT].image, &sprites[SPRITE_ROLL_COUNT].params, NULL);
	}
	for (int i = 0; i < 4; ++i) {

		if (CurrentRollCount / powi(10, i) > 0) {

			int n = CurrentRollCount / powi(10, i) % 10;
			C2D_SpriteSetPos(&sprites[SPRITE_ROLL_0 + n], 95 + j * 10 - i * 20, 30);
			C2D_DrawSprite(&sprites[SPRITE_ROLL_0 + n]);
		}
	}

	//風船
	for (j = 0; j < 5; ++j) {
		if (CurrentBalloonCount / powi(10, j) == 0) break;
	}
	if (CurrentBalloonCount > 0) {
		C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_COUNT], 110, 35);
		C2D_DrawImage(sprites[SPRITE_BALLOON_COUNT].image, &sprites[SPRITE_BALLOON_COUNT].params, NULL);
	}
	for (int i = 0; i < 5; ++i) {

		if (CurrentBalloonCount / powi(10, i) > 0) {

			int n = CurrentBalloonCount / powi(10, i) % 10;
			C2D_SpriteSetPos(&sprites[SPRITE_ROLL_0 + n], 97 + j * 10 - i * 20, 30);
			C2D_DrawSprite(&sprites[SPRITE_ROLL_0 + n]);
		}
	}
}

void draw_gauge(C2D_Sprite  sprites[SPRITES_NUMER]) {

	courselife = Gauge.score;
	if (Gauge.score > Gauge.soul) Gauge.score = Gauge.soul;
	double gauge = 1.0 * ((int)Gauge.score / 200) * 200 / Gauge.soul;
	if (gauge > 1.0) gauge = 1.0;

	//赤
	C2D_DrawRectSolid(123, 76, 0, 250.0 * Gauge.norma / Gauge.soul, 8, C2D_Color32f(102.0 / 255, 0, 0, 1));
	C2D_DrawRectSolid(123, 76, 0, 250.0 * gauge, 8, C2D_Color32f(1, 0, 0, 1));

	//黄
	C2D_DrawRectSolid(123 + 250.0 * Gauge.norma / Gauge.soul, 67, 0, 250 - 250.0 * Gauge.norma / Gauge.soul, 17, C2D_Color32f(102.0 / 255, 68.0 / 255, 0, 1));
	if (250 * gauge - (250.0 * Gauge.norma / Gauge.soul) >= 0)
		C2D_DrawRectSolid(123 + 250.0 * Gauge.norma / Gauge.soul, 67, 0, 250 * gauge - (250.0 * Gauge.norma / Gauge.soul), 17, C2D_Color32f(1, 1, 12.0 / 255, 1));

	//魂
	for (int i = 0; i < 2; ++i) C2D_SpriteSetPos(&sprites[SPRITE_SOUL_ON + i], 385, 75);
	C2D_SpriteSetPos(&sprites[SPRITE_SOUL_EFFECT], 395, 65);
	if ((Gauge.score / 200) * 200 >= Gauge.soul) {
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 0.8);
		C2D_DrawImage(sprites[SPRITE_SOUL_EFFECT].image, &sprites[SPRITE_SOUL_EFFECT].params, &Tint);
		C2D_DrawImage(sprites[SPRITE_SOUL_ON].image, &sprites[SPRITE_SOUL_ON].params, NULL);
	}
	else C2D_DrawImage(sprites[SPRITE_SOUL_OFF].image, &sprites[SPRITE_SOUL_OFF].params, NULL);
}

void draw_gauge_result(C2D_Sprite  sprites[SPRITES_NUMER]) {

	int diff = 50, scores = (int)Gauge.score;
	double x_start = 123 - diff, x_end = 250 - diff;
	double gauge = 1.0 * (scores / 200) * 200 / Gauge.soul;
	if (gauge > 1.0) gauge = 1.0;

	//赤
	C2D_DrawRectSolid(x_start, 76, 0, x_end * Gauge.norma / Gauge.soul, 8, C2D_Color32f(102.0 / 255, 0, 0, 1));
	C2D_DrawRectSolid(x_start, 76, 0, x_end * gauge, 8, C2D_Color32f(1, 0, 0, 1));

	//黄
	C2D_DrawRectSolid(x_start + x_end * Gauge.norma / Gauge.soul, 67, 0, x_end - x_end * Gauge.norma / Gauge.soul, 17, C2D_Color32f(102.0 / 255, 68.0 / 255, 0, 1));
	if (x_end * gauge - (x_end * Gauge.norma / Gauge.soul) >= 0)
		C2D_DrawRectSolid(x_start + x_end * Gauge.norma / Gauge.soul, 67, 0, x_end * gauge - (x_end * Gauge.norma / Gauge.soul), 17, C2D_Color32f(1, 1, 12.0 / 255, 1));

	//魂
	for (int i = 0; i < 2; ++i) C2D_SpriteSetPos(&sprites[SPRITE_SOUL_ON + i], 385 - diff * 2, 75);
	C2D_SpriteSetPos(&sprites[SPRITE_SOUL_EFFECT], 395 - diff * 2, 65);
	if ((Gauge.score / 200) * 200 >= Gauge.soul) {
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 0.8);
		C2D_DrawImage(sprites[SPRITE_SOUL_EFFECT].image, &sprites[SPRITE_SOUL_EFFECT].params, &Tint);
		C2D_DrawImage(sprites[SPRITE_SOUL_ON].image, &sprites[SPRITE_SOUL_ON].params, NULL);
	}
	else C2D_DrawImage(sprites[SPRITE_SOUL_OFF].image, &sprites[SPRITE_SOUL_OFF].params, NULL);
}

void draw_lane(C2D_Sprite sprites[SPRITES_NUMER], int kcnt, int dcnt) {


	if (get_isBranch() == true) {

		int branch = get_branch_course();

		switch (branch) {
		case COMMAND_N:
		default:
			C2D_SpriteSetPos(&sprites[SPRITE_CHART_NORMAL], 350, 110);
			C2D_DrawImage(sprites[SPRITE_CHART_NORMAL].image, &sprites[SPRITE_CHART_NORMAL].params, NULL);
			break;

		case COMMAND_E:
			C2D_SpriteSetPos(&sprites[SPRITE_LANE_EXPERT], 233, 109);
			C2D_DrawImage(sprites[SPRITE_LANE_EXPERT].image, &sprites[SPRITE_LANE_EXPERT].params, NULL);
			C2D_SpriteSetPos(&sprites[SPRITE_CHART_EXPERT], 350, 110);
			C2D_DrawImage(sprites[SPRITE_CHART_EXPERT].image, &sprites[SPRITE_CHART_EXPERT].params, NULL);
			break;

		case COMMAND_M:
			C2D_SpriteSetPos(&sprites[SPRITE_LANE_MASTER], 233, 109);
			C2D_DrawImage(sprites[SPRITE_LANE_MASTER].image, &sprites[SPRITE_LANE_MASTER].params, NULL);
			C2D_SpriteSetPos(&sprites[SPRITE_CHART_MASTER], 350, 110);
			C2D_DrawImage(sprites[SPRITE_CHART_MASTER].image, &sprites[SPRITE_CHART_MASTER].params, NULL);
			break;
		}
	}

	C2D_DrawRectangle(62,86,0,338,46,C2D_Color32f(0,169.0/255.0,253.0/255.0,kcnt/60.0),C2D_Color32(0x00,0xA9,0xFD,0x00),C2D_Color32f(0,169.0/255.0,253.0/255.0,kcnt/60.0),C2D_Color32(0x00,0xA9,0xFD,0x00));
	C2D_DrawRectangle(62,86,0,338,46,C2D_Color32f(253.0/255.0,0,0,dcnt/60.0),C2D_Color32(0xFD,0x00,0x00,0x00),C2D_Color32f(253.0/255.0,0,0,dcnt/60.0),C2D_Color32(0xFD,0x00,0x00,0x00));
	C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_CIRCLE], NOTES_JUDGE_X, 109);
	C2D_DrawImage(sprites[SPRITE_JUDGE_CIRCLE].image, &sprites[SPRITE_JUDGE_CIRCLE].params, NULL);
	if (isGOGO == true) {
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 0.8);
		C2D_DrawImage(sprites[SPRITE_EFFECT_GOGO].image, &sprites[SPRITE_EFFECT_GOGO].params, &Tint);
	}
}

int start_branch(int knd, double x, double y) {	//分岐

	int branch, CurrentCombo = CurrentPerfectCount + CurrentNiceCount;
	switch (knd) {
	case 0:	//連打
		if (y <= CurrentTotalRollCount) branch = COMMAND_M;
		else if (x <= CurrentTotalRollCount) branch = COMMAND_E;
		else branch = COMMAND_N;
		break;
	case 1:	//精度
		if (y <= CurrentPrecision) branch = COMMAND_M;
		else if (x <= CurrentPrecision) branch = COMMAND_E;
		else branch = COMMAND_N;
		break;
	case 2:	//スコア
		if (y <= CurrentScore) branch = COMMAND_M;
		else if (x <= CurrentScore) branch = COMMAND_E;
		else branch = COMMAND_N;
		break;
	case 3:	//大音符
		if (y <= CurrentDaiNotes) branch = COMMAND_M;
		else if (x <= CurrentDaiNotes) branch = COMMAND_E;
		else branch = COMMAND_N;
		break;
	case 4:	//コンボ数
		if (y <= CurrentCombo) branch = COMMAND_M;
		else if (x <= CurrentCombo) branch = COMMAND_E;
		else branch = COMMAND_N;
		break;
	default:
		branch = COMMAND_N;
		break;
	}
	return branch;
}

void init_branch_section() {	//#SECTION

	CurrentTotalRollCount = 0;
	CurrentPerfectCount = 0;
	CurrentNiceCount = 0;
	CurrentBadCount = 0;
	CurrentScore = 0;
	CurrentPrecision = 0;
	CurrentDaiNotes = 0;
}

void send_gogotime(bool arg) {
	isGOGO = arg;
}

int dan_condition() {
	int isBadCondition = 0; //ここで何回条件に反しているかをリセットする
	const char* types[] = {"jb", "jg", "jp", "s", "r", "h", "g"};
	int counts[] = {TotalBadCount, TotalNiceCount, TotalPerfectCount, TotalScore, TotalRollCount, TotalCount, (int)(Gauge.score / Gauge.soul * 100.0)};
    
	for (int i = 0; i < 7; ++i) {
		if ((strcmp(exam[0][0], types[i]) == 0 && strcmp(exam[0][3], "l") == 0 && counts[i] >= redCdn[0]) ||
		(strcmp(exam[1][0], types[i]) == 0 && strcmp(exam[1][3], "l") == 0 && counts[i] >= redCdn[1]) ||
		(strcmp(exam[2][0], types[i]) == 0 && strcmp(exam[2][3], "l") == 0 && counts[i] >= redCdn[2]) ||
		(strcmp(exam[3][0], types[i]) == 0 && strcmp(exam[3][3], "l") == 0 && counts[i] >= redCdn[3])) ++isBadCondition;
	}

	if (get_notes_finish() == true) { //条件に以上(例:exam[3] = "m")がある場合、曲が終わるまで判定しない
		for (int i = 0; i < 7; ++i) {
			if ((strcmp(exam[0][0], types[i]) == 0 && strcmp(exam[0][3], "m") == 0 && counts[i] < redCdn[0]) ||
			(strcmp(exam[1][0], types[i]) == 0 && strcmp(exam[1][3], "m") == 0 && counts[i] < redCdn[1]) ||
			(strcmp(exam[2][0], types[i]) == 0 && strcmp(exam[2][3], "m") == 0 && counts[i] < redCdn[2]) ||
			(strcmp(exam[3][0], types[i]) == 0 && strcmp(exam[3][3], "m") == 0 && counts[i] < redCdn[3])) ++isBadCondition;
		}
	}
	return isBadCondition;
}


int round_down(int arg) {
	int temp = arg % 10;
	return arg - temp;
}

void calc_base_score(MEASURE_T Measure[MEASURE_MAX], char notes[MEASURE_MAX][NOTES_MEASURE_MAX]) {	//初項と公差を計算　魂ゲージの伸びも

	int NotesCount = 0,i = 0,DiffTmp = 0,BalloonCnt = 0,combo = 0,
	TmpBaseCeilingPoint = 0,NotesCountMax = 0,RollKnd = 0,RollCnt = 0;
	bool isEND = false;
	double init_cnt = 0,diff_cnt = 0,gogo = 1,special = 1,
	roll_start_time = 0,roll_end_time = 0;
	COMMAND_T Command;

	int PerfectNotesCount = 0;	//魂ゲージの伸び計算用

	int level = TJA_Header.level;
	if (level > 10) level = 10;

	switch (TJA_Header.course) {	//基本天井点を設定
	case 0:	//かんたん
		BaseCeilingPoint = 280000 + level * 20000;
		if (scoremode == 3) BaseCeilingPoint = 1000000;
		break;
	case 1:	//ふつう
		BaseCeilingPoint = 350000 + level * 50000;
		if (scoremode == 3) BaseCeilingPoint = 1000000;
		break;
	case 2:	//むずかしい
		BaseCeilingPoint = 500000 + level * 50000;
		if (scoremode == 3) BaseCeilingPoint = 1000000;
		break;
	case 3:	//おに
	case 4:
	case 5:	//太鼓タワー
		if (level == 10) BaseCeilingPoint = 1200000;
		else BaseCeilingPoint = 650000 + level * 50000;
		if (scoremode == 3) BaseCeilingPoint = 1000000;
		break;
	case 6:	//段位道場
		BaseCeilingPoint = 3000000;
		break;
	}
	TmpBaseCeilingPoint = BaseCeilingPoint;

	while (isEND == false && i < MEASURE_MAX && Measure[i].flag == true) {	//小節

		NotesCount = 0;

		if (Measure[i].branch != -1 && Measure[i].branch != COMMAND_M) {

			++i;
			continue;
		}

		if (NotesCount == 0 && notes[Measure[i].notes][0] == '#') {

			get_command_value(notes[Measure[i].notes], &Command);

			switch (Command.knd) {
			case COMMAND_GOGOSTART:
				gogo = 1.2;
				break;
			case COMMAND_GOGOEND:
				gogo = 1.0;
				break;
			case COMMAND_END:
				isEND = true;
				break;
			default:
				break;

			}
			NotesCount = 0;
			++i;
			continue;
		}
		if (scoremode == 3) gogo = 1.0;
		while (notes[Measure[i].notes][NotesCount] != ',' && notes[Measure[i].notes][NotesCount] != '\n' && notes[Measure[i].notes][NotesCount] != '/') {

			++NotesCount;
		}
		if (Measure[i].firstmeasure != -1) NotesCountMax = Measure[Measure[i].firstmeasure].max_notes;
		else NotesCountMax = NotesCount;

		for (int j = 0; j < NotesCount; ++j) {	//ノーツ

			int knd = ctoi(notes[Measure[i].notes][j]);

			if (knd != 0) {

				if (knd == NOTES_DON || knd == NOTES_KATSU || knd == NOTES_BIGDON || knd == NOTES_BIGKATSU) {
					if ((knd == NOTES_BIGDON || knd == NOTES_BIGKATSU) && scoremode != 3) special = 2.0;
					else special = 1.0;
					++combo;
					init_cnt += 1 * gogo * special;

					if (scoremode == 1) {		//旧配点

						if (combo > 100) DiffTmp = 10;
						else DiffTmp = combo / 10;
					}
					else if (scoremode == 2) {	//新配点

						if (combo >= 1 && combo <= 9) DiffTmp = 0;
						else if (combo >= 10 && combo <= 29) DiffTmp = 1;
						else if (combo >= 30 && combo <= 49) DiffTmp = 2;
						else if (combo >= 50 && combo <= 99) DiffTmp = 4;
						else if (combo >= 100) DiffTmp = 8;
					}
					else if (scoremode == 3) DiffTmp = 0;	//ニジイロ配点

					diff_cnt += DiffTmp * gogo * special;

					++PerfectNotesCount;
				}
				else if (knd == NOTES_BALLOON) {		//風船

					if (scoremode != 3) TmpBaseCeilingPoint -= (TJA_Header.balloon[((Measure[i].branch == -1) ? 0 : Measure[i].branch - 11)][BalloonCnt] * 300 + 5000) * gogo;
					if (scoremode == 3) TmpBaseCeilingPoint -= (TJA_Header.balloon[((Measure[i].branch == -1) ? 0 : Measure[i].branch - 11)][BalloonCnt] * 100 + 100);
					++BalloonCnt;
				}
				else if (knd == NOTES_ROLL) {			//連打

					roll_start_time = Measure[i].judge_time + (240.0 / Measure[i].bpm * Measure[i].measure * i / NotesCountMax);
					RollKnd = NOTES_ROLL;
				}
				else if (knd == NOTES_BIGROLL) {		//大連打

					roll_start_time = Measure[i].judge_time + (240.0 / Measure[i].bpm * Measure[i].measure * i / NotesCountMax);
					RollKnd = NOTES_BIGROLL;
				}
				else if (knd == NOTES_ROLLEND) {

					if (roll_start_time != 0) {

						roll_end_time = Measure[i].judge_time + 240.0 / Measure[i].bpm * Measure[i].measure * i / NotesCountMax;
						if (RollKnd == NOTES_ROLL) {
							if (scoremode == 1) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * 15.0);
								TmpBaseCeilingPoint -= RollCnt * 300 * gogo;
							}
							if (scoremode == 2) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * 15.0);
								TmpBaseCeilingPoint -= RollCnt * 100 * gogo;
							}
							if (scoremode == 3) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * level);
								TmpBaseCeilingPoint -= RollCnt * 100;
							}
						}
						else if (RollKnd == NOTES_BIGROLL) {
							if (scoremode == 1) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * 15.0);
								TmpBaseCeilingPoint -= RollCnt * 360 * gogo;
							}
							if (scoremode == 2) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * 15.0);
								TmpBaseCeilingPoint -= RollCnt * 200 * gogo;
							}
							if (scoremode == 3) {
								RollCnt = (int)ceil((roll_end_time - roll_start_time) * level);
								TmpBaseCeilingPoint -= RollCnt * 100;
							}
						}
						roll_start_time = 0;
						roll_end_time = 0;
						RollCnt = 0;
					}
				}
			}
		}
		++i;
	}

	if ((TJA_Header.scoreinit == -1 || TJA_Header.scorediff == -1) && (scoremode == 1 || scoremode == 2)) {	//新配点と旧配点
		diff = (TmpBaseCeilingPoint - (int)(combo / 100) * 10000) / (init_cnt * 4 + diff_cnt);
		init = diff * 4;
	}
	else if (TJA_Header.scoreinit == -1 && scoremode == 3) {
		double scoreNiji = 0,scoretmp = 0;
		while (scoretmp < TmpBaseCeilingPoint) {
			scoreNiji += 10;
			scoretmp = combo * scoreNiji;
		}
		init = scoreNiji;
		diff = 0;
	}
	else if (scoremode == 0) {
		init = 1000;
		diff = 1000;
	}

	Gauge.perfect = 10000.0 / PerfectNotesCount;

	switch (TJA_Header.course) {	//ゲージの伸び率の計算
	case 0:		//かんたん
		Gauge.nice = Gauge.perfect * 3.0 / 4.0;
		Gauge.bad = Gauge.perfect / 2.0;
		if (level <= 1) {
			Gauge.norma = 3600;
			Gauge.soul = 6000;
		}
		else if (level <= 3) {
			Gauge.norma = 3800;
			Gauge.soul = 6333;
		}
		else if (level >= 4) {
			Gauge.norma = 4400;
			Gauge.soul = 7333;
		}
		break;

	case 1:		//ふつう
		Gauge.nice = Gauge.perfect * 3.0 / 4.0;
		if (level <= 2) {
			Gauge.bad = Gauge.perfect / 2.0;
			Gauge.norma = 4595;
			Gauge.soul = 6560;
		}
		else if (level == 3) {
			Gauge.bad = Gauge.perfect / 2.0;
			Gauge.norma = 4868;
			Gauge.soul = 6955;
		}
		else if (level == 4) {
			Gauge.bad = Gauge.perfect * 3.0 / 4.0;
			Gauge.norma = 4925;
			Gauge.soul = 7035;
		}
		else if (level >= 5) {
			Gauge.bad = Gauge.perfect;
			Gauge.norma = 5250;
			Gauge.soul = 7500;
		}
		break;

	case 2:		//むずかしい
		Gauge.nice = Gauge.perfect * 3.0 / 4.0;
		if (level <= 2) {
			Gauge.bad = Gauge.perfect / 2.0;
			Gauge.norma = 5450;
			Gauge.soul = 7750;
		}
		else if (level == 3) {
			Gauge.bad = Gauge.perfect;
			Gauge.norma = 5080;
			Gauge.soul = 7250;
		}
		else if (level <= 4) {
			Gauge.bad = Gauge.perfect * 7.0 / 6.0;
			Gauge.norma = 4840;
			Gauge.soul = 6910;
		}
		else if (level <= 5) {
			Gauge.bad = Gauge.perfect * 5.0 / 4.0;
			Gauge.norma = 4724;
			Gauge.soul = 6750;
		}
		else if (level >= 6) {
			Gauge.bad = Gauge.perfect * 5.0 / 4.0;
			Gauge.norma = 4812;
			Gauge.soul = 6875;
		}
		break;

	case 3:		//おに
	default:
		Gauge.nice = Gauge.perfect / 2.0;
		if (level <= 7) {
			Gauge.bad = Gauge.perfect * 8.0 / 5.0;
			Gauge.norma = 5660;
			Gauge.soul = 7075;
		}
		else if (level == 8) {
			Gauge.bad = Gauge.perfect * 2.0;
			Gauge.norma = 5600;
			Gauge.soul = 7000;
		}
		else if (level >= 9) {
			Gauge.bad = Gauge.perfect * 2.0;
			Gauge.norma = 6000;
			Gauge.soul = 7500;
		}
		break;

	case 5:		//太鼓タワー
		Gauge.perfect = 0;
		Gauge.nice = Gauge.perfect / 2.0;
		Gauge.bad = 1000;
		Gauge.norma = gaugelife * 1000;
		Gauge.soul = gaugelife * 1000;
		Gauge.score = gaugelife * 1000;
		break;

	case 6:		//段位道場
		Gauge.nice = Gauge.perfect / 2.0;
		if (level <= 7) {
			Gauge.bad = Gauge.perfect * 8.0 / 5.0;
			Gauge.soul = 7075;
		}
		else if (level == 8) {
			Gauge.bad = Gauge.perfect * 2.0;
			Gauge.soul = 7000;
		}
		else if (level >= 9) {
			Gauge.bad = Gauge.perfect * 2.0;
			Gauge.soul = 7500;
		}
		Gauge.norma = Gauge.soul;
		if (strcmp(exam[0][0], "g") == 0 && strcmp(exam[0][3], "m") == 0 ) Gauge.norma = Gauge.soul * (redCdn[0] / 100.00);
		else if (strcmp(exam[1][0], "g") == 0 && strcmp(exam[1][3], "m") == 0 ) Gauge.norma = Gauge.soul * (redCdn[1] / 100.00);
		else if (strcmp(exam[2][0], "g") == 0 && strcmp(exam[2][3], "m") == 0 ) Gauge.norma = Gauge.soul * (redCdn[2] / 100.00);
		else if (strcmp(exam[3][0], "g") == 0 && strcmp(exam[3][3], "m") == 0 ) Gauge.norma = Gauge.soul * (redCdn[3] / 100.00);
		break;
	}
	//init_score_after();
}

void update_balloon_count(int arg) {
	CurrentBalloonCount = arg;
}

void draw_emblem(C2D_Sprite  sprites[SPRITES_NUMER]) {

	switch (TJA_Header.course) {
	case COURSE_EASY:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(1, 51.0 / 255.0, 0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_EASY].image, &sprites[SPRITE_EMBLEM_EASY].params, NULL);
		break;
	case COURSE_NORMAL:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(136.0 / 255.0, 204.0 / 255.0, 34.0 / 255.0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_NORMAL].image, &sprites[SPRITE_EMBLEM_NORMAL].params, NULL);
		break;
	case COURSE_HARD:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(51.0 / 255.0, 170.0 / 255.0, 187.0 / 255.0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_HARD].image, &sprites[SPRITE_EMBLEM_HARD].params, NULL);
		break;
	case COURSE_ONI:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(1, 34.0 / 255.0, 204.0 / 255.0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_ONI].image, &sprites[SPRITE_EMBLEM_ONI].params, NULL);
		break;
	case COURSE_EDIT:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(136.0 / 255.0, 34.0 / 255.0, 1, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_EDIT].image, &sprites[SPRITE_EMBLEM_EDIT].params, NULL);
		break;
	case COURSE_TOWER:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(136.0 / 255.0, 86.0 / 255.0, 12.0 / 255.0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_TOWER].image, &sprites[SPRITE_EMBLEM_TOWER].params, NULL);
		break;
	case COURSE_DAN:
		C2D_DrawRectSolid(0, 86, 0, 62, 58, C2D_Color32f(14.0 / 255.0, 88.0 / 255.0, 129.0 / 255.0, 1));
		C2D_DrawImage(sprites[SPRITE_EMBLEM_DAN].image, &sprites[SPRITE_EMBLEM_DAN].params, NULL);
		break;
	}
}

void get_result(RESULT_T* Result) {

	Result->perfect = TotalPerfectCount;
	Result->nice = TotalNiceCount;
	Result->bad = TotalBadCount;
	Result->roll = TotalRollCount;
	Result->combo = MaxComboCount;
	Result->score = CurrentScore;
}
