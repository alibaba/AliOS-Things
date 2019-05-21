/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */
#ifndef __DNS_H__
#define __DNS_H__

struct dns_header {
	uint16_t id;
	union {
		struct {
			uint16_t rcode:4,	/* response code */
			 cd:1,	/* checking disabled RFC-2535 */
			 ad:1,	/* authentic data RFC-2535 */
			 z:1,	/* zero */
			 ra:1,	/* recursion available */
			 rd:1,	/* recursion desired */
			 tc:1,	/* truncated */
			 aa:1,	/* authoritative answer */
			 opcode:4, /* (should be 0 for normal DNS messages) */
			 qr:1;	/* query/response */
		} fields ;
		uint16_t num;
	} flags ;
	uint16_t num_questions;
	uint16_t answer_rrs;
	uint16_t authority_rrs;
	uint16_t additional_rrs;
} __attribute__((packed));

struct dns_question {
	/* query name (label) field */
	uint16_t type;
	uint16_t class;
} __attribute__((packed));

struct dns_rr {
	uint16_t name_ptr;	/* pointer to name */
	uint16_t type;		/* resource type */
	uint16_t class;		/* resource class */
	uint32_t ttl;		/* time to live */
	uint16_t rdlength;	/* resource data length */
	uint32_t rd;		/* resource data: we only provide a
				   4-byte data response (an IP
				   address) but this is actually a
				   field of length rdlength */
} __attribute__((packed));

#endif				/* __DNS_H__ */
