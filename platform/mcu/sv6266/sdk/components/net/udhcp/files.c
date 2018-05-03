/* vi: set sw=4 ts=4: */
/*
 * DHCP server config and lease file manipulation
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
//#include <netinet/ether.h>

#include "udhcp_common.h"
#include "udhcpd.h"
#include <netstack.h>
#include <string.h>
#if DHCPD_SUPPORT

static struct server_config_t *please_config = NULL;

#if 0
/* on these functions, make sure your datatype matches */
static int FAST_FUNC read_str(const char *line, void *arg)
{
	char **dest = arg;

	free(*dest);
	*dest = xstrdup(line);
	return 1;
}

static int FAST_FUNC read_u32(const char *line, void *arg)
{
	*(uint32_t*)arg = bb_strtou32(line, NULL, 10);
	return errno == 0;
}

static int FAST_FUNC read_staticlease(const char *const_line, void *arg)
{
	char *line;
	char *mac_string;
	char *ip_string;
	struct ether_addr mac_bytes; /* it's "struct { uint8_t mac[6]; }" */
	uint32_t nip;

	/* Read mac */
	line = (char *) const_line;
	mac_string = strtok_r(line, " \t", &line);
	if (!mac_string || !ether_aton_r(mac_string, &mac_bytes))
		return 0;

	/* Read ip */
	ip_string = strtok_r(NULL, " \t", &line);
	if (!ip_string || !udhcp_str2nip(ip_string, &nip))
		return 0;

	add_static_lease(arg, (uint8_t*) &mac_bytes, nip);

	log_static_leases(arg);

	return 1;
}

struct config_keyword {
	const char *keyword;
	int (*handler)(const char *line, void *var) FAST_FUNC;
	void *var;
	const char *def;
};

static const struct config_keyword keywords[] = {
	/* keyword        handler           variable address               default */
	{"start"        , udhcp_str2nip   , &server_config.start_ip     , "192.168.0.20"},
	{"end"          , udhcp_str2nip   , &server_config.end_ip       , "192.168.0.254"},
	{"interface"    , read_str        , &server_config.interface    , "eth0"},
	/* Avoid "max_leases value not sane" warning by setting default
	 * to default_end_ip - default_start_ip + 1: */
	{"max_leases"   , read_u32        , &server_config.max_leases   , "235"},
	{"auto_time"    , read_u32        , &server_config.auto_time    , "7200"},
	{"decline_time" , read_u32        , &server_config.decline_time , "3600"},
	{"conflict_time", read_u32        , &server_config.conflict_time, "3600"},
	{"offer_time"   , read_u32        , &server_config.offer_time   , "60"},
	{"min_lease"    , read_u32        , &server_config.min_lease_sec, "60"},
	{"lease_file"   , read_str        , &server_config.lease_file   , LEASES_FILE},
	{"pidfile"      , read_str        , &server_config.pidfile      , "/var/run/udhcpd.pid"},
	{"siaddr"       , udhcp_str2nip   , &server_config.siaddr_nip   , "0.0.0.0"},
	/* keywords with no defaults must be last! */
	{"option"       , udhcp_str2optset, &server_config.options      , ""},
	{"opt"          , udhcp_str2optset, &server_config.options      , ""},
	{"notify_file"  , read_str        , &server_config.notify_file  , NULL},
	{"sname"        , read_str        , &server_config.sname        , NULL},
	{"boot_file"    , read_str        , &server_config.boot_file    , NULL},
	{"static_lease" , read_staticlease, &server_config.static_leases, ""},
};
enum { KWS_WITH_DEFAULTS = ARRAY_SIZE(keywords) - 6 };

void FAST_FUNC read_config(const char *file)
{
	parser_t *parser;
	const struct config_keyword *k;
	unsigned i;
	char *token[2];

	for (i = 0; i < KWS_WITH_DEFAULTS; i++)
		keywords[i].handler(keywords[i].def, keywords[i].var);

	parser = config_open(file);
	while (config_read(parser, token, 2, 2, "# \t", PARSE_NORMAL)) {
		for (k = keywords, i = 0; i < ARRAY_SIZE(keywords); k++, i++) {
			if (strcasecmp(token[0], k->keyword) == 0) {
				if (!k->handler(token[1], k->var)) {
					bb_error_msg("can't parse line %u in %s",
							parser->lineno, file);
					/* reset back to the default value */
					k->handler(k->def, k->var);
				}
				break;
			}
		}
	}
	config_close(parser);

	server_config.start_ip = ntohl(server_config.start_ip);
	server_config.end_ip = ntohl(server_config.end_ip);
}
#endif

