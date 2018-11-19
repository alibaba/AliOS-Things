/*----------------------------------------------------------------------------*/
/*
 *  Copyright (C) Bosch Connected Devices and Solutions.
 *  All Rights Reserved. Confidential.
 *
 *  Distribution only to people who need to know this information in
 *  order to do their job.(Need-to-know principle).
 *  Distribution to persons outside the company, only if these persons
 *  signed a non-disclosure agreement.
 *  Electronic transmission, e.g. via electronic mail, must be made in
 *  encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 *  @ingroup BCDS_ESSENTIALS
 *  @defgroup BCDS_RETCODE Retcode
 *  @{
 *  @brief Return code composition and handling.
 *  @details This module provides helpful functions for creation and handling of return codes. It supports
 *  general error codes (see #Retcode_General_E), package and module IDs, severity information
 *  (see #Retcode_Severity_E) and a callback that can be registered to be called whenever a module calls
 *  #Retcode_RaiseError(#Retcode_T).
 *
 *  @file
 **/

/* header definition ******************************************************** */
#ifndef BCDS_RETCODE_H_
#define BCDS_RETCODE_H_

/* additional interface header files */
#include "BCDS_Basics.h"

#ifndef BCDS_PACKAGE_ID
#warning "BCDS_PACKAGE_ID not defined. The BCDS_PACKAGE_ID is set to the default value 0"
#define BCDS_PACKAGE_ID 0
#endif

#ifndef BCDS_MODULE_ID
#define BCDS_MODULE_ID 0
#endif

/**
 * @brief This is the return code data type of the BCDS platform.
 * Each retcode is composed of four parts: package, module, severity and code.
 *
 * The package (most left byte - 8-bit value) indicates the package where the retcode
 * has been raised.
 *
 * The module (second left byte - 8-bit value) indicates the module where the retcode
 * has been raised.
 *
 * The severity  (Bits 15 - 12, 4-bit value) indicates the impact of
 * the retcode, i.e. warning, error, fatal error or just an info.
 *
 * The code (Bits 11 - 0, - 12 bit value) indicates the error code.
 * There are some general error codes defined in #Retcode_General_E, ranging from 0 up to #RETCODE_FIRST_CUSTOM_CODE.
 * Packages and projects may define their own codes starting from #RETCODE_FIRST_CUSTOM_CODE up to the code 4095.
 *
 * @see  Retcode_Severity_T
 */
typedef uint32_t Retcode_T;

/**
 * @brief	Enumeration to represent the General return codes.
 * The first codes from 0 to #RETCODE_FIRST_CUSTOM_CODE are reserved as general error codes.
 * Packages and projects may define their own codes starting
 * from #RETCODE_FIRST_CUSTOM_CODE up to the code 4095.
 */
enum Retcode_General_E
{
    RETCODE_SUCCESS = 0,
    RETCODE_FAILURE,
    RETCODE_OUT_OF_RESOURCES,
    RETCODE_INVALID_PARAM,
    RETCODE_NOT_SUPPORTED,
    RETCODE_INCONSITENT_STATE,
    RETCODE_UNINITIALIZED,
	RETCODE_NULL_POINTER,
	RETCODE_RTOS_QUEUE_ERROR,
	RETCODE_UNEXPECTED_BEHAVIOR,
	RETCODE_SEMAPHORE_ERROR,
	RETCODE_DOPPLE_INITIALIZATION,
	RETCODE_TIMEOUT,
	RETCODE_INVALID_SEMAPHORE_IN_ISR,
	//...
	RETCODE_FIRST_CUSTOM_CODE = 65,
};

/**
 * @brief	Enumeration to represent the Severity of errors.
 */
enum Retcode_Severity_E
{
    RETCODE_SEVERITY_NONE = 0,
	RETCODE_SEVERITY_FATAL,
	RETCODE_SEVERITY_ERROR,
	RETCODE_SEVERITY_WARNING,
    RETCODE_SEVERITY_INFO,
};

/**
 * @brief	Typedef to represent the Severity of error.
 */
typedef enum Retcode_Severity_E Retcode_Severity_T;

/**
 * @brief	Typedef to represent the Callback function to be called
 * for handling the Error.
 *
 * @param[in] error: The error that was raised
 * @param[in] isFromIsr: TRUE if the error was raised from ISR, FALSE if not
 */
typedef void (* Retcode_ErrorHandlingFunc_T)(Retcode_T error, bool isFromIsr);

/**
 * @brief 	This function initializes the Retcode module.
 *
 * @details This function initializes the Retcode module. It registers an
 * 			error handling callback function. When an error is raised,
 * 			registered callback function of the corresponding component is called.
 * 			This function shall be called only once.
 *
 * @param [in]	func : A callback function which is executed when an error is raised.
 *
 * @retval #RETCODE_OK : On successful init
 * @retval #RETCODE_INCONSITENT_STATE : When this function is called more than once
 * @retval #RETCODE_NULL_POINTER : When the function pointer is NULL
 */
