#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "time.h"
#include "notes.h"
#include "tja.h"
#include "audio.h"
#include "playback.h"
#include "score.h"
#include "select.h"
#include "option.h"
#include "skin.h"
#include "result.h"
#include "main.h"
#include "vorbis.h"

extern int course, courselife, TotalBadCount; //combo;
extern float NowBPM;
extern bool isGOGO;
C2D_Sprite sprites[SPRITES_NUMER];			//画像用
static C2D_SpriteSheet spriteSheet;
C2D_TextBuf g_dynamicBuf;
C2D_Text dynText;
bool isPause = false, isNotesStart = false, isMusicStart = false, isPlayMain = false, isExit = false;
char buffer[BUFFER_SIZE];
int dn_x,dn_y,dg_x,dg_y;

static void load_sprites();
static int time_count(double TIME), exist_file(const char* path);

void draw_debug(float x, float y, const char *text) {

	//使用例
	//snprintf(get_buffer(), BUFFER_SIZE, "%d", 10);
	// draw_debug(300, 0, get_buffer());

	C2D_TextBufClear(g_dynamicBuf);
	C2D_TextParse(&dynText, g_dynamicBuf, text);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, x, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(0.0f, 1.0f, 0.0f, 1.0f));
}

void init_main() {

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	g_dynamicBuf = C2D_TextBufNew(4096);
	gfxSetWide(false);
	osSetSpeedupEnable(true);
	gfxSetDoubleBuffering(GFX_TOP, true);
}

void exit_main() {

	C2D_TextBufDelete(g_dynamicBuf);

	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	exit_music();
	exit_option();
	exit_skin();
}

void button_game(bool *isDon,bool *isKatsu,OPTION_T Option, unsigned int key) {

	int numKeys = 20;

	int Optionkeys[] = {
		Option.KEY_A,Option.KEY_B,Option.KEY_X,Option.KEY_Y,Option.KEY_R,Option.KEY_ZR,Option.KEY_L,Option.KEY_ZL,Option.KEY_DUP,Option.KEY_DDOWN,Option.KEY_DRIGHT,Option.KEY_DLEFT,
		Option.KEY_CPAD_UP,Option.KEY_CPAD_DOWN,Option.KEY_CPAD_RIGHT,Option.KEY_CPAD_LEFT,Option.KEY_CSTICK_UP,Option.KEY_CSTICK_DOWN,Option.KEY_CSTICK_RIGHT,Option.KEY_CSTICK_LEFT };
	unsigned int keys[] = {
		KEY_A,KEY_B,KEY_X,KEY_Y,KEY_R,KEY_ZR,KEY_L,KEY_ZL,KEY_DUP,KEY_DDOWN,KEY_DRIGHT,KEY_DLEFT,
		KEY_CPAD_UP,KEY_CPAD_DOWN,KEY_CPAD_RIGHT,KEY_CPAD_LEFT,KEY_CSTICK_UP,KEY_CSTICK_DOWN,KEY_CSTICK_RIGHT,KEY_CSTICK_LEFT};

	for (int i = 0; i < numKeys;++i) {
		if (key & keys[i]) {
			if (Optionkeys[i] == KEY_DON) *isDon = true;
			else if (Optionkeys[i] == KEY_KATSU) *isKatsu = true;
		}
	}
}

bool check_dsp1() { //DSP1を起動しているか確認

	FILE* fp = fopen(PATH_DSP1, "r");

	if (fp == NULL) return false;
	fclose(fp);

	return true;
}

int touch_x, touch_y, touch_cnt,PreTouch_x,PreTouch_y;	//タッチ用

