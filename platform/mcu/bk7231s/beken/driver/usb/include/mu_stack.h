/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * MUSB-MicroSW USB Software Platform Stack API.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_STACK_H__
#define __MUSB_STACK_H__

#include "mu_tools.h"
#include "mu_list.h"

/**
 * MUSB_Stack.
 * A LIFO data container.
 * This stack is a Last-In-First-Out container for arbitrary data,
 * where each data element is assumed the same size.
 * A static data buffer can be used, but the stack can still grow beyond it.
 * @field wNextTopIndex index to use for next push,
 * which is one beyond the current top of the stack
 * @field Array array implementation this stack uses
 */
typedef struct
{
    uint_fast16_t wNextTopIndex;
    MUSB_Array Array;
} MUSB_Stack;

/**
 * Initialize a stack (a LIFO data container).
 * Call this (portable) or initialize the struct members (non-portable).
 * A static data buffer can be used, but the stack can still grow beyond it.
 * @param pStack pointer to user-allocated struct or NULL to allocate one
 * @param wItemSize size of each array element
 * @param wStaticItemCount the number of elements for which space is statically allocated
 * @param pStaticBuffer the buffer of statically-allocated elements
 * @return a non-NULL pointer on success (the given pStack if non-NULL)
 * @return NULL on failure
 */
extern MUSB_Stack *MUSB_StackInit(MUSB_Stack *pStack,
                                  uint_fast16_t wItemSize,
                                  uint_fast16_t wStaticItemCount,
                                  void *pStaticBuffer);

/**
 * Remove all items from a stack.
 * Locking access to the stack, if needed, must be done outside these functions.
 * @param pStack stack pointer
 * @return TRUE on success
 * @return FALSE on failure (invalid stack)
 */
extern uint8_t MUSB_StackClear(MUSB_Stack *pStack);

/**
 * Count stack items.
 * @param pStack stack pointer
 * @return the number of items currently on the stack
 */
extern uint_fast16_t MUSB_StackSize(MUSB_Stack *pStack);

/**
 * Get the top stack item, without removing it.
 * @param pStack stack pointer
 * @return pointer to element or NULL if not found
 */
extern void *MUSB_StackTop(MUSB_Stack *pStack);

/**
 * Pop an item from the stack.
 * This removes and returns the top item.
 * Locking access to the stack, if needed, must be done outside these functions.
 * @param pStack stack pointer
 * @param pItem where to store the item
 * @return TRUE on success
 * @return FALSE on failure (invalid or empty stack)
 */
extern uint8_t MUSB_StackPop(MUSB_Stack *pStack, void *pItem);

/**
 * Push an item onto the stack.
 * The given item, if possible, is made the new top of the stack.
 * Locking access to the stack, if needed, must be done outside these functions.
 * @param pStack stack pointer
 * @param pItem pointer to the item
 * @return TRUE on success
 * @return FALSE on failure (invalid or full stack)
 */
extern uint8_t MUSB_StackPush(MUSB_Stack *pStack, const void *pItem);

#endif	/* multiple inclusion protection */