Retcode_T Retcode_Initialize(Retcode_ErrorHandlingFunc_T func);

/**
 * @brief	Raises the error passed as parameter to the error handling function.
 * @details Each component may use this function to raise an error to the application
 * if it cannot handle the error itself.
 * This call results in the invocation of the error handling function which has
 * been given while the initialization of the Retcode module using the function
 * Retcode_Initialize().
 *
 * @param [in]	error : The error to be raised to the application
 *
 * @see Retcode_Initialize
 * @see Retcode_ErrorHandlingFunc_T
 */
void Retcode_RaiseError(Retcode_T error);

/**
 * @brief Raises the error passed as parameter to the error handling function, use from ISR (Interrupt) context only.
 * @details Each component may use this function to raise an error from ISR to the application.
 * This call results in the invocation of the error handling function with the isFromIsr flag
 * set. As this is called from ISR, the application can define a custom approach
 * (in the registered callback function) based on the isFromIsr flag
 * on how it wants to deal with the error from ISR. i.e. The application can log it into user page,
 * post-process the error and so on, which cannot be done from ISR.
 *
 * @param [in]  error : The error to be raised to the application
 *
 * @see Retcode_Initialize
 * @see Retcode_ErrorHandlingFunc_T
 */
void Retcode_RaiseErrorFromIsr(Retcode_T error);

/**
 * @brief	A helper macro to make to create the package
 *
 * @note Do not use directly, use #RETCODE(Severity, Code) instead!
 */
#define RETCODE_MK_PACKAGE(Package)    ( (Package & 0x000000FFUL) << 24UL )

/**
 * @brief	A helper macro to make Module Id
* @note Do not use directly, use #RETCODE(Severity, Code) instead!
 */
#define RETCODE_MK_MODULEID(ModuleID)  ( (ModuleID & 0x000000FFUL) << 16UL )

/**
 * @brief	A helper macro to make severity
 *
 * @note Do not use directly, use #RETCODE(Severity, Code) instead!
 */
#define RETCODE_MK_SEVERITY(Severity)  ( (Severity & 0x0000000FUL) << 12UL )

/**
 * @brief	A helper macro to make code
 *
 * @note Do not use directly, use #RETCODE(Severity, Code) instead!
 */
#define RETOCDE_MK_CODE(Code)		   (Code & 0x00000FFFUL)

/**
 * @brief	This Macro composes a return code based on input parameters.
 * @details	This Macro composes a return code based on input parameters.
 * @param [in]	PackageID : Package ID of the corresponding module
 * @param [in]	ModuleID : Package ID of the corresponding module
 * @param [in]	Severity : Severity of the error
 * @param [in]	Code : Error code
 *
 * @note Do not use directly, use #RETCODE(Severity, Code) instead!
 */
#define RETCODE_COMPOSE(PackageID, ModuleID, Severity, Code) \
			( (Retcode_T) ( RETCODE_MK_PACKAGE(PackageID) | RETCODE_MK_MODULEID(ModuleID) | RETCODE_MK_SEVERITY(Severity) | RETOCDE_MK_CODE(Code) ) )

/**
 * @brief	This macro should be used to compose a Retcode_T value.
 * @details This macro creates a #Retcode_T from its four parts package, module, severity and code. While severity and code are passed as parameters,
 * the package and module are taken from the macro BCDS_PACKAGE_ID and BCDS_MODULE_ID, which must be defined in each package / module.
 * @param [in]	Severity : Severity of the error, see #Retcode_Severity_E
 * @param [in]	Code : Error code
 *
 * Example:
 * @code
 *  	#define BCDS_PACKAGE_ID 42 // can be defined in makefile of the package, must be unique within global scope
 *  	#undef BCDS_MODULE_ID
 *		#define BCDS_MODULE_ID BCDS_MODULE_ID_MY_MODULE // must be unique within package scope
 *
 *		Retcode_T MyFunction(void)
 *		{
 *			...
 *			return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
 *		}
 * @endcode
 */
#define RETCODE(Severity, Code) \
				(RETCODE_COMPOSE((uint32_t)BCDS_PACKAGE_ID, (uint32_t)BCDS_MODULE_ID, (uint32_t)Severity, (uint32_t)Code))

/**
 * @brief	A helper macro to return success
 */
#define RETCODE_OK				((Retcode_T) 0)

/**
 * @brief	This function provides the Package of a return code.
 *
 * @param [in]	retcode : return code
 *
 * @return	uint32_t : Package ID of the corresponding module
 */

static BCDS_INLINE uint32_t Retcode_GetPackage(Retcode_T retcode)
{
	return (((uint32_t) retcode >> 24UL) & 0x000000FFUL);
}

