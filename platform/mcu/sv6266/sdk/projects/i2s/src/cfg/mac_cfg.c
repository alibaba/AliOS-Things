#include <stdint.h>
#include <cJSON.h>
struct wifi_cfg {
    char mac_string[4096];
};

const struct wifi_cfg g_wifi_cfg __attribute__((section(".cfg_mac_data"))) = {
    .mac_string = "{\"addr1\": \"44:57:18:3f:79:A1\", \"addr2\": \"44:57:18:3f:79:A1\"}",
};

cJSON *wifi_cfg_init() {
    return cJSON_Parse(g_wifi_cfg.mac_string);
}

void macstring_parser(char *macstr, char *addr)
{
    u8 len = strlen(macstr);
    u8 i, index = 0;

    if(len > 17)
    {
        goto err;
    }

    addr[index] = 0;
    for(i = 0; i < len; i++)
    {
        if(index > 5)
            goto  err;
        
        if('0' <= macstr[i] && macstr[i] <= '9')
        {
            addr[index] = (addr[index] << 4) + macstr[i] - '0';
        }
        else if('a' <= macstr[i] && macstr[i] <= 'f')
        {
            addr[index] = (addr[index] << 4) + macstr[i] - 'a' + 10;
        }
        else if('A' <= macstr[i] && macstr[i] <= 'F')
        {
            addr[index] = (addr[index] << 4) + macstr[i] - 'A' + 10;
        }
        else if(macstr[i] == ':')
        {
            index++;
            addr[index] = 0;
        }
        else
        {
            goto err;
        }
        
    }

    return;

err:
    printf("Illegal mac string : %s\n", macstr);
}

void wifi_cfg_get_addr1(const cJSON *handle, char addr[6]) {
    char *tmpchar = cJSON_GetObjectItem(handle, "addr1")->valuestring;
    macstring_parser(tmpchar, addr);
//    printf("addr1:%s\n", tmpchar);
}

void wifi_cfg_get_addr2(const cJSON *handle, char addr[6]) {
    char *tmpchar = cJSON_GetObjectItem(handle, "addr2")->valuestring;
    macstring_parser(tmpchar, addr);
//    printf("addr2:%s\n", tmpchar);
}

void wifi_cfg_deinit(cJSON *handle) {
    cJSON_Delete(handle);
}

