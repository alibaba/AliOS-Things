
#include "types.h"
#include "atcmd.h"
#include "error.h"
#include "mdns.h"
#include "dns_sd.h"
#include "wificonf.h"
#include "lwip/def.h"

#include "lwip/autoip.h"
#include "wifinetstack.h"
#include "HTTPServer.h"
//#include "drv_spimst.h"

#define ServiceType "_http._tcp"
#define ServiceDomain "local"
#define ServicePort 5123
#define NewServiceName "New - Bonjour Service Name-tim"
#define ReServiceName "Re - Bonjour Service Name-tim"
#define STATIC_IP "192.168.1.1"
#define STATIC_MASK "255.255.255.0"
#define STATIC_GW "192.168.1.1"

static DNSServiceRef client;
extern int wac_start();

int RemoveBonjourService()
{
    int err = -1;
    DNSServiceRefDeallocate(client);
    client = NULL;
    err = 0;
    return err;
}

int RegisterBonjourService(char* servicename, char* type, unsigned short port)
{
    DNSServiceErrorType err;
    
    printf("Register Bonjour Service: %s type: %s port: %d\n", servicename, type, port);
    // Register Bonjour Service
    err = DNSServiceRegister(&client,                      // DNSServiceRef
            0,                                         // DNSServiceFlags
            kDNSServiceInterfaceIndexAny,                  // interface index
            servicename, type,                             // service name
            "local",                                 // domain
            NULL,                                          // host
            htons(port),                            // port
            NULL,                // txt record length
            NULL,              // txt record pointer
            NULL,                                          // callback
            NULL);                                         // context
    if (!err) {
        printf("Register Bonjour Service successfully!\n");
    } else {
        printf("Register Bonjour Service failed.\n");
    }
    
    return err;
}

int ReNameBonjourService(char* servicename, char* type, unsigned short port)
{
    int err = 0;
    err = RemoveBonjourService();
    if (err) {
        printf("Remove Bonjour Service failed\n");
    } else {
        err = RegisterBonjourService(servicename, type, port);
        if (err) {
            printf("Register Bonjour Service failed\n");
        }
    }
    return err;
}

void homekit_mdns_reg()
{
    RegisterBonjourService(NewServiceName, ServiceType, ServicePort);
    OS_TaskDelete(NULL);
}

void homekit_mdns_start()
{
    mdnsd_start();
    OS_TaskDelete(NULL);
}

void cli_mdns_reg(char* servicename, char* type, unsigned short port)
{
    //printf("--- mdns_reg_cli task begin ---\n");
    if (OS_TaskCreate( homekit_mdns_reg, "mdns_reg", 2048, NULL, OS_TASK_PRIO3, NULL ) != 1)
    {
        printf("mdns_reg task create fail\n");
    }
}

int cli_mdns_start()
{
    if (OS_TaskCreate(homekit_mdns_start, "mdns_start", 4096, NULL, OS_TASK_PRIO3, NULL ) != true)
    {
        //atcmdprintf("mdns start create fail\n");
        printf("[%d] %s task create fail\n", __LINE__, __func__);
        return -1;
    }
    return 0;
}

void ip_addr_setting(uip_ip4addr_t *ip, char *para)
{
    //uint32_t ip[4];
    char *ptr, *head, *tail;
    int i;

    ptr = para;
    
    for(i = 0; i < 4; i ++) {
        head = ptr;
        while(*ptr && *ptr != '.') ptr ++;
        tail = ptr ++;
        *tail = 0;
        ip->u8[i] = atoi(head);
    }
    //IP4_ADDR(ip, ip[0], ip[1], ip[2], ip[3]);
}

void interface_get()
{
    u32 ip, gw, mask;
    netdev_getipv4info("et0", &ip, &gw, &mask);
    printf("\nGet IP: %d.%d.%d.%d\n", (ip >> 0) & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);
    printf("GW: %d.%d.%d.%d\n", (gw >> 0) & 0xff, (gw >> 8) & 0xff, (gw >> 16) & 0xff, (gw >> 24) & 0xff);
    printf("Mask: %d.%d.%d.%d\n", (mask >> 0) & 0xff, (mask >> 8) & 0xff, (mask >> 16) & 0xff, (mask >> 24) & 0xff);
}

void interface_setting(stParam *param)
{
    if((param->argc == 3) || (param->argc == 4)) {
        uip_ip4addr_t	local_ip_addr;
        uip_ip4addr_t	net_mask;
        uip_ip4addr_t	gateway_ip_addr;
        ip_addr_setting(&local_ip_addr, param->argv[1]);
        ip_addr_setting(&net_mask, param->argv[2]);
        if(param->argc == 4) {
            ip_addr_setting(&gateway_ip_addr, param->argv[3]);
        } else {
            //memset(gateway_ip_addr, 0, sizeof(uip_ip4addr_t));
        }
        netdev_setipv4info(NULL, local_ip_addr, net_mask, gateway_ip_addr);
        interface_get();
    } else {
        printf("Usage: interface set IP NETMASK [GATEWAY]\n");
    }
}

