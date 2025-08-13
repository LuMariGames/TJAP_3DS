#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3.h"

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

	*buffSize = mpg123_outblock(mh) * 16;
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

uint64_t decodeMp3(void* buffer)
{
	size_t done = 0;
	mpg123_read(mh, buffer, *buffSize, &done);
	return done / (sizeof(int16_t));
}

void exitMp3(void)
{
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}

int isMp3(const char *path)
{
#if 0
	int err;
	int result = 1;
	mpg123_handle *mh = NULL;
	long rate;
	int channels, encoding;

	// Initialise the library
	if (mpg123_init() != MPG123_OK)
		goto out;

	// Create a decoder handle
	mh = mpg123_new(NULL, &err);
	if (!mh)
		goto exit_init;

	// skip ID3v2 tags rather than parsing them (so tag-only files don’t count as valid mp3)
	mpg123_param(mh, MPG123_SKIP_ID3V2, 1, 0);

	// limit how many bytes to scan for a frame sync (e.g. 2048 bytes)
	mpg123_param(mh, MPG123_RESYNC_LIMIT, 2048, 0);

	// Try opening the file
	err = mpg123_open(mh, path);
	if (err != MPG123_OK)
		goto exit_handle;

	// Query the decoded format
	if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
		goto close_handle;

	// Parse first frame in file
	err = mpg123_framebyframe_next(mh);
	if (err != MPG123_OK)
	{
		// If we can't read the first frame, it's not a valid MP3
		goto close_handle;
	}

	// All checks passed: valid MP3
	result = 0;

close_handle:
	mpg123_close(mh);

exit_handle:
	mpg123_delete(mh);

exit_init:
	mpg123_exit();

out:
	return result;
#else
    unsigned char buf[4];
	FILE *f = fopen(path, "rb");
	int ret = 1;

	if(!f) return 1;

    if(fread(buf, 1, 4, f) < 4)
		goto out;

    // ID3v2 tag?
    if(buf[0]=='I' && buf[1]=='D' && buf[2]=='3') {
		ret = 0;
		goto out;
	}

    // MPEG frame sync: 11 one-bits in a row
    if(buf[0]==0xFF && (buf[1]&0xE0)==0xE0) {
		ret = 0;
		goto out;
	}

out:
	fclose(f);
    return ret;
#endif
}

