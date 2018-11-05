/*******************************************************************************
  Timer Device Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_tmr_compatibility.h

  Summary:
    Timer device driver interface header file.

  Description:
    This header file contains macro definition for APIs which will be deprecated 
	later. This header file is included in drv_tmr.h.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_TMR_DEPRECATED_H
#define _DRV_TMR_DEPRECATED_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void DRV_TMR_Tasks_ISR ( SYS_MODULE_OBJ object )

  Summary:
    Maintains the driver's state machine, processes the events and implements 
	its ISR.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is used to maintain the driver's internal state machine and
    processes the timer events in interrupt-driven implementations
    (DRV_TMR_INTERRUPT_MODE == true).

  Precondition:
    The DRV_TMR_Initialize function must have been called for the specified Timer
    driver instance.

  Parameters:
    object      - Object handle for the specified driver instance (returned from
                  DRV_TMR_Initialize)

  Returns:
    None

  Example:
    <code>
        void __ISR(_TIMER_2_VECTOR, ipl4) _InterruptHandler_TMR2(void)
        {
            DRV_TMR_Tasks_ISR(appDrvObjects.drvTmrObject);
        }
    </code>

  Remarks:
    This function is normally not called directly by an application.
    It is called by the timer driver raw ISR.

    This function will execute in an ISR context and will never block or access any
    resources that may cause it to block.
    
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_Tasks" instead of this.
*/

