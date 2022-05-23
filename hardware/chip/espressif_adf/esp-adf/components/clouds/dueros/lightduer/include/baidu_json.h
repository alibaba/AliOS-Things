/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef baidu_json__h
#define baidu_json__h

#include "stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* baidu_json Types: */
#define baidu_json_False  (1 << 0)
#define baidu_json_True   (1 << 1)
#define baidu_json_NULL   (1 << 2)
#define baidu_json_Number (1 << 3)
#define baidu_json_String (1 << 4)
#define baidu_json_Array  (1 << 5)
#define baidu_json_Object (1 << 6)

#define baidu_json_IsReference 256
#define baidu_json_StringIsConst 512

/* The baidu_json structure: */
typedef struct baidu_json
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct baidu_json *next;
    struct baidu_json *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct baidu_json *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==baidu_json_String */
    char *valuestring;
    /* The item's number, if type==baidu_json_Number */
    int valueint;
    /* The item's number, if type==baidu_json_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} baidu_json;

typedef struct baidu_json_Hooks
{
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} baidu_json_Hooks;

/* Supply malloc, realloc and free functions to baidu_json */
extern void baidu_json_InitHooks(baidu_json_Hooks* hooks);
extern int baidu_json_Uninit(void);

/* Supply a block of JSON, and this returns a baidu_json object you can interrogate. Call baidu_json_Delete when finished. */
extern baidu_json *baidu_json_Parse(const char *value);
/* Render a baidu_json entity to text for transfer/storage. Free the char* when finished. */
extern char  *baidu_json_Print(const baidu_json *item);
/* Render a baidu_json entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *baidu_json_PrintUnformatted(const baidu_json *item);
/* Render a baidu_json entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
extern char *baidu_json_PrintBuffered(const baidu_json *item, int prebuffer, int fmt);
/* Delete a baidu_json entity and all subentities. */
extern void   baidu_json_Delete(baidu_json *c);

/* Returns the number of items in an array (or object). */
extern int	  baidu_json_GetArraySize(const baidu_json *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern baidu_json *baidu_json_GetArrayItem(const baidu_json *array, int item);
/* Get item "string" from object. Case insensitive. */
extern baidu_json *baidu_json_GetObjectItem(const baidu_json *object, const char *string);
extern int baidu_json_HasObjectItem(const baidu_json *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when baidu_json_Parse() returns 0. 0 when baidu_json_Parse() succeeds. */
extern const char *baidu_json_GetErrorPtr(void);

/* These calls create a baidu_json item of the appropriate type. */
extern baidu_json *baidu_json_CreateNull(void);
extern baidu_json *baidu_json_CreateTrue(void);
extern baidu_json *baidu_json_CreateFalse(void);
extern baidu_json *baidu_json_CreateBool(int b);
extern baidu_json *baidu_json_CreateNumber(double num);
// len indicate the length of string, if len <=0, the length will be got throuth strlen(string)
extern baidu_json *baidu_json_CreateString(const char *string, size_t len);
extern baidu_json *baidu_json_CreateArray(void);
extern baidu_json *baidu_json_CreateObject(void);

/* These utilities create an Array of count items. */
extern baidu_json *baidu_json_CreateIntArray(const int *numbers, int count);
extern baidu_json *baidu_json_CreateFloatArray(const float *numbers, int count);
extern baidu_json *baidu_json_CreateDoubleArray(const double *numbers, int count);
extern baidu_json *baidu_json_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
extern void baidu_json_AddItemToArray(baidu_json *array, baidu_json *item);
extern void	baidu_json_AddItemToObject(baidu_json *object, const char *string, baidu_json *item);
extern void	baidu_json_AddItemToObjectCS(baidu_json *object, const char *string, baidu_json *item);	/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the baidu_json object */
/* Append reference to item to the specified array/object. Use this when you want to add an existing baidu_json to a new baidu_json, but don't want to corrupt your existing baidu_json. */
extern void baidu_json_AddItemReferenceToArray(baidu_json *array, baidu_json *item);
extern void	baidu_json_AddItemReferenceToObject(baidu_json *object, const char *string, baidu_json *item);

/* Remove/Detatch items from Arrays/Objects. */
extern baidu_json *baidu_json_DetachItemFromArray(baidu_json *array, int which);
extern void   baidu_json_DeleteItemFromArray(baidu_json *array, int which);
extern baidu_json *baidu_json_DetachItemFromObject(baidu_json *object, const char *string);
extern void   baidu_json_DeleteItemFromObject(baidu_json *object, const char *string);

/* Update array items. */
extern void baidu_json_InsertItemInArray(baidu_json *array, int which, baidu_json *newitem); /* Shifts pre-existing items to the right. */
extern void baidu_json_ReplaceItemInArray(baidu_json *array, int which, baidu_json *newitem);
extern void baidu_json_ReplaceItemInObject(baidu_json *object,const char *string,baidu_json *newitem);

/* Duplicate a baidu_json item */
extern baidu_json *baidu_json_Duplicate(const baidu_json *item, int recurse);
/* Duplicate will create a new, identical baidu_json item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error. If not, then baidu_json_GetErrorPtr() does the job. */
extern baidu_json *baidu_json_ParseWithOpts(const char *value, const char **return_parse_end, int require_null_terminated);

extern void baidu_json_Minify(char *json);

extern void baidu_json_release(void *ptr);

/* Macros for creating things quickly. */
#define baidu_json_AddNullToObject(object,name) baidu_json_AddItemToObject(object, name, baidu_json_CreateNull())
#define baidu_json_AddTrueToObject(object,name) baidu_json_AddItemToObject(object, name, baidu_json_CreateTrue())
#define baidu_json_AddFalseToObject(object,name) baidu_json_AddItemToObject(object, name, baidu_json_CreateFalse())
#define baidu_json_AddBoolToObject(object,name,b) baidu_json_AddItemToObject(object, name, baidu_json_CreateBool(b))
#define baidu_json_AddNumberToObject(object,name,n) baidu_json_AddItemToObject(object, name, baidu_json_CreateNumber(n))
#define baidu_json_AddStringToObject(object,name,s) baidu_json_AddItemToObject(object, name, baidu_json_CreateString(s, 0))
#define baidu_json_AddStringToObjectWithLength(object,name,s,len) baidu_json_AddItemToObject(object, name, baidu_json_CreateString(s, len))

/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the baidu_json object */
#define baidu_json_AddNullToObjectCS(object,name) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateNull())
#define baidu_json_AddTrueToObjectCS(object,name) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateTrue())
#define baidu_json_AddFalseToObjectCS(object,name) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateFalse())
#define baidu_json_AddBoolToObjectCS(object,name,b) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateBool(b))
#define baidu_json_AddNumberToObjectCS(object,name,n) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateNumber(n))
#define baidu_json_AddStringToObjectCS(object,name,s) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateString(s, 0))
#define baidu_json_AddStringToObjectCSWithLength(object,name,s,len) baidu_json_AddItemToObjectCS(object, name, baidu_json_CreateString(s, len))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define baidu_json_SetIntValue(object,val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))
#define baidu_json_SetNumberValue(object,val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))

/* Macro for iterating over an array */
#define baidu_json_ArrayForEach(pos, head) for(pos = (head)->child; pos != NULL; pos = pos->next)

#ifdef __cplusplus
}
#endif

#endif
