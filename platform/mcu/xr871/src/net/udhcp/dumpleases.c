#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>

#include "libbb_udhcp.h"

#define REMAINING 0
#define ABSOLUTE 1

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
};

#ifdef BB_VER
int dumpleases_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	FILE *fp;
	int i, c, mode = REMAINING;
	long expires;
	char file[255] = "/var/lib/misc/udhcpd.leases";
	struct lease_t lease;
	struct in_addr addr;

	static struct option options[] = {
		{"absolute", 0, 0, 'a'},
		{"remaining", 0, 0, 'r'},
		{"file", 1, 0, 'f'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};

	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "arf:h", options, &option_index);
		if (c == -1) break;

		switch (c) {
		case 'a': mode = ABSOLUTE; break;
		case 'r': mode = REMAINING; break;
		case 'f':
			strncpy(file, optarg, 255);
			file[254] = '\0';
			break;
		case 'h':
			printf("Usage: dumpleases -f <file> -[r|a]\n\n");
			printf("  -f, --file=FILENAME             Leases file to load\n");
			printf("  -r, --remaining                 Interepret lease times as time remaing\n");
			printf("  -a, --absolute                  Interepret lease times as expire time\n");
			break;
		}
	}

	if (!(fp = fopen(file, "r"))) {
		perror("could not open input file");
		return 0;
	}

	printf("Mac Address       IP-Address      Expires %s\n", mode == REMAINING ? "in" : "at");
	/*     "00:00:00:00:00:00 255.255.255.255 Wed Jun 30 21:49:08 1993" */
	while (fread(&lease, sizeof(lease), 1, fp)) {

		for (i = 0; i < 6; i++) {
			printf("%02x", lease.chaddr[i]);
			if (i != 5) printf(":");
		}
		addr.s_addr = lease.yiaddr;
		printf(" %-15s", inet_ntoa(addr));
		expires = ntohl(lease.expires);
		printf(" ");
		if (mode == REMAINING) {
			if (!expires) printf("expired\n");
			else {
				if (expires > 60*60*24) {
					printf("%ld days, ", expires / (60*60*24));
					expires %= 60*60*24;
				}
				if (expires > 60*60) {
					printf("%ld hours, ", expires / (60*60));
					expires %= 60*60;
				}
				if (expires > 60) {
					printf("%ld minutes, ", expires / 60);
					expires %= 60;
				}
				printf("%ld seconds\n", expires);
			}
		} else printf("%s", ctime(&expires));
	}
	fclose(fp);

	return 0;
}
