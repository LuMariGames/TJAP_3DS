#include "header.h"
#include "option.h"
#include "select.h"
#include "main.h"
#include "vorbis.h"
#include <jansson.h>

OPTION_T Option;
json_t *json;
json_error_t error_json;
int option_page = 1,max_option_page = 5, comboVoice;
double black;

//数字キーボードで入力
double input_number_keyboard(int max_digits,bool isDot,bool isMinus) {	//最大桁数、少数、マイナス

	SwkbdState swkbd;
	swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, max_digits);
	//swkbdSetPasswordMode(&swkbd, SWKBD_PASSWORD_HIDE_DELAY);
	swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
	swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
	if (isDot && isMinus == false) swkbdSetNumpadKeys(&swkbd, L'.', 0);
	if (isDot == false && isMinus) swkbdSetNumpadKeys(&swkbd, L'-', 0);
	if (isDot && isMinus==true) swkbdSetNumpadKeys(&swkbd, L'.', L'-');
	swkbdInputText(&swkbd, get_buffer(), BUFFER_SIZE);
	return atof(get_buffer());
}
char* input_normal_keyboard() {

	SwkbdState swkbd;
	swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, -1);
	swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT);
	if (Option.lang == LANG_EN) {
		swkbdSetHintText(&swkbd, "Enter the song title");
		swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "close", false);
		swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "search", true);
	}
	else if (Option.lang == LANG_ES) {
		swkbdSetHintText(&swkbd, "Introduce el título de la canción");
		swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "cerca", false);
		swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "buscar", true);
	}
	else {
		swkbdSetHintText(&swkbd, "曲名を入力");
		swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "閉じる", false);
		swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "検索", true);
	}
	swkbdInputText(&swkbd, get_buffer(), BUFFER_SIZE);
	return get_buffer();
}
void init_button_mapping() {

	Option.KEY_A = KEY_KATSU;
	Option.KEY_B = KEY_DON;
	Option.KEY_Y = KEY_DON;
	Option.KEY_X = KEY_KATSU;
	Option.KEY_R = KEY_KATSU;
	Option.KEY_L = KEY_KATSU;
	Option.KEY_ZR = KEY_KATSU;
	Option.KEY_ZL = KEY_KATSU;
	Option.KEY_DRIGHT = KEY_DON;
	Option.KEY_DDOWN = KEY_DON;
	Option.KEY_DLEFT = KEY_KATSU;
	Option.KEY_DUP = KEY_KATSU;
	Option.KEY_CPAD_RIGHT = KEY_DON;
	Option.KEY_CPAD_DOWN = KEY_DON;
	Option.KEY_CPAD_LEFT = KEY_KATSU;
	Option.KEY_CPAD_UP = KEY_KATSU;
	Option.KEY_CSTICK_RIGHT = KEY_KATSU;
	Option.KEY_CSTICK_DOWN = KEY_DON;
	Option.KEY_CSTICK_LEFT = KEY_DON;
	Option.KEY_CSTICK_UP = KEY_KATSU;
}

void draw_button_mapping_icon(double x,double y,int key, C2D_Sprite sprites[SPRITES_NUMER]) {

	C2D_ImageTint Tint;
	C2D_AlphaImageTint(&Tint, 0.5);

	int sprite;
	bool isNotes = true;
	switch (key) {
	case KEY_NONE:
	default:
			isNotes = false;
			break;
	case KEY_DON:
		sprite = SPRITE_DON;
		break;
	case KEY_KATSU:
		sprite = SPRITE_KATSU;
		break;
	}

	if (isNotes) {
		C2D_SpriteSetPos(&sprites[sprite], x, y);
		C2D_DrawSpriteTinted(&sprites[sprite], &Tint);
	}
}