void FAST_FUNC load_config(struct server_config_t* psconfig)
{
	u32 softap_ip = 0,local_ip = 0;
	
    struct ip4_addr ipaddr, subnet, gw;//, dns;
	if (psconfig == NULL)
		return;

	memset(psconfig, 0, sizeof(struct server_config_t));
	if (please_config == NULL){
        netdev_getipv4info(IF1_NAME, &ipaddr.addr, &gw.addr, &subnet.addr);
        
		local_ip = softap_ip = htonl(ipaddr.addr);
		softap_ip &= 0xFFFFFF00;
		local_ip &= 0xFF;
		if (local_ip >= 0x80)
			local_ip -= 0x64;
		else
			local_ip += 0x64;

		psconfig->max_leases = 10;
		psconfig->start_ip = softap_ip | local_ip;
		psconfig->end_ip = softap_ip | (local_ip + psconfig->max_leases - 1);
        //dns.addr = ipaddr.addr;

        psconfig->subnet = htonl(subnet.addr);
        psconfig->gw = htonl(gw.addr);
        psconfig->dns = htonl(ipaddr.addr);//(dns.addr);

		psconfig->interface = NULL;
		psconfig->auto_time = 7200;
		psconfig->decline_time = 3600;
		psconfig->conflict_time = 3600;
		psconfig->offer_time = 60;
		psconfig->min_lease_sec = 60;

		psconfig->pidfile = NULL;
		psconfig->siaddr_nip = 0;
		psconfig->static_leases = NULL;
	} else {
		psconfig->start_ip = please_config->start_ip;
		psconfig->end_ip = please_config->end_ip;
        
		psconfig->subnet = please_config->subnet;
		psconfig->gw = please_config->gw;
		psconfig->dns = please_config->dns;
        
		psconfig->max_leases = please_config->max_leases;
		psconfig->auto_time = please_config->auto_time;
		psconfig->decline_time = please_config->decline_time;
		psconfig->conflict_time = please_config->conflict_time;
		psconfig->offer_time = please_config->offer_time;
		psconfig->min_lease_sec = please_config->min_lease_sec;
		psconfig->max_lease_sec = please_config->max_lease_sec;
		OS_MemFree((void*)please_config);
		please_config = NULL;
	}
    
#ifndef CONFIG_OS_RHINO
    UDHCP_DEBUG("start_ip =|  %3d  |  %3d  |  %3d  |  %3d \r\n",
                      ip4_addr4_16(&psconfig->start_ip),
                      ip4_addr3_16(&psconfig->start_ip),
                      ip4_addr2_16(&psconfig->start_ip),
                      ip4_addr1_16(&psconfig->start_ip));
    UDHCP_DEBUG("end_ip   =|  %3d  |  %3d  |  %3d  |  %3d \r\n",
                      ip4_addr4_16(&psconfig->end_ip),
                      ip4_addr3_16(&psconfig->end_ip),
                      ip4_addr2_16(&psconfig->end_ip),
                      ip4_addr1_16(&psconfig->end_ip));
#endif
}

void FAST_FUNC write_leases(void)
{
	return;
#if 0
	int fd;
	unsigned i;
	leasetime_t curr;
	int64_t written_at;

	fd = open_or_warn(server_config.lease_file, O_WRONLY|O_CREAT|O_TRUNC);
	if (fd < 0)
		return;

	curr = written_at = time(NULL);

	written_at = SWAP_BE64(written_at);
	full_write(fd, &written_at, sizeof(written_at));

	for (i = 0; i < server_config.max_leases; i++) {
		leasetime_t tmp_time;

		if (g_leases[i].lease_nip == 0)
			continue;

		/* Screw with the time in the struct, for easier writing */
		tmp_time = g_leases[i].expires;

		g_leases[i].expires -= curr;
		if ((signed_leasetime_t) g_leases[i].expires < 0)
			g_leases[i].expires = 0;
		g_leases[i].expires = htonl(g_leases[i].expires);

		/* No error check. If the file gets truncated,
		 * we lose some leases on restart. Oh well. */
		full_write(fd, &g_leases[i], sizeof(g_leases[i]));

		/* Then restore it when done */
		g_leases[i].expires = tmp_time;
	}
	close(fd);

	if (server_config.notify_file) {
		char *argv[3];
		argv[0] = server_config.notify_file;
		argv[1] = server_config.lease_file;
		argv[2] = NULL;
		spawn_and_wait(argv);
	}
    #endif
}

