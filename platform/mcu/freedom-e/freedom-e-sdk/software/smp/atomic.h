#ifndef SIFIVE_ATOMIC_H
#define SIFIVE_ATOMIC_H

#define ATOMIC_INIT(x)		\
	{			\
		.counter = (x),	\
	}

typedef struct {
  int counter;
} atomic_t;

static inline int atomic_xchg(atomic_t *v, int n)
{
  register int c;

  __asm__ __volatile__ (
    "amoswap.w.aqrl %0, %2, %1"
    : "=r" (c), "+A" (v->counter)
    : "r" (n));
  return c;
}

static inline void mb(void)
{
  __asm__ __volatile__ ("fence");
}

#endif
