#ifndef  MEMNAME_H
#define  MEMNAME_H

#define malloc(a)  ckmalloc(a, __func__, __LINE__)
#define free(a)    ckfree(a)

#endif