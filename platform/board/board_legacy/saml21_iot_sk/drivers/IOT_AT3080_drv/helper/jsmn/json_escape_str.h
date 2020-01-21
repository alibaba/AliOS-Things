#ifndef _JSON_ESCAPE_STR_H
#define _JSON_ESCAPE_STR_H

#ifndef JSON_MIN
#define JSON_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef JSON_MAX
#define JSON_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

struct json_printbuf {
  char *buf;
  int bpos;
  int size;
};

extern int json_escape_str(struct json_printbuf *pb, const char *str, int len);
extern struct json_printbuf* json_printbuf_new(void);
extern void json_printbuf_reset(struct json_printbuf *p);
extern void json_printbuf_free(struct json_printbuf *p);


#endif
