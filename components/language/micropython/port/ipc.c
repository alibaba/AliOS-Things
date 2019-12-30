#include <stdio.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"

extern const mp_obj_type_t pyb_ipc_type;
// this is the actual C-structure for our new object
typedef struct _pyb_ipc_obj_t
{
    // base represents some basic information, like type
    mp_obj_base_t base;
    // a member created by us
    kbuf_queue_t* queue;
} pyb_ipc_obj_t;


/*
STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}, PYB_LED_TOP_LEFT_1},
    {{&pyb_led_type}, PYB_LED_TOP_RIGHT_1},
    {{&pyb_led_type}, PYB_LED_TOP_RIGHT_2},
    {{&pyb_led_type}, PYB_LED_TOP_LEFT_2},
};
*/

void ipc_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    pyb_ipc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "queue(%u)", self->queue);
}

STATIC mp_obj_t ipc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    kbuf_queue_t *queue;
    // check arguments
    mp_arg_check_num(n_args, n_kw, 3, 3, false);


    char* pname = (char*)mp_obj_str_get_str(args[0]);
    //printf("name:%s\r\n",pname);


    mp_uint_t size = mp_obj_get_int(args[1]);
    mp_uint_t max_msg = mp_obj_get_int(args[2]);


    pyb_ipc_obj_t* ipc_obj = m_new_obj(pyb_ipc_obj_t);
    if (!ipc_obj) {
        mp_raise_OSError(ENOMEM);
    }

    mp_int_t  ret = krhino_buf_queue_dyn_create(&queue, pname, size, max_msg);
    if(ret){
        mp_raise_OSError(EFAULT);
    }

    ipc_obj->base.type = &pyb_ipc_type;
    ipc_obj->queue = queue;

    return MP_OBJ_FROM_PTR(ipc_obj);
}


static mp_obj_t ipc_obj_send(mp_obj_t self_in, mp_obj_t msg_in, mp_obj_t size_in)
{
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(self_in);
    pyb_ipc_obj_t* ipc_obj = (pyb_ipc_obj_t *)self;

    void *msg = (void *)mp_obj_get_int(msg_in);
    mp_uint_t size = mp_obj_get_int(size_in);

    mp_int_t  ret =  krhino_buf_queue_send(ipc_obj->queue, msg, size);
    if(ret){
        printf("err:%d\r\n",ret);
        mp_raise_OSError(EFAULT);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(ipc_obj_send_obj, ipc_obj_send);


static mp_obj_t ipc_obj_recv(size_t n_args, const mp_obj_t *args)
{
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    pyb_ipc_obj_t* ipc_obj = (pyb_ipc_obj_t *)self;

    mp_uint_t tick = mp_obj_get_int(args[1]);
    void *msg = (void *)mp_obj_get_int(args[2]);
    void *psize = mp_obj_get_int(args[3]);

    mp_int_t  ret =  krhino_buf_queue_recv(ipc_obj->queue, tick , msg, psize);
    if(ret && ((ret != RHINO_NO_PEND_WAIT) && (ret != RHINO_BLK_TIMEOUT))){
        printf("ipc_obj_recv ret:%d\r\n",ret);
        mp_raise_OSError(EFAULT);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ipc_obj_recv_obj, 4, ipc_obj_recv);



STATIC const mp_rom_map_elem_t ipc_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ipc)},
    {MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&ipc_obj_send_obj)},
    {MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&ipc_obj_recv_obj)},
};

STATIC MP_DEFINE_CONST_DICT(ipc_locals_dict, ipc_locals_dict_table);

const mp_obj_type_t pyb_ipc_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ipc,
    .print = ipc_obj_print,
    .make_new = ipc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&ipc_locals_dict,
};



