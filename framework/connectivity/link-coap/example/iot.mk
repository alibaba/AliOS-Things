TARGET      := coap-server coap-client
DEPENDS     += src
SRCS_coap-server := CoAPExample.c CoAPServer.c
SRCS_coap-client := CoAPClient.c
LDFLAGS     += -llink-coap -lplatform -lpthread
