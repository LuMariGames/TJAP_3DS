#include "header.h"
#include "notes.h"
#include "tja.h"
#include "score.h"
#include "main.h"
#include "select.h"
#include "option.h"
#include "conv.h"
#include <stdio.h>

char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX], *exam[4][4];
int tja_cnt = 0, MeasureMaxNumber = 0, stme, edme = 0, gaugelife;
uint64_t Cdn[2][4];
double MainFirstMeasureTime;	//最初に"到達"する小節の到達所要時間　最初に"生成"はMeasure[0]で取得;
bool isBranch = false;
float mix[12];

TJA_HEADER_T Current_Header;
MEASURE_T Measure[MEASURE_MAX];

void get_command_value(char* buf, COMMAND_T *Command), sort_measure_insertion(MEASURE_T t[], int array_size);
double calc_first_measure_time();

void init_measure_structure() {

	for (int i = 0; i < MEASURE_MAX; ++i) {

		Measure[i].create_time = INT_MAX;
		Measure[i].judge_time = INT_MAX;
		Measure[i].pop_time = INT_MAX;
		Measure[i].bpm = 0;
		Measure[i].scroll = 0;
		Measure[i].notes = 0;
		Measure[i].flag = false;
		Measure[i].isDispBarLine = true;
		Measure[i].firstmeasure = -1;
		Measure[i].start_measure_count = 0;
		Measure[i].max_notes = 0;
		Measure[i].original_id = -1;
		Measure[i].notes_count = 0;
		Measure[i].command = -1;
		//Measure[i].sudn_time = 0;
		Measure[i].lyric = "";
	}
}

void init_tja() {

	tja_cnt = 0;
	MeasureMaxNumber = 0;
	MainFirstMeasureTime = 0;
	init_measure_structure();
	isBranch = false;
}