void switch_button_mapping(int *key) {

	switch (*key) {
	case KEY_NONE:
	default:
		*key = KEY_DON;
		break;
	case KEY_DON:
		*key = KEY_KATSU;
		break;
	case KEY_KATSU:
		*key = KEY_NONE;
		break;
	}
}

void adjust_judge_range() {

	if (Option.judge_range_perfect <= 0) Option.judge_range_perfect = DEFAULT_JUDGE_RANGE_PERFECT;
	if (Option.judge_range_nice <= 0) Option.judge_range_nice = DEFAULT_JUDGE_RANGE_NICE;
	if (Option.judge_range_bad <= 0) Option.judge_range_bad = DEFAULT_JUDGE_RANGE_BAD;
}

void calc_option_page(u16 px, u16 py, unsigned int key) {

	float width, height,x,y;

	if (option_page > 1) {
		x = 30, y = 205;
		draw_option_text(x, y, "←", true, &width, &height, 2.5, 1.5);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			if (option_page != 1) option_page--;
		}
	}
	if (option_page < max_option_page) {
		x = 230, y = 205;
		draw_option_text(x, y, "→", true, &width, &height, 2.5, 1.5);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			if (option_page < max_option_page) ++option_page;
		}
	}
	snprintf(get_buffer(), BUFFER_SIZE, "%d/%d", option_page,max_option_page);
	draw_option_text(145, 218, get_buffer(), true, &width, &height);
}

