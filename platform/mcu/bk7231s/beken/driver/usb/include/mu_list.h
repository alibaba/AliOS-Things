/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSB-MicroSW Array/List Utilities API.
 * $Revision: 1.5 $
 */

#ifndef __MUSB_LIST_H__
#define __MUSB_LIST_H__

#include "mu_tools.h"

/**
* MUSB_Array.
* A "dynamic" array which can partially or completely
* use static storage.
* This is most useful for a list whose size is likely to grow
* but not shrink (e.g. a controller's local endpoint resources).
*
* @field wItemSize size of each item
*
* @field wStaticItemCount number of items static buffer has space for
*
* @field pStaticBuffer static buffer, if any
*
* @field wItemCount current item count
*
* @field wDynamicItemCount number of items dynamic buffer has space for
*
* @field pDynamicBuffer grows as needed, by dwItemSize*dwStaticItemCount
*/
typedef struct
{
    uint_fast16_t wItemSize;
    uint_fast16_t wStaticItemCount;
    void *pStaticBuffer;
    uint_fast16_t wItemCount;
    uint_fast16_t wDynamicItemCount;
    void *pDynamicBuffer;
} MUSB_Array;

/**
 * Initialize an array.
 * Call this (portable) or initialize the struct members (non-portable).
 * @param pArray pointer to user-allocated struct or NULL to allocate one
 * @param wItemSize size of each array element
 * @param wStaticItemCount the number of elements for which space is statically allocated
 * @param pStaticBuffer the buffer of statically-allocated elements
 * @return a non-NULL pointer on success (the given pArray if non-NULL)
 * @return NULL on failure
 */
extern MUSB_Array *MUSB_ArrayInit(MUSB_Array *pArray,
                                  uint_fast16_t wItemSize,
                                  uint_fast16_t wStaticItemCount,
                                  void *pStaticBuffer);

/**
 * Count array items.
 * @param pArray array pointer
 * @return the number of items currently in the array
 */
extern uint_fast16_t MUSB_ArrayLength(MUSB_Array *pArray);

/**
 * Get an array item.
 * Locking access to the array, if needed, must be done outside these functions.
 * @param pArray array pointer
 * @param wIndex the index (counting from 0)
 * @return pointer to array element or NULL if not found
 */
extern void *MUSB_ArrayFetch(MUSB_Array *pArray, uint_fast16_t wIndex);

/**
 * Append to array.
 * Append an item to the end of an array.
 * Locking access to the array, if needed, must be done outside these functions.
 * @param pArray array pointer
 * @param pItem item pointer
 * @return TRUE on success
 * @return FALSE on failure (out of memory)
 */
extern uint8_t MUSB_ArrayAppend(MUSB_Array *pArray, const void *pItem);

/**
 * Remove all items from an array.
 * Locking access to the array, if needed, must be done outside these functions.
 * @memo clear array
 * @param pArray array pointer
 */
extern void MUSB_ArrayClear(MUSB_Array *pArray);

/**
 * MUSB_LinkedList (linked list).
 * A generic singly-linked list (can be implemented as static array).
 * @field pItem generic item pointer
 * @field pNext link to next
 * @field dwCount reference count, visitor, etc. (application's choice)
 */
typedef struct _MUSB_LinkedList
{
    void *pItem;
    struct _MUSB_LinkedList *pNext;
    unsigned long dwCount;
} MUSB_LinkedList;

/**
 * REQUIRED list initialization.
 * Initialize the given list so it is ready for use.
 * @param pList list pointer
 */
extern void MUSB_ListInit(MUSB_LinkedList *pList);

/**
 * Count list items.
 * Get the number of items in the given list
 * (NOTE: currently, this is time(N), because it traverses the list).
 * Locking access to the list, if needed, must be done outside these functions.
 * @param pList list pointer
 * @return number of items in list
 */
extern uint_fast16_t MUSB_ListLength(MUSB_LinkedList *pList);

/**
 * Find list item.
 * Find the index'th item in the given list
 * (NOTE: currently, this is time(N), because it traverses the list).
 * @param pList list pointer
 * @param wIndex the index (counting from 0) of the desired item
 * @return item pointer if found
 * @return NULL if no such item
 */
extern void *MUSB_ListFindItem(MUSB_LinkedList *pList, uint_fast16_t wIndex);

/**
 * Find list record.
 * Find the index'th record in the given list
 * (NOTE: currently, this is time(N), because it traverses the list).
 * @param pList list pointer
 * @param wIndex the index (counting from 0) of the desired item
 * @return record pointer if found
 * @return NULL if no such item
 */
extern MUSB_LinkedList *MUSB_ListFindRecord(MUSB_LinkedList *pList,
        uint_fast16_t wIndex);

/**
 * Append list.
 * Append an item to the end of the given list.
 * Locking the list to prevent simultaneous insert/remove calls, if necessary,
 * is the caller's responsibility.
 * (NOTE: currently, this is time(N), because it traverses the list).
 * @param pList list pointer
 * @param pItem item pointer
 * @param dwCount value for count field (application's choice)
 * @return TRUE on success
 * @return FALSE on failure (no such list or out of memory)
 */
extern uint8_t MUSB_ListAppendItem(MUSB_LinkedList *pList, void *pItem,
                                   unsigned long dwCount);

/**
 * Insert into list.
 * Insert an item at the given position in the given list.
 * (NOTE: currently, this is time(N), because it traverses the list).
 * Locking the list to prevent simultaneous append/remove calls, if necessary,
 * is the caller's responsibility.
 * @param pList list pointer
 * @param wItemIndex insertion position (counting from 0)
 * @param pItem item pointer
 * @param dwCount value for count field (application's choice)
 * @return TRUE on success
 * @return FALSE on failure (no such list or out of memory)
 */
extern uint8_t MUSB_ListInsertItem(MUSB_LinkedList *pList,
                                   uint_fast16_t wItemIndex,
                                   void *pItem, unsigned long dwCount);

/**
 * Remove from list.
 * Remove an item from the given list.
 * (NOTE: currently, this is time(N), because it traverses the list).
 * Locking the list to prevent simultaneous insert/append calls, if necessary,
 * is the caller's responsibility.
 * @param pList list pointer
 * @param pItem item pointer
 */
extern void MUSB_ListRemoveItem(MUSB_LinkedList *pList, void *pItem);

#endif	/* multiple inclusion protection */