int main() {

	init_main();

	touchPosition tp;	//下画面タッチした座標

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	TJA_HEADER_T TJA_Header;
	LIST_T SelectedSong;
	OPTION_T Option;
	SKIN_T Skin;

	int cnt = 0, notes_cnt = 0, scene_state = SCENE_SELECTLOAD,warning=-1, course = COURSE_ONI, tmp=0; //BeforeCombo = 0;
	double FirstMeasureTime = INT_MAX, offset = 0, CurrentTimeMain = -1000;

	while (aptMainLoop()) {

		hidScanInput();
		hidTouchRead(&tp);
		unsigned int key = hidKeysDown();

		if (isExit == true) break;

		bool isDon = false, isKatsu = false;
		get_option(&Option);

		//描画開始(値を「C3D_FRAME_SYNCDRAW」にしないとクラッシュ)
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		//上画面
		C2D_TargetClear(top, C2D_Color32(0x42, 0x42, 0x42, 0xFF));
		C2D_SceneBegin(top);

		switch (scene_state) {

		case SCENE_SELECTLOAD:	//ロード画面

			load_option();
			get_option(&Option);
			load_skin();
			get_skin(&Skin);
			dn_x = Skin.don_x, dn_y = Skin.don_y, dg_x = Skin.don_gogo_x, dg_y = Skin.don_gogo_y;
			if (Option.exse == false) load_sound();
			else if (Option.exse == true) sd_load_sound();
			snprintf(get_buffer(), BUFFER_SIZE, "TJAPlayer for 3DS v%s", VERSION);
			load_sprites();
			draw_select_text(120, 70, get_buffer());
			draw_select_text(120, 100, "Now Loading...");
			C3D_FrameEnd(0);
			load_file_main();
			if (check_dsp1() == true) scene_state = SCENE_SELECTSONG;
			else { 
				warning = WARNING_DSP1;
				scene_state = SCENE_WARNING; 
			}
			//load_combo();
			break;

		case SCENE_WARNING:		//警告画面

			//下画面
			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));
			C3D_FrameDrawOn(bottom);
			C2D_SceneTarget(bottom);

			switch (warning) {
			case WARNING_DSP1:
				tmp = message_window(tp, key, TEXT_WARNING_DSP1);
				break;

			case WARNING_WAVE_NO_EXIST:
				tmp = message_window(tp, key, TEXT_WARNING_WAVE_NO_EXIST);
				break;

			case WARNING_WAVE_NOT_OGG:
				tmp = message_window(tp, key, TEXT_WARNING_WAVE_NOT_OGG);
				break;
			}
			if (tmp == 1 || key & KEY_A) {
				scene_state = SCENE_SELECTSONG;
				warning = -1;
			}
			break;

		case SCENE_SELECTSONG:	//選曲

			if (cnt == 0) {
				select_ini();
			}

			disp_file_list();
			get_SelectedId(&SelectedSong, &course);

			//下画面
			C2D_TargetClear(bottom, C2D_Color32(0x42, 0x42, 0x42, 0xFF));
			C2D_SceneBegin(bottom);
			draw_option(tp.px, tp.py, key, sprites);

			if (key & KEY_UP)		update_cursor(KEY_UP);
			if (key & KEY_DOWN)		update_cursor(KEY_DOWN);
			if (key & KEY_RIGHT)		update_cursor(KEY_RIGHT);
			if (key & KEY_LEFT)		update_cursor(KEY_LEFT);
			if (key & KEY_A)		update_cursor(KEY_A);
			if (key & KEY_B)		update_cursor(KEY_B);
			if (key & KEY_X)		update_cursor(KEY_X);

			if (get_isGameStart() == true) {
				scene_state = SCENE_MAINLOAD;
				cnt = -1;
			}
			isPause = false;
			if (key & KEY_START) isExit = true;
			break;

		case SCENE_MAINLOAD:	 //ロード中

			init_tja();
			load_tja_head(course, SelectedSong);
			//init_main_music();
			get_tja_header(&TJA_Header);
			init_score();
			init_notes(TJA_Header);
			if (SelectedSong.course_exist[course] == false) load_tja_notes(-1, SelectedSong);
			else load_tja_notes(course, SelectedSong);
			time_ini();
			offset = TJA_Header.offset + Option.offset;
			notes_cnt = 0;
			isNotesStart = false, isMusicStart = false, isPlayMain = false;
			FirstMeasureTime = INT_MAX;
			CurrentTimeMain = -1000;
			//BeforeCombo = -1;

			tmp = check_wave(SelectedSong);
			if (tmp == -1) scene_state = SCENE_MAINGAME;
			else {

				warning = tmp;
				scene_state = SCENE_WARNING;
				select_ini();
			}
			cnt = -60;
			break;

		case SCENE_MAINGAME:		//演奏画面

			if (isPause == false) {

				if (tp.px != 0 && tp.py != 0) {

					PreTouch_x = touch_x, PreTouch_y = touch_y;
					touch_x = tp.px, touch_y = tp.py;

					if (
						(key & KEY_TOUCH || 
							pow((touch_x - PreTouch_x)*(touch_x - PreTouch_x) + (touch_y - PreTouch_y)*(touch_y - PreTouch_y), 0.5) > 20.0
						) &&
						(tp.px - 160)*(tp.px - 160) + (tp.py - 135)*(tp.py - 135) <= 105 * 105 &&
						touch_cnt < 2) {
						isDon = true;
						++touch_cnt;
					}
					else if (
						(
						key & KEY_TOUCH ||
						pow((touch_x - PreTouch_x)*(touch_x - PreTouch_x) + (touch_y - PreTouch_y)*(touch_y - PreTouch_y), 0.5) > 20.0 
							)&&
						touch_cnt < 2) {
						isKatsu = true;
						++touch_cnt;
					}
				}
				else {
					touch_x = 0, touch_y = 0, touch_cnt = 0, PreTouch_x = 0, PreTouch_y = 0;
				}

				button_game(&isDon, &isKatsu, Option, key);
			}

			C2D_DrawSprite(&sprites[SPRITE_TOP_2]);

			C2D_DrawSprite(&sprites[SPRITE_DONCHAN_0 + time_count(CurrentTimeMain)]);

			C2D_DrawSprite(&sprites[SPRITE_TOP_3]);
			C2D_DrawSprite(&sprites[SPRITE_TOP]);
			draw_lane(sprites);
			draw_gauge(sprites);
			draw_emblem(sprites);

			if (isNotesStart == true) {
				tja_to_notes(isDon, isKatsu, notes_cnt, sprites);
				if (isPause == false) ++notes_cnt;
			}
			draw_score(sprites);
			draw_title();
			if (course == COURSE_DAN) draw_condition();
			if (Option.dispFps == true) draw_fps();

			//下画面
			C2D_TargetClear(bottom, C2D_Color32(0xFF, 0xE7, 0x8C, 0xFF));
			C2D_SceneBegin(bottom);
			C2D_DrawSprite(&sprites[SPRITE_BOTTOM]);

			if (isPause == true) {
				tmp = pause_window(tp, key);

				switch (tmp) {
				case 1:
					isPlayMain = true;
					stopPlayback();
					scene_state = SCENE_MAINLOAD;
					break;

				case 2:
					isPlayMain = true;
					stopPlayback();
					cnt = -1;
					scene_state = SCENE_SELECTSONG;
					break;
				}

				if (tmp > -1) {
					togglePlayback();
					toggle_time(0);
					toggle_time(1);
					isPause = !isPause;
					play_sound(SOUND_DON);
				}
				if (key & KEY_DUP) toggle_auto();
			}

			if (cnt == 0) {
				FirstMeasureTime = get_FirstMeasureTime();
				play_main_music(&isPlayMain, SelectedSong);
			}
			if (cnt >= 0) CurrentTimeMain = get_current_time(TIME_MAINGAME);

			if (isDon == true)   play_sound(SOUND_DON);		//ドン
			if (isKatsu == true) play_sound(SOUND_KATSU);		//カツ

			if (key & KEY_SELECT || (key & KEY_START)) {
				togglePlayback();
				toggle_time(0);
				toggle_time(1);
				isPause = !isPause;
			}

			//譜面が先
			if (offset > 0 && (isNotesStart == false || isMusicStart == false)) {

				if (CurrentTimeMain >= 0 && isNotesStart == false) isNotesStart = true;
				if (CurrentTimeMain >= offset + FirstMeasureTime && isMusicStart == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
			}

			//音楽が先
			else if (offset <= 0 && (isNotesStart == false || isMusicStart == false)) {

				if (CurrentTimeMain >= FirstMeasureTime && isPlayMain == false) {
					isPlayMain = true;
					isMusicStart = true;
				}
				if (CurrentTimeMain >= (-1.0) * offset && isNotesStart == false) {
					isNotesStart = true;
				}
			}

			if (TotalBadCount > 0) {
				switch (Option.special) {
				case 1:
					scene_state = SCENE_RESULT;
					cnt = -1;
					break;

				case 2:
					isPlayMain = true;
					stopPlayback();
					scene_state = SCENE_MAINLOAD;
					break;
				}
			}
			if ((get_notes_finish() == true && ndspChnIsPlaying(CHANNEL) == false) || (courselife == 0 && course == COURSE_TOWER)) {
				scene_state = SCENE_RESULT;
				cnt = -1;
			}

			//コンボボイス
			/*if ((int)(combo/100) != BeforeCombo && combo < (get_isauto() ? 1600 : 5100) && combo >= 50) {
				play_sound(combo/100+(get_isauto() ? 55 : 4));
				BeforeCombo = combo/100;
			}
			if (combo < 50) {
				BeforeCombo = -1;
			}*/
			break;

		case SCENE_RESULT:

			stopPlayback();
			draw_gauge_result(sprites);
			draw_result();
			if (key & KEY_START) {
				cnt = -1;
				scene_state = SCENE_SELECTSONG;
			}
			break;
		}

		//描画終了
		C3D_FrameEnd(0);
		if (isPause == false) ++cnt;
	}
	exit_main();
	return 0;
}

