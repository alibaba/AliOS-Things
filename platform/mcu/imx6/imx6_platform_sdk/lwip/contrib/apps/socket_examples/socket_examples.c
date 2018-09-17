
#include "socket_examples.h"

#include "lwip/opt.h"

#if LWIP_SOCKET

#include "lwip/sockets.h"
#include "lwip/sys.h"

#include <string.h>
#include <stdio.h>

#ifndef SOCK_TARGET_HOST
#define SOCK_TARGET_HOST  "192.168.1.1"
#endif

#ifndef SOCK_TARGET_PORT
#define SOCK_TARGET_PORT  80
#endif

/** This is an example function that tests
    blocking- and nonblocking connect. */
static void
sockex_nonblocking_connect(void *arg)
{
  int s;
  int ret;
  u32_t opt;
  struct sockaddr_in addr;
  fd_set readset;
  fd_set writeset;
  fd_set errset;
  struct timeval tv;
  u32_t ticks_a, ticks_b;
  int err;

  LWIP_UNUSED_ARG(arg);
  /* set up address to connect to */
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  /* first try blocking: */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should succeed */
  LWIP_ASSERT("ret == 0", ret == 0);

  /* write something */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == 4", ret == 4);

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  /* now try nonblocking and close before being connected */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);
  /* nonblocking */
  opt = lwip_fcntl(s, F_GETFL, 0);
  LWIP_ASSERT("ret != -1", ret != -1);
  opt |= O_NONBLOCK;
  ret = lwip_fcntl(s, F_SETFL, opt);
  LWIP_ASSERT("ret != -1", ret != -1);
  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should have an error: "inprogress" */
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EINPROGRESS", err == EINPROGRESS);
  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);
  /* try to close again, should fail with EBADF */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EBADF", err == EBADF);
  printf("closing socket in nonblocking connect succeeded\n");

  /* now try nonblocking, connect should succeed:
     this test only works if it is fast enough, i.e. no breakpoints, please! */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* nonblocking */
  opt = 1;
  ret = lwip_ioctl(s, FIONBIO, &opt);
  LWIP_ASSERT("ret == 0", ret == 0);

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should have an error: "inprogress" */
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EINPROGRESS", err == EINPROGRESS);

  /* write should fail, too */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EINPROGRESS", err == EINPROGRESS);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  /* select without waiting should fail */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);
  LWIP_ASSERT("ret == 0", ret == 0);
  LWIP_ASSERT("!FD_ISSET(s, &writeset)", !FD_ISSET(s, &writeset));
  LWIP_ASSERT("!FD_ISSET(s, &readset)", !FD_ISSET(s, &readset));
  LWIP_ASSERT("!FD_ISSET(s, &errset)", !FD_ISSET(s, &errset));

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  ticks_a = sys_now();
  /* select with waiting should succeed */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, NULL);
  ticks_b = sys_now();
  LWIP_ASSERT("ret == 1", ret == 1);
  LWIP_ASSERT("FD_ISSET(s, &writeset)", FD_ISSET(s, &writeset));
  LWIP_ASSERT("!FD_ISSET(s, &readset)", !FD_ISSET(s, &readset));
  LWIP_ASSERT("!FD_ISSET(s, &errset)", !FD_ISSET(s, &errset));

  /* now write should succeed */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == 4", ret == 4);

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("select() needed %d ticks to return writable\n", ticks_b - ticks_a);


  /* now try nonblocking to invalid address:
     this test only works if it is fast enough, i.e. no breakpoints, please! */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* nonblocking */
  opt = 1;
  ret = lwip_ioctl(s, FIONBIO, &opt);
  LWIP_ASSERT("ret == 0", ret == 0);

  addr.sin_addr.s_addr++;

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should have an error: "inprogress" */
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EINPROGRESS", err == EINPROGRESS);

  /* write should fail, too */
  ret = lwip_write(s, "test", 4);
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EINPROGRESS", err == EINPROGRESS);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  /* select without waiting should fail */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);
  LWIP_ASSERT("ret == 0", ret == 0);

  FD_ZERO(&readset);
  FD_SET(s, &readset);
  FD_ZERO(&writeset);
  FD_SET(s, &writeset);
  FD_ZERO(&errset);
  FD_SET(s, &errset);
  ticks_a = sys_now();
  /* select with waiting should eventually succeed and return errset! */
  ret = lwip_select(s + 1, &readset, &writeset, &errset, NULL);
  ticks_b = sys_now();
  LWIP_ASSERT("ret > 0", ret > 0);
  LWIP_ASSERT("FD_ISSET(s, &errset)", FD_ISSET(s, &errset));
  LWIP_ASSERT("!FD_ISSET(s, &readset)", !FD_ISSET(s, &readset));
  LWIP_ASSERT("!FD_ISSET(s, &writeset)", !FD_ISSET(s, &writeset));

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("select() needed %d ticks to return error\n", ticks_b - ticks_a);
  printf("all tests done, thread ending\n");
}

