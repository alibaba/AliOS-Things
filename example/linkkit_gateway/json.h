#ifndef __JSON_H__
#define __JSON_H__

#define BOOLEAN BOOLEAN_JSON

typedef enum {
    UNKNOWN,

    INT,
    ARRAY,
    DOUBLE,
    STRING,
    OBJECT,
    BOOLEAN,
} AJsonObjType;

typedef struct {
    AJsonObjType type;  /* see AJsonObjType */

    int key_len;
    char *key;

    int val_len;
    char *val;
} AJsonObject;

int AJson_Parse(AJsonObject *root, char *js, int len);

AJsonObject *AJson_First(AJsonObject *root, AJsonObject *tmp);
AJsonObject *AJson_Next (AJsonObject *root, AJsonObject *tmp);

int AJson_GetInt(AJsonObject *obj, int *val);
int AJson_GetDouble(AJsonObject *obj, double *val);
int AJson_GetString(AJsonObject *obj, char **str, int *str_len);
int AJson_CopyString(AJsonObject *obj, char *buf, int len);
int AJson_GetBoolean(AJsonObject *obj, int *val);

#define AJson_ForEachObject(parent, k, k_len, obj) \
    char *k = NULL;                                \
    int k_len = 0;                                 \
    AJsonObject tmp##k, *obj = NULL;               \
    for (obj = AJson_First(parent, &tmp##k);       \
            ({ if (obj) {                          \
                 k = (obj)->key;                   \
                 k_len = (obj)->key_len;           \
             }; obj;}); obj = AJson_Next(parent, &tmp##k))

#define AJson_ForEachObjectInArray(parent, obj)    \
    AJsonObject tmp##k, *obj = NULL;               \
    for (obj = AJson_First(parent, &tmp##k); obj; obj = AJson_Next(parent, &tmp##k))

#endif /* __JSON_H__ */
