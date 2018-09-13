/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "net/HTTPClient/HTTPCUsr_api.h"
#include "net/mbedtls/mbedtls.h"

static int HTTPC_checksum_check(void *url)
{
	if (strstr(url, "checksum") != NULL)
		return 0;
	return -1;
}
static char HTTPC_cal_checksum(void *buffer, int length)
{
	unsigned char *cal = (unsigned char *)buffer;
	unsigned char result = 0;
	while (length != 0) {
		result += cal[--length];
	}
	return result;
}

static unsigned char checksum = 0;
static int checksum_flag = 0;
static int HTTPC_get_test(HTTPParameters *clientParams)
{
	int nRetCode = 0;
	int recvSize = 0;
	char *buf = NULL;
	if ((buf = malloc(HTTP_CLIENT_BUFFER_SIZE)) == NULL) {
		return -1;
	}
	do {
		nRetCode = HTTPC_get(clientParams,buf, 4096,(INT32 *)&recvSize);
		if (checksum_flag == 1)
			checksum += HTTPC_cal_checksum(buf,recvSize);

		if (nRetCode != HTTP_CLIENT_SUCCESS)
		break;
	} while(1);
	if (buf) {
		free(buf);
		buf = NULL;
	}
	if (nRetCode == HTTP_CLIENT_EOS) {
		CMD_DBG("HTTP GET SUCCESS\n");
		nRetCode = HTTP_CLIENT_SUCCESS;
	}

	return nRetCode;
}

#ifdef HTTPC_CMD_REGISTER_USER_CALLBACK
security_client user_param;
extern const char   mbedtls_test_cas_pem[];
extern const size_t mbedtls_test_cas_pem_len;

void* get_certs()
{
	memset(&user_param, 0, sizeof(user_param));
	user_param.pCa = (char *)mbedtls_test_cas_pem;
	user_param.nCa = mbedtls_test_cas_pem_len;
	return &user_param;
}
#endif

static int HTTPC_get_test_fresh(HTTPParameters *clientParams)
{
	int ret = 0;
	unsigned int toReadLength = 4096;
	unsigned int Received = 0;
	HTTP_CLIENT httpClient;
	memset((void *)&httpClient, 0, sizeof(httpClient));
	char *buf = malloc(toReadLength);
	if (buf == NULL) {
		CMD_ERR("malloc pbuffer failed..\n");
		return -1;
	}

	clientParams->HttpVerb = VerbGet;

#ifdef HTTPC_CMD_REGISTER_USER_CALLBACK
	HTTPC_Register_user_certs(get_certs);
#endif

	if (HTTPC_open(clientParams) != 0) {
		CMD_ERR("http open err..\n");
	} else if (HTTPC_request(clientParams, NULL) != 0) {
		CMD_ERR("http request err..\n");
	} else if (HTTPC_get_request_info(clientParams, &httpClient) != 0){
		CMD_ERR("http get request info err..\n");
	}else if (httpClient.TotalResponseBodyLength != 0) {
		do {
			if ((ret = HTTPC_read(clientParams, buf, toReadLength, (void *)&Received)) != 0) {
				if (ret == 1000) {
					CMD_DBG("The end..\n");
				} else
					CMD_ERR("Transfer err...ret:%d\n",ret);
			} else {
				//CMD_DBG("get data :%d\n", Received);
			}

			if (checksum_flag == 1)
				checksum += HTTPC_cal_checksum(buf,Received);

			if (ret != 0)
				break;

		} while (1);

	}
	free(buf);
	HTTPC_close(clientParams);
	return ret;
}

