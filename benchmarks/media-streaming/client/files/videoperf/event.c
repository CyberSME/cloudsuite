/*
    httperf -- a tool for measuring web server performance
    Copyright (C) 2000  Hewlett-Packard Company
    Contributed by David Mosberger-Tang <davidm@hpl.hp.com>

    This file is part of httperf, a web server performance measurment
    tool.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <httperf.h>
#include <event.h>
#include <call.h>

#define MAX_NUM_OPS	4

static const char * const event_name[EV_NUM_EVENT_TYPES] =
  {
    "EV_PERF_SAMPLE",
    "EV_HOSTNAME_LOOKUP_START",
    "EV_HOSTNAME_LOOKUP_STOP",
    "EV_SESS_NEW",
    "EV_SESS_FAILED",
    "EV_SESS_DESTROYED",
    "EV_CONN_NEW",
    "EV_CONN_CONNECTING",
    "EV_CONN_CONNECTED",
    "EV_CONN_CLOSE",
    "EV_CONN_DESTROYED",
    "EV_CONN_FAILED",
    "EV_CONN_TIMEOUT",
    "EV_CALL_NEW",
    "EV_CALL_ISSUE",
    "EV_CALL_SEND_START",
    "EV_CALL_SEND_RAW_DATA",
    "EV_CALL_SEND_STOP",
    "EV_CALL_RECV_START",
    "EV_CALL_RECV_HDR",
    "EV_CALL_RECV_RAW_DATA",
    "EV_CALL_RECV_DATA",
    "EV_CALL_RECV_FOOTER",
    "EV_CALL_RECV_STOP",
    "EV_CALL_DESTROYED",
    "EV_DUMP_STATS"
  };

typedef struct Event_Action
  {
    int num_ops;
    struct closure
      {
	Event_Handler op;
	Any_Type arg;
      }
    closure[MAX_NUM_OPS];
  }
Event_Action;

static Event_Action action[EV_NUM_EVENT_TYPES] = {{0, }};

void
event_register_handler (Event_Type et, Event_Handler handler, Any_Type arg)
{
  struct closure *c;
  int n;

  n = action[et].num_ops;
  if (n >= MAX_NUM_OPS)
    {
      fprintf (stderr, "event_register_handler: sorry, attempted to register "
	       "more than %d handlers\n", MAX_NUM_OPS);
      exit (1);
    }
  c = action[et].closure + n;
  c->op = handler;
  c->arg = arg;
  action[et].num_ops = n + 1;
}

void
event_signal (Event_Type type, Object *obj, Any_Type arg)
{
  Event_Action *act = action + type;
  struct closure *c, *end;

  if (DBG > 1)
    {
      assert (NELEMS (event_name) == EV_NUM_EVENT_TYPES);
      fprintf (stderr, "event_signal: %s (obj=%p,arg=%lx)\n",
	       event_name[type], obj, arg.l);
      if (obj) {
        if (obj->type == OBJ_CALL) {
  	      Call *c = (Call *) obj;
  	      fprintf(stderr, "event_signal:    current time %22.8lf timeout    %22.8lf\n",
		      timer_now_forced(), c->timeout); 
  	      fprintf(stderr, "event_signal:    send_start   %22.8lf recv_start %22.8lf\n", 
		      c->basic.time_send_start, c->basic.time_recv_start);
        }
      }
    }

  end = &act->closure[act->num_ops];
  for (c = &act->closure[0]; c < end; ++c)
    (*c->op) (type, obj, c->arg, arg);
}
