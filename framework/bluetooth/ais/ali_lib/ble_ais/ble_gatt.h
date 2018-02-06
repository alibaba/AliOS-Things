#ifndef _AIS_GATT_H_
#define _AIS_GATT_H_

#include <bluetooth/gatt.h>

#define BT_GATT_PRIMARY_SERVICE2(_service, _handle)			\
{									\
	.handle = _handle,						\
	.uuid = BT_UUID_GATT_PRIMARY,					\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_service,				\
	.user_data = _service,						\
}

#define BT_GATT_CHARACTERISTIC2(_uuid, _props, _handle)			\
{									\
	.handle = _handle,						\
	.uuid = BT_UUID_GATT_CHRC,					\
	.perm = BT_GATT_PERM_READ,					\
	.read = bt_gatt_attr_read_chrc,					\
	.user_data = (&(struct bt_gatt_chrc) { .uuid = _uuid,		\
					       .properties = _props, }),\
}



#define BT_GATT_CCC2(_cfg, _cfg_changed, _handle)					\
{									\
	.handle = _handle,						\
	.uuid = BT_UUID_GATT_CCC,					\
	.perm = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,			\
	.read = bt_gatt_attr_read_ccc,					\
	.write = bt_gatt_attr_write_ccc,				\
	.user_data = (&(struct _bt_gatt_ccc) { .cfg = _cfg,		\
					       .cfg_len = ARRAY_SIZE(_cfg), \
					       .cfg_changed = _cfg_changed, }),\
}

#define BT_GATT_DESCRIPTOR2(_uuid, _perm, _read, _write, _value, _handle)	\
{										\
	.handle = _handle,							\
	.uuid = _uuid,								\
	.perm = _perm,								\
	.read = _read,								\
	.write = _write,							\
	.user_data = _value,						\
}

#endif
