/*******************************************************************************
  Queue Interface
  
  Company:
    Microchip Technology Inc.
    
  File Name:
    sys_queue.h

  Summary:
    Queue interface.

  Description:

*******************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

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


#ifndef _QUEUE_H
#define _QUEUE_H

/*****************************************************************************/
/* Queue Element Object

   Summary
    Structure required for maintaining the queue element.

   Description
    Defines the structure required for maintaining the queue element.

   Remarks:
    None.
*/

typedef struct _QUEUE_ELEMENT_OBJECT
{
    // The buffer Q Element
    void                                   *data;

    // Flag to indicate that the element is in use
    bool                                   inUse;

    // The next queue element in the queue
    struct _QUEUE_ELEMENT_OBJECT           *next;

}QUEUE_ELEMENT_OBJECT;


/*****************************************************************************/
/* Queue Object

   Summary
    Structure required for maintaining the queue.

   Description
    Defines the structure required for maintaining the queue.

   Remarks:
    None.
*/

typedef struct
{
    // The Head queue element in the queue
    QUEUE_ELEMENT_OBJECT            *head;

    // The Tail queue element in the queue
    QUEUE_ELEMENT_OBJECT            *tail;

    // The number of objects in the queue.
    uint8_t                         numElements;

    // The size of the queue
    uint8_t                         size;

    // Pool of elements from which the elements are created from and deleted to.
    // If qElementPool is NULL, the elements are allocated dynamically
    QUEUE_ELEMENT_OBJECT            *qElementPool;

}QUEUE_OBJECT;


// *****************************************************************************
/*
  Function:
    void QUEUE_Initialize(QUEUE_OBJECT *queue, uint8_t size, QUEUE_ELEMENT_OBJECT *qelementPool)

  Summary:
    Initializes a queue object.

  Description:
    This function initializes a queue object.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    QUEUE_OBJECT queue;
    QUEUE_ELEMENT_OBJECT myqelementPool[20];
    QUEUE_Initialize(queue, MY_Q_SIZE , myqelementPool);
    </code>

  Remarks:
    None.

*/

void QUEUE_Initialize(QUEUE_OBJECT *queue, uint8_t size, QUEUE_ELEMENT_OBJECT *qelementPool);


// *****************************************************************************
/*
  Function:
    void QUEUE_Reset(QUEUE_OBJECT *queue);

  Summary:
    Deletes a Buffer Queue object.

  Description:
    This function deletes the buffer queue object.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    QUEUE_Reset(queue);
    </code>

  Remarks:
    None.

*/

void QUEUE_Reset(QUEUE_OBJECT *queue);

// *****************************************************************************
/*
  Function:
    QUEUE_ELEMENT_OBJECT* QUEUE_Front(QUEUE_OBJECT *queue);

  Summary:
    Returns a reference to the value at the front of a non-empty queue.

  Description:
    This function returns a reference to the value at the front of a non-empty queue.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    QUEUE_OBJECT         q;
    QUEUE_ELEMENT_OBJECT *qelement = NULL;
    QUEUE_Initialize(&q, MY_Q_SIZE, false);
    qelement = QUEUE_Front(q);
    </code>

  Remarks:
    None.

*/

#define QUEUE_Front(queue) ((queue)->head)


// *****************************************************************************
/*
  Function:
    void QUEUE_Pop(QUEUE_OBJECT *queue);

  Summary:
    Removes the item at the front of a non-empty queue.

  Description:
    This function removes the item at the front of a non-empty queue.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    QUEUE_OBJECT         q;
    QUEUE_ELEMENT_OBJECT *qelement = NULL;

    QUEUE_Initialize(&q, MY_Q_SIZE, false);

    QUEUE_Pop(q);

    </code>

  Remarks:
    None.

*/

void QUEUE_Pop(QUEUE_OBJECT *queue);

// *****************************************************************************
/*
  Function:
    int8_t QUEUE_Push(QUEUE_OBJECT *queue, QUEUE_ELEMENT_OBJECT *qelement);

  Summary:
    Inserts the argument qelement at the back of the queue.

  Description:
    This function inserts the argument qelement at the back of the queue.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    QUEUE_OBJECT         q;
    QUEUE_ELEMENT_OBJECT qelement;
    QUEUE_Initialize(&q, MY_Q_SIZE, false);
    QUEUE_Push(q, &qelement);
    </code>

  Remarks:
    None.

*/

int8_t QUEUE_Push(QUEUE_OBJECT *queue, void *data);


// *****************************************************************************
/*
  Function:
    bool QUEUE_IsEmpty(QUEUE_OBJECT *queue);

  Summary:
    Returns the empty statues of the queue.

  Description:
    This function returns whether or not the queue is empty.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    - true  - The queue is empty
	- false - The queue is not empty.

  Example:
    <code>
    QUEUE_OBJECT         q;
    QUEUE_ELEMENT_OBJECT qelement;

    QUEUE_Initialize(&q, MY_Q_SIZE, false);

    if(!QUEUE_IsEmpty(q))
    {
        QUEUE_Pop(q);
    }

    </code>

  Remarks:
    None.

*/

#define QUEUE_IsEmpty(queue) ((0 == (queue)->numElements)?true:false)

// *****************************************************************************
/*
  Function:
    bool QUEUE_IsFull(QUEUE_OBJECT *queue);

  Summary:
    Returns the full status of the queue.

  Description:
    This function returns whether or not the queue is full.

  Precondition:
    QUEUE_Initialize should have been called.

  Parameters:
    None.

  Returns:
    - true  - The queue is full
	- false - The queue is not full

  Example:
    <code>
    QUEUE_OBJECT         q;
    QUEUE_ELEMENT_OBJECT qelement;

    QUEUE_Initialize(&q, MY_Q_SIZE, false);

    if(!QUEUE_IsFull(q))
    {
        QUEUE_Push(q, &qelement);
    }

    </code>

  Remarks:
    None.

*/

#define QUEUE_IsFull(queue) (((queue)->size <= (queue)->numElements)?true:false)


#endif //_QUEUE_H

/*******************************************************************************
 End of File
*/