#line 1 "memmgr.c"







#line 1 "./memmgr.h"







































































typedef unsigned char byte;
typedef unsigned long ulong;






void memmgr_init();



void* memmgr_alloc(ulong nbytes);



void memmgr_free(void* ap);




void memmgr_print_stats();



#line 9 "memmgr.c"

typedef ulong Align;

union mem_header_union
{
    struct
    {


        union mem_header_union* next;



        ulong size;
    } s;



    Align align_dummy;
};

typedef union mem_header_union mem_header_t;



static mem_header_t base;



static mem_header_t* freep = 0;



static byte pool[8 * 1024] = {0};
static ulong pool_free_pos = 0;


void memmgr_init()
{
    base.s.next = 0;
    base.s.size = 0;
    freep = 0;
    pool_free_pos = 0;
}


void memmgr_print_stats()
{

    mem_header_t* p;

    printf("------ Memory manager stats ------\n\n");
    printf(    "Pool: free_pos = %lu (%lu bytes left)\n\n",
            pool_free_pos,8 * 1024 - pool_free_pos);

    p = (mem_header_t*) pool;

    while (p < (mem_header_t*) (pool + pool_free_pos))
    {
        printf(    "  * Addr: 0x%8lu; Size: %8lu\n",
                p, p->s.size);

        p += p->s.size;
    }

    printf("\nFree list:\n\n");

    if (freep)
    {
        p = freep;

        while (1)
        {
            printf(    "  * Addr: 0x%8lu; Size: %8lu; Next: 0x%8lu\n",
                    p, p->s.size, p->s.next);

            p = p->s.next;

            if (p == freep)
                break;
        }
    }
    else
    {
        printf("Empty\n");
    }

    printf("\n");

}


static mem_header_t* get_mem_from_pool(ulong nquantas)
{
    ulong total_req_size;

    mem_header_t* h;

    if (nquantas < 16)
        nquantas = 16;

    total_req_size = nquantas * sizeof(mem_header_t);

    if (pool_free_pos + total_req_size <= 8 * 1024)
    {
        h = (mem_header_t*) (pool + pool_free_pos);
        h->s.size = nquantas;
        memmgr_free((void*) (h + 1));
        pool_free_pos += total_req_size;
    }
    else
    {
        return 0;
    }

    return freep;
}










void* memmgr_alloc(ulong nbytes)
{
    mem_header_t* p;
    mem_header_t* prevp;





    ulong nquantas = (nbytes + sizeof(mem_header_t) - 1) / sizeof(mem_header_t) + 1;




    if ((prevp = freep) == 0)
    {
        base.s.next = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.next; ; prevp = p, p = p->s.next)
    {

        if (p->s.size >= nquantas)
        {

            if (p->s.size == nquantas)
            {



                prevp->s.next = p->s.next;
            }
            else
            {
                p->s.size -= nquantas;
                p += p->s.size;
                p->s.size = nquantas;
            }

            freep = prevp;
            return (void*) (p + 1);
        }







        else if (p == freep)
        {
            if ((p = get_mem_from_pool(nquantas)) == 0)
            {



                return 0;
            }
        }
    }
}







void memmgr_free(void* ap)
{
    mem_header_t* block;
    mem_header_t* p;


    block = ((mem_header_t*) ap) - 1;




    for (p = freep; !(block > p && block < p->s.next); p = p->s.next)
    {





        if (p >= p->s.next && (block > p || block < p->s.next))
            break;
    }



    if (block + block->s.size == p->s.next)
    {
        block->s.size += p->s.next->s.size;
        block->s.next = p->s.next->s.next;
    }
    else
    {
        block->s.next = p->s.next;
    }



    if (p + p->s.size == block)
    {
        p->s.size += block->s.size;
        p->s.next = block->s.next;
    }
    else
    {
        p->s.next = block;
    }

    freep = p;
}