void draw_option(u16 px, u16 py, unsigned int key, C2D_Sprite sprites[SPRITES_NUMER]) {
	
	float width,height,x,y;
	int XSense=65, YSense=30,XCnt = 0,YCnt = 1,gap = 25, opv = 0, x2, y2, key_interval = 30;

	switch (option_page) {

	case 1:
		snprintf(get_buffer(), BUFFER_SIZE, "ver%s", VERSION);
		draw_option_text(320, 0, get_buffer(), true, &width, &height);
		draw_option_text(320-width, 0, get_buffer(), true, &width, &height);
		//オート
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_AUTO], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isAuto == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isAuto == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isAuto = true;
		XCnt = 0, ++YCnt;

		//はやさ
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_SPEED], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%.2f", Option.speed);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.speed = input_number_keyboard(5, true,false);
			if (Option.speed > 10.0) Option.speed = 10.0;
			else if (Option.speed < 1.0) Option.speed = 1.0;
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.speed = 1.0;
		XCnt = 0, ++YCnt;

		//ステルス
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_STELTH], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isStelth == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isStelth == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isStelth = true;
		XCnt = 0, ++YCnt;

		//あべこべ
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_SWAP], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.isSwap == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.isSwap == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.isSwap = true;
		XCnt = 0, ++YCnt;

		//ランダム
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RANDOM], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.random == 0, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_R25], Option.random == 0.25, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.25;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_R50], Option.random == 0.5, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.random = 0.5;
		XCnt = 0, ++YCnt;

		//言語
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_LANG], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_JP], Option.lang == LANG_JP, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_JP;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_EN], Option.lang == LANG_EN, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_EN;
		x = XSense * XCnt + gap+10, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ES], Option.lang == LANG_ES, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.lang = LANG_ES;
		XCnt = 0, ++YCnt;
		break;

	case 2:
		XSense = 65, YSense = 30, XCnt = 0, YCnt = 1, gap = 100;
		//バッファ
		/*x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_BUFFERSIZE], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%d", get_buffer_size());
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.buffer_size = (int)input_number_keyboard(5, false,false);
			if (Option.buffer_size < 1024) Option.buffer_size = 1024;
			put_buffer_size(Option.buffer_size);
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.buffer_size = DEFAULT_BUFFER_SIZE;
			put_buffer_size(Option.buffer_size);
		}
		XCnt = 0, ++YCnt;*/

		//offset
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, "offset", true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		if (Option.offset == 0)snprintf(get_buffer(), BUFFER_SIZE, "±%.3f", Option.offset);
		else if (Option.offset > 0)snprintf(get_buffer(), BUFFER_SIZE, "+%.3f", Option.offset);
		else snprintf(get_buffer(), BUFFER_SIZE, "%.3f", Option.offset);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.offset = input_number_keyboard(6, true,true);
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.offset = 0;
		XCnt = 0, ++YCnt;

		//曲のはやさ
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_MUSICSPEED], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%.2f", Option.musicspeed);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.musicspeed = input_number_keyboard(5, true,false);
			if (Option.musicspeed > 1.5) Option.musicspeed = 1.5;
			else if (Option.musicspeed < 0.5) Option.musicspeed = 0.5;
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.musicspeed = 1.0;
		XCnt = 0, ++YCnt;

		//特殊
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_SPECIAL], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		if (Option.special == 1) draw_option_text(x, y, Text[Option.lang][TEXT_KANPEKI], true, &width, &height);
		else if (Option.special == 2) draw_option_text(x, y, Text[Option.lang][TEXT_TOKUN], true, &width, &height);
		else draw_option_text(x, y, Text[Option.lang][TEXT_OFF], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			opv = ++Option.special % 3;
			Option.special = opv;
		}
		XCnt = 0, ++YCnt;

		//fps
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_DISP_FPS], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.dispFps == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.dispFps = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.dispFps == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.dispFps = true;
		XCnt = 0, ++YCnt;
		break;

	case 3:
		XSense = 65, YSense = 30, XCnt = 0, YCnt = 1, gap = 100;
		//良
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_PERFECT], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%.3f", Option.judge_range_perfect);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.judge_range_perfect = input_number_keyboard(5, true, false);
			adjust_judge_range();
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.judge_range_perfect = DEFAULT_JUDGE_RANGE_PERFECT;
		XCnt = 0, ++YCnt;

		//可
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_NICE], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%.3f", Option.judge_range_nice);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.judge_range_nice = input_number_keyboard(5, true, false);
			adjust_judge_range();
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.judge_range_nice = DEFAULT_JUDGE_RANGE_NICE;
		XCnt = 0, ++YCnt;

		//不可
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_BAD], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%.3f", Option.judge_range_bad);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.judge_range_bad = input_number_keyboard(5, true, false);
			adjust_judge_range();
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.judge_range_bad = DEFAULT_JUDGE_RANGE_BAD;
		XCnt = 0, ++YCnt;

		//連打速度(AUTO)
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_COMBO_VOICE], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		snprintf(get_buffer(), BUFFER_SIZE, "%d", Option.Voice);
		draw_option_text(x, y, get_buffer(), true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			Option.Voice = input_number_keyboard(3, false,false);
		}
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.Voice = 2;
		XCnt = 0, ++YCnt;

		//はやさ(固定)
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_FIXROLL], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.fixroll == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.fixroll = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.fixroll == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.fixroll = true;
		XCnt = 0, ++YCnt;
		break;

	case 4:
		//ボタンマッピング

		draw_option_text(-1, 25, Text[Option.lang][TEXT_BUTTON_MAPPING], true, &width, &height);
		x = 130, y = 120;
		draw_option_text(-1, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		x = BOTTOM_WIDTH / 2 - width / 2;
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) init_button_mapping();

		//ABYX
		x = 280, y = 150;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_A, sprites);
		draw_option_text(x, y, "A", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_A);

		x -= key_interval, y += key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_B, sprites);
		draw_option_text(x, y, "B", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_B);

		x -= key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_Y, sprites);
		draw_option_text(x, y, "Y", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_Y);

		x += key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_X, sprites);
		draw_option_text(x, y, "X", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_X);

		//Dパット(十字キー)
		C2D_DrawRectangle(47, 130, 0, 30, 80, C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4));
		C2D_DrawRectangle(22, 155, 0, 80, 30, C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4), C2D_Color32f(1, 1, 1, 0.4));
		key_interval = 25;
		x = 80, y = 160;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_DRIGHT, sprites);
		draw_option_text(x, y, "ー", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_DRIGHT);

		x -= key_interval, y += key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_DDOWN, sprites);
		draw_option_text(x, y, "｜", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_DDOWN);

		x -= key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_DLEFT, sprites);
		draw_option_text(x, y, "ー", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_DLEFT);

		x += key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_DUP, sprites);
		draw_option_text(x, y, "｜", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_DUP);

		//CPAD
		C2D_DrawCircleSolid(60, 80, 0,35, C2D_Color32f(1, 1, 1, 0.4));
		key_interval = 25;
		x = 80, y = 70;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CPAD_RIGHT, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CPAD_RIGHT);

		x -= key_interval, y += key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CPAD_DOWN, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CPAD_DOWN);

		x -= key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CPAD_LEFT, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CPAD_LEFT);

		x += key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CPAD_UP, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CPAD_UP);

		//Cスティック
		C2D_DrawCircleSolid(230, 80, 0, 15, C2D_Color32f(1, 1, 1, 0.4));
		key_interval = 20;
		x = 245, y = 70;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CSTICK_RIGHT, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CSTICK_RIGHT);

		x -= key_interval, y += key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CSTICK_DOWN, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CSTICK_DOWN);

		x -= key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CSTICK_LEFT, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CSTICK_LEFT);

		x += key_interval, y -= key_interval;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_CSTICK_UP, sprites);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_CSTICK_UP);

		//L,ZL,ZR,R
		x = 20, y = 10;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_L, sprites);
		draw_option_text(x, y, "L", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_L);

		x += 40;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_ZL, sprites);
		draw_option_text(x-3, y, "ZL", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_ZL);

		x = 250;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_ZR, sprites);
		draw_option_text(x - 3, y, "ZR", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_ZR);

		x += 40;
		draw_button_mapping_icon(x + 6, y + 10, Option.KEY_R, sprites);
		draw_option_text(x, y, "R", true, &width, &height);
		width = 26, height = 26, x2 = x - 5, y2 = y - 5;
		if ((y2 < py && y2 + height > py && x2 < px && x2 + width > px) && key & KEY_TOUCH) switch_button_mapping(&Option.KEY_R);
		break;

	case 5:
		snprintf(get_buffer(), BUFFER_SIZE, "実験的設定");
		draw_option_text(320, 0, get_buffer(), true, &width, &height);
		draw_option_text(320-width, 0, get_buffer(), true, &width, &height);
		XSense = 65, YSense = 30, XCnt = 0, YCnt = 1, gap = 100;

		//外部効果音
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_EXSE], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_OFF], Option.exse == false, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.exse = false;
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_ON], Option.exse == true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) Option.exse = true;
		XCnt = 0, ++YCnt;

		//プレイヤー(双打譜面用)
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_PLAYER], true, &width, &height);
		x = XSense * XCnt + gap, y = YSense * YCnt, ++XCnt;
		if (Option.player == 1) draw_option_text(x, y, Text[Option.lang][TEXT_1P], true, &width, &height);
		else if (Option.player == 2) draw_option_text(x, y, Text[Option.lang][TEXT_2P], true, &width, &height);
		else draw_option_text(x, y, Text[Option.lang][TEXT_OFF], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) {
			opv = ++Option.player % 3;
			Option.player = opv;
		}
		XCnt = 0, ++YCnt;

		//楽曲検索
		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_SEARCH], true, &width, &height);
		++XCnt;
		x = XSense * XCnt + gap, y = YSense * YCnt;
		draw_option_text(x, y, Text[Option.lang][TEXT_RESET], true, &width, &height);
		if ((y < py && y + height > py && x < px && x + width > px) && key & KEY_TOUCH) strcpy(Option.SongTitle, "\0");
		XCnt = 0, ++YCnt;

		x = XSense * XCnt, y = YSense * YCnt, ++XCnt;
		C2D_DrawRectSolid(2,y,0,316,height,C2D_Color32(0x33,0x33,0x33,0xFF));
		if (strcmp(Option.SongTitle, "\0") != 0) {
			snprintf(get_buffer(), 257, "%s", Option.SongTitle);
			draw_option_text(x, y, get_buffer(), true, &width, &height);
		}
		if ((y < py && y + height > py && 0 < px && 400 > px) && key & KEY_TOUCH) {
			char* test = input_normal_keyboard();
			strcpy(Option.SongTitle, test);
		}
		XCnt = 0, ++YCnt;
		break;
	}

	calc_option_page(px,py,key);
}

