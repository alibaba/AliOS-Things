/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <deque>
#include <exception>
#include <iomanip> 
#include <ios> 
#include <algorithm> 
#include <bitset> 
#include <cctype>   
#include <stdexcept> 
#include <utility> 
#include <cerrno>  
#include <complex>  
#include <istream>
#include <queue>  
#include <set>
#include <cwchar> 
#include <fenv.h> 
#include <inttypes.h> 
#include <stdbool.h> 
#include <stdint.h> 
#include <tgmath.h> 
#include <map>
#include <sys/time.h>
#include <stdio.h>
#include <vector>
#include <hash_set>
#include <ext/hash_map>
#include <string.h>
#include <pthread.h>
#include <functional>
#include <list>
#include <unistd.h>
#include <numeric>
#include <iterator>

#include "cmsis_os.h"
#include "hal_trace.h"
//#include "hal_timer.h"

#define NUM_THREADS 2

//#pragma STDC FENV_ACCESS ON

//Setting this parameter represents the state value that can access floating-point operation exceptions
using namespace std;
using namespace __gnu_cxx;

////////////////////////static and dynamic polymorphic/////////////////

static int Add(int left, int right)
{
    return left + right;
}
static float Add(float left, int right)
{
    return left + right;
}

class TakeBus
{
public:
	void TakeBusToSubway()
	{
		char plan[]="go to Subway--->please take bus of 318";
	}
	void TakeBusToStation()
	{
		char plan[]="go to Station--->pelase Take Bus of 306 or 915";
	}
};

class Bus
{
	int a;
public:
	virtual void TakeBusToSomewhere(TakeBus& tb) = 0;
};

class Subway :public Bus
{
public:
	virtual void TakeBusToSomewhere(TakeBus& tb)
	{
		tb.TakeBusToSubway();
	}
};

class Station :public Bus
{
public:
	virtual void TakeBusToSomewhere(TakeBus& tb)
	{
		tb.TakeBusToStation();
	}
};


TakeBus _tb;
Subway _sbw;
Station _statn;

/*********************************************************/
static int cpp_polymorphic(void)
{
	/////////////////static/////////////////
	int a;
	TakeBus tb;
	float b;
    a=Add(10, 20);
    b=Add(10.5f,20);
	/////////////////////dynamic/////////////////////
	Bus* bu = NULL;
	
	if ((rand() % 2) & 1)
		bu = new Subway;
	else
		bu = new Station;

	bu->TakeBusToSomewhere(tb);
	TRACE("%-32s%s", __FUNCTION__, ((30!=a) || (30.5!=b) || (bu==NULL))?"FAIL":"PASS");
	delete bu;
	bu=NULL;
    return 0;
}

