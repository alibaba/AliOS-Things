#ifndef _USR_DATA_H__
#define _USR_DATA_H__

/* HTTP data structure */
#ifndef LWIP_HTTPD_CGI
#define LWIP_HTTPD_CGI            1
#endif

#ifndef LWIP_HTTPD_SSI
#define LWIP_HTTPD_SSI            1
#endif

#ifndef LWIP_HTTPD_SUPPORT_POST
#define LWIP_HTTPD_SUPPORT_POST   1
#endif

/** Set this to 1 to support HTTP request coming in in multiple packets/pbufs */
#ifndef LWIP_HTTPD_SUPPORT_REQUESTLIST
#define LWIP_HTTPD_SUPPORT_REQUESTLIST      1
#endif

#if LWIP_HTTPD_CGI
#ifndef LWIP_HTTPD_MAX_CGI_PARAMETERS
#define LWIP_HTTPD_MAX_CGI_PARAMETERS 16
#endif
#endif


#if LWIP_HTTPD_SSI

/** LWIP_HTTPD_SSI_MULTIPART==1: SSI handler function is called with 2 more
 * arguments indicating a counter for insert string that are too long to be
 * inserted at once: the SSI handler function must then set 'next_tag_part'
 * which will be passed back to it in the next call. */
//
#ifndef LWIP_HTTPD_SSI_MULTIPART
#define LWIP_HTTPD_SSI_MULTIPART    0
#endif

enum tag_check_state { 
  TAG_NONE,      
  TAG_LEADIN,     /* Tag lead in "<!--#" being processed */
  TAG_FOUND,      /* Tag name being read, looking for lead-out start */
  TAG_LEADOUT,    /* Tag lead out "-->" being processed */
  TAG_SENDING     /* Sending tag replacement string */
};
#endif /* LWIP_HTTPD_SSI */

struct http_state {
  struct fs_file *handle;
  char *file;       /* Pointer to first unsent byte in buf. */

#if LWIP_HTTPD_SUPPORT_REQUESTLIST
  struct pbuf *req;
#endif /* LWIP_HTTPD_SUPPORT_REQUESTLIST */

#if LWIP_HTTPD_SSI || LWIP_HTTPD_DYNAMIC_HEADERS
  char *buf;        /* File read buffer. */
  int buf_len;      /* Size of file read buffer, buf. */
#endif /* LWIP_HTTPD_SSI || LWIP_HTTPD_DYNAMIC_HEADERS */
  u32_t left;       /* Number of unsent bytes in buf. */
  u8_t retries;
#if LWIP_HTTPD_SSI
  const char *parsed;     /* Pointer to the first unparsed byte in buf. */
#if !LWIP_HTTPD_SSI_INCLUDE_TAG
  const char *tag_started;/* Poitner to the first opening '<' of the tag. */
#endif /* !LWIP_HTTPD_SSI_INCLUDE_TAG */
  const char *tag_end;    /* Pointer to char after the closing '>' of the tag. */
  u32_t parse_left; /* Number of unparsed bytes in buf. */
  u16_t tag_index;   /* Counter used by tag parsing state machine */
  u16_t tag_insert_len; /* Length of insert in string tag_insert */
#if LWIP_HTTPD_SSI_MULTIPART
  u16_t tag_part; /* Counter passed to and changed by tag insertion function to insert multiple times */
#endif /* LWIP_HTTPD_SSI_MULTIPART */
  u8_t tag_check;   /* true if we are processing a .shtml file else false */
  u8_t tag_name_len; /* Length of the tag name in string tag_name */
  char tag_name[LWIP_HTTPD_MAX_TAG_NAME_LEN + 1]; /* Last tag name extracted */
  char tag_insert[LWIP_HTTPD_MAX_TAG_INSERT_LEN + 1]; /* Insert string for tag_name */
  enum tag_check_state tag_state; /* State of the tag processor */
#endif /* LWIP_HTTPD_SSI */
#if LWIP_HTTPD_CGI
  char *params[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Params extracted from the request URI */
  char *param_vals[LWIP_HTTPD_MAX_CGI_PARAMETERS]; /* Values for each extracted param */
#endif /* LWIP_HTTPD_CGI */
#if LWIP_HTTPD_DYNAMIC_HEADERS
  const char *hdrs[NUM_FILE_HDR_STRINGS]; /* HTTP headers to be sent. */
  u16_t hdr_pos;     /* The position of the first unsent header byte in the
                        current string */
  u16_t hdr_index;   /* The index of the hdr string currently being sent. */
#endif /* LWIP_HTTPD_DYNAMIC_HEADERS */
#if LWIP_HTTPD_TIMING
  u32_t time_started;
#endif /* LWIP_HTTPD_TIMING */
#if LWIP_HTTPD_SUPPORT_POST
  u32_t post_content_len_left;
#if LWIP_HTTPD_POST_MANUAL_WND
  u32_t unrecved_bytes;
  struct tcp_pcb *pcb;
  u8_t no_auto_wnd;
#endif /* LWIP_HTTPD_POST_MANUAL_WND */
  s32 cgi_handler_index; 
  char *response_file; 
#endif /* LWIP_HTTPD_SUPPORT_POST*/
};
#endif //_USR_DATA_H__
