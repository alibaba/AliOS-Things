/*
    FreeRTOS V7.0.0 - Copyright (C) 2011 Real Time Engineers Ltd.

	FreeRTOS supports many tools and architectures. V7.0.0 is sponsored by:
	Atollic AB - Atollic provides professional embedded systems development
	tools for C/C++ development, code analysis and test automation.
	See http://www.atollic.com

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************
*/

/*
 * This is the list implementation used by the scheduler.  While it is tailored
 * heavily for the schedulers needs, it is also available for use by
 * application code.
 *
 * xLists can only store pointers to xListItems.  Each xListItem contains a
 * numeric value (xItemValue).  Most of the time the lists are sorted in
 * descending item value order.
 *
 * Lists are created already containing one list item.  The value of this
 * item is the maximum possible that can be stored, it is therefore always at
 * the end of the list and acts as a marker.  The list member pxHead always
 * points to this marker - even though it is at the tail of the list.  This
 * is because the tail contains a wrap back pointer to the true head of
 * the list.
 *
 * In addition to it's value, each list item contains a pointer to the next
 * item in the list (pxNext), a pointer to the list it is in (pxContainer)
 * and a pointer to back to the object that contains it.  These later two
 * pointers are included for efficiency of list manipulation.  There is
 * effectively a two way link between the object containing the list item and
 * the list item itself.
 *
 *
 * \page ListIntroduction List Implementation
 * \ingroup FreeRTOSIntro
 */
//  maintainer: Halley

#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/*
 * Definition of the only type of object that a list can contain.
 */
typedef struct xLIST_ITEM
{
	unsigned long xItemValue;				/*< The value being listed.  In most cases this is used to sort the list in descending order. */
	volatile struct xLIST_ITEM* pxNext;	/*< Pointer to the next xListItem in the list. */
	volatile struct xLIST_ITEM* pxPrevious;/*< Pointer to the previous xListItem in the list. */
	void* pvOwner;							/*< Pointer to the object (normally a TCB) that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself. */
	void* pvContainer;						/*< Pointer to the list in which this list item is placed (if any). */
} xListItem;

typedef struct xMINI_LIST_ITEM
{
	unsigned long xItemValue;
	volatile struct xLIST_ITEM* pxNext;
	volatile struct xLIST_ITEM* pxPrevious;
} xMiniListItem;

/*
 * Definition of the type of queue used by the scheduler.
 */
typedef struct xLIST
{
	volatile unsigned long uxNumberOfItems;
	volatile xListItem* pxIndex;			/*< Used to walk through the list.  Points to the last item returned by a call to pvListGetOwnerOfNextEntry(). */
	volatile xMiniListItem xListEnd;		/*< List item that contains the maximum possible item value meaning it is always at the end of the list and is therefore used as a marker. */
} xList;

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when manipulating whole lists rather than
 * single entries, as sometimes we already know the next/prev entries and we can generate better
 * code by using them directly rather than using the generic single-entry routines.
 */

struct list_head
{
	struct list_head* next, *prev;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) \
struct list_head name = LIST_HEAD_INIT(name)
/*
 * list_entry - get the data struct entry through the struct member
 */
