#pragma once

void draw_debug(float x, float y, const char *text),debug_touch(int x, int y);
bool get_isPause(),get_isMusicStart();;
char* get_buffer();
int powi(int x, int y),pause_window(touchPosition tp, unsigned int key),
message_window(touchPosition tp, unsigned int key,int text);
double starttime();

#define BUFFER_SIZE 160 //バッファ用文字列のサイズ
