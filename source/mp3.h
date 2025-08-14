#pragma once
#include <stdint.h>
#include "playback.h"

void setMp3(struct decoder_fn* decoder);
int isMp3(const char *path);
void init_mpg123(), exit_mpg123();
