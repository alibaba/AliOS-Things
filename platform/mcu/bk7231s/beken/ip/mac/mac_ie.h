/**
 ****************************************************************************************
 *
 * @file mac_ie.h
 *
 * @brief MAC Information Elements related API declarations.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MAC_IE_H_
#define _MAC_IE_H_

/**
 ****************************************************************************************
 * @defgroup CO_MAC_IE CO_MAC_IE
 * @ingroup CO_MAC
 * @brief  Common defines,structures
 *
 * This module contains defines commonly used for MAC
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

struct ieee80211_vendor_ie {
    u8 element_id;
    u8 len;
    u8 oui[3];
    u8 oui_type;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Find an information element in the variable part of a management frame body.
 *
 * @param[in] buffer Pointer to the variable part of the management frame body to look
 * for the IE into.
 * @param[in] buflen Length of the frame body variable part.
 * @param[in] ie_id  Identifier of the information element to look for.
 *
 * @return NULL if the IE is not found otherwise the pointer to the first IE ID location
 * in the frame passed as a parameter.
 * @warning To find a vendor specific information element, see mac_vsie_find.
 ****************************************************************************************
 */
uint32_t mac_ie_find(uint32_t addr,
                     uint16_t buflen,
                     uint8_t ie_id);

/**
 ****************************************************************************************
 * @brief Remove an information element from the variable part of a management frame body.
 *
 * @param[in] buffer Pointer to the variable part of the management frame body to remove
 * the IE from.
 * @param[in,out] buflen Length of the frame body variable part, gets updated after the
 * IE is actually removed (if it is found).
 * @param[in] ie_id Identifier of the information element to remove.
 *
 * @return true if the IE was found and removed, false otherwise.
 * @warning To remove a vendor specific information element, see mac_vsie_remove.
 ****************************************************************************************
 */
bool mac_ie_remove(uint8_t *buffer,
                   uint16_t* buflen,
                   uint8_t ie_id);

/**
 ****************************************************************************************
 * @brief Add an information element in the variable part of a management frame body.
 *
 * This function starts by removing the previous identical IE from the frame body and then
 * adds the IE passed as a parameter in the frame body.
 * The IE are added in the frame in an ascending order, based on the IE ID value.
 *
 * @param[in,out] buffer Pointer to the variable part of the management frame body to
 * add the IE into.
 * @param[in,out] buflen Length of the frame body variable part, gets updated after the
 * IE is added.
 * @param[in] ie Pointer to the buffer containing the information element to add.
 *
 * @warning To add a vendor specific information element, see mac_vsie_add.
 ****************************************************************************************
 */
void mac_ie_add(uint8_t *buffer,
                uint16_t* buflen,
                uint8_t const *ie);

/**
 ****************************************************************************************
 * @brief Find a vendor specific information element in the variable part of a management
 * frame body.
 *
 * @param[in] buffer Pointer to the variable part of the management frame body to look
 * for the IE into.
 * @param[in] buflen Length of the frame body variable part.
 * @param[in] oui Pointer to the OUI identifier to look for.
 * @param[in] ouilen Length of the OUI identifier.
 *
 * @return NULL if the VSIE is not found otherwise the pointer to the first VSIE ID
 * location in the frame passed as a parameter.
 ****************************************************************************************
 */
uint32_t mac_vsie_find(uint32_t addr,
                       uint16_t buflen,
                       uint8_t const *oui,
                       uint8_t ouilen);

/**
 ****************************************************************************************
 * @brief Remove a vendor specific information element from the variable part of a
 * management frame body.
 *
 * @param[in] buffer Pointer to the variable part of the management frame body to remove
 * the IE from.
 * @param[in,out] buflen Length of the frame body variable part, gets updated after the
 * VSIE is actually removed (if it is found).
 * @param[in] oui Pointer to the OUI identifier to remove.
 * @param[in] ouilen Length of the OUI identifier.
 *
 * @return true if the VSIE was found and removed, false otherwise.
 ****************************************************************************************
 */
bool mac_vsie_remove(uint8_t *buffer,
                     uint16_t *buflen,
                     uint8_t const *oui,
                     uint8_t ouilen);

/**
 ****************************************************************************************
 * @brief Add a vendor specific information element in the variable part of a management
 * frame body.
 *
 * This function starts by removing the previous identical VSIE from the frame body and
 * then adds the VSIE passed as a parameter in the frame body.
 *
 * @param[in,out] buffer Pointer to the variable part of the management frame body to
 * add the IE into.
 * @param[in,out] buflen Length of the frame body variable part, gets updated after the
 * IE is added.
 * @param[in] vsie Pointer to the VSIE to add.
 * @param[in] ouilen Length of the OUI identifier.
 ****************************************************************************************
 */
void mac_vsie_add(uint8_t *buffer,
                  uint16_t *buflen,
                  uint8_t const *vsie,
                  uint8_t ouilen);

uint8_t *mac_vendor_ie_find(unsigned int oui, 
							uint8_t oui_type, 
							uint8_t *ies, 
							int len);

/// @}
#endif // _MAC_IE_H_
