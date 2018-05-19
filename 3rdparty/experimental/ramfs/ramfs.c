/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "k_api.h"
#include "ramfs.h"

#define LL_NODE_META_SIZE (sizeof(ll_node_t*) + sizeof(ll_node_t*))
#define LL_PREV_P_OFFSET(ll_p) (ll_p->n_size)
#define LL_NEXT_P_OFFSET(ll_p) (ll_p->n_size + sizeof(ll_node_t*))

static ramfs_ent_t* ramfs_ent_get(const char * fn);
static ramfs_ent_t* ramfs_ent_new(const char * fn);
static void * ll_ins_head(ll_t * ll_p);
static void ll_rem(ll_t  * ll_p, void * node_p);
static void * ll_get_head(ll_t * ll_p);
static void * ll_get_tail(ll_t * ll_p);
static void * ll_get_next(ll_t * ll_p, void * n_act);
static void * ll_get_prev(ll_t * ll_p, void * n_act);
static void node_set_prev(ll_t * ll_p, ll_node_t* act, ll_node_t* prev);
static void node_set_next(ll_t * ll_p, ll_node_t* act, ll_node_t* next);
static void node_set_prev(ll_t * ll_p, ll_node_t* act, ll_node_t* prev);
static void node_set_next(ll_t * ll_p, ll_node_t* act, ll_node_t* next);
static void ll_init(ll_t * ll_p, uint32_t n_size);

static ll_t file_ll;
static bool inited = false;

/**
 * Create a driver for ufs and initialize it.
 */
void ramfs_init(void)
{
    ll_init(&file_ll, sizeof(ramfs_ent_t));

    inited = true;
}

/**
 * Give the state of the ufs
 * @return true if ufs is initialized and can be used else false
 */
bool ramfs_ready(void)
{
    return inited;
}

/**
 * Open a file in ufs
 * @param file_p pointer to a ramfs_file_t variable
 * @param fn name of the file. There are no directories so e.g. "myfile.txt"
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return RAMFS_RES_OK: no error, the file is opened
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_open (void * file_p, const char * fn, ramfs_mode_t mode)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    ramfs_ent_t* ent = ramfs_ent_get(fn);

    fp->ent = NULL;
    
    /*If the file not exists ...*/
    if( ent == NULL) { 
        if((mode & RAMFS_MODE_WR) != 0) {  /*Create the file if opened for write*/
            ent = ramfs_ent_new(fn);   
            if(ent == NULL) return RAMFS_RES_FULL; /*No space for the new file*/
        } else { 
            return RAMFS_RES_NOT_EX;       /*Can not read not existing file*/
        }
    } 
    
    /*Can not write already opened and const data files*/
    if((mode & RAMFS_MODE_WR) != 0) {
        if(ent->oc != 0) return RAMFS_RES_LOCKED;
        if(ent->const_data != 0) return RAMFS_RES_DENIED;
    }
    
    /*No error, the file can be opened*/
    fp->ent = ent;
    fp->ent->ar = mode & RAMFS_MODE_RD ? 1 : 0;
    fp->ent->aw = mode & RAMFS_MODE_WR ? 1 : 0;
    fp->rwp = 0;
    ent->oc ++;
    
    return RAMFS_RES_OK;
}

/**
 * Create a file with a constant data
 * @param fn name of the file (directories are not supported)
 * @param const_p pointer to a constant data
 * @param len length of the data pointed by 'const_p' in bytes
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_create_const(const char * fn, const void * const_p, uint32_t len)
{
    ramfs_file_t file;
    ramfs_res_t res;
    
    /*Error if the file already exists*/
    res = ramfs_open(&file, fn, RAMFS_MODE_RD);
    if(res == RAMFS_RES_OK) {
        ramfs_close(&file);
        return RAMFS_RES_DENIED;
    }
    
    ramfs_close(&file);
    
    res = ramfs_open(&file, fn, RAMFS_MODE_WR);
    if(res != RAMFS_RES_OK) return res;
    
    ramfs_ent_t* ent = file.ent;
    
    if(ent->data_d != NULL) return RAMFS_RES_DENIED;
    
    ent->data_d = (void *) const_p;
    ent->size = len;
    ent->const_data = 1;
    
    res = ramfs_close(&file);
    if(res != RAMFS_RES_OK) return res;
    
    return RAMFS_RES_OK;
}

