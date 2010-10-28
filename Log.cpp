// Copyright 2010 apanloco
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Log.h"

#include <cell/sysmodule.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netex/net.h>
#include <netdb.h>
#include <stdio.h>

#ifdef ENABLE_LOG
unsigned int log_counter = 0;
int socket_fd = -1;
struct sockaddr_in address;
#endif

#define BUF_LEN 1024

void log_init(const char *host, unsigned short port)
{
#ifdef ENABLE_LOG
  unsigned int temp;
  struct hostent *hp;

  (void) cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
  (void) sys_net_initialize_network();
  
  memset(&address, 0, sizeof(address));
  
  if ((unsigned int)-1 != (temp = inet_addr(host))) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = temp;
  }
  else {
    if (NULL == (hp = gethostbyname(host))) {
      return;
    }
    address.sin_family = hp->h_addrtype;
    memcpy(&address.sin_addr, hp->h_addr, hp->h_length);
  }

  address.sin_port = htons(port);

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    return;
  }

  log("%s: created udp socket, address setup towards %s:%d\n", __func__, inet_ntoa(address.sin_addr), port);
#endif
}

void log_close() {
#ifdef ENABLE_LOG
  if(socket_fd != -1) {
    socketclose(socket_fd);
  }
#endif
}

void log(const char *format, ...) {
#ifdef ENABLE_LOG
  if(socket_fd == -1) {
    return;
  }

  char buf[BUF_LEN];
  int max = sizeof(buf);

  va_list va;
  va_start(va, format);

  int written = vsnprintf(buf, max, format, va);

  if (written > max) {
    written = max;
  }

  va_end(va);
  
  (void) sendto(socket_fd, buf, written, 0, (struct sockaddr *)&address, sizeof(address));
#endif
}
