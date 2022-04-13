#ifdef __INTERCONNECTION__

#ifndef __APP_INTERCONNECTION_TLV_H__
#define __APP_INTERCONNECTION_TLV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  CCMP_CHILD_MASK        0x80
#define  CCMP_BIGLEN_MASK       0x80

typedef struct _TlvItem
{
    uint8_t                     type; // bit7= 1,with subtype ; bit7=0, no subtype  type value: bit6~bit0 ; general error = 127
    uint16_t                    length;
    uint8_t                     *value;
    struct _TlvItem             *brotherNode;
    struct _TlvItem             *childNode;
    struct _TlvItem             *parentNode;

}TLV_ITEM_T;

void app_interconnection_tlv_get_big_endian_u16_from_item(TLV_ITEM_T* item, uint16_t* dataPtr);

void app_interconnection_tlv_get_big_endian_u32_from_item(TLV_ITEM_T* item, uint32_t* dataPtr);

uint32_t app_interconnection_tlv_item_get_length_size(uint32_t len);

void app_interconnection_tlv_item_print(uint32_t spaceNumber, TLV_ITEM_T *item);

TLV_ITEM_T *app_interconnection_tlv_item_tree_malloc(uint8_t *buf, uint32_t len);

TLV_ITEM_T *app_interconnection_tlv_item_tree_malloc_long_msg(uint8_t *buf, uint32_t len);

void app_interconnection_tlv_item_tree_free(TLV_ITEM_T *tree);

TLV_ITEM_T *app_interconnection_creat_new_tlv_item(uint8_t type,uint32_t len,uint8_t *value);

void free_tlv_item(TLV_ITEM_T *item);

void tlv_item_add_child(TLV_ITEM_T *parent, TLV_ITEM_T *child);

void tlv_item_add_brother(TLV_ITEM_T *brother, TLV_ITEM_T *me);

TLV_ITEM_T *app_interconnection_tlv_get_item_with_type(TLV_ITEM_T *root, uint8_t type);

TLV_ITEM_T* app_interconnection_tlv_get_brother_node(TLV_ITEM_T *root);

TLV_ITEM_T* app_interconnection_tlv_get_child_node(TLV_ITEM_T *root);

uint8_t app_interconnection_tlv_get_item_type(TLV_ITEM_T *item);

uint8_t* app_interconnection_tlv_get_item_value(TLV_ITEM_T *item,uint32_t *length);

uint32_t app_interconnection_tlv_get_item_tree_length(TLV_ITEM_T* root);

uint8_t* app_interconnection_tlv_malloc_for_item(TLV_ITEM_T *item,uint32_t *length);

uint8_t* app_interconnection_tlv_malloc_for_item_tree_with_margin(TLV_ITEM_T *item, uint8_t head, uint8_t tail, uint32_t *length);

#ifdef __cplusplus
}
#endif

#endif

#endif