/**
 * Close an opened file 
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open) 
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_close (void * file_p)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    
    if(fp->ent == NULL) return RAMFS_RES_OK;
    
    /*Decrement the Open counter*/
    if(fp->ent->oc > 0) {
        fp->ent->oc--;
    }
    
    return RAMFS_RES_OK;
}

/**
 * Remove a file. The file can not be opened.
 * @param fn '\0' terminated string
 * @return RAMFS_RES_OK: no error, the file is removed
 *         RAMFS_RES_DENIED: the file was opened, remove failed
 */
ramfs_res_t ramfs_remove(const char * fn)
{
    ramfs_ent_t* ent = ramfs_ent_get(fn);
    
    /*Can not be deleted is opened*/
    if(ent->oc != 0) return RAMFS_RES_DENIED;
    
    ll_rem(&file_ll, ent);
    krhino_mm_free(ent->fn_d);
    ent->fn_d = NULL;
    if(ent->const_data == 0){
        krhino_mm_free(ent->data_d);
        ent->data_d = NULL;
    }
    
    krhino_mm_free(ent);
    
    return RAMFS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open )
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read 
 * @param br the real number of read bytes (Byte Read)
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    
    ramfs_ent_t* ent = fp->ent;
    *br = 0;
    
    if(ent->data_d == NULL || ent->size == 0) { /*Don't read empty files*/
        return RAMFS_RES_OK;
    } else if(fp->ent->ar == 0) {    /*The file is not opened for read*/
        return RAMFS_RES_DENIED;
    } 

    /*No error, read the file*/
    if(fp->rwp + btr > ent->size) {  /*Check too much bytes read*/
       *br =  ent->size - fp->rwp;
    } else {
       *br = btr;
    }

    /*Read the data*/    
    uint8_t * data8_p;
    if(ent->const_data == 0) {
        data8_p = (uint8_t*) ent->data_d;
    } else {
        data8_p = ent->data_d;
    }
    
    data8_p += fp->rwp;
    memcpy(buf, data8_p, *br);

    fp->rwp += *br; /*Refresh the read write pointer*/
    
    return RAMFS_RES_OK;
}

/**
 * Write data to an opened file
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open)
 * @param buf pointer to a memory block which content will be written
 * @param btw the number Bytes To Write
 * @param bw The real number of written bytes (Byte Written)
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_write (void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    *bw = 0;
    
    if(fp->ent->aw == 0) return RAMFS_RES_DENIED; /*Not opened for write*/
    
    ramfs_ent_t* ent = fp->ent;
    
    /*Reallocate data array if it necessary*/
    uint32_t new_size = fp->rwp + btw;
    if(new_size > ent->size) {
        uint8_t* new_data = krhino_mm_realloc(ent->data_d, new_size);
        if(new_data == NULL) return RAMFS_RES_FULL; /*Cannot allocate the new memory*/
            
        ent->data_d = new_data;
        ent->size = new_size;
    }
    
    /*Write the file*/
    uint8_t * data8_p = (uint8_t*) ent->data_d;
    data8_p += fp->rwp;
    memcpy(data8_p, buf, btw);
    *bw = btw;
    fp->rwp += *bw;
    
    return RAMFS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open )
 * @param pos the new position of read write pointer
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_seek (void * file_p, uint32_t pos)
{    
    ramfs_file_t * fp = file_p;    /*Convert type*/
    ramfs_ent_t* ent = fp->ent;

    /*Simply move the rwp before EOF*/
    if(pos < ent->size) {
        fp->rwp = pos;
    } else { /*Expand the file size*/
        if(fp->ent->aw == 0) return RAMFS_RES_DENIED;       /*Not opened for write*/
        
        uint8_t* new_data = krhino_mm_realloc(ent->data_d, pos);
        if(new_data == NULL) return RAMFS_RES_FULL; /*Out of memory*/
            
        ent->data_d = new_data;
        ent->size = pos;
        fp->rwp = pos; 
    }
    
    return RAMFS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open )
 * @param pos_p pointer to to store the result
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_tell (void * file_p, uint32_t * pos_p)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    
    *pos_p = fp->rwp;
    
    return RAMFS_RES_OK;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open )
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_trunc (void * file_p)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    ramfs_ent_t* ent = fp->ent;
    
    if(fp->ent->aw == 0) return RAMFS_RES_DENIED; /*Not opened for write*/
    
    void * new_data = krhino_mm_realloc(ent->data_d, fp->rwp);
    if(new_data == NULL) return RAMFS_RES_FULL; /*Out of memory*/
    
    ent->data_d = new_data;
    ent->size = fp->rwp;
    
    return RAMFS_RES_OK;
}

