#include "os_common.h"
#include "os_queue.h"
#include "typedef.h"
os_queue_handle_t os_queue_create(u32_t queue_len,u32_t item_size)
{
	int ret = 0;
	os_queue__t *queue = NULL;
	char *buf = NULL;
	buf = nos_malloc(queue_len*item_size);
	queue = nos_malloc(sizeof(os_queue__t));
	NULL_PARA_CHECK(queue,NULL);
	NULL_PARA_CHECK(buf,NULL);
	ret = krhino_fix_buf_queue_create(queue, "default_queue", buf, item_size, queue_len);
	if(0 == ret)
	{
		queue_debug_info("os queue create sucess,handle:%x,len:%u,size:%u\r\n",queue,queue_len,item_size);
		return queue;
	}
	else
	{
		nos_free(buf);
		nos_free(queue);
		queue_debug_error("os queue create fail,reason:%d\r\n",ret);
		return NULL;
	}
	
}

int os_queue_send(os_queue_handle_t queue_handle,void *send_buf,os_tick_type_t wait_time)
{
	NULL_PARA_CHECK(queue_handle,os_false);
	int ret = 0;
	int count = 0;
	unsigned int item_size = ((kbuf_queue_t*)queue_handle)->max_msg_size;
	ret = krhino_buf_queue_send(queue_handle,send_buf,item_size);

	while(RHINO_BUF_QUEUE_FULL == ret)//if queue full wait 
	{
			if(0 == wait_time)break;
			os_delay(20);
			count+=20;
			ret = krhino_buf_queue_send((kbuf_queue_t*)queue_handle,send_buf,item_size);
			if(0 == ret || count>=wait_time)
			{
				break;
			}
			if(count == 5000)
			{
				queue_debug_info("queue send fail after 5 seconds because of queue full,block time:%u\r\n",wait_time);
			}
	}
	if(0 != ret)
	{
		queue_debug_error("os queue send fail,reason:%d\r\n",ret);
		return os_false;
	}
	else 
	{
		queue_debug_info("os queue send sucess,handle:%x\r\n",queue_handle);
		return os_true;
	}

}
int os_queue_recive(os_queue_handle_t queue_handle,void *recv_buf,os_tick_type_t wait_time)
{
	NULL_PARA_CHECK(queue_handle,os_false);
	NULL_PARA_CHECK(recv_buf,os_false);

	int ret = 0;
	int recv_size=0;
	if(OS_WAIT_FOREVER == wait_time)
	{	
		ret = krhino_buf_queue_recv(queue_handle,RHINO_WAIT_FOREVER,recv_buf,&recv_size);
	}
	else
	{	
		ret = krhino_buf_queue_recv(queue_handle,krhino_ms_to_ticks(wait_time),recv_buf,&recv_size);
	}
	if(0 == ret)
	{
		queue_debug_info("os queue recive sucess,handle:%x,size:%d\r\n",queue_handle,recv_size);
		return os_true;
	}
	else
	{
		queue_debug_error("os queue recive fail,reason:%d,block time:%x\r\n",ret,wait_time);
		return os_false;
	}
}
int os_queue_delete(os_queue_handle_t queue_handle)
{
	NULL_PARA_CHECK(queue_handle,os_false);
	int ret = 0;
	ret = krhino_buf_queue_del(queue_handle);
	if(0!=ret)
	{
		queue_debug_error("os queue delete fail,reason:%d\r\n",ret);
		return os_false;
	}
	queue_debug_info("os queue delete sucess,handle:%x\r\n",queue_handle);
	if(NULL != queue_handle)
		nos_free(queue_handle);
	return os_true;
}

int os_queue_used_nums(os_queue_handle_t queue_handle)
{
	NULL_PARA_CHECK(queue_handle,os_false);
	int ret =0;
	kbuf_queue_info_t queue_info;
	ret = krhino_buf_queue_info_get(queue_handle,&queue_info);
	if(0!=ret)
	{
		queue_debug_error("queue query error,handle:%x reason:%d\r\n",queue_handle,ret);
		return os_false;
	}
	queue_debug_info("queue query sucess,handle:%x used items:%d\r\n",queue_handle,queue_info.cur_num);
	return queue_info.cur_num;
}
int os_queue_free_nums(os_queue_handle_t queue_handle)
{
	NULL_PARA_CHECK(queue_handle,os_false);
	int available_num = 0;
	int ret = 0;
	
	int num_in_queue = os_queue_used_nums(queue_handle);
	//available_num = ((os_queue__t*)queue_handle)->max_msg_size - num_in_queue;
	available_num = (((os_queue__t*)queue_handle)->ringbuf).freesize/(((os_queue__t*)queue_handle)->ringbuf).blk_size;
	if(num_in_queue>=0)
	{
		queue_debug_info("queue query sucess,handle:%x available items:%d\r\n",queue_handle,available_num);
	}
	else
	{
		queue_debug_error("queue query error,handle:%x reason:%d\r\n",queue_handle,ret);
		return os_false;
	}
	return available_num;
}