int get_lang() {
	return Option.lang;
}

void toggle_auto() {
	Option.isAuto = !Option.isAuto;
}

bool get_isauto() {
	return Option.isAuto;
}

float mspeed() {
	return Option.musicspeed;
}

void init_option() {

	Option.isAuto = false;
	Option.isStelth = false;
	Option.dispFps = false;
	Option.random = 0;
	Option.speed = 1.0;
	Option.isSwap = false;
	Option.lang = LANG_JP;
	Option.buffer_size = DEFAULT_BUFFER_SIZE;
	Option.offset = 0.0;
	Option.fixroll = false;
	Option.special = 0;
	Option.player = 0;
	Option.musicspeed = 1.0;
	Option.judge_range_perfect = DEFAULT_JUDGE_RANGE_PERFECT;
	Option.judge_range_nice = DEFAULT_JUDGE_RANGE_NICE;
	Option.judge_range_bad = DEFAULT_JUDGE_RANGE_BAD;

	init_button_mapping();

	option_page = 1;
}

void save_option() {

	json_object_set(json, "lang", json_integer(Option.lang));
	json_object_set(json, "buffer_size", json_integer(Option.buffer_size));
	json_object_set(json, "isAuto", json_boolean(Option.isAuto));
	json_object_set(json, "isStelth", json_boolean(Option.isStelth));
	json_object_set(json, "isSwap", json_boolean(Option.isSwap));
	json_object_set(json, "dispFps", json_boolean(Option.dispFps));
	json_object_set(json, "random", json_real(Option.random));
	json_object_set(json, "speed", json_real(Option.speed));
	json_object_set(json, "offset", json_real(Option.offset));
	json_object_set(json, "KEY_A", json_integer(Option.KEY_A));
	json_object_set(json, "KEY_B", json_integer(Option.KEY_B));
	json_object_set(json, "KEY_X", json_integer(Option.KEY_X));
	json_object_set(json, "KEY_Y", json_integer(Option.KEY_Y));
	json_object_set(json, "KEY_R", json_integer(Option.KEY_R));
	json_object_set(json, "KEY_ZR", json_integer(Option.KEY_ZR));
	json_object_set(json, "KEY_L", json_integer(Option.KEY_L));
	json_object_set(json, "KEY_ZL", json_integer(Option.KEY_ZL));
	json_object_set(json, "KEY_DUP", json_integer(Option.KEY_DUP));
	json_object_set(json, "KEY_DDOWN", json_integer(Option.KEY_DDOWN));
	json_object_set(json, "KEY_DRIGHT", json_integer(Option.KEY_DRIGHT));
	json_object_set(json, "KEY_DLEFT", json_integer(Option.KEY_DLEFT));
	json_object_set(json, "KEY_CPAD_UP", json_integer(Option.KEY_CPAD_UP));
	json_object_set(json, "KEY_CPAD_DOWN", json_integer(Option.KEY_CPAD_DOWN));
	json_object_set(json, "KEY_CPAD_RIGHT", json_integer(Option.KEY_CPAD_RIGHT));
	json_object_set(json, "KEY_CPAD_LEFT", json_integer(Option.KEY_CPAD_LEFT));
	json_object_set(json, "KEY_CSTICK_UP", json_integer(Option.KEY_CSTICK_UP));
	json_object_set(json, "KEY_CSTICK_DOWN", json_integer(Option.KEY_CSTICK_DOWN));
	json_object_set(json, "KEY_CSTICK_RIGHT", json_integer(Option.KEY_CSTICK_RIGHT));
	json_object_set(json, "KEY_CSTICK_LEFT", json_integer(Option.KEY_CSTICK_LEFT));
	json_object_set(json, "judge_range_perfect", json_integer(round(Option.judge_range_perfect * 1000)));
	json_object_set(json, "judge_range_nice", json_integer(round(Option.judge_range_nice * 1000)));
	json_object_set(json, "judge_range_bad", json_integer(round(Option.judge_range_bad * 1000)));
	json_object_set(json, "Voice", json_integer(Option.Voice));
	json_object_set(json, "fixroll", json_boolean(Option.fixroll));
	json_object_set(json, "blacktext", json_real(Option.blacktext));
	json_object_set(json, "special", json_integer(Option.special));
	json_object_set(json, "exse", json_boolean(Option.exse));
	json_object_set(json, "player", json_integer(Option.player));
	
	json_dump_file(json, SETTING_FILE, 0);
}

