/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

// @file tc_net_shutdown.c
// @brief Test Case Example for shutdown() API
#include <tinyara/config.h>
#include <errno.h>
#include <sys/stat.h>
#include <net/if.h>
#include <apps/netutils/netlib.h>
#include "tc_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<pthread.h>

#define PORTNUM 1112
#define MAXRCVLEN 20
int mut = 0;
/**
   * @fn                   :shutdown_wait
   * @brief                :function to wait on semaphore
   * @scenario             :
   * API's covered         :
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void shutdown_wait(void)
{
	while (mut <= 0) {

		printf("");
	}
	mut--;
}

/**
   * @fn                   :shutdown_signal
   * @brief                :function to signal semaphore
   * @scenario             :
   * API's covered         :
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void shutdown_signal(void)
{
	mut++;
}

/**
   * @fn                   :tc_net_shutdown_recv_p
   * @brief                :
   * @scenario             :
   * API's covered         :shutdown()
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void tc_net_shutdown_recv_p(int fd)
{
	int ret = shutdown(fd, SHUT_RD);
	if (ret == -1) {

		printf("  \ntc_net_shutdown_recv_p:FAIL %d\n", errno);
		nw_total_fail++;
		RETURN_ERR;
	}
	printf("  \ntc_net_shutdown_recv_p:PASS\n");
	nw_total_pass++;

}

/**
   * @fn                   :tc_net_shutdown_send_p
   * @brief                :
   * @scenario             :
   * API's covered         :shutdown()
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void tc_net_shutdown_send_p(int fd)
{
	char *msg = "Hello World !\n";
	int ret = shutdown(fd, SHUT_WR);

	if (ret == -1) {

		printf("  \ntc_net_shutdown_send_p:FAIL %d\n", errno);
		nw_total_fail++;
		RETURN_ERR;
	}

	printf("  \ntc_net_shutdown_send_p:PASS\n");
	nw_total_pass++;

}

/**
   * @fn                   :tc_net_shutdown_sendrecv_p
   * @brief                :
   * @scenario             :
   * API's covered         :shutdown()
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void tc_net_shutdown_sendrecv_p(int fd)
{
	int ret = shutdown(fd, SHUT_RDWR);
	if (ret == -1) {

		printf("  \ntc_net_shutdown_sendrecv_p:FAIL %d\n", errno);
		nw_total_fail++;
		RETURN_ERR;
	}
	printf("  \ntc_net_shutdown_sendrecv_p:PASS\n");
	nw_total_pass++;

}

/**
   * @fn                   :tc_net_shutdown_n
   * @brief                :
   * @scenario             :
   * API's covered         :shutdown()
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void tc_net_shutdown_n(void)
{
	int ret = shutdown(-1, SHUT_RDWR);
	if (ret != -1) {

		printf("  \ntc_net_shutdown_n:FAIL %d\n", errno);
		nw_total_fail++;
		RETURN_ERR;
	}
	printf("  \ntc_net_shutdown_n:PASS\n");
	nw_total_pass++;

}

/**
   * @fn                   :tc_net_shutdown_sock_n
   * @brief                :
   * @scenario             :
   * API's covered         :shutdown()
   * Preconditions         :
   * Postconditions        :
   * @return               :void
   */
void tc_net_shutdown_sock_n(int fd)
{
	int ret = shutdown(fd, SHUT_RD);
	if (ret != -1) {

		printf("  \ntc_net_shutdown_sock_n:FAIL %d\n", errno);
		nw_total_fail++;
		RETURN_ERR;
	}
	printf("  \ntc_net_shutdown_sock_n:PASS\n");
	nw_total_pass++;

}

/**
   * @fn                   :shutdown_server
   * @brief                :
   * @scenario             :
   * API's covered         :socket,bind,listen,send,accept,close
   * Preconditions         :
   * Postconditions        :
   * @return               :void *
   */
void *shutdown_server(void *args)
{
	char *msg = "Hello World !\n";
	struct sockaddr_in sa;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sa, 0, sizeof(sa));

	sa.sin_family = PF_INET;
	sa.sin_port = htons(PORTNUM);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(SocketFD, (struct sockaddr *)&sa, sizeof(sa));

	listen(SocketFD, 1);

	shutdown_signal();

	int ConnectFD = accept(SocketFD, NULL, NULL);
	tc_net_shutdown_send_p(ConnectFD);
	int val = send(ConnectFD, msg, strlen(msg), 0);
	if (val == -1)
		printf("\nShutdown send successful %d\n", errno);
	tc_net_shutdown_recv_p(ConnectFD);
	val = recv(ConnectFD, msg, 1024, 0);
	if (val == -1)
		printf("\nShutdown recv successful %d\n", errno);
	tc_net_shutdown_n();
	tc_net_shutdown_sock_n(SocketFD);
	close(ConnectFD);
	close(SocketFD);

	return 0;

}

/**
   * @fn                   :shut_client
   * @brief                :
   * @scenario             :
   * API's covered         :socket,connect,recvfrom,close
   * Preconditions         :
   * Postconditions        :
   * @return               :void *
   */
void *shutdown_client(void *args)
{
	char buffer[MAXRCVLEN];
	int len, mysocket;
	struct sockaddr_in dest;

	mysocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = PF_INET;
	dest.sin_addr.s_addr = inet_addr("127.0.0.1");
	dest.sin_port = htons(PORTNUM);

	shutdown_wait();

	connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
	len = recv(mysocket, buffer, MAXRCVLEN, 0);
	buffer[len] = '\0';
	tc_net_shutdown_recv_p(mysocket);
	int val = recv(mysocket, buffer, MAXRCVLEN, 0);
	if (val == -1)
		printf("\nShutdown recv successful %d\n", errno);
	tc_net_shutdown_sendrecv_p(mysocket);
	val = recv(mysocket, buffer, MAXRCVLEN, 0);
	if (val == -1)
		printf("\nShutdown recv successful %d\n", errno);
	val = send(mysocket, buffer, strlen(buffer), 0);
	if (val == -1)
		printf("\nShutdown recv successful %d\n", errno);
	tc_net_shutdown_n();
	close(mysocket);
	return 0;

}

/****************************************************************************
 * Name: shutdown()
 ****************************************************************************/
int net_shutdown_main(void)
{

	pthread_t Server, Client;

	pthread_create(&Server, NULL, shutdown_server, NULL);
	pthread_create(&Client, NULL, shutdown_client, NULL);

	pthread_join(Server, NULL);

	pthread_join(Client, NULL);

	return 0;
}