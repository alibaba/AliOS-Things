#include "bone_engine_inl.h"

/*
 * Create a global array refs in the heap stash.
 */
void bone_engine_ref_setup(duk_context *ctx)
{
    duk_push_heap_stash(ctx);

    /* Create a new array with one `0` at index `0`. */
    duk_push_array(ctx);
    duk_push_int(ctx, 0);
    duk_put_prop_index(ctx, -2, 0);
    /* Store it as "refs" in the heap stash */
    duk_put_prop_string(ctx, -2, "refs");

    duk_pop(ctx);
}

int bone_engine_ref(duk_context *ctx)
{
    int ref;
    if (duk_is_undefined(ctx, -1)) {
        duk_pop(ctx);
        return 0;
    }
    /* Get the "refs" array in the heap stash */
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    /* ref = refs[0] */
    duk_get_prop_index(ctx, -1, 0);
    ref = duk_get_int(ctx, -1);
    duk_pop(ctx);

    /* If there was a free slot, remove it from the list */
    if (ref != 0) {
        /* refs[0] = refs[ref] */
        duk_get_prop_index(ctx, -1, ref);
        duk_put_prop_index(ctx, -2, 0);
    }
    /* Otherwise use the end of the list */
    else {
        /* ref = refs.length; */
        ref = duk_get_length(ctx, -1);
    }

    /* swap the array and the user value in the stack */
    duk_insert(ctx, -2);

    /* refs[ref] = value */
    duk_put_prop_index(ctx, -2, ref);

    /* Remove the refs array from the stack. */
    duk_pop(ctx);

    return ref;
}

void bone_engine_push_ref(duk_context *ctx, int ref)
{
    if (!ref) {
        duk_push_undefined(ctx);
        return;
    }
    /* Get the "refs" array in the heap stash */
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    duk_get_prop_index(ctx, -1, ref);

    duk_remove(ctx, -2);
}

void bone_engine_unref(duk_context *ctx, int ref)
{
    if (!ref) return;

    /* Get the "refs" array in the heap stash */
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "refs");
    duk_remove(ctx, -2);

    /* Insert a new link in the freelist */

    /* refs[ref] = refs[0] */
    duk_get_prop_index(ctx, -1, 0);
    duk_put_prop_index(ctx, -2, ref);
    /* refs[0] = ref */
    duk_push_int(ctx, ref);
    duk_put_prop_index(ctx, -2, 0);

    duk_pop(ctx);
}