bool load_tja_head(int course,LIST_T Song) {

	FILE *fp;
	char buf[512];
	bool isCourseMatch = true,isSTART = false;
	OPTION_T Option;
	get_option(&Option);
	memset(mix, 0, sizeof(mix));

	Current_Header.title = (char*)"No title";
	Current_Header.subtitle = (char*)"";
	Current_Header.subtitle_state = -1;
	Current_Header.level = 0;
	Current_Header.bpm = 60.0;
	Current_Header.wave = (char*)"audio.ogg";
	Current_Header.offset = 0;
	Current_Header.balloon[0][0] = 5;
	Current_Header.songvol = 80;
	Current_Header.sevol = 80;
	Current_Header.scoreinit = -1;
	Current_Header.scorediff = -1;
	Current_Header.course = course;
	Current_Header.style = 1;
	Current_Header.life = -1;
	Current_Header.bg = (char*)"bg.t3x";
	Current_Header.bgfps = 0;
	//Current_Header.Bgoffst
	//Current_Header.demostart = 0;
	//Current_Header.side = 3;
	Current_Header.scoremode = 3;
	exam[0][0] = (char*)"";
	exam[0][1] = (char*)"";
	exam[0][2] = (char*)"";
	exam[0][3] = (char*)"";
	exam[1][0] = (char*)"";
	exam[1][1] = (char*)"";
	exam[1][2] = (char*)"";
	exam[1][3] = (char*)"";
	exam[2][0] = (char*)"";
	exam[2][1] = (char*)"";
	exam[2][2] = (char*)"";
	exam[2][3] = (char*)"";
	exam[3][0] = (char*)"";
	exam[3][1] = (char*)"";
	exam[3][2] = (char*)"";
	exam[3][3] = (char*)"";

	int cnt = -1;
	char abs_path[512];

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.tja);
	if ((fp = fopen(abs_path, "r")) != NULL) {

		char* temp = NULL;
		while (fgets(buf, 512, fp) != NULL) {

			++cnt;
			temp = (char *)malloc((strlen(buf) + 1));
			mix[0] = Current_Header.songvol / 100.0;
			mix[1] = Current_Header.songvol / 100.0;
			if (isCourseMatch && Current_Header.style == 1 && strstr(buf, "#START") == buf) {
				isSTART = true;
				free(temp);
				break;
			}
			else if (isCourseMatch && Option.player == 1 && Current_Header.style == 2 && strstr(buf, "#START P1") == buf) {
				isSTART = true;
				free(temp);
				break;
			}
			else if (isCourseMatch && Option.player == 2 && Current_Header.style == 2 && strstr(buf, "#START P2") == buf) {
				isSTART = true;
				free(temp);
				break;
			}

			if (strstr(buf, "TITLE:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.title = temp;
				}
				continue;
			}
			if (cnt == 0) {
				if (strstr(buf, "TITLE:") == buf + 3 && strstr(buf, "SUBTITLE:") == 0) {
					if (buf[9] != '\n' && buf[9] != '\r') {
						strlcpy(temp, buf + 9, strlen(buf) - 10);
						Current_Header.title = temp;
					}
					continue;
				}
			}

			if (strstr(buf, "SUBTITLE:") == buf) {
				if (buf[9] != '\n' && buf[9] != '\r') {

					strlcpy(temp, buf + 9, strlen(buf) - 10);

					if (strstr(temp, "--") == temp) Current_Header.subtitle_state = 1;
					else if (strstr(temp, "++") == temp) Current_Header.subtitle_state = 2;
					else Current_Header.subtitle_state = 0;

					if (Current_Header.subtitle_state != 0) {
						strlcpy(temp, buf + 9 + 2, strlen(buf) - 10 - 2);
					}
					Current_Header.subtitle = temp;
				}
				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) { //モード毎に難易度を決めるタグ
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.level = atoi(temp);
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BPM:") == buf) { //最初のBPMを決めるタグ
				if (buf[4] != '\n' && buf[4] != '\r') {
					strlcpy(temp, buf + 4, strlen(buf) - 5);
					Current_Header.bpm = atof(temp);
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "WAVE:") == buf) { //音源のファイル名を記述するタグ、このタグが無いと基本的に動かない
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.wave = temp;
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BGIMG:") == buf) { //上画面の下背景に追加する画像ファイルを記述するタグ、対応拡張子は.t3xである。
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.bg = temp;
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BGFPS:") == buf) { //モード毎に難易度を決めるタグ
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.bgfps = atof(temp);
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "OFFSET:") == buf) { //楽曲と譜面のタイミングを決めるタグ
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					Current_Header.offset = atof(temp);
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BALLOON:") == buf) {  //分岐しなかった際に使われる風船の打数
				if (buf[8] != '\n' && buf[8] != '\r') {
					strlcpy(temp, buf + 8, strlen(buf) - 9);
					char *tp = strtok(temp, ",");
					Current_Header.balloon[0][0] = atoi(tp);
					int cnt = 1;
					while ((tp = strtok(NULL, ","))) {
						Current_Header.balloon[0][cnt] = atoi(tp);
						++cnt;
					}
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BALLOONNOR:") == buf) { //「普通譜面」に分岐した際に使われる風船の打数
				if (buf[11] != '\n' && buf[11] != '\r') {
					strlcpy(temp, buf + 11, strlen(buf) - 12);
					char *tp = strtok(temp, ",");
					Current_Header.balloon[1][0] = atoi(tp);
					int cnt = 1;
					while ((tp = strtok(NULL, ","))) {
						Current_Header.balloon[1][cnt] = atoi(tp);
						++cnt;
					}
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BALLOONEXP:") == buf) { //「玄人譜面」に分岐した際に使われる風船の打数
				if (buf[11] != '\n' && buf[11] != '\r') {
					strlcpy(temp, buf + 11, strlen(buf) - 12);
					char *tp = strtok(temp, ",");
					Current_Header.balloon[2][0] = atoi(tp);
					int cnt = 1;
					while ((tp = strtok(NULL, ","))) {
						Current_Header.balloon[2][cnt] = atoi(tp);
						++cnt;
					}
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "BALLOONMAS:") == buf) { //「達人譜面」に分岐した際に使われる風船の打数
				if (buf[11] != '\n' && buf[11] != '\r') {
					strlcpy(temp, buf + 11, strlen(buf) - 12);
					char *tp = strtok(temp, ",");
					Current_Header.balloon[3][0] = atoi(tp);
					int cnt = 1;
					while ((tp = strtok(NULL, ","))) {
						Current_Header.balloon[3][cnt] = atoi(tp);
						++cnt;
					}
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "SONGVOL:") == buf) { //楽曲の音量を調節するタグ
				if (buf[8] != '\n' && buf[8] != '\r') {
					strlcpy(temp, buf + 8, strlen(buf) - 9);
					Current_Header.songvol = atoi(temp) * 0.8;
				}
				free(temp);
				continue;
			}

			/*if (strstr(buf, "SEVOL:") == buf) { //SEの音量調整は今後実装する予定
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					Current_Header.sevol = atoi(temp) * 0.8;
				}
				continue;
			}*/

			if (strstr(buf, "SCOREINIT:") == buf) { //スコアの加算に使うタグ、　最初に加算する点数を決める。
				if (buf[10] != '\n' && buf[10] != '\r') { //このタグが無い場合は自動で計算される
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scoreinit = atoi(temp);
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "SCOREDIFF:") == buf) { //スコアの加算に使うタグ、　最初に加算する点数に加える値を決める。
				if (buf[10] != '\n' && buf[10] != '\r') { //このタグが無い場合も自動で計算されるが、一部例外的に計算しない場合がある
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scorediff = atoi(temp);
				}
				free(temp);
				continue;
			}


			if (strstr(buf, "COURSE:") == buf) { //譜面モードを切り替えるタグ、「かんたん」から「おに裏」に加え「太鼓タワー」と「段位道場」に変えれる
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					if (strlen(temp) == 1) course = atoi(temp);		//数字表記
					else if (strcmp(temp, "Easy") == 0 || strcmp(temp, "easy") == 0)   course = COURSE_EASY;	//文字表記
					else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) course = COURSE_NORMAL;
					else if (strcmp(temp, "Hard") == 0 || strcmp(temp, "hard") == 0)   course = COURSE_HARD;
					else if (strcmp(temp, "Oni") == 0 || strcmp(temp, "oni") == 0)    course = COURSE_ONI;
					else if (strcmp(temp, "Edit") == 0 || strcmp(temp, "edit") == 0)   course = COURSE_EDIT;
					else if (strcmp(temp, "Tower") == 0 || strcmp(temp, "tower") == 0)   course = COURSE_TOWER;
					else if (strcmp(temp, "Dan") == 0 || strcmp(temp, "dan") == 0)   course = COURSE_DAN;

					if (Current_Header.course == course) {
						isCourseMatch = true;
					}
					else isCourseMatch = false;
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "EXAM1:") == buf) { //「段位道場」のみで使えるタグ、１つ目の条件を設定する
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					char *a = strtok(temp, ",");
					exam[0][0] = a;
					cnt = 1;
					while ((a = strtok(NULL, ","))) {
						exam[0][cnt] = a;
						if (cnt == 1) Cdn[0][0] = atoi(a);
						else if (cnt == 2) Cdn[1][0] = atoi(a);
						++cnt;
					}
				}
				continue;
			}

			if (strstr(buf, "EXAM2:") == buf) { //「段位道場」のみで使えるタグ、２つ目の条件を設定する
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					char *b = strtok(temp, ",");
					exam[1][0] = b;
					cnt = 1;
					while ((b = strtok(NULL, ","))) {
						exam[1][cnt] = b;
						if (cnt == 1) Cdn[0][1] = atoi(b);
						else if (cnt == 2) Cdn[1][1] = atoi(b);
						++cnt;
					}
				}
				continue;
			}

			if (strstr(buf, "EXAM3:") == buf) { //「段位道場」のみで使えるタグ、３つ目の条件を設定する
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					char *c = strtok(temp, ",");
					exam[2][0] = c;
					cnt = 1;
					while ((c = strtok(NULL, ","))) {
						exam[2][cnt] = c;
						if (cnt == 1) Cdn[0][2] = atoi(c);
						else if (cnt == 2) Cdn[1][2] = atoi(c);
						++cnt;
					}
				}
				continue;
			}

			if (strstr(buf, "EXAM4:") == buf) { //「段位道場」のみで使えるタグ、４つ目の条件を設定する
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					char *d = strtok(temp, ",");
					exam[3][0] = d;
					cnt = 1;
					while ((d = strtok(NULL, ","))) {
						exam[3][cnt] = d;
						if (cnt == 1) Cdn[0][3] = atoi(d);
						else if (cnt == 2) Cdn[1][3] = atoi(d);
						++cnt;
					}
				}
				continue;
			}

			if (strstr(buf, "STYLE:") == buf) { //双打譜面であるかの分岐タグ
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					if (strlen(temp) == 1) Current_Header.style = atoi(temp);		//数字表記
					else if (strcmp(temp, "Single") == 0 || strcmp(temp, "single") == 0) Current_Header.style = 1;	//文字表記
					else if (strcmp(temp, "Double") == 0 || strcmp(temp, "double") == 0) Current_Header.style = 2;
				}
				free(temp);
				continue;
			}

			if (strstr(buf, "LIFE:") == buf) { //「太鼓タワー」のみ使えるタグ、体力ゲージの役割を成している
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.life = atoi(temp);
					gaugelife = Current_Header.life;
				}
				free(temp);
				continue;
			}

			/*if (strstr(buf, "DEMOSTART:") == buf) { //選曲画面に使うタグ、今は使わない
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.demostart = atof(temp);
				}
				continue;
			}

			if (strstr(buf, "SIDE:") == buf) { //PCソフト「太鼓さん次郎」系統の専用タグ、ここでは使わない
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(temp, buf + 5, strlen(buf) - 6);
					Current_Header.side = atoi(temp);
				}
				continue;
			}*/

			if (strstr(buf, "SCOREMODE:") == buf) { //スコアの計算方法を変えるタグ、デフォルトではアーケード版に似た配点方法
				if (buf[10] != '\n' && buf[10] != '\r') {
					strlcpy(temp, buf + 10, strlen(buf) - 11);
					Current_Header.scoremode = atoi(temp);
				}
				free(temp);
				continue;
			}
			free(temp);
		}
		fclose(fp);
		return isSTART;
	}
	else {
		return false;	//tjaファイルが開けなかった時
	}
	return false;
}

