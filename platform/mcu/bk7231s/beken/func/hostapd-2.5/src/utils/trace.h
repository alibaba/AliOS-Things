/*
 * Backtrace debugging
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef TRACE_H
#define TRACE_H

#define WPA_TRACE_LEN             16

#ifndef WPA_TRACE
#define WPA_TRACE_INFO
#define WPA_TRACE_REF(n)          
#define wpa_trace_dump(title, ptr) do { } while (0)
#define wpa_trace_record(ptr)      do { } while (0)
#define wpa_trace_show(title)      do { } while (0)
#define wpa_trace_add_ref(ptr, name, addr)    do { } while (0)
#define wpa_trace_remove_ref(ptr, name, addr) do { } while (0)
#define wpa_trace_check_ref(addr)             do { } while (0)
#endif /* WPA_TRACE */

#ifndef WPA_TRACE_BFD
#define wpa_trace_dump_funcname(title, pc) do { } while (0)
#endif /* WPA_TRACE_BFD */

#endif /* TRACE_H */
