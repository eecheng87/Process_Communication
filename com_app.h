#ifndef COM_APP_H
#define COM_APP_H
#include <stddef.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_PAYLOAD 1024  /* maximum payload size*/
#define NETLINK_USER 17
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
#endif  //ifndef COM_APP_H
