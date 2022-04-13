#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aos/cli.h>

#define MAX_BUFFER 200

static void show_help(void)
{
    aos_cli_printf("Usage: grep <key_string> <file>\n");
}

int grep_main(int argc,char **argv)
{
    char buffer[MAX_BUFFER];
    FILE *fp;
    int key_sum = 0, res_sum = 0;
    char *buf_ptr = NULL;
    char const *key_ptr = NULL;

    if (argc < 3) {
        show_help();
        return -1;
    }

    if (strlen(argv[2]) == 0) {
        show_help();
        return -1;
    }

    fp = fopen(argv[2],"r");
    if (!fp) {
        aos_cli_printf("open file %s failed\n", argv[2]);
        return -1;
    }
    key_sum= strlen(argv[1]);
    while (fgets(buffer, sizeof(buffer), fp) != 0) {
        buf_ptr= buffer;
        key_ptr= argv[1];
        while (*buf_ptr!= '\0') {
            if ((*buf_ptr == '\0') | (*key_ptr == '\0'))
                break;
            if (*buf_ptr == *key_ptr) {
                key_ptr++;
                if (*key_ptr == '\0') {
                    aos_cli_printf("%s",buffer);
                    res_sum++;
                }
            } else {
                key_ptr = argv[1];
            }
            buf_ptr++;
        }
    }
    fclose(fp);
    return 0;
}
ALIOS_CLI_CMD_REGISTER(grep_main, grep, search string from file);
