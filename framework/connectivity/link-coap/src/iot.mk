LIBA_TARGET     := liblink-coap.a

CFLAGS           += 
#CFLAGS          += -DCOAP_OBSERVE_SERVER_DISABLE -DCOAP_OBSERVE_CLIENT_DISABLE
DEPENDS         := platform
LIB_HEADERS     := \
    CoAPExport.h \
	CoAPNetwork.h\
	lite-list.h\
	CoAPObserve.h\
	CoAPResource.h\
	CoAPMessage.h\
	CoAPPlatform.h
