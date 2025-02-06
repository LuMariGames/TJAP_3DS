#include "header.h"
#include "skin.h"
#include "select.h"
#include "main.h"
#include "vorbis.h"
#include <jansson.h>

SKIN_T Skin;
json_t *sjson;
json_error_t error_sjson;

void init_skin() {

	Skin.don_x = 76;
	Skin.don_y = 51;
	Skin.don_gogo_x = 84;
	Skin.don_gogo_y = 45;
	Skin.d1num = 6;
	Skin.d2num = 6;
	Skin.d3num = 6;
}

void save_skin() {

	json_object_set(sjson, "don_x", json_integer(Skin.don_x));
	json_object_set(sjson, "don_y", json_integer(Skin.don_y));
	json_object_set(sjson, "don_go_x", json_integer(Skin.don_gogo_x));
	json_object_set(sjson, "don_go_y", json_integer(Skin.don_gogo_y));
	json_object_set(sjson, "d1num", json_integer(Skin.d1num));
	json_object_set(sjson, "d2num", json_integer(Skin.d2num));
	json_object_set(sjson, "d3num", json_integer(Skin.d3num));

	json_dump_file(sjson, SKIN_SETTING_FILE, 0);
}

void load_skin() {

	init_skin();
	
	sjson = json_load_file(SKIN_SETTING_FILE, 0, &error_sjson);

	if (sjson != NULL){

		Skin.don_x = json_integer_value(json_object_get(sjson, "don_x"));
		Skin.don_y = json_integer_value(json_object_get(sjson, "don_y"));
		Skin.don_gogo_x = json_integer_value(json_object_get(sjson, "don_go_x"));
		Skin.don_gogo_y = json_integer_value(json_object_get(sjson, "don_go_y"));
		Skin.d1num = json_integer_value(json_object_get(sjson, "d1num"));
		Skin.d2num = json_integer_value(json_object_get(sjson, "d2num"));
		Skin.d3num = json_integer_value(json_object_get(sjson, "d3num"));
	}
	if (sjson == NULL) {			//開けなかった時
		sjson = json_pack("{}");	//ファイル空の時はこれしないとセットできなくなる
		save_skin();			//書き込み
	}
}

void exit_skin() {

	json_decref(sjson);
}

void get_skin(SKIN_T *TMP) {

	TMP->don_x = Skin.don_x;
	TMP->don_y = Skin.don_y;
	TMP->don_gogo_x = Skin.don_gogo_x;
	TMP->don_gogo_y = Skin.don_gogo_y;
	TMP->d1num = Skin.d1num;
	TMP->d2num = Skin.d2num;
	TMP->d3num = Skin.d3num;
}
