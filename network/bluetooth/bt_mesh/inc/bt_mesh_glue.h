#ifndef _GLUE_H_
#define _GLUE_H_

#include <stdint.h>
//#include <unistd.h>

typedef void * bt_mesh_conn_t;

struct bt_mesh_conn_cb {
        /** @brief A new connection has been established.
         *
         *  This callback notifies the application of a new connection.
         *  In case the err parameter is non-zero it means that the
         *  connection establishment failed.
         *
         *  @param conn New connection object.
         *  @param err HCI error. Zero for success, non-zero otherwise.
         */
        void (*connected)(bt_mesh_conn_t conn, uint8_t err);

        /** @brief A connection has been disconnected.
         *
         *  This callback notifies the application that a connection
         *  has been disconnected.
         *
         *  @param conn Connection object.
         *  @param reason HCI reason for the disconnection.
         */
        void (*disconnected)(bt_mesh_conn_t conn, uint8_t reason);
};

/** @brief This is a 'tentative' type and should be used as a pointer only */
struct bt_mesh_uuid {
        uint8_t type;
};

struct bt_mesh_uuid_16 {
        struct bt_mesh_uuid uuid;
        uint16_t val;
};

struct bt_mesh_uuid_32 {
        struct bt_mesh_uuid uuid;
        uint32_t val;
};

struct bt_mesh_uuid_128 {
        struct bt_mesh_uuid uuid;
        uint8_t val[16];
};

/** @brief GATT Attribute structure. */
struct bt_mesh_gatt_attr {
	/** Attribute UUID */
	const struct bt_mesh_uuid	*uuid;

	/** Attribute read callback
	 *
	 *  @param conn   The connection that is requesting to read
	 *  @param attr   The attribute that's being read
	 *  @param buf    Buffer to place the read result in
	 *  @param len    Length of data to read
	 *  @param offset Offset to start reading from
	 *
	 *  @return Number fo bytes read, or in case of an error
	 *          BT_GATT_ERR() with a specific ATT error code.
	 */
	int			(*read)(struct bt_mesh_conn *conn,
					const struct bt_mesh_gatt_attr *attr,
					void *buf, uint16_t len,
					uint16_t offset);

	/** Attribute write callback
	 *
	 *  @param conn   The connection that is requesting to write
	 *  @param attr   The attribute that's being written
	 *  @param buf    Buffer with the data to write
	 *  @param len    Number of bytes in the buffer
	 *  @param offset Offset to start writing from
	 *  @param flags  Flags (BT_GATT_WRITE_*)
	 *
	 *  @return Number of bytes written, or in case of an error
	 *          BT_GATT_ERR() with a specific ATT error code.
	 */
	int			(*write)(struct bt_mesh_conn *conn,
					 const struct bt_mesh_gatt_attr *attr,
					 const void *buf, uint16_t len,
					 uint16_t offset, uint8_t flags);

	/** Attribute user data */
	void			*user_data;
	/** Attribute handle */
	uint16_t		handle;
	/** Attribute permissions */
	uint8_t			perm;
};

/** @brief GATT Service structure */
struct bt_mesh_gatt_service {
	/** Service Attributes */
	struct bt_mesh_gatt_attr	*attrs;
	/** Service Attribute count */
	uint16_t			attr_count;
};

#endif