void load_option() {

	init_option();
	
	json = json_load_file(SETTING_FILE, 0, &error_json);

	if (json != NULL){

		Option.lang = json_integer_value(json_object_get(json, "lang"));
		Option.buffer_size = json_integer_value(json_object_get(json, "buffer_size"));
		Option.isAuto = json_boolean_value(json_object_get(json, "isAuto"));
		Option.isStelth = json_boolean_value(json_object_get(json, "isStelth"));
		Option.isSwap = json_boolean_value(json_object_get(json, "isSwap"));
		Option.dispFps = json_boolean_value(json_object_get(json, "dispFps"));
		Option.random = json_real_value(json_object_get(json, "random"));
		Option.speed = json_real_value(json_object_get(json, "speed"));
		Option.offset = json_real_value(json_object_get(json, "offset"));
		Option.KEY_A = json_integer_value(json_object_get(json, "KEY_A"));
		Option.KEY_B = json_integer_value(json_object_get(json, "KEY_B"));
		Option.KEY_X = json_integer_value(json_object_get(json, "KEY_X"));
		Option.KEY_Y = json_integer_value(json_object_get(json, "KEY_Y"));
		Option.KEY_R = json_integer_value(json_object_get(json, "KEY_R"));
		Option.KEY_ZR = json_integer_value(json_object_get(json, "KEY_ZR"));
		Option.KEY_L = json_integer_value(json_object_get(json, "KEY_L"));
		Option.KEY_ZL = json_integer_value(json_object_get(json, "KEY_ZL"));
		Option.KEY_DUP = json_integer_value(json_object_get(json, "KEY_DUP"));
		Option.KEY_DDOWN = json_integer_value(json_object_get(json, "KEY_DDOWN"));
		Option.KEY_DRIGHT = json_integer_value(json_object_get(json, "KEY_DRIGHT"));
		Option.KEY_DLEFT = json_integer_value(json_object_get(json, "KEY_DLEFT"));
		Option.KEY_CPAD_UP = json_integer_value(json_object_get(json, "KEY_CPAD_UP"));
		Option.KEY_CPAD_DOWN = json_integer_value(json_object_get(json, "KEY_CPAD_DOWN"));
		Option.KEY_CPAD_RIGHT = json_integer_value(json_object_get(json, "KEY_CPAD_RIGHT"));
		Option.KEY_CPAD_LEFT = json_integer_value(json_object_get(json, "KEY_CPAD_LEFT"));
		Option.KEY_CSTICK_UP = json_integer_value(json_object_get(json, "KEY_CSTICK_UP"));
		Option.KEY_CSTICK_DOWN = json_integer_value(json_object_get(json, "KEY_CSTICK_DOWN"));
		Option.KEY_CSTICK_RIGHT = json_integer_value(json_object_get(json, "KEY_CSTICK_RIGHT"));
		Option.KEY_CSTICK_LEFT = json_integer_value(json_object_get(json, "KEY_CSTICK_LEFT"));
		Option.judge_range_perfect = (double)json_integer_value(json_object_get(json, "judge_range_perfect"))/1000;
		Option.judge_range_nice = (double)json_integer_value(json_object_get(json, "judge_range_nice"))/1000;
		Option.judge_range_bad = (double)json_integer_value(json_object_get(json, "judge_range_bad"))/1000;
		Option.Voice = json_integer_value(json_object_get(json, "Voice"));
		Option.fixroll = json_boolean_value(json_object_get(json, "fixroll"));
		Option.blacktext = json_real_value(json_object_get(json, "blacktext"));
		Option.special = json_integer_value(json_object_get(json, "special"));
		Option.exse = json_boolean_value(json_object_get(json, "exse"));
		Option.player = json_integer_value(json_object_get(json, "player"));
		
		adjust_judge_range();
	}
	if (json == NULL) {			//開けなかった時
		json = json_pack("{}");	//ファイル空の時はこれしないとセットできなくなる
		save_option();			//書き込み
	}
}

