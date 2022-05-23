#include <string.h>
#include <ctype.h>
#include "audio_error.h"
#include "esp_log.h"
#include "audio_mem.h"

static char *TAG = "AUDIO_URL";

static unsigned char char_to_hex(unsigned char x)
{
    return  x > 9 ? x + 0x37 : x + 0x30;
}

char *audio_url_encode(const char *str)
{
    int out_len = (strlen(str) + 1) * 3;
    char *final = audio_malloc(out_len);
    AUDIO_MEM_CHECK(TAG, final, return NULL);
    char *tmp = final;
    for (size_t i = 0; i < strlen(str); i++) {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') || (str[i] == '_') ||
            (str[i] == '.') || (str[i] == '!') ||
            (str[i] == '@') || (str[i] == '#') ||
            (str[i] == '$') || (str[i] == '&') ||
            (str[i] == '*') || (str[i] == '(') ||
            (str[i] == ')') || (str[i] == '=') ||
            (str[i] == ':') || (str[i] == '/') ||
            (str[i] == ',') || (str[i] == ';') ||
            (str[i] == '?') || (str[i] == '+') ||
            (str[i] == '\'') || (str[i] == '~')) {
            *final++ = str[i];
        } else {
            *final++ = '%';
            *final++ = char_to_hex((unsigned char)str[i] >> 4);
            *final++ = char_to_hex((unsigned char)str[i] % 16);
        }
    }
    *final = 0;
    return tmp;
}

char *audio_url_decode(const char *str)
{
    char *final = audio_malloc(strlen(str) + 1);
    AUDIO_MEM_CHECK(TAG, final, return NULL);
    char *tmp = final;
    while (*str) {
        if (*str == '%') {
            char buffer[3] = { str[1], str[2], 0 };
            *final++ = strtol(buffer, NULL, 16);
            str += 3;
        } else {
            *final++ = *str++;
        }
    }
    *final = 0;
    return tmp;
}
