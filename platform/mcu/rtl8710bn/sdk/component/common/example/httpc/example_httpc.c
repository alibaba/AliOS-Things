#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <httpc/httpc.h>

#define SERVER_HOST  "httpbin.org"

static void example_httpc_thread(void *param)
{
	struct httpc_conn *conn = NULL;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: HTTPC\n");

	/* test GET via HTTP to http://httpbin.org/get?param1=test_data1&param2=test_data2 */

	conn = httpc_conn_new(HTTPC_SECURE_NONE, NULL, NULL, NULL);

	if(conn) {
		if(httpc_conn_connect(conn, SERVER_HOST, 80, 0) == 0) {
			/* HTTP GET request */
			// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
			httpc_request_write_header_start(conn, "GET", "/get?param1=test_data1&param2=test_data2", NULL, 0);
			// add other required header fields if necessary
			httpc_request_write_header(conn, "Connection", "close");
			// finish and send header
			httpc_request_write_header_finish(conn);

			// receive response header
			if(httpc_response_read_header(conn) == 0) {
				httpc_conn_dump_header(conn);

				// receive response body
				if(httpc_response_is_status(conn, "200 OK")) {
					uint8_t buf[1024];
					int read_size = 0, total_size = 0;

					while(1) {
						memset(buf, 0, sizeof(buf));
						read_size = httpc_response_read_data(conn, buf, sizeof(buf) - 1);

						if(read_size > 0) {
							total_size += read_size;
							printf("%s", buf);
						}
						else {
							break;
						}

						if(conn->response.content_len && (total_size >= conn->response.content_len))
							break;
					}
				}
			}
		}
		else {
			printf("\nERROR: httpc_conn_connect\n");
		}

		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	/* test POST via HTTPS to https://httpbin.org/post with data of param1=test_data1&param2=test_data2 */

	conn = httpc_conn_new(HTTPC_SECURE_TLS, NULL, NULL, NULL);

	if(conn) {
		if(httpc_conn_connect(conn, SERVER_HOST, 443, 0) == 0) {
			/* HTTP POST request */
			char *post_data = "param1=test_data1&param2=test_data2";
			// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
			httpc_request_write_header_start(conn, "POST", "/post", NULL, strlen(post_data));
			// add other header fields if necessary
			httpc_request_write_header(conn, "Connection", "close");
			// finish and send header
			httpc_request_write_header_finish(conn);
			// send http body
			httpc_request_write_data(conn, post_data, strlen(post_data));

			// receive response header
			if(httpc_response_read_header(conn) == 0) {
				httpc_conn_dump_header(conn);

				// receive response body
				if(httpc_response_is_status(conn, "200 OK")) {
					uint8_t buf[1024];
					int read_size = 0, total_size = 0;

					while(1) {
						memset(buf, 0, sizeof(buf));
						read_size = httpc_response_read_data(conn, buf, sizeof(buf) - 1);

						if(read_size > 0) {
							total_size += read_size;
							printf("%s", buf);
						}
						else {
							break;
						}

						if(conn->response.content_len && (total_size >= conn->response.content_len))
							break;
					}
				}
			}
		}
		else {
			printf("\nERROR: httpc_conn_connect\n");
		}

		httpc_conn_close(conn);
		httpc_conn_free(conn);
	}

	vTaskDelete(NULL);
}

void example_httpc(void)
{
	if(xTaskCreate(example_httpc_thread, ((const char*)"example_httpc_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_httpc_thread) failed", __FUNCTION__);
}
