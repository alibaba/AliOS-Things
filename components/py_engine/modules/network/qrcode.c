
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "cJSON.h"
#include "dev_info.h"
#include "modnetwork.h"
#include "usb_s_uvc_ioctl.h"
#include "zbar.h"

int network_qrcode_scan(void *gray, int w, int h, char *ouput, int max_len)
{
    int ret = -1, len = 0;
    char *data = NULL;
    zbar_image_scanner_t *scanner = NULL;
    zbar_image_t *image = NULL;
    const zbar_symbol_t *symbol = NULL;
    zbar_symbol_type_t type = ZBAR_NONE;

    do {
        scanner = zbar_image_scanner_create();
        if (NULL == scanner) {
            break;
        }
        /* configure the reader */
        zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

        /* wrap image data */
        image = zbar_image_create();
        if (NULL == image) {
            break;
        }

        zbar_image_set_format(image, *(int *)"Y800");
        zbar_image_set_size(image, w, h);
        zbar_image_set_data(image, gray, w * h, NULL);

        /* scan the image for barcodes */
        ret = zbar_scan_image(scanner, image);
        if (0 >= ret) {
            break;
        }

        symbol = zbar_image_first_symbol(image);
        for (; symbol; symbol = zbar_symbol_next(symbol)) {
            /* do something useful with results */
            type = zbar_symbol_get_type(symbol);
            if (ZBAR_QRCODE != type) {
                printf("zbar code ----> %s, skip\n",
                       zbar_get_symbol_name(type));
                continue;
            }

            data = (char *)zbar_symbol_get_data(symbol);
            len = (unsigned int)zbar_symbol_get_data_length(symbol);

            if (data && max_len > len) {
                memcpy(ouput, data, len);
                ret = len;
            }
        }
    } while (0);

    if (image) {
        zbar_image_free_data(image);
        zbar_image_destroy(image);
    }

    if (scanner) {
        zbar_image_scanner_destroy(scanner);
    }
    return ret;
}

int network_qrscan_result_process(const char *result, char *ssid, char *passwd)
{
    int ret = 0;
    cJSON *ssid_elem, *passwd_elem;
    cJSON *root = cJSON_Parse(result);
    if (root == NULL) {
        return 0;
    }
    // {"ssid":"aaaaa","pwd":"sssddddd"}
    ssid_elem = cJSON_GetObjectItem(root, "ssid");
    passwd_elem = cJSON_GetObjectItem(root, "pwd");

    do {
        if (ssid_elem == NULL || passwd_elem == NULL)
            break;

        if (!cJSON_IsString(ssid_elem) || !cJSON_IsString(passwd_elem))
            break;

        if (strlen(ssid_elem->valuestring) > NETWORK_SSID_MAX_LEN ||
            strlen(passwd_elem->valuestring) > NETWORK_PASSWD_MAX_LEN) {
            break;
        }

        strncpy(ssid, ssid_elem->valuestring, NETWORK_SSID_MAX_LEN);
        strncpy(passwd, passwd_elem->valuestring, NETWORK_PASSWD_MAX_LEN);
        ret = 1;
    } while (0);

    cJSON_Delete(root);

    return ret;
}