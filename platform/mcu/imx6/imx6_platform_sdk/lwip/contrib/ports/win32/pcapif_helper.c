/**
 * pcapif_helper.c - This file is part of lwIP pcapif and provides helper functions
 * for managing the link state.
 */

#include "pcapif_helper.h"


#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32

#ifdef _MSC_VER
#pragma warning (disable: 4201) /* don't warn about union without name */
#endif /* _MSC_VER */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <packet32.h>
#include <ntddndis.h>

struct pcapifh_linkstate {
  LPADAPTER        lpAdapter;
  PPACKET_OID_DATA ppacket_oid_data;
};

struct pcapifh_linkstate* pcapifh_linkstate_init(const char *adapter_name)
{
  struct pcapifh_linkstate* state = (struct pcapifh_linkstate*)malloc(sizeof(struct pcapifh_linkstate));
  if (state != NULL) {
    memset(state, 0, sizeof(struct pcapifh_linkstate));
    state->ppacket_oid_data = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA) + sizeof(NDIS_MEDIA_STATE));
    if (state->ppacket_oid_data == NULL) {
      free(state);
      state = NULL;
    } else {
      state->lpAdapter = PacketOpenAdapter((char*)adapter_name);
      if ((state->lpAdapter == NULL) || (state->lpAdapter->hFile == INVALID_HANDLE_VALUE)) {
        /* failed to open adapter */
        free(state);
        state = NULL;
      }
    }
  }
  return state;
}

enum pcapifh_link_event pcapifh_linkstate_get(struct pcapifh_linkstate* state)
{
  enum pcapifh_link_event ret = PCAPIF_LINKEVENT_UNKNOWN;
  if (state != NULL) {
    state->ppacket_oid_data->Oid    = OID_GEN_MEDIA_CONNECT_STATUS;
    state->ppacket_oid_data->Length = sizeof(NDIS_MEDIA_STATE);
    if (PacketRequest(state->lpAdapter, FALSE, state->ppacket_oid_data)) {
      NDIS_MEDIA_STATE fNdisMediaState;
      fNdisMediaState = (*((PNDIS_MEDIA_STATE)(state->ppacket_oid_data->Data)));
      ret = ((fNdisMediaState == NdisMediaStateConnected) ? PCAPIF_LINKEVENT_UP : PCAPIF_LINKEVENT_DOWN);
    }
  }
  return ret;
}

void pcapifh_linkstate_close(struct pcapifh_linkstate* state)
{
  if (state != NULL) {
    if (state->lpAdapter != NULL) {
      PacketCloseAdapter(state->lpAdapter);
    }
    if (state->ppacket_oid_data != NULL) {
      free(state->ppacket_oid_data);
    }
    free(state);
  }
}

#else /* WIN32 */

/* @todo: add linux/unix implementation? */

struct pcapifh_linkstate {
  u8_t empty;
};

struct pcapifh_linkstate* pcapifh_linkstate_init(const char *adapter_name)
{
  LWIP_UNUSED_ARG(adapter_name);
  return NULL;
}

enum pcapifh_link_event pcapifh_linkstate_get(struct pcapifh_linkstate* state)
{
  LWIP_UNUSED_ARG(state);
  LWIP_ASSERT("not implemented", 0);
  return LINKEVENT_UNKNOWN;
}
void pcapifh_linkstate_close(struct pcapifh_linkstate* state)
{
  LWIP_UNUSED_ARG(state);
  LWIP_ASSERT("not implemented", 0);
}

#endif /* WIN32 */