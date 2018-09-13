#ifndef DHCPD_CFG_H_H
#define DHCPD_CFG_H_H

#define DHCPD_ADDR_START         "192.168.51.100"
#define DHCPD_ADDR_END           "192.168.51.104"
#define DHCPD_INTERFACE          "en1"
#define DHCPD_OPTION             ""
#define DHCPD_OPT                ""
#define DHCPD_REMAIN             "yes"
#define DHCPD_MAX_LEASES         "5"
#define DHCPD_AUTO_TIME          "7200"
#define DHCPD_DECLINE_TIME       "3600"
#define DHCPD_CONFLICT_TIME      "3600"
#define DHCPD_OFFER_TIME         "60"
#define DHCPD_MIN_LEASE          "60"
#define DHCPD_SIADDR             "0.0.0.0"
#define DHCPD_SNAME              "IOT"

#define u_int8_t                 uint8_t
#define u_int16_t                uint16_t
#define u_int32_t                uint32_t

#define u_int                    uint32_t
#define u_char                   uint8_t
#define u_short                  uint16_t

#endif /* DHCPD_CFG_H_H */