inline static void load_sprites() {

	if (exist_file("sdmc:/tjafiles/theme/default.t3x")) {
		spriteSheet = C2D_SpriteSheetLoad("sdmc:/tjafiles/theme/default.t3x");
	}
	else {
		spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	}
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	for (int i = 0; i < SPRITES_NUMER; ++i) {
		C2D_SpriteFromSheet(&sprites[i], spriteSheet, i);
		C2D_SpriteSetCenter(&sprites[i], 0.5f, 0.5f);
	}
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON], 13, 13);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_1], 9, 12);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_2], 9, 26);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_3], 9, 31);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_4], 9, 45);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_5], 9, 51);
	C2D_SpriteSetCenterRaw(&sprites[SPRITE_BALLOON_6], 9, 59);
	for (int i = 0; i < 4; ++i) C2D_SpriteSetPos(&sprites[SPRITE_EFFECT_PERFECT + i], 93, 109);

	C2D_SpriteSetPos(&sprites[SPRITE_EFFECT_GOGO], 110, 92);

	C2D_SpriteSetPos(&sprites[SPRITE_TOP], TOP_WIDTH * 0.5, TOP_HEIGHT * 0.5);
	C2D_SpriteSetPos(&sprites[SPRITE_TOP_2], TOP_WIDTH * 0.5, 43);
	C2D_SpriteSetPos(&sprites[SPRITE_TOP_3], TOP_WIDTH * 0.5, 200);
	C2D_SpriteSetPos(&sprites[SPRITE_BOTTOM], BOTTOM_WIDTH * 0.5, BOTTOM_HEIGHT * 0.5);
	C2D_SpriteSetPos(&sprites[SPRITE_DONCHAN_0], dn_x, dn_y);
	C2D_SpriteSetPos(&sprites[SPRITE_DONCHAN_1], dn_x, dn_y);
	C2D_SpriteSetPos(&sprites[SPRITE_DONCHAN_2], dg_x, dg_y);
	C2D_SpriteSetPos(&sprites[SPRITE_DONCHAN_3], dg_x, dg_y);
	for (int i = 0; i < 7; ++i)C2D_SpriteSetPos(&sprites[SPRITE_EMBLEM_EASY + i], 31, 113);
}