/** This is an example function that tests
    the recv function (timeout etc.). */
static void
sockex_testrecv(void *arg)
{
  int s;
  int ret;
  int err;
  int opt;
  struct sockaddr_in addr;
  size_t len;
  char rxbuf[1024];
  fd_set readset;
  fd_set errset;
  struct timeval tv;

  LWIP_UNUSED_ARG(arg);
  /* set up address to connect to */
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  /* first try blocking: */

  /* create the socket */
  s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s >= 0", s >= 0);

  /* connect */
  ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
  /* should succeed */
  LWIP_ASSERT("ret == 0", ret == 0);

  /* set recv timeout (100 ms) */
  opt = 100;
  ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));
  LWIP_ASSERT("ret == 0", ret == 0);

  /* write the start of a GET request */
#define SNDSTR1 "G"
  len = strlen(SNDSTR1);
  ret = lwip_write(s, SNDSTR1, len);
  LWIP_ASSERT("ret == len", ret == (int)len);

  /* should time out if the other side is a good HTTP server */
  ret = lwip_read(s, rxbuf, 1);
  LWIP_ASSERT("ret == -1", ret == -1);
  err = errno;
  LWIP_ASSERT("errno == EAGAIN", err == EAGAIN);

  /* write the rest of a GET request */
#define SNDSTR2 "ET / HTTP_1.1\r\n\r\n"
  len = strlen(SNDSTR2);
  ret = lwip_write(s, SNDSTR2, len);
  LWIP_ASSERT("ret == len", ret == (int)len);

  /* wait a while: should be enough for the server to send a response */
  sys_msleep(1000);

  /* should not time out but receive a response */
  ret = lwip_read(s, rxbuf, 1024);
  LWIP_ASSERT("ret > 0", ret > 0);

  /* now select should directly return because the socket is readable */
  FD_ZERO(&readset);
  FD_ZERO(&errset);
  FD_SET(s, &readset);
  FD_SET(s, &errset);
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  ret = lwip_select(s + 1, &readset, NULL, &errset, &tv);
  LWIP_ASSERT("ret == 1", ret == 1);
  LWIP_ASSERT("!FD_ISSET(s, &errset)", !FD_ISSET(s, &errset));
  LWIP_ASSERT("FD_ISSET(s, &readset)", FD_ISSET(s, &readset));

  /* should not time out but receive a response */
  ret = lwip_read(s, rxbuf, 1024);
  /* might receive a second packet for HTTP/1.1 servers */
  if (ret > 0) {
    /* should return 0: closed */
    ret = lwip_read(s, rxbuf, 1024);
    LWIP_ASSERT("ret == 0", ret == 0);
  }

  /* close */
  ret = lwip_close(s);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("sockex_testrecv finished successfully\n");
}

/** helper struct for the 2 functions below (multithreaded: thread-argument) */
struct sockex_select_helper {
  int socket;
  int wait_read;
  int expect_read;
  int wait_write;
  int expect_write;
  int wait_err;
  int expect_err;
  int wait_ms;
  sys_sem_t sem;
};

/** helper thread to wait for socket events using select */
static void
sockex_select_waiter(void *arg)
{
  struct sockex_select_helper *helper = (struct sockex_select_helper *)arg;
  int ret;
  fd_set readset;
  fd_set writeset;
  fd_set errset;
  struct timeval tv;

  LWIP_ASSERT("helper != NULL", helper != NULL);

  FD_ZERO(&readset);
  FD_ZERO(&writeset);
  FD_ZERO(&errset);
  if (helper->wait_read) {
    FD_SET(helper->socket, &readset);
  }
  if (helper->wait_write) {
    FD_SET(helper->socket, &writeset);
  }
  if (helper->wait_err) {
    FD_SET(helper->socket, &errset);
  }

  tv.tv_sec = helper->wait_ms / 1000;
  tv.tv_usec = (helper->wait_ms % 1000) * 1000;

  ret = lwip_select(helper->socket, &readset, &writeset, &errset, &tv);
  if (helper->expect_read || helper->expect_write || helper->expect_err) {
    LWIP_ASSERT("ret > 0", ret > 0);
  } else {
    LWIP_ASSERT("ret == 0", ret == 0);
  }
  if (helper->expect_read) {
    LWIP_ASSERT("FD_ISSET(helper->socket, &readset)", FD_ISSET(helper->socket, &readset));
  } else {
    LWIP_ASSERT("!FD_ISSET(helper->socket, &readset)", !FD_ISSET(helper->socket, &readset));
  }
  if (helper->expect_write) {
    LWIP_ASSERT("FD_ISSET(helper->socket, &writeset)", FD_ISSET(helper->socket, &writeset));
  } else {
    LWIP_ASSERT("!FD_ISSET(helper->socket, &writeset)", !FD_ISSET(helper->socket, &writeset));
  }
  if (helper->expect_err) {
    LWIP_ASSERT("FD_ISSET(helper->socket, &errset)", FD_ISSET(helper->socket, &errset));
  } else {
    LWIP_ASSERT("!FD_ISSET(helper->socket, &errset)", !FD_ISSET(helper->socket, &errset));
  }
  sys_sem_signal(&helper->sem);
}

