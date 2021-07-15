# 概述

WiFi Provisioning API 是一套简单易用的WiFi网络配置接口。

# 示例代码

```c
static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    switch (event) {
        case WIFI_PROV_EVENT_TIMEOUT:
            /* timeout */
            break;
        case WIFI_RPOV_EVENT_GOT_RESULT:
            /* we got the result */
            break;
    }
}

void main()
{
    wifi_prov_softap_register("YoC");

    wifi_prov_start(wifi_prov_get_method_id("softap"), wifi_pair_callback, 120);
}
```

