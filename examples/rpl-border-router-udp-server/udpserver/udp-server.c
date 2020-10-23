#define _GNU_SOURCE

#include "contiki.h"
#include "contiki-net.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "net/ipv6/uiplib.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678
#define FIFO_FILE "MYFIFO"

static struct simple_udp_connection udp_conn;	
static int fd;
//, reply_control, first_run_only_if_65 = 0;
//static char msg_cntr[2] = {0};

PROCESS(udp_server_process, "UDP server");
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  char str[UIPLIB_IPV6_MAX_STR_LEN];
	char buf[UIPLIB_IPV6_MAX_STR_LEN];

	LOG_INFO("Received request '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
	
	uiplib_ipaddr_snprint(str, sizeof(str), sender_addr);
	//	Adding only the node ID to str
	str[0] = str[18];
	str[1] = str[19];
	//	Example data :
	//	rssi: -62, from fe80::212:7402:2:20 to ff02::1, time: 302
	//	Removing 'rssi: '
	data += 6;
	//	Adding RSSI to str
	str[2] = (char) *data++;
	str[3] = (char) *data++;
	str[4] = (char) *data++;
	// Removing ', from fe80::212:7402:2:'
	data += 24;
	// Adding mobile ID to str
	str[5] = (char) *data++;
	// Removing ' to ff02::1, time: '
	data += 20;
	str[6] = (char) *data++;
	str[7] = (char) *data++;
	str[8] = (char) *data++;
	str[9] = ';';
	
	int i;
	for ( i = 10 ; i < UIPLIB_IPV6_MAX_STR_LEN ; i++ ) {
		str[i] = '\0';
	}
	// Adding time to str
	//strcat(str,",");
	//strcat(str,(char *) data);
	//strcat(str,";");

	fd = open(FIFO_FILE, O_WRONLY);
	write(fd, str, sizeof(str));
	close(fd);

	memset(buf, 0, UIPLIB_IPV6_MAX_STR_LEN * (sizeof buf[0]) );

	fd = open(FIFO_FILE, O_RDONLY);
	read(fd, buf, sizeof buf);
	close(fd);

	/*printf("\nBefore Loop: \tmsg_cntr: %s\treply_control: %d\tbuf[21][22]: %c%c\n", msg_cntr, reply_control, buf[21], buf[22]);
	
	if ( ( msg_cntr[0] != buf[21] && msg_cntr[1] != buf[22] ) || !(first_run_only_if_65) ) {
			msg_cntr[0] = buf[21];
			msg_cntr[1] = buf[22];
			reply_control = 1;
			first_run_only_if_65 = 1;
			printf("\nInside Loop: \tmsg_cntr: %s\treply_control: %d\n", msg_cntr, reply_control);
	}*/

#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
	//if ( reply_control ) {
  LOG_INFO("Sending response.\n");
	simple_udp_sendto(&udp_conn, buf, sizeof(buf), sender_addr);
		//reply_control = 0;
	//}
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{

	PROCESS_BEGIN();

	/* Create the FIFO if it does not exist */
	mkfifo(FIFO_FILE, 0666);

  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
