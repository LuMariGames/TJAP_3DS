﻿#pragma once

void draw_debug(float x, float y, const char *text);
bool get_isPause();
void debug_touch(int x, int y);
bool get_isMusicStart();
char* get_buffer();
int powi(int x, int y);
int pause_window(touchPosition tp, unsigned int key);
int message_window(touchPosition tp, unsigned int key,int text);
int exist_file(const char* path);
int time_count(double TIME);

#define BUFFER_SIZE 160 //バッファ用文字列のサイズ
