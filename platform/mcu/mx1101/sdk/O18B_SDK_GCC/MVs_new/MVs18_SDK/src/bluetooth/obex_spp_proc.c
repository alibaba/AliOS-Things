//  maintainer: Halley
#include "type.h"
#include "app_config.h"
#include "bt_stack_api.h"

void NewBTDevInfoUpdate(uint8_t* UniCode16/*BE*/, uint32_t Len, uint8_t* NewBdAddr);

void ObexCbFunc(const ObexCallbackInfo* Info)
{
	switch(Info->event)
	{
		case BT_OBEX_CONNECTED:
			DBG("```````BT_OBEX_CONNECTED\n");
			break;
		case BT_OBEX_DISCONNECTED:
			DBG("````````BT_OBEX_DISCONNECTED\n");
			break;

		case BT_OBEX_DATA_IND:
			if(BT_OBEX_NAME == Info->type)
			{
				NewBTDevInfoUpdate(Info->segdata, Info->seglen, 0);
			}
			break;

		default:
			break;
	}
}
