/* files.h */
#ifndef _FILES_H
#define _FILES_H

struct config_keyword {
	char keyword[14];
	int (*handler)(char *line, void *var);
	void *var;
	char def[30];
};

#ifdef DHCPD_FS
int read_config(char *file);
void write_leases(void);
void read_leases(char *file);
#else
int init_config();
#endif
#endif
