
/**
  * @file     	HaasLog.h
  * @author   	HaasAI Group
  * @version	V1.0.0
  * @date    	2021-01-10
  * @license  	GNU General Public License (GPL)
  * @brief
  * @attention
  *  This file is part of HaasAI.                                \n
  *  This program is free software; you can redistribute it and/or modify 		\n
  *  it under the terms of the GNU General Public License version 3 as 		    \n
  *  published by the Free Software Foundation.                               	\n
  *  You should have received a copy of the GNU General Public License   		\n
  *  along with HaasAI. If not, see <http://www.gnu.org/licenses/>.       			\n
  *  Unless required by applicable law or agreed to in writing, software       	\n
  *  distributed under the License is distributed on an "AS IS" BASIS,         	\n
  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  	\n
  *  See the License for the specific language governing permissions and     	\n
  *  limitations under the License.   											\n
  *   																			\n
  * @htmlonly
  * <span style="font-weight: bold">History</span>
  * @endhtmlonly
  * Version|Author|Date|Describe
  * ------|----|------|--------
  * V1.0|HaasAI Group|2021-01-10|Create File
  * <h2><center>&copy;COPYRIGHT 2021 WELLCASA All Rights Reserved.</center></h2>
  */
#ifndef HAAS_LOG_H
#define HAAS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define LOG_ENABLE_D
#endif

#define LOG_ENABLE_I
#define LOG_ENABLE_W
#define LOG_ENABLE_E

#ifdef LOG_ENABLE_D
#define LOG_D(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_D(fmt, args...)
#endif

#ifdef LOG_ENABLE_I
#define LOG_I(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_I(fmt, args...)
#endif

#ifdef LOG_ENABLE_W
#define LOG_W(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_W(fmt, args...)
#endif

#ifdef LOG_ENABLE_E
#define LOG_E(fmt, args...) \
    do {printf("%s:%d", __FUNCTION__, __LINE__); printf("\n\r");printf(fmt,##args);printf("\n\r");} while(0)
#else
#define LOG_E(fmt, args...)
#endif

#ifdef LOG_ENABLE_E
    #define debug_line() printf(" ********** %s, %d ********** \n\n", __func__, __LINE__)
#else
    #define debug_line()
#endif

#ifdef __cplusplus
}
#endif

#endif