/**
 * @brief	This function provides the Package of a return code.
 *
 * @param [in]	retcode : return code
 *
 * @return	uint32_t : Module ID of the corresponding module
 */
static BCDS_INLINE uint32_t Retcode_GetModuleId(Retcode_T retcode)
{
	return (((uint32_t) retcode >> 16UL) & 0x000000FFUL);
}

/**
 * @brief	This function provides the Severity of a return code.
 *
 * @param [in]	retcode : return code
 *
 * @return	uint32_t : Severity of the error
 */
static BCDS_INLINE Retcode_Severity_T Retcode_GetSeverity(Retcode_T retcode)
{
	return ((Retcode_Severity_T) (((uint32_t) retcode >> 12UL) & 0x0000000FUL));
}

/**
 * @brief	This function provides the Error code of a return code.
 *
 * @param [in]	retcode : return code
 *
 * @return	uint32_t : Error code
 */
static BCDS_INLINE uint32_t Retcode_GetCode(Retcode_T retcode)
{
	return ((uint32_t) retcode & 0x00000FFFUL);
}

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_Initialize
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   This function initializes the Retcode module.
 *
 * @details This function initializes the Retcode module. It registers an
 *          error handling callback function. When an error is raised,
 *          registered callback function of the corresponding component is called.
 *          This function shall be called only once.
 *
 * @param [in]  func : A callback function which is executed when an error is raised.
 *
 * @retval RETCODE_OK : On successful init
 * @retval RETCODE_INCONSITENT_STATE : When this function is called more than once
 * @retval RETCODE_NULL_POINTER : When the function pointer is NULL
 */
BCDS_DEPRECATED(Retcode_T Retcode_initialize(Retcode_ErrorHandlingFunc_T func));

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_RaiseError
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   Raises the error passed as parameter to the error handling function.
 * @details Each component may use this function to raise an error to the application
 * if it cannot handle the error itself.
 * This call results in the invocation of the error handling function which has
 * been given while the initialization of the Retcode module using the function
 * Retcode_Initialize().
 *
 * @param [in]  error : The error to be raised to the application
 *
 * @see Retcode_Initialize
 * @see Retcode_ErrorHandlingFunc_T
 */
BCDS_DEPRECATED(void Retcode_raiseError(Retcode_T error));

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_RaiseErrorFromIsr
 * @todo replace vxx.xx.xx with next version
 *
 * @brief Raises the error passed as parameter to the error handling function, use from ISR (Interrupt) context only.
 * @details Each component may use this function to raise an error from ISR to the application.
 * This call results in the invocation of the error handling function with the isFromIsr flag
 * set. As this is called from ISR, the application can define a custom approach
 * (in the registered callback function) based on the isFromIsr flag
 * on how it wants to deal with the error from ISR. i.e. The application can log it into user page,
 * post-process the error and so on, which cannot be done from ISR.
 *
 * @param [in]  error : The error to be raised to the application
 *
 * @see Retcode_Initialize
 * @see Retcode_ErrorHandlingFunc_T
 */
BCDS_DEPRECATED(void Retcode_raiseErrorFromIsr(Retcode_T error));

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_GetSeverity
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   This function provides the Severity of a return code.
 *
 * @param [in]  retcode : return code
 *
 * @return  uint32_t : Severity of the error
 */
BCDS_DEPRECATED(static BCDS_INLINE Retcode_Severity_T Retcode_getSeverity(Retcode_T retcode));
static BCDS_INLINE Retcode_Severity_T Retcode_getSeverity(Retcode_T retcode)
{
    return Retcode_GetSeverity(retcode);
}

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_GetCode
 * @todo replace vxx.xx.xx with next version
 *
 * @brief   This function provides the Error code of a return code.
 *
 * @param [in]  retcode : return code
 *
 * @return  uint32_t : Error code
 */
BCDS_DEPRECATED(static BCDS_INLINE uint32_t Retcode_getCode(Retcode_T retcode));
static BCDS_INLINE uint32_t Retcode_getCode(Retcode_T retcode)
{
    return Retcode_GetCode(retcode);
}

/**
 * @deprecated since version vxx.xx.xx, replaced by #Retcode_GetPackage
 * @todo replace vxx.xx.xx with next version

 * @brief   This function provides the Package of a return code.
 *
 * @param [in]  retcode : return code
 *
 * @return  uint32_t : Package ID of the corresponding module
 */
BCDS_DEPRECATED(static BCDS_INLINE uint32_t Retcode_getPackage(Retcode_T retcode));
static BCDS_INLINE uint32_t Retcode_getPackage(Retcode_T retcode)
{
    return Retcode_GetPackage(retcode);
}

#endif /* BCDS_RETCODE_H_ */
/**@} */
/** ************************************************************************* */