static int HTTPC_muti_get_test(HTTPParameters *clientParams,char *url0, char *url1)
{
	int ret = 0;
	unsigned int toReadLength = 4096;
	unsigned int Received = 0;
	char *pUrl0 = NULL;
	char *pUrl1 = NULL;
	char *buf = malloc(toReadLength);
	if (buf == NULL) {
		CMD_ERR("malloc pbuffer failed..\n");
		return -1;
	}
	HTTP_CLIENT httpClient;
	memset((void *)&httpClient, 0, sizeof(httpClient));
	clientParams->HttpVerb = VerbGet;
	if ((pUrl0 = url0) == NULL) {
		CMD_ERR("http request NULL url0..\n");
		ret = -1;
		goto request_release;
	}
	if ((pUrl1 = url1) == NULL) {
		CMD_ERR("http request NULL url..\n");
		ret = -1;
		goto request_release;
	}
	strlcpy(clientParams->Uri, pUrl0,sizeof(clientParams->Uri));
	CMD_DBG("http test get url0..\n");
	if (HTTPC_open(clientParams) != 0) {
		CMD_ERR("http open err..\n");
		ret = -1;
		goto request_release;
	}
	pUrl0 = NULL;
	goto direct_request;
set_url1:
	HTTPC_reset_session(clientParams);
	strlcpy(clientParams->Uri, pUrl1,sizeof(clientParams->Uri));
	CMD_DBG("http test get url1..\n");
	pUrl1 = NULL;
direct_request:
	if (HTTPC_request(clientParams, NULL) != 0) {
		CMD_ERR("http request err..\n");
		goto request_release;
	}
	if (HTTPC_get_request_info(clientParams, &httpClient) != 0){
		CMD_ERR("http get request info err..\n");
	}else if (httpClient.TotalResponseBodyLength != 0) {
		do {
			if ((ret = HTTPC_read(clientParams, buf, toReadLength, (void *)&Received)) != 0) {
				if (ret == 1000) {
					CMD_DBG("The end..\n");
				} else
					CMD_ERR("Transfer err...ret:%d\n",ret);
			} else {
				//CMD_DBG("get data :%d\n", Received);
			}
			if (checksum_flag == 1)
				checksum += HTTPC_cal_checksum(buf,Received);
			if (ret != 0)
				break;
		} while (1);

	}
	if (pUrl1 != NULL && checksum == 0xFF) {
		checksum = 0;
		goto set_url1;
	}
request_release:
	free(buf);
	HTTPC_close(clientParams);
	return ret;
}

#if 0
char *headers = "testkey0:testvalue0&testkey1:testvalue1";
void* get_heads()
{
	return headers;
}
#endif
static int HTTPC_post_test(HTTPParameters *clientParams, char *credentials)
{
	int ret = 0;
	unsigned int toReadLength = 4096;
	unsigned int Received = 0;
	char *buf = malloc(toReadLength);
	if (buf == NULL)
		CMD_ERR("malloc pbuffer failed..\n");
	HTTP_CLIENT httpClient;
	memset((void *)&httpClient, 0, sizeof(httpClient));

	memset(buf, 0, toReadLength);
	clientParams->HttpVerb = VerbPost;
	clientParams->pData = buf;
	memcpy(buf, credentials, strlen(credentials));
	clientParams->pLength = strlen(credentials);
request:
	if ((ret = HTTPC_open(clientParams)) != 0) {
		CMD_ERR("http open err..\n");
		goto relese;
	}

	if ((ret = HTTPC_request(clientParams, NULL)) != 0) {
		CMD_ERR("http request err..\n");
		goto relese;
	}
	if ((ret = HTTPC_get_request_info(clientParams, &httpClient)) != 0) {
		CMD_ERR("http get request info err..\n");
		goto relese;
	}
	if (httpClient.HTTPStatusCode != HTTP_STATUS_OK) {
		if((httpClient.HTTPStatusCode == HTTP_STATUS_OBJECT_MOVED) ||
				(httpClient.HTTPStatusCode == HTTP_STATUS_OBJECT_MOVED_PERMANENTLY)) {
			CMD_DBG("Redirect url..\n");
			HTTPC_close(clientParams);
			memset(clientParams, 0, sizeof(*clientParams));
			clientParams->HttpVerb = VerbGet;
			if (httpClient.RedirectUrl->nLength < sizeof(clientParams->Uri))
				strncpy(clientParams->Uri, httpClient.RedirectUrl->pParam,
						httpClient.RedirectUrl->nLength);
			else
				goto relese;
			CMD_DBG("go to request.\n");
			goto request;

		} else {
			ret = -1;
			CMD_DBG("get result not correct.\n");
			goto relese;
		}
	}
	if (httpClient.TotalResponseBodyLength != 0 || (httpClient.HttpFlags & HTTP_CLIENT_FLAG_CHUNKED )) {

		do {
			memset(buf, 0, 4096);
			if ((ret = HTTPC_read(clientParams, buf, toReadLength, (void *)&Received)) != 0) {
				//CMD_DBG("get data,Received:%d\n",Received);
				if (ret == 1000) {
					ret = 0;
					CMD_DBG("The end..\n");
				} else
					CMD_ERR("Transfer err...ret:%d\n",ret);

				if (checksum_flag == 1)
					checksum += HTTPC_cal_checksum(buf,Received);

				break;
			} else {
				//CMD_DBG("get data,Received:%d\n",Received);

			}
			if (checksum_flag == 1)
				checksum += HTTPC_cal_checksum(buf,Received);
		} while (1);
	}
relese:
	free(buf);
	HTTPC_close(clientParams);
	return ret;
}