void FAST_FUNC read_leases(const char *file)
{
	return;
    #if 0
	struct dyn_lease lease;
	int64_t written_at, time_passed;
	int fd;
#if defined CONFIG_UDHCP_DEBUG && CONFIG_UDHCP_DEBUG >= 1
	unsigned i = 0;
#endif

	fd = open_or_warn(file, O_RDONLY);
	if (fd < 0)
		return;

	if (full_read(fd, &written_at, sizeof(written_at)) != sizeof(written_at))
		goto ret;
	written_at = SWAP_BE64(written_at);

	time_passed = time(NULL) - written_at;
	/* Strange written_at, or lease file from old version of udhcpd
	 * which had no "written_at" field? */
	if ((uint64_t)time_passed > 12 * 60 * 60)
		goto ret;

	while (full_read(fd, &lease, sizeof(lease)) == sizeof(lease)) {
//FIXME: what if it matches some static lease?
		uint32_t y = ntohl(lease.lease_nip);
		if (y >= server_config.start_ip && y <= server_config.end_ip) {
			signed_leasetime_t expires = ntohl(lease.expires) - (signed_leasetime_t)time_passed;
			if (expires <= 0)
				continue;
			/* NB: add_lease takes "relative time", IOW,
			 * lease duration, not lease deadline. */
			if (add_lease(lease.lease_mac, lease.lease_nip,
					expires,
					lease.hostname, sizeof(lease.hostname)
				) == 0
			) {
				bb_error_msg("too many leases while loading %s", file);
				break;
			}
#if defined CONFIG_UDHCP_DEBUG && CONFIG_UDHCP_DEBUG >= 1
			i++;
#endif
		}
	}
	log1("Read %d leases", i);
 ret:
	close(fd);
    #endif
}

int FAST_FUNC dhcps_set_info_api(dhcps_info *if_dhcps)
{
    u32 softap_ip = 0;
    struct ip4_addr ipaddr;

    if (if_dhcps == NULL)
        return false;

    netdev_getipv4info(IF1_NAME, &ipaddr.addr, NULL, NULL);
    softap_ip = htonl(ipaddr.addr);

#if 0	
    if_dhcps->start_ip = htonl(if_dhcps->start_ip);
    if_dhcps->end_ip = htonl(if_dhcps->end_ip);
    
    if_dhcps->subnet = htonl(if_dhcps->subnet);
    if_dhcps->gw = htonl(if_dhcps->gw);
    if_dhcps->dns = htonl(if_dhcps->dns);
#endif	

#if 0
	printf("start_ip=0x%x\n",if_dhcps->start_ip);
	printf("end_ip=0x%x\n",if_dhcps->end_ip);
	printf("subnet=0x%x\n",if_dhcps->subnet);
	printf("gw=0x%x\n",if_dhcps->gw);
	printf("dns=0x%x\n",if_dhcps->dns);
	printf("softap_ip=0x%x\n",softap_ip);
#endif

    /*config ip information can't contain local ip*/
    if ((if_dhcps->start_ip <= softap_ip) && (softap_ip <= if_dhcps->end_ip))
        return -1;


    /*config ip information must be in the same segment as the local ip*/
    softap_ip >>= 8;
    if (((if_dhcps->start_ip >> 8) != softap_ip) ||
        ((if_dhcps->end_ip >> 8) != softap_ip)){
        return -1;
    }


    /*config ip information can't exceed the maximum number of leases*/
    if ((if_dhcps ->end_ip - if_dhcps->start_ip) > if_dhcps->max_leases){
		//printf("(start_ip,end_ip,max_leases)=(0x%x,0x%x,0x%x)\n",if_dhcps->start_ip,if_dhcps->end_ip,if_dhcps->max_leases);
        return -1;
    }


    if (please_config != NULL)
    {
        OS_MemFree(please_config);
    }
    
    please_config = (struct server_config_t*)OS_MemAlloc(sizeof(struct server_config_t));
    
    if (please_config == NULL)
        return -1;

    please_config->start_ip = if_dhcps->start_ip;
    please_config->end_ip = if_dhcps->end_ip;
    please_config->max_leases = if_dhcps->max_leases;
    
    please_config->subnet = if_dhcps->subnet;
    please_config->gw = if_dhcps->gw;
    please_config->dns = if_dhcps->dns;
    
    please_config->auto_time = if_dhcps->auto_time;
    please_config->decline_time = if_dhcps->decline_time;
    please_config->conflict_time = if_dhcps->conflict_time;
    please_config->offer_time = if_dhcps->offer_time;
    please_config->min_lease_sec = if_dhcps->min_lease_sec;
    please_config->max_lease_sec = if_dhcps->max_lease_sec;
#ifndef CONFIG_OS_RHINO
    UDHCP_DEBUG("start_ip =|  %3d  |  %3d  |  %3d  |  %3d \r\n",
                      ip4_addr4_16(&if_dhcps->start_ip),
                      ip4_addr3_16(&if_dhcps->start_ip),
                      ip4_addr2_16(&if_dhcps->start_ip),
                      ip4_addr1_16(&if_dhcps->start_ip));
    UDHCP_DEBUG("end_ip   =|  %3d  |  %3d  |  %3d  |  %3d \r\n",
                      ip4_addr4_16(&if_dhcps->end_ip),
                      ip4_addr3_16(&if_dhcps->end_ip),
                      ip4_addr2_16(&if_dhcps->end_ip),
                      ip4_addr1_16(&if_dhcps->end_ip));
#endif

    return 0;
}

#endif //#if DHCPD_SUPPORT
