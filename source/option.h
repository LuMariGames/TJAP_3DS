﻿#pragma once

const char Text[3][56][128] = {
	{
		"ON",
		"OFF",
		"かんたん",
		"ふつう",
		"むずかしい",
		"おに",
		"おに裏",
		"太鼓タワー",
		"段位道場",
		"オート",
		"はやさ",
		"x1",
		"x2",
		"x3",
		"x4",
		"ステルス",
		"あべこべ",
		"ランダム",
		"25%",
		"50%",
		"Lang.",
		"日本語",
		"English",
		"Español",
		"ﾊﾞｯﾌｧｻｲｽﾞ",
		"リセット",
		"fps表示",
		"連打速度",
		"はやさ(固定)",
		"ボタン割り当て",
		"スコア",
		"最大ｺﾝﾎﾞ数",
		"良",
		"可",
		"不可",
		"連打数",
		"STARTボタンを押して戻る",
		"演奏を続ける",
		"はじめからやり直す",
		"曲選択に戻る",
		"DSP1を起動していないため音は流れません",
		"音楽ファイルが読み込めません\n\nTJAファイルをUTF-8に変換しましたか？",
		"音楽ファイルがOggファイルではありません",
		"不可の数",
		"可の数",
		"良の数",
		"スコア",
		"連打数",
		"叩けた数",
		"魂ゲージ",
		"以上",
		"未満",
		"とくしゅ",
		"かんぺき",
		"とっくん",
		"外部効果音",
	},
	{
		"ON",
		"OFF",
		"Easy",
		"Normal",
		"Hard",
		"Extreme",
		"Extra",
		"Tower",
		"Dan-i Dojo",
		"Auto",
		"Speed",
		"x1",
		"x2",
		"x3",
		"x4",
		"Vanish",
		"Inverse",
		"Random",
		"25%",
		"50%",
		"Lang.",
		"日本語",
		"English",
		"Español",
		"BufferSize",
		"Reset",
		"Display fps",
		"RollSpeed",
		"Speed(Fixed)",
		"Button mapping",
		"Score",
		"MAX Combo",
		"GOOD",
		"OK",
		"BAD",
		"Drumroll",
		"Press START",
		"Continue",
		"Retry",
		"Back to Select song",
		"No sound is played because\nDSP1 is not activated.",
		"The sound file does not exist.\n\nHave you converted the character encoding\nof the TJA file to UTF8?",
		"The sound file is not an Ogg file.",
		"Bads",
		"Nices",
		"Perfects",
		"Scores",
		"Rolls",
		"Hits",
		"Gauge",
		"more",
		"less",
		"special",
		"perfection",
		"Training",
		"External sound",
	},
	{
		"Sí",
		"No",
		"Fácil",
		"Normal",
		"Difícil",
		"Extremo",
		"Extra",
		"torre",
		"Dan-i Dojo",
		"Auto",
		"Velocidad",
		"x1",
		"x2",
		"x3",
		"x4",
		"Invisible",
		"Inversa",
		"Random",
		"25%",
		"50%",
		"Idioma",
		"日本語",
		"English",
		"Español",
		"Tamaño del búfer",
		"Reiniciar",
		"Mostrar fps",
		"Redoblevelocidad",
		"Velocidad(Fijo)",
		"Mapeo de botones",
		"Puntuación",
		"Combo máximo",
		"Bien",
		"OK",
		"Mal",
		"Redoble",
		"Presiona START",
		"Continuar",
		"Reintentar",
		"Volver al menú",
		"No sound is played because\nDSP1 is not activated.",
		"The sound file does not exist.\n\nHave you converted the character encoding\nof the TJA file to UTF8?",
		"The sound file is not an Ogg file.",
		"Mal",
		"OK",
		"bien",
		"Puntuación",
		"Redoble",
		"visitas",
		"indicador",
		"más que",
		"menos que",
		"especial",
		"perfección",
		"Capacitación",
		"Sonido externo",
	}
};

typedef struct {

	int lang,buffer_size,Voice,special;
	bool isAuto, isStelth,isSwap,dispFps,fixroll,exse;
	float speed, random,offset,blacktext,
		judge_range_perfect,judge_range_nice,judge_range_bad;
	int KEY_A, KEY_B, KEY_DRIGHT, KEY_DLEFT, KEY_DUP, KEY_DDOWN, KEY_R, KEY_L, KEY_X, KEY_Y,
		KEY_ZL, KEY_ZR, KEY_CSTICK_RIGHT, KEY_CSTICK_LEFT, KEY_CSTICK_UP, KEY_CSTICK_DOWN,
		KEY_CPAD_RIGHT, KEY_CPAD_LEFT, KEY_CPAD_UP, KEY_CPAD_DOWN;
} OPTION_T;

enum Lang_knd {
	LANG_JP = 0,
	LANG_EN,
	LANG_ES,
};

enum KEY_KND {
	KEY_NONE = 0,
	KEY_DON,
	KEY_KATSU,
};

enum Text_knd {
	TEXT_ON,
	TEXT_OFF,
	TEXT_EASY,
	TEXT_NORMAL,
	TEXT_HARD,
	TEXT_ONI,
	TEXT_EDIT,
	TEXT_TOWER,
	TEXT_DAN,
	TEXT_AUTO,
	TEXT_SPEED,
	TEXT_X1,
	TEXT_X2,
	TEXT_X3,
	TEXT_X4,
	TEXT_STELTH,
	TEXT_SWAP,
	TEXT_RANDOM,
	TEXT_R25,
	TEXT_R50,
	TEXT_LANG,
	TEXT_JP,
	TEXT_EN,
	TEXT_ES,
	TEXT_BUFFERSIZE,
	TEXT_RESET,
	TEXT_DISP_FPS,
	TEXT_COMBO_VOICE,
	TEXT_FIXROLL,
	TEXT_BUTTON_MAPPING,
	TEXT_SCORE,
	TEXT_MAXCOMBO,
	TEXT_PERFECT,
	TEXT_NICE,
	TEXT_BAD,
	TEXT_ROLLCOUNT,
	TEXT_PRESSSTART,
	TEXT_CONTINUE,
	TEXT_STARTOVER,
	TEXT_RETURNSELECT,
	TEXT_WARNING_DSP1,
	TEXT_WARNING_WAVE_NO_EXIST,
	TEXT_WARNING_WAVE_NOT_OGG,
	TEXT_NUM_BAD,
	TEXT_NUM_NICE,
	TEXT_NUM_PERFECT,
	TEXT_NUM_SCORE,
	TEXT_NUM_ROLL,
	TEXT_NUM_HIT,
	TEXT_NUM_GAUGE,
	TEXT_NUM_UP,
	TEXT_NUM_DOWN,
	TEXT_SPECIAL,
	TEXT_KANPEKI,
	TEXT_TOKUN,
	TEXT_EXSE,
};

int get_lang();
void draw_option(u16 px, u16 py, unsigned int key , C2D_Sprite sprites[SPRITES_NUMER]);
void toggle_auto();
void get_option(OPTION_T *TMP);
void init_option();
void load_option(),exit_option(),save_option();