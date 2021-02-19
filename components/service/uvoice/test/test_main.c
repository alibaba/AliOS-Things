#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_init.h"
#include "uvoice_event.h"


extern void uvoice_play_test(int argc, char **argv);
extern void uvoice_record_test(int argc, char **argv);
extern void test_tts_handle(int argc, char **argv);

static void handle_input(char *buffer)
{
	char *ptr = buffer;
	char *begin;
	char *end;
	char *temp;
	int len;
	int argc = 0;
	int i;
	char *argv[10];

	if (!ptr)
		return -1;

	memset(argv, 0, sizeof(argv));
	end = ptr + strlen(ptr) - 1;

	while (*end == ' ' || *end == '\n') {
		*end = '\0';
		end--;
	}

	while (*ptr == ' ')
		ptr++;

	while (1) {
		begin = ptr;
		ptr = strchr(ptr, ' ');
		if (!ptr) {
			ptr = begin;
			if (strlen(ptr) <= 0)
				break;
			if (strlen(ptr) > 0)
				ptr += strlen(ptr);
		}
		end = ptr;
		len = end - begin;
		if (len <= 0)
			break;
		temp = malloc(len + 1);
		if (!temp) {
			printf("%s: alloc argv buffer failed !\n", __func__);
			break;
		}
		memcpy(temp, begin, len);
		temp[len] = '\0';
		//printf("%s: argv[%d] %s\n", __func__, argc, temp);
		argv[argc++] = temp;

		while (*ptr == ' ')
			ptr++;
	}

	if (argc <= 0)
		return;

	if (!strcmp(argv[0], "play"))
		uvoice_play_test(argc, argv);
	else if (!strcmp(argv[0], "record"))
		uvoice_record_test(argc, argv);
	else if (!strcmp(argv[0], "tts"))
		test_tts_handle(argc, argv);

	for (i = 0; i < argc; i++) {
		if (argv[i]) {
			//printf("%s: free argv[%d]\n", __func__, i);
			free(argv[i]);
		}
	}
}


static void player_state_cb(uvoice_event_t *event, void *data)
{
	//printf("%s: type %u code %u value %d\n", __func__, event->type, event->code, event->value);
}

int main(int argc, char **argv)
{
	char buffer[2048];

	uvoice_init();

	uvoice_event_register(UVOICE_EV_PLAYER,
		player_state_cb, NULL);

	printf("\r>>> ");
	memset(buffer, 0, sizeof(buffer));
	while (fgets(buffer, sizeof(buffer), stdin)) {
		if (!strncmp(buffer, "exit", strlen("exit")))
			break;
		handle_input(buffer);
		printf("\r>>> ");
		memset(buffer, 0, sizeof(buffer));
	}
	return 0;
}

