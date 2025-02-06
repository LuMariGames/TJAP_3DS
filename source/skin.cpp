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
	Skin.dance_1 = 100;
	Skin.dance_2 = 200;
	Skin.dance_3 = 300;
}

void save_skin() {

	json_object_set(sjson, "don_x", json_integer(Skin.don_x));
	json_object_set(sjson, "don_y", json_integer(Skin.don_y));
	json_object_set(sjson, "don_go_x", json_integer(Skin.don_gogo_x));
	json_object_set(sjson, "don_go_y", json_integer(Skin.don_gogo_y));
	json_object_set(sjson, "dance1st", json_integer(Skin.dance_1));
	json_object_set(sjson, "dance2nd", json_integer(Skin.dance_2));
	json_object_set(sjson, "dance3rd", json_integer(Skin.dance_3));

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
		Skin.dance_1 = json_integer_value(json_object_get(sjson, "dance1st"));
		Skin.dance_2 = json_integer_value(json_object_get(sjson, "dance2nd"));
		Skin.dance_3 = json_integer_value(json_object_get(sjson, "dance3rd"));
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
	TMP->dance_1 = Skin.dance_1;
	TMP->dance_2 = Skin.dance_2;
	TMP->dance_3 = Skin.dance_3;
}
