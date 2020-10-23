/*
 * Copyright (c) 201, RISE SICS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "web-sense-server"
#define LOG_LEVEL LOG_LEVEL_INFO

/* HTTP server */
#include "httpd-simple.h"
/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_routes(struct httpd_state *s))
{
	char buff[15];

	PSOCK_BEGIN(&s->sout);
	
	int temperature = 15 + rand() % 25;
	
	sprintf(buff, "{\"temp\":%u}", temperature);
	printf("send json to requester\n");
	
	SEND_STRING(&s->sout, buff);
	
	
	PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/

/* Declare and auto-start this file's process */
PROCESS(web_sense_server, "Web Sense Server");
AUTOSTART_PROCESSES(&web_sense_server);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(web_sense_server, ev, data) {
	PROCESS_BEGIN();
	
	httpd_init();

  	LOG_INFO("Contiki-NG Web Sense Server started\n");

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
		httpd_appcall(data);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{
	return generate_routes;
}
