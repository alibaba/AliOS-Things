
#include <aos/cli.h>
#include "driver/chip/hal_chip.h"
#include "common/cmd/cmd_mem.h"
#include "common/cmd/cmd_defs.h"

#if 0
#define readl(addr)		(*((volatile unsigned long  *)(addr)))
#define writel(v, addr)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

static void hexdump(char* msg, uint32_t addr, uint32_t len)
{
	int i;
	printf("%s:", msg);
	for (i = 0; i < len; i += 4) {
		if (i % 16 == 0) {
			printf("\n");
			printf("%08x: ", addr + i);
		}
		printf("%08x ", readl(addr + i));
	}
	printf("\n");
}
#endif
static void handle_mem_cmd(char *pwbuf, int blen, int argc, char **argv)
{
	char cmd_buf[255];
	int i;
	enum cmd_status status;
	memset(cmd_buf, 0, sizeof(cmd_buf));
	if(argc > 1) {
		for (i=1; i<argc; i++) {
			if (i == 1) {
				snprintf(cmd_buf, strlen(argv[1]) + 1, "%s", argv[i]);
			} else {
			     snprintf(cmd_buf + strlen(cmd_buf),
                     strlen(cmd_buf) + strlen(argv[i]) + 1, " %s", argv[i]);
			}
			//printf("%s,%s, i %d\n", __func__, cmd_buf, i);
		}
		//printf("%s,%s\n", __func__, cmd_buf);
		status = cmd_mem_exec(&cmd_buf);
		if(status != CMD_STATUS_ACKED) {
			printf("%s,err status %d\n", __func__, status);
		}
	} else {
		printf("%s, param is not correct\n", __func__);
	}
#if 0
	uint32_t addr;
	uint32_t len;
	uint32_t val;
	uint32_t op = 0;

	if (argc < 3) {
		printf("cmd_mem: Wrong Arguments\n");
		return;
	}
	if (strcmp(argv[1], "read") == 0) {
		op = 0;
	} else if (strcmp(argv[1], "write") == 0) {
		op = 1;
	} else if (strcmp(argv[1], "dump") == 0) {
		op = 2;
	}

	addr = strtoul(argv[2], NULL, 16);
	/* check alignment */
	if (addr & 0x1) {
		printf("** addr is not aligned !!\n");
		return;
	}

	if (op == 0) {
		val = readl(addr);
		printf("read: [0x%x]:: 0x%08x", addr, val);
	} else if (op == 1) {
		if (argc < 4) {
			printf("** Wrong Arguments !!\n");
			return;
		}
		val = strtoul(argv[3], NULL, 16);
		writel(val, addr);
		printf("write: [0x%x]: 0x%08x", addr, val);
	} else {
		if (argc < 4) {
			len = 256;
		} else {
			len = strtoul(argv[3], NULL, 10);
		}
		hexdump("dump", addr, len);
	}
	printf("\n");
	return ;
#endif
}

static struct cli_command ncmd = {
    .name = "mem",
    .help = "mem [read addr | write addr val | dump addr len]",
    .function = handle_mem_cmd,
};

void cli_cmd_add_mem(void)
{
	aos_cli_register_command(&ncmd);
}
