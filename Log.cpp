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

#include <stdio.h>
#include <string.h>
#include <sys/process.h>
#include <sys/paths.h>
#include <cell/cell_fs.h>
#include <cell/sysmodule.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>


unsigned int log_counter = 0;
int log_fd = -1;
int socket_fd = -1;
struct sockaddr_in address;

#define BUF_LEN 1024

void log_init_file(const char *filename) {
  (void) cellSysmoduleInitialize();

  if (cellSysmoduleLoadModule(CELL_SYSMODULE_FS)) {
    sys_process_exit(1);
  }

//  if (!isMounted(MOUNT_POINT)) {
//     sys_process_exit(1);
//  }

  CellFsErrno err = cellFsOpen(filename, CELL_FS_O_RDWR|CELL_FS_O_CREAT, &log_fd, NULL, 0);
  if(err != CELL_FS_SUCCEEDED) {
    log_fd = -1;
    return;
  }

  log_file("%s: log file '%s' opened\n", __func__, filename);
}

void log_init_net(const char *host, unsigned short port)
{
  unsigned int temp;
  struct hostent *hp;
  int ret;

  log_file("%s: calling cellSysmoduleLoadModule(CELL_SYSMODULE_NET)\n", __func__);
  ret = cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
  if (ret < 0) {
    log_file("%s: WARNING, call to cellSysmoduleLoadModule() failed (%d)\n", __func__, ret);
  }

  log_file("%s: calling sys_net_initialize_network\n", __func__);
  ret = sys_net_initialize_network();
  if (ret < 0) {
    log_file("%s: WARNING, sys_net_initialize_network() failed (%d)\n", __func__, ret);
  }

#if 0
  log_file("%s: calling cellNetCtlInit\n", __func__);
  ret = cellNetCtlInit();
  if (ret < 0) {
    log_file("%s: WARNING, call to cellNetCtlInit() failed (%x)\n", __func__, ret);
  }

  int state;
  log_file("%s: calling cellNetCtlGetState\n", __func__);
  ret = cellNetCtlGetState(&state);
  if (ret < 0) {
    log_file("%s: WARNING, call to cellNetCtlGetState() failed (%x)\n", __func__, ret);
  } else {
    if (state == CELL_NET_CTL_STATE_IPObtained) {
      log_file("%s: Obtained IP!\n");
    } else {
      log_file("%s: WARNING, IP not obtained\n");
    }
  }
#endif
  
  memset(&address, 0, sizeof(address));
  
  log_file("%s: inet_addr\n", __func__);
  if ((unsigned int)-1 != (temp = inet_addr(host))) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = temp;
  }
  else {
    log_file("%s: gethostbyname\n", __func__);
    if (NULL == (hp = gethostbyname(host))) {
      log_file("%s: unknown host '%s', errno: %d\n", __func__, host, sys_net_h_errno);
      return;
    }
    address.sin_family = hp->h_addrtype;
    memcpy(&address.sin_addr, hp->h_addr, hp->h_length);
  }

  address.sin_port = htons(port);

  log_file("%s: calling socket()\n", __func__);
  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    log_file("%s: call to socket() failed, errno: %d\n", __func__, sys_net_errno);
    return;
  }

  log_file("%s: created udp socket, address setup towards %s:%d\n", __func__, inet_ntoa(address.sin_addr), port);
  log_net ("%s: created udp socket, address setup towards %s:%d\n", __func__, inet_ntoa(address.sin_addr), port);
}

void log_close() {
  if(log_fd != -1) {
    (void) cellFsClose(log_fd);
  }
}

void log_file(const char *format, ...) {
  if(log_fd == -1) {
    return;
  }
  char buf[BUF_LEN];
  int max = sizeof(buf);
  uint64_t sw;
  va_list va;
  va_start(va, format);
  int written = vsnprintf(buf, max, format, va);
  if (written > max) {
    written = max;
  }
  va_end(va);
  (void) cellFsWrite(log_fd, (const void *)buf, (uint64_t)written, &sw);
  (void) cellFsFsync(log_fd); // really slow, but just to be sure it's there in case of crash
}

void log_net(const char *format, ...) {
  if(socket_fd == -1) {
    log_file("%s: cannot log to net, socket not initialized\n", __func__);
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
  
  int ret = sendto(socket_fd, buf, written, 0, (struct sockaddr *)&address, sizeof(address));

  if (ret < 0) {
    log_file("%s: sendto() failed, errno: %d\n", __func__, sys_net_errno);
    return;
  }

}