static int HTTPC_head_test(HTTPParameters *clientParams)
{
	return 0;
}
static int httpc_exec(char *cmd)
{
	//CMD_DBG("HTTPC TEST %s\n",cmd);
	int ret = -1;
	int argc = 0;
	char *argv[6];

	HTTPParameters *clientParams;
	clientParams = malloc(sizeof(*clientParams));
	if (!clientParams) {
		return -1;
	} else{
		memset(clientParams,0,sizeof(HTTPParameters));
	}

	argc = cmd_parse_argv(cmd, argv, 6);
	if (argc < 2) {
		CMD_ERR("invalid httpc cmd, argc %d\n", argc);
		ret = -1;
		goto releaseParams;
	}

	if (HTTPC_checksum_check(argv[1]) == 0)
		checksum_flag = 1;
	else
		checksum_flag = 0;
	checksum = 0;

	strcpy(clientParams->Uri,argv[1]);
	if (cmd_strncmp(argv[0],"get",3) == 0) {
		if ((ret = HTTPC_get_test(clientParams)) != 0)
			goto releaseParams;
	} else if (cmd_strncmp(argv[0], "post", 4) == 0) {
		if (argc < 3) {
			CMD_ERR("invalid httpc cmd(post), argc %d\n", argc);
			ret = -1;
			goto releaseParams;
		}
		if ((ret = HTTPC_post_test(clientParams, argv[2])) != 0)
			goto releaseParams;

	} else if (cmd_strncmp(argv[0], "-get", 4) == 0) {

		if ((ret = HTTPC_get_test_fresh(clientParams)) != 0)
			goto releaseParams;

	} else if (cmd_strncmp(argv[0], "head", 4) == 0) {
		if ((ret = HTTPC_head_test(clientParams)) != 0)
			goto releaseParams;
	}else if (cmd_strncmp(argv[0], "auth-get", 8) == 0) {
		// net httpc auth-get(0) url(1) test(id)(2) 12345678(key)(3)
		if (argc < 4) {
			CMD_ERR("invalid httpc cmd(auth-get), argc %d\n", argc);
			ret = -1;
			goto releaseParams;
		}
		cmd_memcpy(clientParams->UserName, argv[2], cmd_strlen(argv[2]));
		cmd_memcpy(clientParams->Password, argv[3], cmd_strlen(argv[3]));
		clientParams->AuthType = AuthSchemaDigest;
		if ((ret = HTTPC_get_test_fresh(clientParams)) != 0)
			goto releaseParams;
	}else if (cmd_strncmp(argv[0], "ssl-get", 7) == 0) {
		//net httpc ssl-get(0) url(1)
		if ((ret = HTTPC_get_test_fresh(clientParams)) != 0)
			goto releaseParams;

	}else if (cmd_strncmp(argv[0], "ssl-post", 8) == 0) {
		//net httpc ssl-post(0) url(1) data(2)
		if (argc < 3) {
			CMD_ERR("invalid httpc cmd(ssl-post), argc %d\n", argc);
			ret = -1;
			goto releaseParams;
		}
		if ((ret = HTTPC_post_test(clientParams, argv[2])) != 0)
			goto releaseParams;
	}else if (cmd_strncmp(argv[0], "muti-get", 8) == 0) {
		if (argc < 3) {
			CMD_ERR("invalid httpc cmd(muti-get), argc %d\n", argc);
			ret = -1;
			goto releaseParams;
		}
		clientParams->Flags |= HTTP_CLIENT_FLAG_KEEP_ALIVE;
		if ((ret = HTTPC_muti_get_test(clientParams,argv[1], argv[2])) != 0)
			goto releaseParams;
	}else if (cmd_strncmp(argv[0], "muti-post", 8) == 0) {
	}

releaseParams:
	if (clientParams != NULL) {
		free(clientParams);
		clientParams = NULL;
	}
	if (checksum_flag == 1) {
		printf("[httpc test]:checksum = %#x\n",checksum);
		if (checksum != 0xff)
			ret = -1;
		else
			ret = 0;
	}
	checksum_flag = 0;
	checksum = 0;
	return ret;
}