void load_tja_head_simple(LIST_T *List) {		//選曲用のヘッダ取得


	snprintf(List->title, sizeof(List->title), "No Title");
	snprintf(List->wave, sizeof(List->wave), "audio.ogg");
	for (int i = 0; i < 5; ++i) {
		List->level[i] = 0;
		List->course[i] = false;
	}

	FILE *fp;
	char buf[128],*temp = NULL;
	int course = COURSE_ONI,cnt = 0;
	char abs_path[512];

	snprintf(abs_path, sizeof(abs_path), "%s/%s", List->path, List->tja);
	if ((fp = fopen(abs_path, "r")) != NULL) {

		while (fgets(buf, 128, fp) != NULL) {

			temp = (char *)malloc((strlen(buf) + 1));

			if (strstr(buf, "TITLE:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(List->title, buf + 6, strlen(buf) - 7);
				}
				continue;
			}
			if (strstr(buf, "SUBTITLE:") == buf) {
				if (buf[9] != '\n' && buf[9] != '\r') {
					strlcpy(List->subtitle, buf + 9, strlen(buf) - 10);
				}
				continue;
			}

			if (strstr(buf, "WAVE:") == buf) {
				if (buf[5] != '\n' && buf[5] != '\r') {
					strlcpy(List->wave, buf + 5, strlen(buf) - 6);
				}
				continue;
			}

			if (strstr(buf, "COURSE:") == buf) {
				if (buf[7] != '\n' && buf[7] != '\r') {
					strlcpy(temp, buf + 7, strlen(buf) - 8);
					if (strlen(temp) == 1) course = atoi(temp);			//数字表記
					else if (strcmp(temp, "Easy") == 0 || strcmp(temp, "easy") == 0)   course = COURSE_EASY;	//文字表記
					else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) course = COURSE_NORMAL;
					else if (strcmp(temp, "Hard") == 0 || strcmp(temp, "hard") == 0)   course = COURSE_HARD;
					else if (strcmp(temp, "Oni") == 0 || strcmp(temp, "oni") == 0)    course = COURSE_ONI;
					else if (strcmp(temp, "Edit") == 0 || strcmp(temp, "edit") == 0)   course = COURSE_EDIT;
					else if (strcmp(temp, "Tower") == 0 || strcmp(temp, "tower") == 0)   course = COURSE_TOWER;
					else if (strcmp(temp, "Dan") == 0 || strcmp(temp, "dan") == 0)   course = COURSE_DAN;
					List->course[course] = true;
					List->course_exist[course] = true;
				}

				continue;
			}

			if (strstr(buf, "LEVEL:") == buf) {
				if (buf[6] != '\n' && buf[6] != '\r') {
					strlcpy(temp, buf + 6, strlen(buf) - 7);
					List->level[course] = atoi(temp);
					List->course[course] = true;
				}
				continue;
			}
			++cnt;
		}
	}
	free(temp);
	fclose(fp);
}

