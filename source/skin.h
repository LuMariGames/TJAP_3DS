typedef struct {

	int don_x,don_y,don_gogo_x,don_gogo_y,
	d1num,d2num,d3num,d1total,d2total,d3total,d1anime[128],d2anime[128],d3anime[128];	//踊り子関連
} SKIN_T;

void get_skin(SKIN_T *TMP);
void init_skin(),load_skin(),exit_skin();
