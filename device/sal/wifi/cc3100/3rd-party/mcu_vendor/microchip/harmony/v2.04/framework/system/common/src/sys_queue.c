#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "system/common/sys_common.h"
#include "system/common/sys_queue.h"


// *****************************************************************************
/******************************************************************************
  Function:
    QUEUE_OBJECT *QUEUE_Create();
 *****************************************************************************/

void QUEUE_Initialize(QUEUE_OBJECT *queue, uint8_t size, QUEUE_ELEMENT_OBJECT *qelementPool)
{

    //SYS_ASSERT((queue != (QUEUE_OBJECT *)NULL),"Queue is NULL ");

    {
        queue->head         = NULL;
        queue->tail         = NULL;
        queue->numElements  = 0;
        queue->size         = size;
        queue->qElementPool = qelementPool;
    }

}


// *****************************************************************************
/******************************************************************************
  Function:
    void QUEUE_Reset(QUEUE_OBJECT *queue);
 *****************************************************************************/

void QUEUE_Reset(QUEUE_OBJECT *queue)
{
        queue->head         = NULL;
        queue->tail         = NULL;
        queue->numElements  = 0;
        queue->size         = 0;
}


// *****************************************************************************
/******************************************************************************
  Function:
    int8_t QUEUE_PushBuffer(QUEUE_OBJECT *queue, DRV_HANDLE handle, uint8_t buffer,
        uint32_t size);

 *****************************************************************************/

int8_t QUEUE_Push(QUEUE_OBJECT *queue, void *data)
{
int8_t i =0;
int8_t elementIndex = 0;
QUEUE_ELEMENT_OBJECT *tail = NULL;
QUEUE_ELEMENT_OBJECT *queue_element = NULL;

    for(i = 0 ; i < queue->size ; i++)
    {
        if(!queue->qElementPool[i].inUse)
        {
            queue->qElementPool[i].inUse = true;
            queue_element = &queue->qElementPool[i];
            elementIndex = i;
            break;
        }
    }

    if(queue_element== (QUEUE_ELEMENT_OBJECT *)NULL)
    {
        SYS_ASSERT((queue_element== (QUEUE_ELEMENT_OBJECT *)NULL), "Unable to create queue elements ");
    }

    else
    {
        queue_element->next = NULL;
        queue_element->data = data;

        /* OSAL Lock Mutex - TODO */
        if(queue->head == (QUEUE_ELEMENT_OBJECT *)NULL)
        {
            queue->head = queue_element;
        }
        else
        {
            tail = queue->tail;
            while(tail->next != NULL){tail = (QUEUE_ELEMENT_OBJECT *)tail->next;}
            tail->next = (QUEUE_ELEMENT_OBJECT *)queue_element;
        }
        
        queue->tail = queue_element;
        queue->numElements++;
        /* OSAL unlock Mutex - TODO */
    }

    return(elementIndex);


}



// *****************************************************************************
/******************************************************************************
  Function:
    void QUEUE_PopBuffer(QUEUE_OBJECT *queue, DRV_HANDLE handle, uint8_t buffer,
        uint32_t size);

 *****************************************************************************/

void QUEUE_Pop(QUEUE_OBJECT *queue)
{
QUEUE_ELEMENT_OBJECT *queue_element = NULL;

    queue_element = queue->head;

    /* OSAL Lock Mutex - TODO */
    if(queue_element != (QUEUE_ELEMENT_OBJECT *)NULL)
    {
        queue->head = (QUEUE_ELEMENT_OBJECT *)queue_element->next;

        queue_element->data = NULL;
        queue_element->next   = NULL;
        queue_element->inUse  = false;
        queue->numElements--;
    }
    
    if(queue->head == (QUEUE_ELEMENT_OBJECT *)NULL)
    {
        queue->tail = (QUEUE_ELEMENT_OBJECT *)NULL;
    }

    /* OSAL unlock Mutex - TODO */

}

/*******************************************************************************
 End of File
*/
