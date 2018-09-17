/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int SysRegWrite(unsigned long phyaddr, unsigned int val)
{
	*((volatile unsigned int *)(phyaddr & ~0x00000003)) = val;
	return 0;
}


static int SysRegRead(unsigned long phyaddr, unsigned int *val)
{
	*val = *((volatile unsigned int *)(phyaddr & ~0x00000003));
	return 0;
}

void cmd_regs(char *pwbuf, int blen, int argc, char **argv)
{
	unsigned long m_phyaddr;
	unsigned int m_val;
	unsigned int m_len;
	unsigned int i;
			
	/* for command */
	if (1 == argc)
	{
		printf("\n\n\"regs help\" for help\n\n");
		printf("Usage:\t\n");
		printf("regs [reg]\t\t\t\tread\t\n");
		printf("regs [reg] [val]\t\t\t\twrite\t\n");
		printf("regs -d [reg] [len]\t\t\t\treadn\t\n");
	}
	else if (2 == argc)
	{	
		m_phyaddr = strtoul(argv[1],NULL,16);
		SysRegRead(m_phyaddr, &m_val);
		printf("[reg] = 0x%08lx,[val]= 0x%08x\n",m_phyaddr,m_val);
	}
	else if (3 == argc)
	{	
		m_phyaddr = strtoul(argv[1],NULL,16);
		m_val = strtoul(argv[2],NULL,16);
		SysRegWrite(m_phyaddr,m_val);
		printf("[reg] = 0x%08lx,[val]= 0x%08x\n",m_phyaddr,m_val);
	}
	else if (4 == argc)
	{
		if(!strcmp(argv[1], "-d"))
		{
			m_phyaddr = strtoul(argv[2],NULL,16);
			m_len = strtoul(argv[3],NULL,16);
			printf("[reg] = 0x%08lx,[len]= 0x%08x\n",m_phyaddr,m_len);
			for(i = 0; i < m_len; i+= 4)
			{
				if(!(i % 16))
					printf("\n%08lx: ",m_phyaddr + i);
				SysRegRead(m_phyaddr + i, &m_val);
				printf("%08x ",m_val);
			}
			printf("\n");
		}
	}
	
	return;
}