void white_tja(LIST_T Song) {

	FILE *fp;
	char abs_path[512], tja_text[16384];
	memset(tja_text, 0, sizeof(tja_text));
	int text_byte = 0;

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.tja);
	if ((fp = fopen(abs_path, "r")) != NULL) {

		tja_cnt = 0;
		memset(tja_notes, 0, sizeof(tja_notes));
		while (fgets(tja_notes[tja_cnt], NOTES_MEASURE_MAX, fp) != NULL || tja_cnt < MEASURE_MAX) {

			++tja_cnt;
		}
		for (int i = 0, j = tja_cnt; i < j; ++i) {

			strcat(tja_text, tja_notes[i]);
			text_byte += strlen(tja_notes[i]);
		}
		tja_text[text_byte + 1] = 0;
		char* src = tja_text;
		char* dst = tja_text;

		while (*src) {
			if (src[0] == '\r' && src[1] == '\n') {
				*dst++ = '\n';
				src += 2;
			} else {
				*dst++ = *src++;
			}
		}
		*src = 0;
		*dst = 0;
		fclose(fp);
		fp = fopen(abs_path, "w");
		SwkbdState swkbd;
		swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 1, 16384);
		swkbdSetInitialText(&swkbd, tja_text);
		swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT | SWKBD_MULTILINE);
		swkbdInputText(&swkbd, tja_text, sizeof(tja_text));
		dst = tja_text;
		while (*dst) {
			if (*dst == '\n') {
				fputc('\r', fp);
				fputc('\n', fp);
				dst++;
			} else {
				fputc(*dst++, fp);
			}
		}
		fclose(fp);
	}
}

void conv_tja(LIST_T Song) {

	FILE *fp;
	char abs_path[512], tja_text[16384];
	memset(tja_text, 0, sizeof(tja_text));
	int text_byte = 0;

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.tja);
	if ((fp = fopen(abs_path, "r")) != NULL) {

		tja_cnt = 0;
		memset(tja_notes, 0, sizeof(tja_notes));
		while (fgets(tja_notes[tja_cnt], NOTES_MEASURE_MAX, fp) != NULL || tja_cnt < MEASURE_MAX) {

			++tja_cnt;
		}
		for (int i = 0, j = tja_cnt; i < j; ++i) {

			strcat(tja_text, tja_notes[i]);
			text_byte += strlen(tja_notes[i]);
		}
		tja_text[text_byte + 1] = 0;
		fclose(fp);
		fp = fopen(abs_path, "w");
		char* dst = tja_text;
		while (*dst) {
			if (strncmp(sijs2u8(dst[0], dst[1]), "-10", 3) == 0) {
				fputc(*dst++, fp);
			}
			else {
				fprintf(fp, "%s", sijs2u8(dst[0], dst[1]));
				dst += 2;
			}
		}
		fclose(fp);
	}
}
void sort_measure_insertion(MEASURE_T t[], int array_size) {	//create_timeでソート

	for (int i = 1; i < array_size; ++i) {

		MEASURE_T temp = t[i];
		if (t[i - 1].create_time > temp.create_time) {

			int j = i;
			do {
				t[j] = t[j - 1];
				--j;
			} while (j > 0 && t[j - 1].create_time > temp.create_time);
			t[j] = temp;
		}
	}
}

double calc_first_measure_time() {	//最初に到達する小節の所要時間を計算

	OPTION_T Option;
	get_option(&Option);
	int tmp = -1, tmp2 = 0;
	stme = 0, edme = 0;

	for (int i = 0; i < MEASURE_MAX; ++i) {

		if (Measure[i].flag && Measure[i].command == -1) {

			if (tmp == -1) {	//初回
				tmp = i;
				continue;
			}
			if (Measure[i].judge_time < Measure[tmp].judge_time) tmp = i;
			if ((Measure[i].branch <= COMMAND_N) && (Measure[i].firstmeasure == -1 || Measure[i].firstmeasure == i)) ++tmp2;
			if (Option.measure > tmp2 && (Measure[i].branch <= COMMAND_N) && (Measure[i].firstmeasure == -1 || Measure[i].firstmeasure == i)) stme = i;
			else if (Option.measure <= 0) stme = tmp;
		}
		if (Measure[i].command == COMMAND_END) {
			edme = tmp2;
			break;
		}
	}
	return Measure[stme].judge_time - Measure[stme].create_time;
}

