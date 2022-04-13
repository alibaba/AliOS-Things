#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "aw_list.h"
#include "sunxi-input.h"
#include "cmsis_os.h"

#define input_err(fmt, args...)  printf("%s()%d - "fmt, __func__, __LINE__, ##args)

#define INPUT_SEM_MAX_COUNT 0xFFFFFFFFUL

osSemaphoreDef(dev_semaphore);
static LIST_HEAD(input_dev_list);
static LIST_HEAD(evdev_list);
static unsigned evdev_cnt = 0;

static inline int is_event_support(unsigned int code,
					unsigned long *bm, unsigned int max)
{
	return code <= max && input_test_bit(code, bm);
}

static struct sunxi_input_dev *find_input_dev_by_name(const char *name)
{
	struct sunxi_input_dev *dev;

	list_for_each_entry(dev, &input_dev_list, node) {
		if(!strcmp(dev->name, name))
			return dev;
	}

	return NULL;
}

#if 0
static struct sunxi_evdev *find_evdev_by_name(const char *name)
{
	struct sunxi_evdev *evdev = NULL;

	list_for_each_entry(evdev, &evdev_list, node) {
		if(!strcmp(evdev->name, name))
			return evdev;
	}

	return NULL;
}
#endif

static struct sunxi_evdev *find_evdev_by_fd(int fd)
{
	struct sunxi_evdev *evdev = NULL;

	list_for_each_entry(evdev, &evdev_list, node) {
		if(evdev->fd == fd)
			return evdev;
	}

	return NULL;
}

static void evdev_pass_event(struct sunxi_evdev *evdev, struct sunxi_input_event *event)
{
	evdev->buffer[evdev->head++] = *event;
	evdev->head &= EVENT_BUFFER_SIZE - 1;

	if(evdev->head == evdev->tail) {
		evdev->tail = (evdev->head - 1) & (EVENT_BUFFER_SIZE - 1);
		evdev->packet_head = evdev->tail;
	}

	if (event->type == EV_SYN && event->code == SYN_REPORT) {
		evdev->packet_head = evdev->head;
		if (osOK != osSemaphoreRelease(evdev->sem)) {
			input_err("dedev release osSemaphoreRelease failed!");
		}

	}
}

static void input_pass_event(struct sunxi_input_dev *dev, struct sunxi_input_event *event)
{
	struct sunxi_evdev *evdev = NULL;

	//report input event to all evdev (all task that read).
	list_for_each_entry(evdev, &evdev_list, node) {
		if(!strcmp(evdev->name, dev->name))
		{
			evdev_pass_event(evdev, event);
		}
	}

}

unsigned int valueSecondGlobal;

static void input_handle_event(struct sunxi_input_dev *dev,
				unsigned int type, unsigned int code, unsigned int value)
{
	bool report = false;
	struct sunxi_input_event event;

	switch (type) {
		case EV_SYN :
			report = true;
			break;
		case EV_KEY :
			if(is_event_support(code, dev->keybit, KEY_MAX))
				report = true;
			break;
		case EV_ABS :
			if(is_event_support(code, dev->absbit, ABS_MAX))
				report = true;
			break;
		case EV_REL :
			if(is_event_support(code, dev->relbit, REL_MAX))
				report = true;
			break;
		case EV_MSC :
			if(is_event_support(code, dev->mscbit, MSC_MAX))
				report = true;
			break;
		default :
			break;
	}

	if (report) {
		event.type = type;
		event.code = code;
		event.value = value;
        event.valueSecond = valueSecondGlobal;
		input_pass_event(dev, &event);
	}

}

void sunxi_input_event(struct sunxi_input_dev *dev,
			unsigned int type, unsigned int code, unsigned int value)
{
	if(is_event_support(type, dev->evbit, EV_MAX)) {
		//spin_lock();
		input_handle_event(dev, type, code, value);
		//spin_unlock();
	}

}

static int sunxi_fetch_next_event(struct sunxi_evdev *evdev, struct sunxi_input_event *event)
{
	int have_event;
	uint32_t level;

	//spin_lock_irq(level);

	have_event = evdev->packet_head != evdev->tail;
	if (have_event) {
		*event = evdev->buffer[evdev->tail++];
		evdev->tail &= EVENT_BUFFER_SIZE - 1;
	}

	//spin_unlock_irq(level);

	return have_event;
}


/*------------------------------DRIVER API-------------------------*/
void input_set_capability(struct sunxi_input_dev *dev, unsigned int type, unsigned int code)
{
	switch (type) {
		case EV_KEY:
			input_set_bit(code, dev->keybit);
			break;

		case EV_REL:
			input_set_bit(code, dev->relbit);
			break;

		case EV_ABS:
			input_set_bit(code, dev->absbit);
			break;

		case EV_MSC:
			input_set_bit(code, dev->mscbit);
			break;

		default:
			input_err("input_set_capability: unknown type %u (code %u)\n",
					type, code);
			return;
	}

	input_set_bit(type, dev->evbit);
}

struct sunxi_input_dev *sunxi_input_allocate_device()
{
	struct sunxi_input_dev *dev;

	dev = aos_malloc(sizeof(struct sunxi_input_dev));
	if (dev)
		memset(dev, 0 , sizeof(struct sunxi_input_dev));

	return dev;

}

int sunxi_input_register_device(struct sunxi_input_dev *dev)
{
	if (NULL == dev->name) {
		input_err("err : input device must have a name\n");
		return -1;
	}

	/* Every input device generates EV_SYN/SYN_REPORT events. */
	input_set_bit(EV_SYN, dev->evbit);

	list_add_tail(&dev->node, &input_dev_list);

	return 0;
}

/*---------------------------USER API-------------------------*/
int sunxi_input_open(const char *name)
{
	struct sunxi_input_dev *dev = NULL;
	struct sunxi_evdev *evdev;

	dev = find_input_dev_by_name(name);
	if (NULL == dev) {
		input_err("input dev %s is not exist\n", name);
		return -1;
	}

	evdev = aos_malloc(sizeof(struct sunxi_evdev));
	if (evdev) {
		memset(evdev, 0 ,sizeof(struct sunxi_evdev));
		evdev->name = dev->name;
		evdev->fd = evdev_cnt++;
		evdev->sem = osSemaphoreCreate(osSemaphore(dev_semaphore), 0);
		if(NULL == evdev->sem)
			input_err("init Semaphore fail\n");
		list_add_tail(&evdev->node, &evdev_list);

		return evdev->fd;
	}

	return -1;
}

int sunxi_input_read(int fd, void *buffer, unsigned int size)
{
	unsigned int count = 0;
	struct sunxi_evdev *evdev = NULL;
	struct sunxi_input_event event;

	evdev = find_evdev_by_fd(fd);
	if(NULL == evdev) {
		input_err("input read fd err\n");
		return -1;
	}

	if (evdev->packet_head == evdev->tail) {
		if (osSemaphoreWait(evdev->sem, osWaitForever) <= 0 ) {
			input_err("input take semaphore err\n");
			return -1;
		}
	}

	while(count + sizeof(struct sunxi_input_event) <= size
			&& sunxi_fetch_next_event(evdev, &event)) {
		memcpy(buffer + count, &event, sizeof(struct sunxi_input_event));
		count += sizeof(struct sunxi_input_event);
	}

	return count;
}
