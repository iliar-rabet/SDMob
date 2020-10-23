#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/packetbuf.h"
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (10* CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[100];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  // printf("rep:%s\n",buffer);
  return buffer;
}

static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  	uip_ip6addr_t rcvd_ip;
	// char buff[40];
	data++;	
  LOG_INFO("Received response ");
  // printf("\ndata is:%s\n",data);
  // int i;    
	// for(i=0; i < 8; i++) {
	// 	if ( i == 7 ) {
	// 			buff[35] = '0';
	// 			buff[36] = ( *data++ >> 8 ) & 0xff;
	// 			buff[37] = '0';
	// 			buff[38] = ( *data >> 8 ) & 0xff;
	// 			buff[39] = '\0';
	// 	} else {
	// 			buff[5*i] = '0';
	// 			buff[5*i+1] = ( *data++ >> 8 ) & 0xff;
	// 			buff[5*i+2] = '0';
	// 			buff[5*i+3] = ( *data++ >> 8 ) & 0xff;
	// 			buff[5*i+4] = ':';
	// 	}
	// }


	// for (i = 0; i < 16; i++) printf("%c", (((*data++ >> 8 ) & 0xff ) ));
	
  char buf[100];

  printf("last_rssi %d\n",(signed short)  (PACKETBUF_ATTR_RSSI));
  
  strcpy(buf,replace_str((char *)data,"::",":0:0:0:"));
  // printf("buf:%s\n",buf);
	if(!uiplib_ip6addrconv(buf, &rcvd_ip)) {
    printf("here error!!!\n");
    return;
    }
  
	// Logging received IP address
	LOG_INFO_6ADDR(&rcvd_ip);
	// printf("%s", buff);
	printf(" from ");
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count;
  static char str[32];
  //const char *string;
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if( NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      // LOG_INFO("Sending request %u to ", count);
      // LOG_INFO_6ADDR(&dest_ipaddr);
      // LOG_INFO_("\n");
      snprintf(str, sizeof(str), "hello %d", count);
      printf("sending %s\n",str);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      count++;
      //rpl_neighbor_print_list(string);
    } else {
      LOG_INFO("%d %d\n", NETSTACK_ROUTING.node_is_reachable(), NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr));
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL);
    //   - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
