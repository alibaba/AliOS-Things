/*
 * Simple XZ decoder command line tool
 *
 * Author: Lasse Collin <lasse.collin@tukaani.org>
 *
 * This file has been put into the public domain.
 * You can do whatever you want with this file.
 */

/*
 * This is really limited: Not all filters from .xz format are supported,
 * only CRC32 is supported as the integrity check, and decoding of
 * concatenated .xz streams is not supported. Thus, you may want to look
 * at xzdec from XZ Utils if a few KiB bigger tool is not a problem.
 */

#include "xz.h"

static uint8_t in[BUFSIZ];
static uint8_t out[BUFSIZ];

int main(int argc, char **argv)
{
	struct xz_buf b;
	struct xz_dec *s;
	enum xz_ret ret;
	const char *msg;
	FILE * outfile, *infile; 
	int read_size = BUFSIZ;

	if(argc != 3 || strcmp(argv[1], "--help") == 0)
	{
		printf("xzminidec [input] [output].\r\n");

		return 0;
	}

	infile = fopen(argv[1], "rb");
	if(infile == NULL)
	{
		printf("open file %s failed\r\n", argv[1]);
		return 0;
	}

	outfile = fopen(argv[2], "wb" );
	if(outfile == NULL)
	{
		printf("create file %s failed\r\n", argv[2]);
		return 0;
	}	

	xz_crc32_init();

	/*
	 * Support up to 64 MiB dictionary. The actually needed memory
	 * is allocated once the headers have been parsed.
	 */
	s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
	if (s == NULL) {
		printf("Memory allocation failed\r\n");
		goto exit;
	}

	b.in = in;
	b.in_pos = 0;
	b.in_size = 0;
	b.out = out;
	b.out_pos = 0;
	b.out_size = BUFSIZ;

	while (true) {
		if (b.in_pos == b.in_size) {
			read_size = fread(in, 1, sizeof(in), infile);
			printf("read size = %d\r\n", read_size);
			if(read_size > 0)
			{
				b.in_size = read_size;
				b.in_pos = 0;
			}
		}

		ret = xz_dec_run(s, &b);
		printf("ret = %d\r\n", ret);

		if (b.out_pos == sizeof(out)) {
			if (fwrite(out, 1, b.out_pos, outfile) != b.out_pos) {
				printf("Write error\r\n");
				goto exit;
			}

			b.out_pos = 0;
		}

		if (ret == XZ_OK)
			continue;

		if (fwrite(out, 1, b.out_pos, outfile) != b.out_pos) {
			printf("Write error\r\n");
			goto exit;
		}

		switch (ret) {
		case XZ_STREAM_END:
			xz_dec_end(s);
			return 0;

		case XZ_MEM_ERROR:
			printf("Memory allocation failed\r\n");
			goto exit;

		case XZ_MEMLIMIT_ERROR:
			printf("Memory usage limit reached\r\n");
			goto exit;

		case XZ_FORMAT_ERROR:
			printf("Not a .xz file\r\n");
			goto exit;

		case XZ_OPTIONS_ERROR:
			printf("Unsupported options in the .xz headers\r\n");
			goto exit;

		case XZ_DATA_ERROR:
		case XZ_BUF_ERROR:
			printf("File is corrupt\r\n");
			goto exit;

		default:
			printf("Bug!\r\n");
			goto exit;
		}
	}

exit:
	fclose(infile);
	fclose(outfile);
	xz_dec_end(s);
	return 0;
}
