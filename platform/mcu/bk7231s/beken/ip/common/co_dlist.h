/**
 ****************************************************************************************
 *
 * @file co_dlist.h
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 * @brief Double linked list structures definitions
 *
 ****************************************************************************************
 */

#ifndef _CO_DLIST_H_
#define _CO_DLIST_H_

/**
 ****************************************************************************************
 * @defgroup CO_DLIST CO_DLIST
 * @ingroup COMMON
 * @brief  Double linked list management.
 *
 * This module contains the double linked list structures and handling functions.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// standard includes
#include "co_int.h"
#include "co_bool.h"
// for NULL and size_t
#include <stddef.h>

// for target dependent directives
#include "rwnx_config.h"

// for __INLINE
#include "compiler.h"


/*
 * STRUCTURE DECLARATIONS
 ****************************************************************************************
 */
/// Double linked list element header.
struct co_dlist_hdr
{
    /// Next element in the double linked list (NULL if last).
    struct co_dlist_hdr *next;
    /// Previous element in the double linked list (NULL if first).
    struct co_dlist_hdr *prev;
};

/// Double link list.
struct co_dlist
{
    /// First element.
    struct co_dlist_hdr *first;
    /// Last element.
    struct co_dlist_hdr *last;
    /// Number of element in the list.
    uint32_t cnt;

    #if NX_DEBUG
    /// Maximum number of elements in the list.
    uint32_t maxcnt;
    /// Minimum number of elements in the list.
    uint32_t mincnt;
    #endif // NX_DEBUG
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize the double linked list.
 * @param[in, out] list Pointer to the double linked list.
 ****************************************************************************************
 */
void co_dlist_init(struct co_dlist *list);

/**
 ****************************************************************************************
 * @brief Add an element at the end of the double linked list.
 *
 * @param[in,out] list     Pointer to the double linked list.
 * @param[in,out] list_hdr Pointer to the element to add at the end of the list.
 ****************************************************************************************
 */
void co_dlist_push_back(struct co_dlist *list,
                        struct co_dlist_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Add an element at the beginning of the double linked list.
 *
 * @param[in, out] list     Pointer to the double linked list.
 * @param[in, out] list_hdr Pointer to the element to add at the beginning of the list.
 ****************************************************************************************
 */
void co_dlist_push_front(struct co_dlist *list,
                         struct co_dlist_hdr *list_hdr);

/**
 ****************************************************************************************
 * @brief Extract the first element of the double linked list.
 *
 * @param[in, out] list Pointer to the double linked list.
 *
 * @return The pointer to the element extracted, and NULL if the list is empty.
 ****************************************************************************************
 */
struct co_dlist_hdr* co_dlist_pop_front(struct co_dlist *list);

/**
 ****************************************************************************************
 * @brief Search for a specific element in the list, and extract it if found.
 *
 * @param[in, out] list     Pointer to the double linked list.
 * @param[in]      list_hdr Pointer to the searched element.
 ****************************************************************************************
 */
void co_dlist_extract(struct co_dlist *list,
                      struct co_dlist_hdr const *list_hdr);

/**
 ****************************************************************************************
 * @brief Test if the list is empty.
 *
 * @param[in] list Pointer to the double linked list.
 *
 * @return true if the list is empty, false else otherwise.
 ****************************************************************************************
 */
__INLINE bool co_dlist_is_empty(struct co_dlist const * list)
{
    bool listempty;
    listempty = (list->first == NULL);
    return (listempty);
}

/**
 ****************************************************************************************
 * @brief Return the number of element of the list.
 *
 * @param[in, out] list Pointer to the double linked list.
 *
 * @return The number of elements in the list.
 ****************************************************************************************
 */
__INLINE uint32_t co_dlist_cnt(struct co_dlist const * list)
{
    return(list->cnt);
}

/**
 ****************************************************************************************
 * @brief Pick the first element from the list without removing it.
 *
 * @param[in] list Pointer to the double linked list.
 *
 * @return First element address. Returns NULL pointer if the list is empty.
 ****************************************************************************************
 */
__INLINE struct co_dlist_hdr *co_dlist_pick(struct co_dlist const * list)
{
    return(list->first);
}


/**
 ****************************************************************************************
 * @brief Return following element of a list element.
 *
 * @param[in] list_hdr Pointer to the double linked list element.
 *
 * @return The pointer to the next element.
 ****************************************************************************************
 */
__INLINE struct co_dlist_hdr *co_dlist_next(struct co_dlist_hdr const * list_hdr)
{
    return(list_hdr->next);
}

/// @} end of group CODLIST


#endif // _CO_DLIST_H_