void load_tja_notes(int course, LIST_T Song) {

	int FirstMultiMeasure = -1,	//複数行の小節の最初の小節id 複数出ない場合は-1
		NotesCount = 0, BranchCourse = -1,
		BeforeBranchFirstMultiMeasure = -1, BeforeBranchNotesCount = 0;
	bool isStart = false,isEnd = false,isDispBarLine = true,isNoComma = false,isCourseMatch = false,isSudden = false,
		BeforeBranchIsDispBarLine = true,BeforeBranchIsNoComma = false,BeforeBranchIsDummy = false,isDummy = false;
	FILE *fp;
	COMMAND_T Command;
	OPTION_T Option;
	get_option(&Option);

	double bpm = Current_Header.bpm,NextBpm = bpm,measure = 1,scroll = 1,NextMeasure = 1,delay = 0,percent = 1,sudntime = 0, //movetime = 0,
		BeforeBranchJudgeTime = 0,BeforeBranchCreateTime = 0,BeforeBranchPopTime = 0,BeforeBranchPreJudge = 0,BeforeBranchBpm = 0, //BeforeBranchMoveTime = 0,
		BeforeBranchDelay = 0,BeforeBranchMeasure = 0,BeforeBranchScroll = 1,BeforeBranchNextBpm = 0,BeforeBranchNextMeasure = 0,BeforeBranchPercent = 1;
	std::string ly = "", Beforely = "";

	if (course == -1) isCourseMatch = true;		//コース表記なし
	char abs_path[512];

	snprintf(abs_path, sizeof(abs_path), "%s/%s", Song.path, Song.tja);
	if ((fp = fopen(abs_path, "r")) != NULL) {

		tja_cnt = 0;
		int MeasureCount = 0,CurrentCourse = -1;
		double PreJudge = 0, FirstMeasureTime = 0;

		FirstMeasureTime = (240.0 / bpm * measure)*(NOTES_JUDGE_RANGE / NOTES_AREA) - 240.0 / bpm * measure;
		PreJudge = FirstMeasureTime;

		while (
			(fgets(tja_notes[tja_cnt], NOTES_MEASURE_MAX, fp) != NULL || tja_cnt < MEASURE_MAX) &&
			isEnd == false) {

			if (strstr(tja_notes[tja_cnt], "COURSE:") == tja_notes[tja_cnt]) {

				char* temp = NULL;
				temp = (char *)malloc((strlen(tja_notes[tja_cnt]) + 1));

				strlcpy(temp, tja_notes[tja_cnt] + 7, strlen(tja_notes[tja_cnt]) - 8);
				if (strlen(temp) == 1) CurrentCourse = atoi(temp);		//数字表記
				else if (strcmp(temp, "Easy") ==   0 || strcmp(temp, "easy") == 0)   CurrentCourse = COURSE_EASY;	//文字表記
				else if (strcmp(temp, "Normal") == 0 || strcmp(temp, "normal") == 0) CurrentCourse = COURSE_NORMAL;
				else if (strcmp(temp, "Hard") ==   0 || strcmp(temp, "hard") == 0)   CurrentCourse = COURSE_HARD;
				else if (strcmp(temp, "Oni") ==    0 || strcmp(temp, "oni") == 0)    CurrentCourse = COURSE_ONI;
				else if (strcmp(temp, "Edit") ==   0 || strcmp(temp, "edit") == 0)   CurrentCourse = COURSE_EDIT;
				else if (strcmp(temp, "Tower") ==  0 || strcmp(temp, "tower") == 0)  CurrentCourse = COURSE_TOWER;
				else if (strcmp(temp, "Dan") ==    0 || strcmp(temp, "dan") == 0)    CurrentCourse = COURSE_DAN;

				free(temp);
				if (course == CurrentCourse) isCourseMatch = true;
				continue;
			}

			if (isStart == false && isCourseMatch && Option.player == 1 && strstr(tja_notes[tja_cnt], "#START P1") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}
			else if (isStart == false && isCourseMatch && Option.player == 2 && strstr(tja_notes[tja_cnt], "#START P2") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}
			else if (isStart == false && isCourseMatch && Current_Header.style == 1 && strstr(tja_notes[tja_cnt], "#START") == tja_notes[tja_cnt]) {

				isStart = true;
				continue;
			}

			if (isStart && isCourseMatch) {

				//一文字目がコメントアウトの時スキップ
				if (strstr(tja_notes[tja_cnt], "//") == tja_notes[tja_cnt] || strstr(tja_notes[tja_cnt], "\r") == tja_notes[tja_cnt]) {

					++tja_cnt;
					continue;
				}

				if (strstr(tja_notes[tja_cnt], ",") == NULL && tja_notes[tja_cnt][0] != '#') {
					isNoComma = true;

					if (FirstMultiMeasure == -1) {

						FirstMultiMeasure = MeasureCount;
						Measure[FirstMultiMeasure].original_id = FirstMultiMeasure;	//ソート前のidを格納
					}
				}
				else {
					isNoComma = false;
					Measure[MeasureCount].original_id = MeasureCount;
				}

				if (tja_notes[tja_cnt][0] == '#') {

					get_command_value(tja_notes[tja_cnt], &Command);
					Measure[MeasureCount].command = Command.knd;
					switch (Command.knd) {
					case COMMAND_BPMCHANGE:
						NextBpm = Command.val[0];
						break;
					case COMMAND_MEASURE:
						NextMeasure = Command.val[0];
						break;
					case COMMAND_SCROLL:
						scroll = Command.val[0];
						break;
					case COMMAND_DELAY:
						delay = Command.val[0];
						break;
					case COMMAND_SUDDEN:
						sudntime = Command.val[0];
						//movetime = sudntime - Command.val[1];
						isSudden = true;
						break;
					case COMMAND_LYRIC:
						ly = Command.value_s;
						break;
					case COMMAND_BARLINEON:
						isDispBarLine = true;
						break;
					case COMMAND_BARLINEOFF:
						isDispBarLine = false;
						break;
					case COMMAND_DUMMYON:
						isDummy = true;
						break;
					case COMMAND_DUMMYOFF:
						isDummy = false;
						break;
					case COMMAND_N:
						BranchCourse = COMMAND_N;
						break;
					case COMMAND_E:
						BranchCourse = COMMAND_E;
						break;
					case COMMAND_M:
						BranchCourse = COMMAND_M;
						break;
					case COMMAND_BRANCHSTART:
						BranchCourse = -1;
						isBranch = true;
						break;
					case COMMAND_BRANCHEND:
						BranchCourse = -1;
						break;
					case COMMAND_SECTION:
						Measure[MeasureCount].command = COMMAND_SECTION;
						break;
					case COMMAND_END:
						BranchCourse = -1;
						isEnd = true;
						break;
					}
				}
				else {

					if (isNoComma == true || NotesCount != 0) {	//複数小節

						Measure[MeasureCount].start_measure_count = NotesCount;
						int i = 0;
						while (tja_notes[tja_cnt][i] != '\n' && tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') ++i;
						NotesCount += i - 1;
						if (tja_notes[tja_cnt][i] == '/') ++NotesCount;
						if (tja_notes[tja_cnt][i] != ',' && tja_notes[tja_cnt][i] != '/') --i;
						Measure[MeasureCount].notes_count = i;
					}
				}

				Measure[MeasureCount].flag = true;
				Measure[MeasureCount].notes = tja_cnt;
				Measure[MeasureCount].firstmeasure = FirstMultiMeasure;
				Measure[MeasureCount].bpm = NextBpm;
				Measure[MeasureCount].measure = NextMeasure;
				Measure[MeasureCount].scroll = scroll;
				//Measure[MeasureCount].sudn_time = movetime;
				Measure[MeasureCount].isDummy = isDummy;
				Measure[MeasureCount].judge_time = 240.0 / bpm * measure * percent + PreJudge + delay;
				Measure[MeasureCount].pop_time = Measure[MeasureCount].judge_time - (240.0 * NOTES_JUDGE_RANGE) / (Measure[MeasureCount].bpm * NOTES_AREA);
				Measure[MeasureCount].create_time = Measure[MeasureCount].judge_time + (isSudden ? (240.0 / NextBpm - sudntime) : 0) - (240.0 * NOTES_JUDGE_RANGE) / (Measure[MeasureCount].bpm * (NOTES_AREA * fabs(scroll)));
				Measure[MeasureCount].isDispBarLine = isDispBarLine;
				Measure[MeasureCount].branch = BranchCourse;
				Measure[MeasureCount].lyric = ly;

				if (tja_notes[tja_cnt][0] == '#') {

					if (MeasureCount > 0) {
						Measure[MeasureCount].judge_time = Measure[MeasureCount - 1].judge_time;
						Measure[MeasureCount].create_time = Measure[MeasureCount - 1].create_time;
						//Measure[MeasureCount].isDispBarLine = false;
					}
					switch (Command.knd) {
					case COMMAND_BRANCHSTART:
						BeforeBranchJudgeTime = Measure[MeasureCount].judge_time;
						BeforeBranchCreateTime = Measure[MeasureCount].create_time;
						BeforeBranchPopTime = Measure[MeasureCount].pop_time;
						BeforeBranchBpm = bpm;
						BeforeBranchDelay = delay;
						BeforeBranchMeasure = measure;
						BeforeBranchPreJudge = PreJudge;
						BeforeBranchScroll = scroll;
						BeforeBranchNextBpm = NextBpm;
						BeforeBranchNextMeasure = NextMeasure;
						BeforeBranchIsDispBarLine = isDispBarLine;
						BeforeBranchFirstMultiMeasure = FirstMultiMeasure;
						BeforeBranchIsNoComma = isNoComma;
						BeforeBranchNotesCount = NotesCount;
						BeforeBranchPercent = percent;
						//BeforeBranchMoveTime = movetime;
						BeforeBranchIsDummy = isDummy;
						Beforely = ly;
						if (tja_cnt == 0) Measure[MeasureCount].judge_time = 0;	//ノーツの前に分岐はすぐに判定
						break;
					case COMMAND_M:
					case COMMAND_E:
					case COMMAND_N:
						bpm = BeforeBranchBpm;
						measure = BeforeBranchMeasure;
						delay = BeforeBranchDelay;
						scroll = BeforeBranchScroll;
						NextBpm = BeforeBranchNextBpm;
						NextMeasure = BeforeBranchNextMeasure;
						Measure[MeasureCount].judge_time = BeforeBranchJudgeTime;
						Measure[MeasureCount].create_time = BeforeBranchCreateTime;
						Measure[MeasureCount].pop_time = BeforeBranchPopTime;
						PreJudge = BeforeBranchPreJudge;
						isDispBarLine = BeforeBranchIsDispBarLine;
						FirstMultiMeasure = BeforeBranchFirstMultiMeasure;
						isNoComma = BeforeBranchIsNoComma;
						NotesCount = BeforeBranchNotesCount;
						percent = BeforeBranchPercent;
						//movetime = BeforeBranchMoveTime;
						isDummy = BeforeBranchIsDummy;
						ly = Beforely;
						break;
					}
				}
				else {
					if (!isNoComma) PreJudge = Measure[MeasureCount].judge_time;
					bpm = NextBpm;
					measure = NextMeasure;
					delay = 0;
				}

				if (isNoComma == false && NotesCount != 0 && tja_notes[tja_cnt][0] != '#') {	//複数行小節の最後の行

					Measure[Measure[MeasureCount].firstmeasure].max_notes = NotesCount + 1;
					FirstMultiMeasure = -1;
					NotesCount = 0;

					for (int i = 1, j = MeasureCount - Measure[MeasureCount].firstmeasure + 1; i < j; ++i) {	//judge_timeの調整
						
						if (tja_notes[Measure[MeasureCount].notes][0] != '#') {	//複数行小節の最初の小節以外

							Measure[Measure[MeasureCount].firstmeasure + i].judge_time =
								Measure[Measure[MeasureCount].firstmeasure + i - 1].judge_time +
								(240.0 / Measure[Measure[MeasureCount].firstmeasure + i - 1].bpm * Measure[Measure[MeasureCount].firstmeasure + i - 1].measure)
								* Measure[Measure[MeasureCount].firstmeasure + i - 1].notes_count / Measure[Measure[MeasureCount].firstmeasure].max_notes;	//delayはとりあえず放置

							Measure[Measure[MeasureCount].firstmeasure + i].pop_time    = Measure[Measure[MeasureCount].firstmeasure + i].judge_time - (240.0 * NOTES_JUDGE_RANGE) / (Measure[Measure[MeasureCount].firstmeasure + i].bpm * NOTES_AREA);
							Measure[Measure[MeasureCount].firstmeasure + i].create_time = Measure[Measure[MeasureCount].firstmeasure + i].judge_time + (isSudden ? (240.0 / NextBpm - sudntime) : 0) - (240.0 * NOTES_JUDGE_RANGE) / (Measure[Measure[MeasureCount].firstmeasure + i].bpm * (NOTES_AREA * fabs(Measure[Measure[MeasureCount].firstmeasure + i].scroll)));
							percent = (double)Measure[Measure[MeasureCount].firstmeasure + i].notes_count / (double)Measure[Measure[MeasureCount].firstmeasure].max_notes;

							Measure[Measure[MeasureCount].firstmeasure + i].isDispBarLine = false;	//最初の小節は小節線をオフにしない
						}
					}
					PreJudge = Measure[MeasureCount].judge_time;
				}
				else if (tja_notes[tja_cnt][0] != '#') {
					percent = 1;
				}

				if (isEnd) {
					break;
				}
				++tja_cnt;
				++MeasureCount;
			}
		}

		MeasureMaxNumber = MeasureCount;
		for (int i = 0,n = 0,j = 0; i < MeasureMaxNumber + 1; ++i) {	//次の小節の判定時に発動する命令の調整

			switch (Measure[i].command) {
			case COMMAND_END:
			case COMMAND_JPOSSCROLL:
			case COMMAND_LEVELHOLD:
				Measure[i].create_time = Measure[i].judge_time;
				break;
			case COMMAND_SECTION:
			case COMMAND_GOGOSTART:
			case COMMAND_GOGOEND:
				n = Measure[i].notes + 1;
				while (n <= tja_cnt && tja_notes[n][0] == '#') ++n;
				while (n < tja_cnt && n != Measure[j].notes) ++j;
				Measure[i].judge_time = Measure[j].judge_time;
				break;
			}
		}

		//基本天井点を計算
		calc_base_score(Measure, tja_notes);
		fclose(fp);
		MainFirstMeasureTime = calc_first_measure_time();
		sort_measure_insertion(Measure, MEASURE_MAX);
		stme = get_MeasureId_From_OriginalId(stme);
	}
}

void get_tja_header(TJA_HEADER_T *TJA_Header) {

	*TJA_Header = Current_Header;
}

void tja_to_notes(int isDon, int isKatsu, int count, C2D_Sprite sprites[SPRITES_NUMER]) {

	notes_main(isDon, isKatsu, tja_notes, Measure, count, sprites);
}

//コマンドと値を取り出す
void get_command_value(char* buf, COMMAND_T *Command) {

	OPTION_T Option;
	get_option(&Option);
	bool isComment = false;
	int comment, space, length;

	if (buf[0] == '#') {

		length = strlen(buf);
		comment = 0;
		char* command = (char *)malloc((strlen(buf) + 1));
		char* value = (char *)malloc((strlen(buf) + 1));
		Command->notes = buf;

		if (strstr(buf, "//") != NULL) {	//コメント処理

			comment = strstr(buf, "//") - buf - 1;
			strlcpy(command, buf + 1, comment);
			isComment = true;
		}
		if (strstr(buf, " ") != NULL) {		//値処理

			space = strstr(buf, " ") - buf;
			if (space < comment && isComment) {	//値ありコメントあり

				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), comment - strlen(command) + 1);

			}
			else {	//値ありコメントなし
				strlcpy(command, buf + 1, space);
				strlcpy(value, buf + 1 + strlen(command), length - strlen(command));
			}
		}
		else {	//値なし

			//コメントあり
			if (isComment) strlcpy(command, buf + 1, comment + 1);
			//コメントなし 改行あり
			else if (strstr(buf, "\n") != NULL) strlcpy(command, buf + 1, length - 2);
			//コメントなし　改行なし
			else strlcpy(command, buf + 1, length);

			strlcpy(value, "0", 1);
		}
		Command->command_s = command;
		Command->value_s = value;
		Command->val[0] = 0;
		Command->val[1] = 0;
		Command->val[2] = 0;

		if (strcmp(command, "START") == 0) Command->knd = COMMAND_START;
		else if (strcmp(command, "END") == 0) Command->knd = COMMAND_END;
		else if (strcmp(command, "BPMCHANGE") == 0) {
			Command->knd = COMMAND_BPMCHANGE;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "GOGOSTART") == 0) Command->knd = COMMAND_GOGOSTART;
		else if (strcmp(command, "GOGOEND") == 0) Command->knd = COMMAND_GOGOEND;

		else if (strcmp(command, "MEASURE") == 0) {
			Command->knd = COMMAND_MEASURE;
			if (strstr(value, "/") != NULL) {

				int srash = strstr(value, "/") - value;
				char *denominator = (char *)malloc((strlen(buf) + 1)),
					*molecule = (char *)malloc((strlen(buf) + 1));
				strlcpy(molecule, value + 1, srash);
				strlcpy(denominator, value + srash + 1, strlen(buf) - srash);
				Command->val[0] = strtod(molecule, NULL) / strtod(denominator, NULL);
				free(denominator);
				free(molecule);
			}
			else {
				if (strtod(value, NULL) != 0) Command->val[0] = strtod(value, NULL);
				else Command->val[0] = 1.0;
			}
		}
		else if (strcmp(command, "SUDDEN") == 0) {
			Command->knd = COMMAND_SUDDEN;
			char* tp;
			tp = strtok(value, " ");
			Command->val[0] = strtod(tp, NULL);
			tp = strtok(NULL, " ");
			Command->val[1] = strtod(tp, NULL);
		}
		else if (strcmp(command, "SCROLL") == 0) {
			Command->knd = COMMAND_SCROLL;
			if (Option.fixroll) Command->val[0] = 1;
			else if (!Option.fixroll) Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "DELAY") == 0) {
			Command->knd = COMMAND_DELAY;
			Command->val[0] = strtod(value, NULL);
		}
		else if (strcmp(command, "SECTION") == 0) Command->knd = COMMAND_SECTION;
		else if (strcmp(command, "BRANCHSTART") == 0) {
			Command->knd = COMMAND_BRANCHSTART;
			char* tp;
			tp = strtok(value, ",");
			switch (value[1]) {
			case 'r':Command->val[0] = 0; break;	//連打
			case 'p':Command->val[0] = 1; break;	//精度
			case 's':Command->val[0] = 2; break;	//スコア
			case 'd':Command->val[0] = 3; break;	//大音符
			case 'c':Command->val[0] = 4; break;	//コンボ数
			default: break;
			}
			int i = 1;
			while ((tp = strtok(NULL, ","))) {
				Command->val[i] = strtod(tp, NULL);
				++i;
			}
		}
		else if (strcmp(command, "BRANCHEND") == 0) Command->knd = COMMAND_BRANCHEND;
		else if (strcmp(command, "N") == 0) Command->knd = COMMAND_N;
		else if (strcmp(command, "E") == 0) Command->knd = COMMAND_E;
		else if (strcmp(command, "M") == 0) Command->knd = COMMAND_M;
		else if (strcmp(command, "LEVELHOLD") == 0) Command->knd = COMMAND_LEVELHOLD;
		else if (strcmp(command, "BARLINEOFF") == 0) Command->knd = COMMAND_BARLINEOFF;
		else if (strcmp(command, "BARLINEON") == 0) Command->knd = COMMAND_BARLINEON;
		else if (strcmp(command, "DUMMYEND") == 0) Command->knd = COMMAND_DUMMYOFF;
		else if (strcmp(command, "DUMMYSTART") == 0) Command->knd = COMMAND_DUMMYON;
		else if (strcmp(command, "LYRIC") == 0) {
			Command->knd = COMMAND_LYRIC;
			strlcpy(value, buf + 7, length - 8);
			Command->value_s = value;
		}
		else if (strcmp(command, "JPOSSCROLL") == 0) {
			Command->knd = COMMAND_JPOSSCROLL;
			char* tp;
			tp = strtok(value, " ");
			Command->val[0] = strtod(tp, NULL);
			tp = strtok(NULL, " ");
			Command->val[1] = strtod(tp, NULL);
			tp = strtok(NULL, " ");
			Command->val[2] = strtod(tp, NULL);
			if (Command->val[2] >= 1) Command->val[2] = JPOSMAG;
			else Command->val[2] = JPOSMAG * -1.0;
		}
		/*else if (strcmp(command, "BMSCROLL") == 0) Command->knd = COMMAND_BMSCROLL;
		else if (strcmp(command, "HBSCROLL") == 0) Command->knd = COMMAND_HBSCROLL;*/
		else Command->knd = -1;

		free(command);
		free(value);
	}
	else Command->knd = -1;
}
double get_FirstMeasureTime() {
	return MainFirstMeasureTime;
}
int get_MeasureId_From_OriginalId(int id) {

	for (int i = 0; i < MEASURE_MAX; ++i) {

		if (Measure[i].original_id == id) return i;
	}
	return -1;
}
bool get_isBranch() {
	return isBranch;
}
double get_StartTime() {

	OPTION_T Option;
	get_option(&Option);
	return ((Option.measure > 0) ? ((Current_Header.offset * -1.0) + Option.offset + (Measure[stme].create_time - (240.0 / Current_Header.bpm))) : 0);
}
int get_edme() {
	return edme;
}
