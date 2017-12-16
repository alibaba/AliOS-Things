/**
 * Copyright (C) 2016 The YunOS Project. All rights reserved.
 **/

#ifndef _TEE_TST_H_
#define _TEE_TST_H_

/* secure file system */
typedef void FILE;
extern int errno;
extern FILE *fopen(const char *filename, const char *modes);
extern size_t fread(void *ptr, size_t size, size_t n, FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t n, FILE *s);
extern int fseek(FILE *stream, long int off, int whence);
extern int fclose(FILE *stream);

#endif /* _TEE_TST_H_ */
