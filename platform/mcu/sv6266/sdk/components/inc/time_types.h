#ifndef TIME_TYPES_H
#define TIME_TYPES_H

#define time_after(a,b)		\
	((long)(b) - (long)(a) < 0)

#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	((long)(a) - (long)(b) >= 0)


typedef struct __timer_64_S {
    u32     lt;
    u32     ut;
} Time64_S;

typedef union {
        Time64_S   ts;
        u64        t;
} Time_T;



#endif /* end of include guard: TIME_TYPES_H */
