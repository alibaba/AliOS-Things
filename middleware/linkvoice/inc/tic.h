//
//  tic.h
//  pal_sdk
//

#ifndef tic_h
#define tic_h

typedef struct _tic_t {
    long tv_sec;
    long tv_usec;
}tic_t;


tic_t tic();
long toc(tic_t starter);
int pal_gettimeofday(tic_t *now, void *param);
#endif /* tic_h */
