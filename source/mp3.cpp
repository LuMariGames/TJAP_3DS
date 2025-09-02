#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3.h"
#include "playback.h"

static size_t*			buffSize;
static mpg123_handle	*mh = NULL;
static uint32_t			rate;
static uint8_t			channels;

static int initMp3(const char* file);
static uint32_t rateMp3(void);
static uint8_t channelMp3(void);
static uint64_t decodeMp3(void* buffer);
static void exitMp3(void);

void setMp3(struct decoder_fn* decoder)
{
	decoder->init = &initMp3;
	decoder->rate = &rateMp3;
	decoder->channels = &channelMp3;
	buffSize = &(decoder->buffSize);
	decoder->decode = &decodeMp3;
	decoder->exit = &exitMp3;
}

int initMp3(const char* file)
{
	int err = 0;
	int encoding = 0;

	if((err = mpg123_init()) != MPG123_OK)
		return err;

	if((mh = mpg123_new(NULL, &err)) == NULL)
	{
		printf("Error: %s\n", mpg123_plain_strerror(err));
		return err;
	}

	if(mpg123_open(mh, file) != MPG123_OK ||
			mpg123_getformat(mh, (long *) &rate, (int *) &channels, &encoding) != MPG123_OK)
	{
		printf("Trouble with mpg123: %s\n", mpg123_strerror(mh));
		return -1;
	}
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);
	mpg123_param(mh, MPG123_DOWN_SAMPLE, 1, 1.0);
	*buffSize = mpg123_outblock(mh) * 32;
	return 0;
}

uint32_t rateMp3(void)
{
	return rate;
}
uint8_t channelMp3(void)
{
	return channels;
}
void seekMp3(float time)
{
	mpg123_seek(mh, (uint32_t)(rate * time), SEEK_SET);
}

uint64_t decodeMp3(void* buffer)
{
	size_t done = 0;
	mpg123_read(mh, buffer, *buffSize, &done);
	return done / sizeof(int16_t);
}

void exitMp3(void)
{
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}