/**
 * Give the size of the file in bytes
 * @param file_p file_p pointer to an 'ufs_file_t' variable. (opened with ramfs_open )
 * @param size_p pointer to store the size
 * @return RAMFS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
ramfs_res_t ramfs_size (void * file_p, uint32_t * size_p)
{
    ramfs_file_t * fp = file_p;    /*Convert type*/
    ramfs_ent_t* ent = fp->ent;
    
    *size_p = ent->size;
    
    return RAMFS_RES_OK;
}

/**
 * get access info
 * @param path uFS doesn't support folders so it has to be ""
 * @param mode the info to get
 * @return RAMFS_RES_OK or any error from ramfs_res_t enum
 */
ramfs_res_t ramfs_access(const char * path, int mode)
{
    ramfs_ent_t* ent = ramfs_ent_get(path);
    
    /*If the file not exists ...*/
    if( ent == NULL) { 
        return RAMFS_RES_DENIED;       /*Can not read not existing file*/
    } else if((mode & F_OK ) != 0){
        return RAMFS_RES_OK;
    }

    /*Can not write already opened and const data files*/
    if((mode & R_OK ) != 0) {
        if(ent->ar == 0) {
            return RAMFS_RES_DENIED;
        } else {
            return RAMFS_RES_OK;
        }
    }

    if((mode & W_OK) != 0) {
        if(ent->aw == 0) {
            return RAMFS_RES_DENIED;
        } else {
            return RAMFS_RES_OK;
        }
    }		

    return RAMFS_RES_OK;
}

/**
 * Create a directory
 * @param path the path of file
 * @return RAMFS_RES_OK or any error from ramfs_res_t enum
 */
ramfs_res_t ramfs_mkdir(const char * path)
{
    return RAMFS_RES_OK;
}

/**
 * Initialize a ramfs_read_dir_t variable to directory reading
 * @param rddir_p pointer to a 'ramfs_dir_t' variable
 * @param path the path of file
 * @return RAMFS_RES_OK or any error from ramfs_res_t enum
 */
ramfs_res_t ramfs_dir_open(void * rddir_p, const char * path)
{
    ramfs_dir_t * ramfs_rddir_p = rddir_p;
    
    ramfs_rddir_p->last_ent = NULL;

    return RAMFS_RES_OK;
}

/**
 * Read the next file name
 * @param dir_p pointer to an initialized 'ramfs_dir_t' variable
 * @param fn pointer to buffer to sore the file name
 * @return RAMFS_RES_OK or any error from ramfs_res_t enum
 */
