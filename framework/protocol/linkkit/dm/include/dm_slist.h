#ifndef DM_SLIST_H
#define DM_SLIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DM_SLIST_CLASS get_slist_class()

void list_iterator(const void* _list, handle_fp_t handle_fp, ...);

typedef struct _node {
    void*         data;
    struct _node* next;
} slist_node_t;

typedef struct {
    const void*   _;
    slist_node_t* _head;
    int           _size;
    char*         _name;
} slist_t;

extern const void* get_slist_class();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DM_SLIST_H */