#define HTTPC_THREAD_STACK_SIZE		(4 * 1024)
static OS_Thread_t g_httpc_thread;

void httpc_cmd_task(void *arg)
{
	char *cmd = (char *)arg;
	CMD_LOG(1, "<net> <httpc> <request> <cmd : %s>\n",cmd);
	int ret = httpc_exec(cmd);
	if (ret != 0 && ret != 1000)
		CMD_LOG(1, "<net> <httpc> <response : fail> <%s>\n",cmd);
	else {
		CMD_LOG(1, "<net> <httpc> <response : success> <%s>\n",cmd);
	}
	OS_ThreadDelete(&g_httpc_thread);
}

enum cmd_status cmd_httpc_exec(char *cmd)
{
	if (OS_ThreadIsValid(&g_httpc_thread)) {
		CMD_ERR("httpc task is running\n");
		return CMD_STATUS_FAIL;
	}
	if (OS_ThreadCreate(&g_httpc_thread,
				"",
				httpc_cmd_task,
				(void *)cmd,
				OS_THREAD_PRIO_APP,
				HTTPC_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("httpc task create failed\n");
		return CMD_STATUS_FAIL;
	}
	return CMD_STATUS_OK;
}

#if 0
        if (cmd_strncmp(argv[0],"__post_m",8) == 0) {

                strcpy(ClientParams->Uri,argv[1]);
                int conLength = 0;
                unsigned int toReadLength = 4096;
                unsigned int Received = 0;
                char *pBuffer = malloc(toReadLength);
                if (pBuffer == NULL)
                        printf("malloc pbuffer failed..\n");

                memset(pBuffer, 0, toReadLength);
                ClientParams->HttpVerb = VerbPost;
                ClientParams->pData = pBuffer;
                ClientParams->sData = client_credentials;
                ClientParams->sLength = strlen(client_credentials);

                if (HTTPC_open(ClientParams) != 0) {

                        printf("http open err..\n");

                }
                unsigned int times = 0x2;
                while(times--) {
                        if (HTTPC_request(ClientParams,(void *)&conLength) != 0) {
                                printf("http request err..\n");

                        } else if (conLength != 0 || (((P_HTTP_SESSION)(ClientParams->pHTTP))->HttpFlags & HTTP_CLIENT_FLAG_CHUNKED )) {

                                do {
                                        int retValue = 0;
                                        if ((retValue = HTTPC_read(ClientParams, pBuffer, toReadLength, (void *)&Received)) != 0) {
                                                if (retValue == 1000) {
                                                        printf("The end..\n");
                                                } else
                                                        printf("Transfer err...retValue:%d\n",retValue);
                                                break;


                                        } else {
                                                printf("get data :%d\n", Received);
                                        }

                                } while (1);
                        }
                        if (retValue != 1000 && retValue != 0) {
                                printf("httpc post err...\n");
                                break;

                        } else {
                                printf("*******httpc post success********\n");
                                while(1){
                                        OS_Sleep(20);
                                }
                                ret = retValue;
                        }
                        OS_Sleep(2);
                }

                free(pBuffer);
                HTTPC_close(ClientParams);
        }
#endif