void static_ip_setting()
{
    u32 ip, gw, mask;
    uip_ip4addr_t	local_ip_addr;
    uip_ip4addr_t	net_mask;
    uip_ip4addr_t	gateway_ip_addr;
    ip_addr_setting(&local_ip_addr, STATIC_IP);
    ip_addr_setting(&net_mask, STATIC_MASK);
    ip_addr_setting(&gateway_ip_addr, STATIC_GW);
    netdev_setipv4info(NULL, local_ip_addr, net_mask, gateway_ip_addr);
    interface_get();
}
#if LWIP_AUTOIP
void lwip_autoip_set()
{
    struct netif * pnetif = NULL;
    pnetif = netif_find("et0");
    if (!pnetif) {
        printf("netif find err\n");
    }
    autoip_start(pnetif);
    while((pnetif->autoip->state == AUTOIP_STATE_PROBING) || 
               (pnetif->autoip->state == AUTOIP_STATE_ANNOUNCING)) {
        vTaskDelay(1000);
    }
    uint32_t ip =  pnetif->ip_addr.addr;
    if(ip == 0) {
        printf("AUTOIP timeout\n");
        static_ip_setting();
    } else {
    printf("\nLink-local address: %d.%d.%d.%d\n", (ip >> 0) & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff);
    }
}
#endif
void HomeKitNetworkConnection(void)
{
    static_ip_setting();
#if LWIP_AUTOIP
    lwip_autoip_set();
#endif
}

#define APSSID "SSV_AP8"
#define APPASSPHRASE "nononono"

void wifirspcbfuncTest(WIFI_RSP *msg)
{
    u8 dhcpen;
    u8 mac[6];
    u8 ipv4[4];

    uip_ipaddr_t ipaddr, submask, gateway, dnsserver;
    s8 ret;

    if(msg->wifistatus == 1)
    {
        printf("wifi connected\n");
        get_if_config(&dhcpen, &ipaddr, &submask, &gateway, &dnsserver);
//        get_local_mac(mac, 6);
//        printf("mac             - %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("ip addr         - %d.%d.%d.%d\n", ipaddr.u8[0], ipaddr.u8[1], ipaddr.u8[2], ipaddr.u8[3]);
        printf("netmask         - %d.%d.%d.%d\n", submask.u8[0], submask.u8[1], submask.u8[2], submask.u8[3]);
        printf("default gateway - %d.%d.%d.%d\n", gateway.u8[0], gateway.u8[1], gateway.u8[2], gateway.u8[3]);
        printf("DNS server      - %d.%d.%d.%d\n", dnsserver.u8[0], dnsserver.u8[1], dnsserver.u8[2], dnsserver.u8[3]);
    }
}

void scanAPcbfuncTest()
{
    u8 i;
    
    printf("\nCount:%d\n", getAvailableIndex());
    for(i = 0; i < getAvailableIndex(); i++)
    {
        printf("%2d - name:%32s, rssi:%2d CH:%2d mac:%02x-%02x-%02x-%02x-%02x-%02x\n", i, ap_list[i].name, ap_list[i].rssi, ap_list[i].channel
            , ap_list[i].mac[0], ap_list[i].mac[1], ap_list[i].mac[2], ap_list[i].mac[3], ap_list[i].mac[4], ap_list[i].mac[5]);
    }
    printf("end\n");
    
    if(set_wifi_config((u8 *)APSSID, (u8)strlen(APSSID), (u8 *)APPASSPHRASE, (u8)strlen(APPASSPHRASE), NULL, 0) == 0)
        wifi_connect(wifirspcbfuncTest);
    else
    {
        printf("%s\n",APSSID);
    }
}

int At_Init (stParam *param)
{
#if 0
    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
    drv_phy_channel_change(2, TRUE);
    netstack_init(NULL);
    printf("init end 2\n");
#else
    PBUF_Init();
    NETSTACK_RADIO.init();    
    drv_sec_init();
    netstack_init(NULL);
    printf("init end 2\n");
    DUT_wifi_start(DUT_NONE, NULL, NULL);
    drv_spi_mst_init(2, 0, 0);
#endif
}

int At_Wifi (stParam *param)
{

    DUT_wifi_start(DUT_STA, NULL, NULL);
    //OS_MsDelay(500);
    if(get_DUT_wifi_mode() == DUT_STA || get_DUT_wifi_mode() == DUT_CONCURRENT)
         scan_AP(scanAPcbfuncTest);
    return ERROR_SUCCESS;
}