bool get_isPause() {
	return isPause;
}
bool get_isMusicStart() {
	return isMusicStart;
}
char *get_buffer() {
	return buffer;
}

int powi(int x, int y) {	//なぜかpowのキャストが上手くいかないので整数用powを自作

	int ans = 1;

	for (int i = 0; i < y; ++i) {
		ans = ans * x;
	}
	return ans;
}

C2D_TextBuf g_MainText = C2D_TextBufNew(4096);
C2D_Text MainText;

void draw_window_text(float x, float y, const char* text, float* width, float* height,float size = 1.0) noexcept {

	C2D_TextBufClear(g_MainText);
	C2D_TextParse(&MainText, g_MainText, text);
	C2D_TextOptimize(&MainText);

	C2D_TextGetDimensions(&MainText, size, size, width, height);
	C2D_DrawText(&MainText, C2D_WithColor, BOTTOM_WIDTH / 2 - *width / 2, y, 1.0f, size, size, C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

inline int pause_window(touchPosition tp, unsigned int key) noexcept {

	int margin = 20, result = -1, x, y;
	float width, height;

	C2D_DrawRectSolid(margin, margin, 0, BOTTOM_WIDTH - margin * 2, BOTTOM_HEIGHT - margin * 2, C2D_Color32f(0, 0, 0, 1));

	draw_window_text(-1, margin + 30, Text[get_lang()][TEXT_CONTINUE], &width, &height);		//続ける
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 30;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 0;

	draw_window_text(-1, margin + 80, Text[get_lang()][TEXT_STARTOVER], &width, &height);		//はじめから
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 80;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 1;

	draw_window_text(-1, margin + 130, Text[get_lang()][TEXT_RETURNSELECT], &width, &height);	//曲選択に戻る
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 130;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 2;

	return result;
}

inline int message_window(touchPosition tp, unsigned int key,int text) {

	int margin = 20,result = -1, x, y;
	float width, height;

	C2D_DrawRectSolid(margin, margin, 0, BOTTOM_WIDTH - margin * 2, BOTTOM_HEIGHT - margin * 2, C2D_Color32f(0, 0, 0, 1));
	draw_window_text(-1, margin + 50, Text[get_lang()][text], &width, &height,0.5);

	draw_window_text(-1, margin + 150, "OK", &width, &height);
	x = BOTTOM_WIDTH / 2 - width / 2, y = margin + 150;
	if ((y < tp.py && y + height > tp.py && x < tp.px && x + width > tp.px) && key & KEY_TOUCH) result = 1;

	return result;
}

static int exist_file(const char* path) {

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }
    fclose(fp);
    return 1;
}
inline static int time_count(double TIME) noexcept {
	if ((int)floor(TIME/(60.0/NowBPM)) % 2 == 1) return 1+(isGOGO*2);
	else return 0+(isGOGO*2);
}
