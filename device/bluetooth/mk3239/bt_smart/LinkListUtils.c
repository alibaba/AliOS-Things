/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "LinkListUtils.h"
#include "debug.h"

OSStatus linked_list_init( linked_list_t *list )
{
    OSStatus err = kNoErr;
    require_action( list, exit, err = kParamErr);
    memset( list, 0, sizeof( *list ) );
exit:
    return err;
}

OSStatus linked_list_deinit( linked_list_t *list )
{
    OSStatus err = kNoErr;
    linked_list_node_t *current;

    require_action( list, exit, err = kParamErr);

    /* Traverse through all nodes and detach them */
    current = list->front;

    while ( current != NULL ) {
        linked_list_node_t *next = current->next;

        /* Detach node from the list */
        current->prev = NULL;
        current->next = NULL;

        /* Move to the next node */
        current = next;
    }

    memset( list, 0, sizeof( *list ) );

exit:
    return err;
}

OSStatus linked_list_get_count( linked_list_t *list, uint32_t *count )
{
    OSStatus err = kNoErr;

    require_action( list, exit, err = kParamErr);

    *count = list->count;

exit:
    return err;
}

OSStatus linked_list_set_node_data( linked_list_node_t *node, const void *data )
{
    OSStatus err = kNoErr;

    require_action( node, exit, err = kParamErr);

    node->data = (void *)data;

exit:
    return err;
}

OSStatus linked_list_get_front_node( linked_list_t *list, linked_list_node_t **front_node )
{
    OSStatus err = kNoErr;

    require_action( list, exit, err = kParamErr);

    *front_node = NULL;
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    *front_node = list->front;

exit:
    return err;
}

OSStatus linked_list_get_rear_node( linked_list_t *list, linked_list_node_t **rear_node )
{
    OSStatus err = kNoErr;

    require_action( list, exit, err = kParamErr);

    *rear_node = NULL;
    require_action( list->count, exit, err = kNotFoundErr);

    *rear_node = list->rear;

exit:
    return err;
}

OSStatus linked_list_find_node( linked_list_t *list, linked_list_compare_callback_t callback, void *user_data,
                                linked_list_node_t **node_found )
{
    OSStatus err = kNotFoundErr;
    linked_list_node_t *current;

    require_action( list && callback && node_found, exit, err = kParamErr);

    require_action_quiet(list->count, exit, err = kNotFoundErr);

    current = list->front;

    while ( current != NULL ) {
        if ( callback( current, user_data ) ) {
            *node_found = current;
            err = kNoErr;
            goto exit;
        }

        current = current->next;
    }

exit:
    return err;
}

OSStatus linked_list_insert_node_at_front( linked_list_t *list, linked_list_node_t *node )
{
    OSStatus err = kNoErr;

    require_action( list && node, exit, err = kParamErr);

    if ( list->count == 0 ) {
        list->front = node;
        list->rear  = node;
        node->prev  = NULL;
        node->next  = NULL;
    } else {
        node->prev        = NULL;
        node->next        = list->front;
        list->front->prev = node;
        list->front       = node;
    }

    list->count++;

exit:
    return err;
}

OSStatus linked_list_insert_node_at_rear( linked_list_t *list, linked_list_node_t *node )
{
    OSStatus err = kNoErr;

    require_action( list && node, exit, err = kParamErr);

    if ( list->count == 0 ) {
        list->front = node;
        list->rear  = node;
        node->prev  = NULL;
        node->next  = NULL;
    } else {
        node->next       = NULL;
        node->prev       = list->rear;
        list->rear->next = node;
        list->rear       = node;
    }

    list->count++;

exit:
    return err;
}

OSStatus linked_list_insert_node_before( linked_list_t *list, linked_list_node_t *reference_node,
                                         linked_list_node_t *node_to_insert )
{
    /* WARNING: User must make sure that reference_node is in the list */
    OSStatus err = kNoErr;

    require_action( list && reference_node && node_to_insert, exit, err = kParamErr);
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    if ( reference_node == list->front ) {
        err = linked_list_insert_node_at_front( list, node_to_insert );
    } else {
        node_to_insert->prev       = reference_node->prev;
        node_to_insert->prev->next = node_to_insert;
        node_to_insert->next       = reference_node;
        reference_node->prev       = node_to_insert;

        list->count++;
    }

exit:
    return err;
}

OSStatus linked_list_insert_node_after( linked_list_t *list, linked_list_node_t *reference_node,
                                        linked_list_node_t *node_to_insert )
{
    /* WARNING: User must make sure that reference_node is in the list */
    OSStatus err = kNoErr;

    require_action( list && reference_node && node_to_insert, exit, err = kParamErr);
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    if ( reference_node == list->rear ) {
        err = linked_list_insert_node_at_rear( list, node_to_insert );
    } else {
        node_to_insert->prev       = reference_node;
        node_to_insert->next       = reference_node->next;
        reference_node->next->prev = node_to_insert;
        reference_node->next       = node_to_insert;

        list->count++;
    }
exit:
    return err;
}

OSStatus linked_list_remove_node( linked_list_t *list, linked_list_node_t *node )
{
    /* WARNING: User must make sure that node to remove is in the list */
    OSStatus err = kNoErr;

    require_action( list && node, exit, err = kParamErr);
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    if ( list->count == 1 ) {
        list->front = NULL;
        list->rear  = NULL;
    } else if ( node == list->front ) {
        linked_list_node_t *removed_node;

        return linked_list_remove_node_from_front( list, &removed_node );
    } else if ( node == list->rear ) {
        linked_list_node_t *removed_node;

        return linked_list_remove_node_from_rear( list, &removed_node );
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    /* Make sure that detach node does not point to some arbitrary memory location */
    node->prev = NULL;
    node->next = NULL;

    list->count--;

exit:
    return err;
}

OSStatus linked_list_remove_node_from_front( linked_list_t *list, linked_list_node_t **removed_node )
{
    OSStatus err = kNoErr;

    require_action( list && removed_node, exit, err = kParamErr);
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    *removed_node = list->front;

    if ( list->count == 1 ) {
        list->front = NULL;
        list->rear  = NULL;
    } else {
        list->front       = list->front->next;
        list->front->prev = NULL;
    }

    /* Make sure node does not point to some arbitrary memory location */
    (*removed_node)->prev = NULL;
    (*removed_node)->next = NULL;

    list->count--;

exit:
    return err;
}

OSStatus linked_list_remove_node_from_rear( linked_list_t *list, linked_list_node_t **removed_node )
{
    OSStatus err = kNoErr;

    require_action( list && removed_node, exit, err = kParamErr);
    require_action_quiet( list->count, exit, err = kNotFoundErr);

    *removed_node = list->rear;

    if ( list->count == 1 ) {
        list->front = NULL;
        list->rear  = NULL;
    } else {
        list->rear       = list->rear->prev;
        list->rear->next = NULL;
    }

    /* Make sure node does not point to some arbitrary memory location */
    (*removed_node)->prev = NULL;
    (*removed_node)->next = NULL;

    list->count--;

exit:
    return err;
}