void httpdrCallback( HTTPHeader_t *inHeader, bool stopped, void *callbackContext );

void httpdrCallback( HTTPHeader_t *inHeader, bool stopped, void *callbackContext )
{
    printf("httpdrCallback\n");
}

int At_HTTPD (stParam *param)
{
    HTTPServer_t    *httpServer;
    httpServer = OS_MemAlloc( sizeof( HTTPServer_t ) );
    HTTPServerCreate( "WACPreConfigListener",     // Friendly name of server
                            httpdrCallback,  // Callback function
                            NULL,           // Reference to the WACContext_t
                            httpServer );    // Reference to alloc'd HTTPServer_t

    HTTPServerStart( httpServer );
    return ERROR_SUCCESS;
}

int At_MFi (stParam *param)
{
    uint8_t             i;
    uint8_t             digest[ 20 ];
    uint8_t *           signaturePtr = NULL;
    size_t              signatureLen;
    uint8_t *           certificatePtr = NULL;
    size_t              certificateLen;
    PlatformMFiAuthInitialize();
    printf("OS_MsDelay before\n");
    OS_MsDelay(1);
    printf("OS_MsDelay after\n");
    for(i = 0; i < 20; i++)
        digest[i] = i;
    printf("PlatformMFiAuthCreateSignature\n");
    PlatformMFiAuthCreateSignature( digest, sizeof( digest ), &signaturePtr, &signatureLen );
    PlatformMFiAuthCopyCertificate( &certificatePtr, &certificateLen );
    return ERROR_SUCCESS;
}

int At_Soft (stParam *param)
{	
    DUT_wifi_start(DUT_AP, NULL, NULL);
    //softap_start();
    return ERROR_SUCCESS;
}


int At_Wac (stParam *param)
{	
#if 1
    if (OS_TaskCreate(wac_start, "wac_start", 256, NULL, OS_TASK_PRIO3, NULL ) != true)
    {
        //atcmdprintf("mdns start create fail\n");
        printf("[%d] %s task create fail\n", __LINE__, __func__);
        return -1;
    }
#endif
    return ERROR_SUCCESS;
}

int At_Mdns (stParam *param)
{	
    //printf("At_Mdns\n");
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }
    
    if(strcmp(param->argv[0], "init") == 0) {
        cli_mdns_start();
    } else if(strcmp(param->argv[0], "deinit") == 0) {
        mdnsd_stop();
    } else if(strcmp(param->argv[0], "reg") == 0) {
        if (param->argc != 1 && param->argc != 4) {
            return ERROR_INVALID_PARAMETER;
        }
        if(param->argc == 1)
            cli_mdns_reg(NewServiceName, ServiceType, ServicePort);
        else
            cli_mdns_reg(param->argv[1], param->argv[2], atoi(param->argv[3]));
    } else if(strcmp(param->argv[0], "dereg") == 0) {
        RemoveBonjourService();
    } else if(strcmp(param->argv[0], "reflash") == 0) {
        mdns_update_interface();
    } else if(strcmp(param->argv[0], "rename") == 0) {
        if (param->argc != 1 &&  param->argc != 4) {
            return ERROR_INVALID_PARAMETER;
        }
        if(param->argc == 1)
            ReNameBonjourService(ReServiceName, ServiceType, ServicePort);
        else
            ReNameBonjourService(param->argv[1], param->argv[2], atoi(param->argv[3]));
    } 
    
    return ERROR_SUCCESS;
}

int At_Interface(stParam *param)
{	
    //printf("At_Interface\n");
    if (param->argc < 1) {
        return ERROR_INVALID_PARAMETER;
    }
    
    dhcpc_wrapper_set(NULL, false);
    
    if(strcmp(param->argv[0], "set") == 0) {
        interface_setting(param);
    } else if(strcmp(param->argv[0], "info") == 0) {
        interface_get();
    } else if(strcmp(param->argv[0], "disconnect") == 0) {
        wifi_disconnect(NULL);
    } else if(strcmp(param->argv[0], "reconnect") == 0) {
        HomeKitNetworkConnection();
    } else {
        printf("Usage: interface set|info|disconnect|reconnect\n");
    }
    
    return ERROR_SUCCESS;
}

const at_cmd_info atcmd_homekit_tbl[] =
{
/*-------------------
    ------for debug------ */
    {"init" , At_Init, 0},
    {"wifi" , At_Wifi, 0},
    {"httpd" , At_HTTPD, 0},
    {"mfi" , At_MFi, 0},
    {"softap" , At_Soft, 0},
/*-------------------*/
    {"wac" , At_Wac, 0},
    {"mdns" , At_Mdns, 0},
    {"interface" , At_Interface, 0},
};

u32 get_atcmd_homekit_size()
{
    return sizeof(atcmd_homekit_tbl);
}
