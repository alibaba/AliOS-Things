#ifndef _MB_PROTO_H
#define _MB_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

#define SLAVE_ADDR_BROADCAST    0
#define SLAVE_ADDR_MIN          1
#define SLAVE_ADDR_MAX          247

#define FUNC_CODE_READ_COILS               0x01
#define FUNC_CODE_READ_HOLDING_REGISTERS   0x03

#ifdef __cplusplus
}
#endif
#endif