#define DRV_TMR_Tasks_ISR(object)	DRV_TMR_Tasks(object)

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterValue16BitSet 
    ( 
        DRV_HANDLE handle, 
        uint16_t counterPeriod 
    )

  Summary:
    Updates the 16-bit Timer's counter register.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function updates the 16-bit Timer's value in the counter register. This
    is valid only if the 16-bit mode of the timer is selected('mode' in the INIT
    structure is set to DRV_TMR_OPERATION_MODE_16_BIT). Otherwise use
    DRV_TMR_CounterValue32BitSet function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
						open routine

    counterPeriod   - 16-bit counter period value

  Returns:
    None.

  Example:
    <code>
    //Example to use timer for precision time measurement
	//without configuring an alarm (interrupt based)
	char appState = 0;	
	DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
	
	switch ( appState )
	{
		case 0:
			//Calculate and set the counter period 
			DRV_TMR_CounterValue16BitSet ( tmrHandle, ( 0xFFFF - 0x1000 ) );
			
			//counter starts 
			DRV_TMR_Start ( tmrHandle );
			
			//Trigger an application operation
			app_trigger_operation();
			
			//Check for time-out in the next state
			appState++;
		case 1:
			//Overflows and stops at 0 if no alarm is set
			if ( DRV_TMR_CounterValue16BitGet ( tmrHandle ) == 0 )
			{
				//Time-out
				return false;
			}
			else if ( app_operation_isComplete( ) )
			{
				//Operation is complete before time-out
				return true;
			}
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_CounterValueSet" instead of this.
*/

#define DRV_TMR_CounterValue16BitSet(handle, counterPeriod)	DRV_TMR_CounterValueSet(handle,counterPeriod) 

// *****************************************************************************
/* Function:
    void DRV_TMR_CounterValue32BitSet 
    (
        DRV_HANDLE handle, 
        uint32_t counterPeriod 
    )

  Summary:
    Updates the 32-bit Timer's counter register.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function updates the 32-bit Timer's value in the counter register. This
    is valid only if the 32-bit mode of the timer is selected Otherwise use
    DRV_TMR_CounterValue16BitSet function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	32-Bit timer mode.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       	- A valid open-instance handle, returned from the driver's
                        open routine

    counterPeriod   - 32-bit counter period value

  Returns:
    None.

  Example:
    <code>
    //Example to use timer for precision time measurement
    //without configuring an alarm (interrupt based)
    char appState = 0;  
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    
    switch ( appState )
    {
        case 0:
            //Calculate and set the counter period 
            DRV_TMR_CounterValue32BitSet ( tmrHandle, ( 0xFFFFFFFF - 0xFF343100 ) );
            
            //counter starts 
            DRV_TMR_Start ( tmrHandle );
            
            //Trigger an application operation against which we want to use the time-out
            app_trigger_operation();
            
            //Check for time-out in the next state
            appState++;
        case 1:
            //Overflows and stops at 0 if no alarm is set
            if ( DRV_TMR_CounterValue32BitGet ( tmrHandle ) == 0 )
            {
                //Time-out
                return false;
            }
            else if ( app_operation_isComplete( ) )
            {
                //Operation is complete before time-out
                return true;
            }
    </code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
    32-bit mode.
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_CounterValueSet" instead of this.
*/

#define DRV_TMR_CounterValue32BitSet(handle, counterPeriod)	DRV_TMR_CounterValueSet(handle,counterPeriod)   

// *****************************************************************************
/* Function:
    uint16_t DRV_TMR_CounterValue16BitGet ( DRV_HANDLE handle )

  Summary:
    Reads the 16-bit Timer's counter register.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the 16-bit Timer's value in the counter register. This 
	is valid only if the 16-bit mode of the timer is selected.
	Otherwise use DRV_TMR_CounterValue32BitGet function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       	- A valid open-instance handle, returned from the driver's
                        open routine
                        
  Returns:
    Timer period in 16-bit mode.

  Example:
    <code>
    //Example to use timer for precision time measurement
    //without configuring an alarm (interrupt based)
    char appState = 0;  
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    
    switch ( appState )
    {
        case 0:
            //Calculate and set the counter period 
            DRV_TMR_CounterValue16BitSet ( tmrHandle, ( 0xFFFF - 0x1000 ) );
            
            //counter starts 
            DRV_TMR_Start ( tmrHandle );
            
            //Trigger an application operation
            app_trigger_operation();
            
            //Check for time-out in the next state
            appState++;
        case 1:
            //Overflows and stops at 0 if no alarm is set
            if ( DRV_TMR_CounterValue16BitGet ( tmrHandle ) == 0 )
            {
                //Time-out
                return false;
            }
            else if ( app_operation_isComplete( ) )
            {
                //Operation is complete before time-out
                return true;
            }   
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_CounterValueGet" instead of this.
*/

#define DRV_TMR_CounterValue16BitGet(handle)	DRV_TMR_CounterValueGet(handle)        

// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_CounterValue32BitGet ( DRV_HANDLE handle )

  Summary:
    Reads the 32-bit Timer's counter register.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the 32-bit Timer's value in the counter register. This
    is valid only if the 32-bit mode of the timer is selected Otherwise use
    DRV_TMR_CounterValue16BitGet function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    32-Bit timer mode. 

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle          - A valid open-instance handle, returned from the driver's
						open routine
						
  Returns:
    32-Bit Counter value.

  Example:
    <code>
	//Example to use timer for precision time measurement
	//without configuring an alarm (interrupt based)
	char appState = 0;	
	DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
	
	switch ( appState )
	{
		case 0:
			//Calculate and set the counter period 
			DRV_TMR_CounterValue32BitSet ( tmrHandle, ( 0xFFFFFFFF - 0x23321000 ) );
			
			//counter starts 
			DRV_TMR_Start ( tmrHandle );
			
			//Trigger an application operation
			app_trigger_operation();
			
			//Check for time-out in the next state
			appState++;
		case 1:
			//Overflows and stops at 0 if no alarm is set
			if ( DRV_TMR_CounterValue32BitGet ( tmrHandle ) == 0 )
			{
				//Time-out
				return false;
			}
			else if ( app_operation_isComplete( ) )
			{
				//Operation is complete before time-out
				return true;
			}	
	</code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
	32-bit mode.
	This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_CounterValueGet" instead of this.
*/

#define DRV_TMR_CounterValue32BitGet(handle)	DRV_TMR_CounterValueGet(handle)     

// *****************************************************************************
/* Function:
    void DRV_TMR_Alarm16BitRegister 
    ( 
        DRV_HANDLE handle, 
        uint16_t period, 
        bool isPeriodic, 
        uintptr_t context, 
        DRV_TMR_CALLBACK callBack 
    )

  Summary:
    Sets up an alarm.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets up an alarm, allowing the client to receive a callback
    from the driver when the counter period elapses.  Alarms can be one-shot or
    periodic.  This API is valid only if the 16-bit mode of the timer is
    selected.  Otherwise use DRV_TMR_Alarm32BitRegister function. 

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid handle, returned from DRV_TMR_Open

    period      - 16-bit period which will be loaded into the Timer hardware register.
                  
    isPeriodic  - Flag indicating whether the alarm should be one-shot or periodic.
  
    context     - A reference, call back function will be called with the same 
                    reference.
                    
    callBack    - A call back function which will be called on period elapse.
    
  Returns:
    None

  Example:
    <code>
    //Do the initialization with 'mode' set to DRV_TMR_OPERATION_MODE_16_BIT
    
    void setupTask ()
    {
        DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open
        
        // Calculate the count to be passed on from the clock input
        //Periodically toggle LED
        DRV_TMR_Alarm16BitRegister ( tmrHandle, 0xFF40, true, 0,
                                        ToggleLedCallBack );
    }
    
    void ToggleLedCallBack ( uintptr_t context )
    {
        //Toggle
    }
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_AlarmRegister" instead of this.
*/

#define DRV_TMR_Alarm16BitRegister(handle, period, isPeriodic, context, callBack)	DRV_TMR_AlarmRegister(handle,period,isPeriodic,context,callBack)

// *****************************************************************************
/* Function:
    void DRV_TMR_Alarm32BitRegister 
    (
        DRV_HANDLE handle,
        uint32_t period, 
        bool isPeriodic, 
        uintptr_t context,	
        DRV_TMR_CALLBACK callBack 
    )

  Summary:
    Sets up an alarm.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets up an alarm, allowing the client to receive a callback
    from the driver when the counter period elapses.  Alarms can be one-shot or
    periodic.  This API is valid only if the 32-bit mode of the timer is
    selected Otherwise use DRV_TMR_Alarm16BitRegister function. 

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	32-Bit timer mode. 

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle      - A valid handle, returned from DRV_TMR_Open

    period      - 32-bit period which will be loaded into the Timer hardware register.
				  
	isPeriodic  - Flag indicating whether the alarm should be one-shot or periodic.
  
	context  	- A reference, call back function will be called with the same 
					reference.
	
	callBack 	- A call back function which will be called on period elapse.
 	
  Returns:
    None

  Example:
    <code>
    //Do the initialization with 'mode' set to DRV_TMR_OPERATION_MODE_32_BIT
	
	void setupTask ()
	{
		DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open
		
		// Calculate the count to be passed on from the clock input
		//Periodically toggle LED
		DRV_TMR_Alarm32BitRegister ( tmrHandle, 0xFFFFFF00, true, 0,
										ToggleLedCallBack );
    }
	
	void ToggleLedCallBack ( uintptr_t context )
	{
		//Toggle
	}
    </code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
	32-bit mode.
	This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_AlarmRegister" instead of this.
*/

#define DRV_TMR_Alarm32BitRegister(handle, period, isPeriodic, context, callBack)	DRV_TMR_AlarmRegister(handle,period,isPeriodic,context,callBack)   

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmPeriod16BitSet ( DRV_HANDLE handle, uint16_t value )

  Summary:
    Updates the 16-bit Timer's period.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function updates the 16-bit Timer's period. This API is valid only if 
    the 16-bit mode of the timer is selected
    Otherwise use DRV_TMR_AlarmPeriod32BitSet function. 

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    value        - 16-bit Period value

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open

    DRV_TMR_AlarmPeriod16BitSet ( handle, 0x1000 );
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_AlarmPeriodSet" instead of this.
*/

#define DRV_TMR_AlarmPeriod16BitSet(handle, value)	DRV_TMR_AlarmPeriodSet(handle,value)

// *****************************************************************************
/* Function:
    void DRV_TMR_AlarmPeriod32BitSet ( DRV_HANDLE handle, uint32_t period )

  Summary:
    Updates the 32-bit Timer's period.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function updates the 32-bit Timer's period. This API is valid only if 
	the 32-bit mode of the timer is selected
	Otherwise use DRV_TMR_AlarmPeriod16BitSet function. 

  Precondition:
    The DRV_TMR_Initialize unction must have been called. Must have selected
	32-Bit timer mode. 

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    period        - 32-bit Period value

  Returns:
    None.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open

    DRV_TMR_AlarmPeriod32BitSet ( handle, 0xFFFFFF00 );
    </code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
    32-bit mode.
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_AlarmPeriodSet" instead of this.
*/

#define DRV_TMR_AlarmPeriod32BitSet(handle, period)	DRV_TMR_AlarmPeriodSet(handle,period)    

// *****************************************************************************
/* Function:
    uint16_t DRV_TMR_AlarmPeriod16BitGet ( DRV_HANDLE handle )

  Summary:
    Provides the 16-bit Timer's period.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the 16-bit Timer's period. This API is valid only if 
	the 16-bit mode of the timer is selected.
	Otherwise use DRV_TMR_AlarmPeriod32BitGet function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    16-bit timer period value

  Example:
    <code>
    DRV_HANDLE tmrHandle;  // Returned from DRV_TMR_Open
    uint16_t period;
    
    period = DRV_TMR_AlarmPeriod16BitGet ( tmrHandle );
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_AlarmPeriodGet" instead of this.
*/

#define DRV_TMR_AlarmPeriod16BitGet(handle)	DRV_TMR_AlarmPeriodGet(handle)
    
// *****************************************************************************
/* Function:
    uint32_t DRV_TMR_AlarmPeriod32BitGet ( DRV_HANDLE handle )

  Summary:
    Provides the 32-bit Timer's period.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the 32-bit Timer's period. This API is valid only if 
    the 32-bit mode of the timer is selected
    Otherwise use DRV_TMR_AlarmPeriod16BitGet function. 

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    32-Bit timer mode. 

    DRV_TMR_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    32-bit Timer period value.

  Example:
    <code>
    DRV_HANDLE handle;  // Returned from DRV_TMR_Open
    uint32_t period;

    period = DRV_TMR_AlarmPeriod32BitGet ( handle );
    </code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
	32-bit mode.
	This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_AlarmPeriodGet" instead of this.
*/

#define DRV_TMR_AlarmPeriod32BitGet(handle)	DRV_TMR_AlarmPeriodGet(handle)     

// *****************************************************************************
/* Function:
    void DRV_TMR_Alarm16BitDeregister ( DRV_HANDLE handle )

  Summary:
    Removes a previously set alarm.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function removes a previously set alarm. This API is valid only if 
    the 16-bit mode of the timer is selected
    Otherwise use DRV_TMR_Alarm32BitDeregister function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
    16-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.
    
    DRV_TMR_Alarm16BitRegister function must have been called before.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None.

  Example:
    <code>
    //Example of a key debounce check 
	
	static unsigned int lastReadKey, readKey, keyCount, globalKeyState;
	DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open
	//Do the initialization with 'mode' set to DRV_TMR_OPERATION_MODE_16_BIT
	
	void keyPressDetect ()
	{	
		// Calculate the count to be passed on from the clock input
		//Periodically toggle LED
		DRV_TMR_Alarm16BitRegister ( tmrHandle, 0xFF00, true, 
										DebounceCheck );
    }
	
	void DebounceCheck ( uintptr_t context )
	{
		readKey = AppReadKey();
		
		if ( readKey != lastReadKey )
		{
			lastReadKey = readKey;
			keyCount = 0;
		}
		else
		{
			if ( keyCount > 20 )
			{
				globalKeyState = readKey;
				DRV_TMR_Alarm16BitDeregister ( tmrHandle );
			}
			keyCount++;
		}
	}
    </code>

  Remarks:
    This function will be deprecated later, so avoid using it as much as possible.
	Use generic function "DRV_TMR_AlarmDeregister" instead of this.
*/

#define DRV_TMR_Alarm16BitDeregister(handle)	DRV_TMR_AlarmDeregister(handle)

// *****************************************************************************
/* Function:
    void DRV_TMR_Alarm32BitDeregister ( DRV_HANDLE handle )

  Summary:
    Removes a previously set alarm.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function removes a previously set alarm. This API is valid only if 
	the 32-bit mode of the timer is selected
	Otherwise use DRV_TMR_Alarm16BitDeregister function.

  Precondition:
    The DRV_TMR_Initialize function must have been called. Must have selected
	32-Bit timer mode if mode selection is applicable.

    DRV_TMR_Open must have been called to obtain a valid opened device handle.
	
	DRV_TMR_Alarm32BitRegister function must have been called before.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None.

  Example:
    <code>
    //Example of a key debounce check 
    
    static unsigned int lastReadKey, readKey, keyCount, globalKeyState;
    DRV_HANDLE              tmrHandle;  // Returned from DRV_TMR_Open
    //Do the initialization with 'mode' set to DRV_TMR_OPERATION_MODE_32_BIT
    
    void keyPressDetect ( void )
    {   
        // Calculate the count to be passed on from the clock input
        //Periodically check the key status
        DRV_TMR_Alarm32BitRegister ( tmrHandle, 0xFF0FFD20, true, 0,
                                        DebounceCheck );
    }
    
    void DebounceCheck ( uintptr_t context )
    {
        readKey = AppReadKey();
        
        if ( readKey != lastReadKey )
        {
            lastReadKey = readKey;
            keyCount = 0;
        }
        else
        {
            if ( keyCount > 20 )
            {
                //Key is stable now
                globalKeyState = readKey;
                DRV_TMR_Alarm32BitDeregister ( tmrHandle );
            }
            keyCount++;
        }
    }
    </code>

  Remarks:
    In most of the devices only even numbered instances of timer supports 
    32-bit mode.
    This function will be deprecated later, so avoid using it as much as possible.
    Use generic function "DRV_TMR_AlarmDeregister" instead of this.
*/

#define DRV_TMR_Alarm32BitDeregister(handle)	DRV_TMR_AlarmDeregister(handle)
  

#endif // #ifndef _DRV_TMR_DEPRECATED_H

/*******************************************************************************
 End of File
*/