void exit_option() {

	save_option();
	json_decref(json);
}

void get_option(OPTION_T *TMP) {

	TMP->isAuto = Option.isAuto;
	TMP->isStelth = Option.isStelth;
	TMP->random = Option.random;
	TMP->speed = Option.speed;
	TMP->offset = Option.offset;
	TMP->isSwap = Option.isSwap;
	TMP->lang = Option.lang;
	TMP->buffer_size = Option.buffer_size;
	TMP->dispFps = Option.dispFps;
	TMP->KEY_A = Option.KEY_A;
	TMP->KEY_B = Option.KEY_B;
	TMP->KEY_Y = Option.KEY_Y;
	TMP->KEY_X = Option.KEY_X;
	TMP->KEY_R = Option.KEY_R;
	TMP->KEY_ZR = Option.KEY_ZR;
	TMP->KEY_L = Option.KEY_L;
	TMP->KEY_ZL = Option.KEY_ZL;
	TMP->KEY_DRIGHT = Option.KEY_DRIGHT;
	TMP->KEY_DLEFT= Option.KEY_DLEFT;
	TMP->KEY_DUP = Option.KEY_DUP;
	TMP->KEY_DDOWN = Option.KEY_DDOWN;
	TMP->KEY_CPAD_RIGHT = Option.KEY_CPAD_RIGHT;
	TMP->KEY_CPAD_LEFT = Option.KEY_CPAD_LEFT;
	TMP->KEY_CPAD_UP = Option.KEY_CPAD_UP;
	TMP->KEY_CPAD_DOWN = Option.KEY_CPAD_DOWN;
	TMP->KEY_CSTICK_RIGHT = Option.KEY_CSTICK_RIGHT;
	TMP->KEY_CSTICK_LEFT = Option.KEY_CSTICK_LEFT;
	TMP->KEY_CSTICK_UP = Option.KEY_CSTICK_UP;
	TMP->KEY_CSTICK_DOWN = Option.KEY_CSTICK_DOWN;
	TMP->judge_range_perfect = Option.judge_range_perfect;
	TMP->judge_range_nice = Option.judge_range_nice;
	TMP->judge_range_bad = Option.judge_range_bad;
	TMP->Voice = Option.Voice;
	TMP->fixroll = Option.fixroll;
	TMP->blacktext = Option.blacktext;
	TMP->special = Option.special;
	TMP->exse = Option.exse;
	TMP->player = Option.player;
	strcpy(TMP->SongTitle, Option.SongTitle);
	black = Option.blacktext;
	if (Option.Voice == 0) comboVoice = INT_MAX;
	else comboVoice = Option.Voice;
}