ramfs_res_t ramfs_dir_read(void * dir_p, char * fn)
{
    ramfs_dir_t * ufs_dir_p = dir_p;
    
    if(ufs_dir_p->last_ent == NULL) {
        ufs_dir_p->last_ent = ll_get_head(&file_ll);
    } else {
        ufs_dir_p->last_ent = ll_get_next(&file_ll, ufs_dir_p->last_ent);
    }
    
    if(ufs_dir_p->last_ent != NULL) {
       strcpy(fn, ufs_dir_p->last_ent->fn_d);
    } else {
        fn[0] = '\0';
    }
    
    return RAMFS_RES_OK;
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'ramfs_dir_t' variable
 * @return RAMFS_RES_OK or any error from ramfs_res_t enum
 */
ramfs_res_t ramfs_dir_close(void * rddir_p)
{
    (void)rddir_p;
    return RAMFS_RES_OK;
}

/**
 * Give the size of a drive
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free site [kB]
 * @return RAMFS_RES_OK or any error from 'ramfs_res_t'
 */
ramfs_res_t ramfs_free (uint32_t * total_p, uint32_t * free_p)
{
    return RAMFS_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Gives the ramfs_entry from a filename
 * @param fn filename ('\0' terminated string)
 * @return pointer to the dynamically allocated entry with 'fn' filename.
 *         NULL if no entry found with that name.
 */
static ramfs_ent_t* ramfs_ent_get(const char * fn)
{
    ramfs_ent_t* fp;
    
    LL_READ(file_ll, fp) {
        if(strcmp(fp->fn_d, fn) == 0) {
            return fp;
        } 
    }
    
    return NULL;
}

/**
 * Create a new entry with 'fn' filename 
 * @param fn filename ('\0' terminated string)
 * @return pointer to the dynamically allocated new entry.
 *         NULL if no space for the entry.
 */
static ramfs_ent_t* ramfs_ent_new(const char * fn)
{
    ramfs_ent_t* new_ent = NULL;
    new_ent = ll_ins_head(&file_ll);                 /*Create a new file*/
    if(new_ent == NULL) {
        return NULL;
    }
    
    new_ent->fn_d = krhino_mm_alloc(strlen(fn)  + 1); /*Save the name*/
    strcpy(new_ent->fn_d, fn);
    new_ent->data_d = NULL;
    new_ent->size = 0;
    new_ent->oc = 0;
    new_ent->const_data = 0;
    
    return new_ent;
}

/**
 * Initialize linked list
 * @param ll_dsc pointer to ll_dsc variable
 * @param n_size the size of 1 node in bytes 
 */
void ll_init(ll_t * ll_p, uint32_t n_size)
{
    ll_p->head = NULL;
    ll_p->tail = NULL;

    if(n_size & 0x3) {
        n_size &= ~0x3;
        n_size += 4;
    }

    ll_p->n_size = n_size;   
}

/**
 * Add a new head to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new head
 */
void * ll_ins_head(ll_t * ll_p)
{
    ll_node_t* n_new;
    
    n_new = krhino_mm_alloc(ll_p->n_size + LL_NODE_META_SIZE);
    
    if(n_new != NULL) { 
        node_set_prev(ll_p, n_new, NULL);           /*No prev. before the new head*/
        node_set_next(ll_p, n_new, ll_p->head);     /*After new comes the old head*/
        
        if(ll_p->head != NULL) { /*If there is old head then before it goes the new*/
            node_set_prev(ll_p, ll_p->head, n_new);
        }
        
        ll_p->head = n_new;         /*Set the new head in the dsc.*/
        if(ll_p->tail == NULL) {/*If there is no tail (1. node) set the tail too*/
            ll_p->tail = n_new;
        }
    }
    
    return n_new;
}

/**
 * Add a new tail to a linked list
 * @param ll_p pointer to linked list
 * @return pointer to the new tail
 */
void * ll_ins_tail(ll_t * ll_p)
{   
    ll_node_t* n_new;
    
    n_new = krhino_mm_alloc(ll_p->n_size + LL_NODE_META_SIZE);
    
    if(n_new != NULL) {
        node_set_next(ll_p, n_new, NULL); /*No next after the new tail*/
        node_set_prev(ll_p, n_new, ll_p->tail); /*The prev. before new is tho old tail*/
        if(ll_p->tail != NULL) {    /*If there is old tail then the new comes after it*/
            node_set_next(ll_p, ll_p->tail, n_new);
        }
        
        ll_p->tail = n_new;      /*Set the new tail in the dsc.*/
        if(ll_p->head == NULL) { /*If there is no head (1. node) set the head too*/
            ll_p->head = n_new;
        }
    }
    
    return n_new;
}


/**
 * Remove the node 'node_p' from 'll_p' linked list. 
 * It Dose not free the the memory of node.
 * @param ll_p pointer to the linked list of 'node_p'
 * @param node_p pointer to node in 'll_p' linked list
 */
void ll_rem(ll_t  * ll_p, void * node_p)
{
    if(ll_get_head(ll_p) == node_p) {
        /*The new head will be the node after 'n_act'*/
        ll_p->head = ll_get_next(ll_p, node_p);
        if(ll_p->head == NULL) {
            ll_p->tail = NULL;
        }
        else {
            node_set_prev(ll_p, ll_p->head, NULL);
        }   
    }
    else if(ll_get_tail(ll_p) == node_p) {
        /*The new tail will be the  node before 'n_act'*/
        ll_p->tail = ll_get_prev(ll_p, node_p);
        if(ll_p->tail == NULL) {
            ll_p->head = NULL;
        }
        else {
            node_set_next(ll_p, ll_p->tail, NULL);
        }
    }
    else
    {
        ll_node_t* n_prev = ll_get_prev(ll_p, node_p);
        ll_node_t* n_next = ll_get_next(ll_p, node_p);
        
        node_set_next(ll_p, n_prev, n_next);
        node_set_prev(ll_p, n_next, n_prev);
    }
}

/**
 * Remove and free all elements from a linked list. The list remain valid but become empty.
 * @param ll_p pointer to linked list
 */
void ll_clear(ll_t * ll_p)
{
	void * i;
	void * i_next;

	i = ll_get_head(ll_p);
	i_next = NULL;

	while(i != NULL) {
		i_next = ll_get_next(ll_p, i);

		ll_rem(ll_p, i);
		krhino_mm_free(i);

		i = i_next;
	}
}

/**
 * Move a node to a new linked list
 * @param ll_ori_p pointer to the original (old) linked list
 * @param ll_new_p pointer to the new linked list
 * @param node pointer to a node
 */
void ll_chg_list(ll_t * ll_ori_p, ll_t * ll_new_p, void * node)
{
    ll_rem(ll_ori_p, node);
    
    /*Set node as head*/
    node_set_prev(ll_new_p, node, NULL);      
    node_set_next(ll_new_p, node, ll_new_p->head);
    
    if(ll_new_p->head != NULL) { /*If there is old head then before it goes the new*/
        node_set_prev(ll_new_p, ll_new_p->head, node);
    }

    ll_new_p->head = node;        /*Set the new head in the dsc.*/
    if(ll_new_p->tail == NULL) {     /*If there is no tail (first node) set the tail too*/
        ll_new_p->tail = node;
    }
}

/**
 * Return with head node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p' 
 */
void * ll_get_head(ll_t * ll_p)
{   
    void * head = NULL;
    
    if(ll_p != NULL)    {
        head = ll_p->head;
    }
    
    return head;
}

/**
 * Return with tail node of the linked list
 * @param ll_p pointer to linked list
 * @return pointer to the head of 'll_p'
 */
void * ll_get_tail(ll_t * ll_p)
{
    void * tail = NULL;
    
    if(ll_p != NULL)    {
        tail = ll_p->tail;
    }
    
    return tail;
}

/**
 * Return with the pointer of the next node after 'n_act' 
 * @param ll_p pointer to linked list
 * @param n_act pointer a node 
 * @return pointer to the next node 
 */
void * ll_get_next(ll_t * ll_p, void * n_act)
{
    void * next = NULL;
 
    if(ll_p != NULL)    {
        ll_node_t* n_act_d = n_act;
        memcpy(&next, n_act_d + LL_NEXT_P_OFFSET(ll_p), sizeof(void *));
    } 
    
    return next;
}

/**
 * Return with the pointer of the previous node after 'n_act' 
 * @param ll_p pointer to linked list
 * @param n_act pointer a node 
 * @return pointer to the previous node 
 */
void * ll_get_prev(ll_t * ll_p, void * n_act)
{
    void * prev = NULL;
 
    if(ll_p != NULL) {
        ll_node_t* n_act_d = n_act;
        memcpy(&prev, n_act_d + LL_PREV_P_OFFSET(ll_p), sizeof(void *));
    }
    
    return prev;
}

void ll_swap(ll_t * ll_p, void * n1_p, void * n2_p)
{
    (void)(ll_p);
    (void)(n1_p);
    (void)(n2_p);
    /*TODO*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Set the 'pervious node pointer' of a node 
 * @param ll_p pointer to linked list
 * @param act pointer to a node which prev. node pointer should be set
 * @param prev pointer to a node which should be the previous node before 'act'
 */
static void node_set_prev(ll_t * ll_p, ll_node_t* act, ll_node_t* prev)
{
    memcpy(act + LL_PREV_P_OFFSET(ll_p), &prev, sizeof(ll_node_t*));
}

/**
 * Set the 'next node pointer' of a node 
 * @param ll_p pointer to linked list
 * @param act pointer to a node which next node pointer should be set
 * @param next pointer to a node which should be the next node before 'act'
 */
static void node_set_next(ll_t * ll_p, ll_node_t* act, ll_node_t* next)
{
    memcpy(act + LL_NEXT_P_OFFSET(ll_p), &next, sizeof(ll_node_t*));
}