#define list_entry(ptr, type, member) \
				((type*)((char*)(ptr) - (unsigned long)(&((type*)((char*)0))->member)))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for(pos = (head)->next;(pos) != (head);(pos) = (pos)->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
		    for(pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)



static __inline void INIT_LIST_HEAD(struct list_head* list)
{
	list->next = list;
	list->prev = list;
}

/*
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.This is useful for implementing queues.
 */
static __inline void list_add(struct list_head* new, struct list_head* head)
{
	head->prev->next = new;
	new->prev = head->prev;
	new->next = head;
	head->prev = new;
}

/*
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.This is useful for implementing queues.
 */
static __inline void list_add_tail(struct list_head* new, struct list_head* head)
{
	head->next->prev = new;
	new->next = head->next;
	new->prev = head;
	head->next = new;
}


/*
 * Delete a list entry by making the prev/next entries  point to each other.
 *
 * This is only for internal list manipulation where we know  the prev/next entries already!
 */
static __inline void list_del(struct list_head* list)
{
	list->prev->next = list->next;
	list->next->prev = list->prev;
}

/*
 * supplement static initialization macro as vListInitialise to initialize xList -huangyucai20120406
 */
#define XLIST_INITIALIZER(xList) \
{\
	/*.uxNumberOfItems = */0UL,\
	/*.pxIndex = */(xListItem*)(&(xList).xListEnd),\
	{\
		/*.xListEnd.xItemValue = */CFG_MAX_TICKS_DELAY,\
		/*.xListEnd.pxNext = */(xListItem*)&((xList).xListEnd),\
		/*.xListEnd.pxPrevious = */(xListItem*)&((xList).xListEnd),\
	},\
}

/*
 * Access macro to set the owner of a list item.  The owner of a list item
 * is the object (usually a TCB) that contains the list item.
 *
 * \page listSET_LIST_ITEM_OWNER listSET_LIST_ITEM_OWNER
 * \ingroup LinkedList
 */
#define listSET_LIST_ITEM_OWNER(pxListItem, pxOwner)		(pxListItem)->pvOwner = (void*)(pxOwner)

/*
 * Access macro to set the value of the list item.  In most cases the value is
 * used to sort the list in descending order.
 *
 * \page listSET_LIST_ITEM_VALUE listSET_LIST_ITEM_VALUE
 * \ingroup LinkedList
 */
#define listSET_LIST_ITEM_VALUE(pxListItem, xValue)		(pxListItem)->xItemValue = (xValue)

/*
 * Access macro the retrieve the value of the list item.  The value can
 * represent anything - for example a the priority of a task, or the time at
 * which a task should be unblocked.
 *
 * \page listGET_LIST_ITEM_VALUE listGET_LIST_ITEM_VALUE
 * \ingroup LinkedList
 */
#define listGET_LIST_ITEM_VALUE(pxListItem)				((pxListItem)->xItemValue)

/*
 * Access macro the retrieve the value of the list item at the head of a given list.
 *
 * \page listGET_LIST_ITEM_VALUE listGET_LIST_ITEM_VALUE
 * \ingroup LinkedList
 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxList)			((&((pxList)->xListEnd))->pxNext->xItemValue)

/*
 * Access macro to determine if a list contains any items.  The macro will
 * only have the value true if the list is empty.
 *
 * \page listLIST_IS_EMPTY listLIST_IS_EMPTY
 * \ingroup LinkedList
 */
#define listLIST_IS_EMPTY(pxList)		((pxList)->uxNumberOfItems == (unsigned long)0)

/*
 * Access macro to return the number of items in the list.
 */
#define listCURRENT_LIST_LENGTH(pxList)	((pxList)->uxNumberOfItems)

/*
 * Access function to obtain the owner of the next entry in a list.
 *
 * The list member pxIndex is used to walk through a list.  Calling
 * listGET_OWNER_OF_NEXT_ENTRY increments pxIndex to the next item in the list
 * and returns that entries pxOwner parameter.  Using multiple calls to this
 * function it is therefore possible to move through every item contained in
 * a list.
 *
 * The pxOwner parameter of a list item is a pointer to the object that owns
 * the list item.  In the scheduler this is normally a task control block.
 * The pxOwner parameter effectively creates a two way link between the list
 * item and its owner.
 *
 * @param pxList The list from which the next item owner is to be returned.
 *
 * \page listGET_OWNER_OF_NEXT_ENTRY listGET_OWNER_OF_NEXT_ENTRY
 * \ingroup LinkedList
 */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList)										\
{																						\
	xList* const pxConstList = (pxList);												\
	/* Increment the index to the next item and return the item, ensuring */			\
	/* we don't return the marker used at the end of the list.  */						\
	(pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext;							\
	if((pxConstList)->pxIndex == (xListItem*)&((pxConstList)->xListEnd))				\
	{																					\
		(pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext;						\
	}																					\
	(pxTCB) = (pxConstList)->pxIndex->pvOwner;											\
}
/*
 * Access function to obtain the owner of the first entry in a list.  Lists
 * are normally sorted in ascending item value order.
 *
 * This function returns the pxOwner member of the first item in the list.
 * The pxOwner parameter of a list item is a pointer to the object that owns
 * the list item.  In the scheduler this is normally a task control block.
 * The pxOwner parameter effectively creates a two way link between the list
 * item and its owner.
 *
 * @param pxList The list from which the owner of the head item is to be
 * returned.
 *
 * \page listGET_OWNER_OF_HEAD_ENTRY listGET_OWNER_OF_HEAD_ENTRY
 * \ingroup LinkedList
 */
#define listGET_OWNER_OF_HEAD_ENTRY(pxList)  ((&((pxList)->xListEnd))->pxNext->pvOwner)

/*
 * Check to see if a list item is within a list.  The list item maintains a
 * "container" pointer that points to the list it is in.  All this macro does
 * is check to see if the container and the list match.
 *
 * @param pxList The list we want to know if the list item is within.
 * @param pxListItem The list item we want to know if is in the list.
 * @return TRUE is the list item is in the list, otherwise FALSE.
 * pointer against
 */
#define listIS_CONTAINED_WITHIN(pxList, pxListItem) ((pxListItem)->pvContainer == (void*)(pxList))

/*
 * Must be called before a list is used!  This initialises all the members
 * of the list structure and inserts the xListEnd item into the list as a
 * marker to the back of the list.
 *
 * @param pxList Pointer to the list being initialised.
 *
 * \page vListInitialise vListInitialise
 * \ingroup LinkedList
 */
void vListInitialise(xList* pxList);

/*
 * Must be called before a list item is used.  This sets the list container to
 * null so the item does not think that it is already contained in a list.
 *
 * @param pxItem Pointer to the list item being initialised.
 *
 * \page vListInitialiseItem vListInitialiseItem
 * \ingroup LinkedList
 */
void vListInitialiseItem(xListItem* pxItem);

/*
 * Insert a list item into a list.  The item will be inserted into the list in
 * a position determined by its item value (descending item value order).
 *
 * @param pxList The list into which the item is to be inserted.
 *
 * @param pxNewListItem The item to that is to be placed in the list.
 *
 * \page vListInsert vListInsert
 * \ingroup LinkedList
 */
void vListInsert(xList* pxList, xListItem* pxNewListItem);

/*
 * Insert a list item into a list.  The item will be inserted in a position
 * such that it will be the last item within the list returned by multiple
 * calls to listGET_OWNER_OF_NEXT_ENTRY.
 *
 * The list member pvIndex is used to walk through a list.  Calling
 * listGET_OWNER_OF_NEXT_ENTRY increments pvIndex to the next item in the list.
 * Placing an item in a list using vListInsertEnd effectively places the item
 * in the list position pointed to by pvIndex.  This means that every other
 * item within the list will be returned by listGET_OWNER_OF_NEXT_ENTRY before
 * the pvIndex parameter again points to the item being inserted.
 *
 * @param pxList The list into which the item is to be inserted.
 *
 * @param pxNewListItem The list item to be inserted into the list.
 *
 * \page vListInsertEnd vListInsertEnd
 * \ingroup LinkedList
 */
void vListInsertEnd(xList* pxList, xListItem* pxNewListItem);

/*
 * insert the item before index
 */
void vListInsertIndex(xList* pxList, xListItem* pxNewListItem);

/*
 * Remove an item from a list.  The list item has a pointer to the list that
 * it is in, so only the list item need be passed into the function.
 *
 * @param vListRemove The item to be removed.  The item will remove itself from
 * the list pointed to by it's pxContainer parameter.
 *
 * \page vListRemove vListRemove
 * \ingroup LinkedList
 */
void vListRemove(xListItem* pxItemToRemove);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
