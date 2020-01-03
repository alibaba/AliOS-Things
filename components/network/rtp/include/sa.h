/**
 * @file sa.h  Internal interface to Socket Address
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef SA_H
#define SA_H

/* Net ntop/pton */
int net_inet_ntop(const struct sa *sa, char *buf, int size);
int net_inet_pton(const char *addr, struct sa *sa);
#endif /* SA_H */
