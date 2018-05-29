#include "mx_common.h"
#include "json_escape_str.h"

int json_escape_str(struct json_printbuf *pb, const char *str, int len);
struct json_printbuf* json_printbuf_new(void);
void json_printbuf_reset(struct json_printbuf *p);
void json_printbuf_free(struct json_printbuf *p);

struct json_printbuf* json_printbuf_new(void)
{
  struct json_printbuf *p;

  p = (struct json_printbuf*)calloc(1, sizeof(struct json_printbuf));
  if(!p) return NULL;
  p->size = 4;
  p->bpos = 0;
  if(!(p->buf = (char*)malloc(p->size))) {
    free(p);
    return NULL;
  }
  return p;
}

void json_printbuf_reset(struct json_printbuf *p)
{
  p->buf[0] = '\0';
  p->bpos = 0;
}

void json_printbuf_free(struct json_printbuf *p)
{
  if(p) {
    free(p->buf);
    free(p);
  }
}

static int json_printbuf_memappend(struct json_printbuf *p, const char *buf, int size)
{
  char *t;
  if(p->size - p->bpos <= size) {
    int new_size = JSON_MAX(p->size * 2, p->bpos + size + 8);
//#ifdef json_printbuf_DEBUG
//    MC_DEBUG("json_printbuf_memappend: realloc "
//	     "bpos=%d wrsize=%d old_size=%d new_size=%d\n",
//	     p->bpos, size, p->size, new_size);
//#endif /* json_printbuf_DEBUG */
    if(!(t = (char*)realloc(p->buf, new_size))) return -1;
    p->size = new_size;
    p->buf = t;
  }
  memcpy(p->buf + p->bpos, buf, size);
  p->bpos += size;
  p->buf[p->bpos]= '\0';
  return size;
}

/* string escaping */
int json_escape_str(struct json_printbuf *pb, const char *str, int len)
{
  int pos = 0, start_offset = 0;
  const char *json_hex_chars = "0123456789abcdef";

  unsigned char c;
  
  if(len == 0) {
	  json_printbuf_memappend(pb, str, pos);
	  return 0;
  }
  
  while (len--) {
    c = str[pos];
    switch(c) {
    case '\b':
    case '\n':
    case '\r':
    case '\t':
    case '"':
    case '\\':
    case '/':
      if(pos - start_offset > 0)
	json_printbuf_memappend(pb, str + start_offset, pos - start_offset);
      if(c == '\b') json_printbuf_memappend(pb, "\\b", 2);
      else if(c == '\n') json_printbuf_memappend(pb, "\\n", 2);
      else if(c == '\r') json_printbuf_memappend(pb, "\\r", 2);
      else if(c == '\t') json_printbuf_memappend(pb, "\\t", 2);
      else if(c == '"') json_printbuf_memappend(pb, "\\\"", 2);
      else if(c == '\\') json_printbuf_memappend(pb, "\\\\", 2);
      else if(c == '/') json_printbuf_memappend(pb, "\\/", 2);
      start_offset = ++pos;
      break;
    default:
      if(c < ' ') {
	if(pos - start_offset > 0)
	  json_printbuf_memappend(pb, str + start_offset, pos - start_offset);
	sprintf((char *)pb, "\\u00%c%c",
	        json_hex_chars[c >> 4],
	        json_hex_chars[c & 0xf]);
	start_offset = ++pos;
      } else pos++;
    }
  }
  if(pos - start_offset > 0)
    json_printbuf_memappend(pb, str + start_offset, pos - start_offset);
  return 0;
}
