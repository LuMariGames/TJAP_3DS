#pragma once

void draw_debug(float x, float y, const char *text),set_isPause(),debug_touch(int x, int y),play_songs(char* ptr);
bool get_isPause(),get_isMusicStart();
char* get_buffer();
int powi(int x, int y),pause_window(touchPosition tp, unsigned int key),
message_window(touchPosition tp, unsigned int key,int text),exist_file(const char* path);
double starttime();
u32 GetNextPowerOf2(u32 v);

#define BUFFER_SIZE 160 //バッファ用文字列のサイズ
