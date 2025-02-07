#include "header.h"
#include "skin.h"
#include "select.h"
#include "main.h"
#include "vorbis.h"
#include <jansson.h>

SKIN_T Skin;
json_t *sjson;
json_error_t error_sjson;

char temp[256] = NULL, *tp = NULL;
int cnt = 1;

void init_skin() {

	Skin.don_x = 76;
	Skin.don_y = 51;
	Skin.don_gogo_x = 84;
	Skin.don_gogo_y = 45;
	Skin.d1num = 4;
	Skin.d2num = 4;
	Skin.d3num = 4;
	Skin.d1total = 6;
	Skin.d2total = 6;
	Skin.d3total = 6;
	Skin.d1anime[] = {0,1,2,3,2,3};
	Skin.d2anime[] = {0,1,2,3,2,3};
	Skin.d3anime[] = {0,1,2,3,2,3};
}

void save_skin() {

	json_object_set(sjson, "don_x", json_integer(Skin.don_x));
	json_object_set(sjson, "don_y", json_integer(Skin.don_y));
	json_object_set(sjson, "don_go_x", json_integer(Skin.don_gogo_x));
	json_object_set(sjson, "don_go_y", json_integer(Skin.don_gogo_y));

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
		Skin.d1total = json_integer_value(json_object_get(sjson, "d1total"));
		Skin.d2total = json_integer_value(json_object_get(sjson, "d2total"));
		Skin.d3total = json_integer_value(json_object_get(sjson, "d3total"));
		
		strlcpy(temp, json_string_value(json_object_get(sjson, "d1anime")), sizeof(temp));
		tp = strtok((char*)temp, ",");
		Skin.d1anime[0] = atoi(tp);
		cnt = 1;
		while ((tp = strtok(NULL, ","))) {
			Skin.d1anime[cnt] = atoi(tp);
			++cnt;
		}

		strlcpy(temp, json_string_value(json_object_get(sjson, "d2anime")), sizeof(temp));
		tp = strtok(temp, ",");
		Skin.d2anime[0] = atoi(tp);
		cnt = 1;
		while ((tp = strtok(NULL, ","))) {
			Skin.d2anime[cnt] = atoi(tp);
			++cnt;
		}

		strlcpy(temp, json_string_value(json_object_get(sjson, "d3anime")), sizeof(temp));
		tp = strtok(temp, ",");
		Skin.d3anime[0] = atoi(tp);
		cnt = 1;
		while ((tp = strtok(NULL, ","))) {
			Skin.d3anime[cnt] = atoi(tp);
			++cnt;
		}
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
	TMP->d1total = Skin.d1total;
	TMP->d2total = Skin.d2total;
	TMP->d3total = Skin.d3total;
	TMP->d1anime[] = Skin.d1anime[];
	TMP->d2anime[] = Skin.d2anime[];
	TMP->d3anime[] = Skin.d3anime[];
}