/** This is an example function that tests
    more than one thread being active in select. */
static void
sockex_testtwoselects(void *arg)
{
  int s1;
  int s2;
  int ret;
  struct sockaddr_in addr;
  size_t len;
  err_t lwiperr;
  struct sockex_select_helper h1, h2, h3, h4;

  LWIP_UNUSED_ARG(arg);
  /* set up address to connect to */
  memset(&addr, 0, sizeof(addr));
  addr.sin_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
  addr.sin_addr.s_addr = inet_addr(SOCK_TARGET_HOST);

  /* create the sockets */
  s1 = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s1 >= 0", s1 >= 0);
  s2 = lwip_socket(AF_INET, SOCK_STREAM, 0);
  LWIP_ASSERT("s2 >= 0", s2 >= 0);

  /* connect, should succeed */
  ret = lwip_connect(s1, (struct sockaddr*)&addr, sizeof(addr));
  LWIP_ASSERT("ret == 0", ret == 0);
  ret = lwip_connect(s2, (struct sockaddr*)&addr, sizeof(addr));
  LWIP_ASSERT("ret == 0", ret == 0);

  /* write the start of a GET request */
#define SNDSTR1 "G"
  len = strlen(SNDSTR1);
  ret = lwip_write(s1, SNDSTR1, len);
  LWIP_ASSERT("ret == len", ret == (int)len);
  ret = lwip_write(s2, SNDSTR1, len);
  LWIP_ASSERT("ret == len", ret == (int)len);

  h1.wait_read  = 1;
  h1.wait_write = 1;
  h1.wait_err   = 1;
  h1.expect_read  = 0;
  h1.expect_write = 0;
  h1.expect_err   = 0;
  lwiperr = sys_sem_new(&h1.sem, 0);
  LWIP_ASSERT("lwiperr == ERR_OK", lwiperr == ERR_OK);
  h1.socket = s1;
  h1.wait_ms = 500;

  h2 = h1;
  lwiperr = sys_sem_new(&h2.sem, 0);
  LWIP_ASSERT("lwiperr == ERR_OK", lwiperr == ERR_OK);
  h2.socket = s2;
  h2.wait_ms = 1000;

  h3 = h1;
  lwiperr = sys_sem_new(&h3.sem, 0);
  LWIP_ASSERT("lwiperr == ERR_OK", lwiperr == ERR_OK);
  h3.socket = s2;
  h3.wait_ms = 1500;

  h4 = h1;
  lwiperr = sys_sem_new(&h4.sem, 0);
  LWIP_ASSERT("lwiperr == ERR_OK", lwiperr == ERR_OK);
  h4.socket = s2;
  h4.wait_ms = 2000;

  /* select: all sockets should time out if the other side is a good HTTP server */

  sys_thread_new("sockex_select_waiter1", sockex_select_waiter, &h2, 0, 0);
  sys_msleep(100);
  sys_thread_new("sockex_select_waiter2", sockex_select_waiter, &h1, 0, 0);
  sys_msleep(100);
  sys_thread_new("sockex_select_waiter2", sockex_select_waiter, &h4, 0, 0);
  sys_msleep(100);
  sys_thread_new("sockex_select_waiter2", sockex_select_waiter, &h3, 0, 0);

  sys_sem_wait(&h1.sem);
  sys_sem_wait(&h2.sem);
  sys_sem_wait(&h3.sem);
  sys_sem_wait(&h4.sem);

  /* close */
  ret = lwip_close(s1);
  LWIP_ASSERT("ret == 0", ret == 0);
  ret = lwip_close(s2);
  LWIP_ASSERT("ret == 0", ret == 0);

  printf("sockex_testtwoselects finished successfully\n");
}

void socket_examples_init(void)
{
  sys_thread_new("sockex_nonblocking_connect", sockex_nonblocking_connect, NULL, 0, 0);
  sys_thread_new("sockex_testrecv", sockex_testrecv, NULL, 0, 0);
  /*sys_thread_new("sockex_testtwoselects", sockex_testtwoselects, NULL, 0, 0);*/
}

#endif /* LWIP_SOCKETS */
