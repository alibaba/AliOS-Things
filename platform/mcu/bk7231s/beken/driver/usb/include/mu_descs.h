/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSB-MicroSW USB Software Platform descriptor analysis library API.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_DESCRIPTORS_H__
#define __MUSB_DESCRIPTORS_H__

#include "mu_dev.h"

/**
 * Find a descriptor of a given type.
 *
 * @param pConfig pointer to a full configuration descriptor in which to search
 *
 * @param bDescriptorType the type of descriptor desired
 *
 * @param bIndex the 0-based index of the instance of the desired descriptor
 * (e.g. if there are 3 HID descriptors, bIndex==2 retrieves the third)
 *
 * @return descriptor pointer if found; NULL on failure
 */
extern const uint8_t *MUSB_FindDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    uint8_t bDescriptorType,
    uint8_t bIndex);

/**
 * Count alternate interfaces.
 * A given USB device configuration has a specified number of interfaces,
 * given by its bNumInterfaces.
 * Each interface has at least one descriptor, with additional ones
 * for "alternate" settings.
 * The first descriptor for a given interface is called "alternate setting 0."
 * Each interface descriptor (primary or alternate) is followed by
 * a number of endpoint descriptors, given by bNumEndpoints.
 * Therefore, the zero-based index of endpoint descriptor E
 * for interface N (>0) is:
 <pre>
 iEnd = E + bNumEndpoints[0] * NumAlternates[0] + ... + bNumEndpoints[N-1] * NumAlternates[N-1]
 </pre>
 * where bNumEndpoints[M] is interface M's bNumEndpoints field,
 * and NumAlternates[M] is the total number of alternate settings
 * (including setting 0) for interface M.
 * The array of NumAlternates is computed by this function.
 * It is the index iEnd that should be passed to MUSB_FindDescriptor
 * to find the desired endpoint's descriptor.
 * In a similar fashion, the index of the descriptor for alternate
 * setting A for interface N (>0) is:
 <pre>
 iAlt = A + NumAlternates[0] + ... + NumAlternates[N-1]
 </pre>
 * so iAlt would be passed to MUSB_FindDescriptor to find the desired
 * alternate interface's descriptor.
 *
 * @param aCount array in which to store NumAlternates for each interface.
 * The appropriate size is given by the configuration's bNumInterfaces,
 * so the array is assumed at least this size.
 *
 * @param pConfig pointer to configuration descriptor
 *
 * @return the number of interfaces actually found (therefore the number of filled
 * elements in aCount)
 */
extern uint8_t MUSB_CountAlternates(uint8_t *aCount,
                                    const MUSB_ConfigurationDescriptor *pConfig);

/**
 * Find an interface descriptor
 *
 * @param pConfig pointer to configuration descriptor
 *
 * @param bInterfaceNumber the number of the desired interface
 *
 * @param bAlternateSetting the desired alternate setting
 *
 * @return pointer to the requested descriptor, or NULL if not found.
 * This is actually a pointer within a contiguous descriptor block,
 * so it can be used in a call to MUSB_FindDescriptorInBlock
 * to find class-specific interface descriptors.
 */
extern const MUSB_InterfaceDescriptor *MUSB_FindInterfaceDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    uint8_t bInterfaceNumber,
    uint8_t bAlternateSetting);

/**
 * Find an endpoint descriptor
 *
 * @param pConfig pointer to the configuration in which the interface resides,
 * to limit the search based on wTotalLength
 *
 * @param pInterfaceDescriptor pointer to an interface descriptor,
 * e.g. from a successful call to MUSB_FindInterfaceDescriptor
 *
 * @param bEndpointIndex the 0-based index of the desired endpoint descriptor
 *
 * @return pointer to the requested descriptor, or NULL if not found.
 * This is actually a pointer within a contiguous descriptor block,
 * so it can be used in a call to MUSB_FindDescriptorInBlock
 * to find class-specific endpoint descriptors.
 */
extern const MUSB_EndpointDescriptor *MUSB_FindEndpointDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    const MUSB_InterfaceDescriptor *pInterfaceDescriptor,
    uint8_t bEndpointIndex);

/**
 * Find an arbitrary descriptor within a descriptor block.
 * A descriptor block could be the return value of
 * MUSB_FindInterfaceDescriptor or MUSB_FindEndpointDescriptor.
 * This is useful for finding class-specific descriptors associated with
 * the already-found type, since they are required to follow it.
 *
 * @param pConfig pointer to the configuration in which the block resides,
 * to limit the search based on wTotalLength
 *
 * @param pDescriptorBlock pointer to a descriptor of arbitrary type.
 * The search begins here, and is limited to either the end of the
 * configuration or when another descriptor of this same type is encountered.
 *
 * @param bDescriptorType the type of descriptor desired
 *
 * @param bIndex the index (counting from 0) of the instance of the desired descriptor
 * (e.g. if there are 3 HID descriptors, bIndex==2 retrieves the third)
 *
 * @return pointer to the requested descriptor, or NULL if not found
 */
extern const uint8_t *MUSB_FindDescriptorInBlock(
    const MUSB_ConfigurationDescriptor *pConfig,
    const uint8_t *pDescriptorBlock,
    uint8_t bDescriptorType,
    uint8_t bIndex);

#endif	/* multiple inclusion protection */
