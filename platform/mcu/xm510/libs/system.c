#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>


unsigned int sleep(unsigned int seconds)
{
	aos_msleep(seconds * 1000);
}


unsigned int usleep(unsigned int useconds)
{
	unsigned int ms;
	ms = useconds / 1000;
	if(ms == 0)
		ms = 1;
	aos_msleep(useconds / 1000);
}



void *aos_malloc_align(size_t size, size_t align)
{
    void *align_ptr;
    void *ptr;
    size_t align_size;

    /* align the alignment size to 4 byte */
    align = ((align + 0x03) & ~0x03);

    /* get total aligned size */
    align_size = ((size + 0x03) & ~0x03) + align;
    /* allocate memory block from heap */
    ptr = aos_malloc(align_size);
    if (ptr != NULL)
    {
        /* the allocated memory block is aligned */
        if (((unsigned int)ptr & (align - 1)) == 0)
        {
            align_ptr = (void *)((unsigned int)ptr + align);
        }
        else
        {
            align_ptr = (void *)(((unsigned int)ptr + (align - 1)) & ~(align - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((unsigned int *)((unsigned int)align_ptr - sizeof(void *))) = (unsigned int)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

void aos_free_align(void *ptr)
{
    void *real_ptr;

    real_ptr = (void *) * (unsigned int *)((unsigned int)ptr - sizeof(void *));
    aos_free(real_ptr);
}

void *memcpy(void *dst, const void *src, size_t n)
{
#if 0
    char *tmp = (char *)dst, *s = (char *)src;

    while (n--)
        *tmp++ = *s++;

    return dst;
#else

#define UNALIGNED(X, Y)                                               \
                        (((int)X & (sizeof(int) - 1)) | \
                         ((int)Y & (sizeof(int) - 1)))
#define BIGBLOCKSIZE    (sizeof(int) << 2)
#define LITTLEBLOCKSIZE (sizeof(int))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

    char *dst_ptr = (char *)dst;
    char *src_ptr = (char *)src;
    int *aligned_dst;
    int *aligned_src;
    int len = n;

    /* If the size is small, or either SRC or DST is unaligned,
    then punt into the byte copy loop.  This should be rare. */
    if (!TOO_SMALL(len) && !UNALIGNED(src_ptr, dst_ptr))
    {
        aligned_dst = (int *)dst_ptr;
        aligned_src = (int *)src_ptr;

        /* Copy 4X long words at a time if possible. */
        while (len >= BIGBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible. */
        while (len >= LITTLEBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier. */
        dst_ptr = (char *)aligned_dst;
        src_ptr = (char *)aligned_src;
    }

    while (len--)
        *dst_ptr++ = *src_ptr++;

    return dst;
#undef UNALIGNED
#undef BIGBLOCKSIZE
#undef LITTLEBLOCKSIZE
#undef TOO_SMALL
#endif
}
