#ifndef COM_KMODULE_H
#define COM_KMODULE_H

#include <linux/module.h>
#include <stddef.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <linux/init.h>
# include <linux/sched.h>
# include <linux/netlink.h>
# include <net/sock.h>
# include <net/net_namespace.h>
#define NETLINK_TEST 17
static void nl_recv_msg (struct sk_buff *skb);
char* kstrtok(char *s1, const char *delimit);
int katoi(char *str);
static struct sock *socketptr = NULL;

struct netlink_kernel_cfg cfg =
{
    .input = nl_recv_msg,
};
struct mailbox
{
    //0: unqueued
    //1: queued
    unsigned char type;
    int msg_data_count;
    struct msg_data *msg_data_head;
    struct msg_data *msg_data_tail;
};

struct msg_data
{
    char buf[256];
    struct msg_data *next;
};
struct mailbox MB[1001];
static int isReg[1001] = {0}; // 0 means unregist, otherwise means registed

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Apple pie");
MODULE_DESCRIPTION("A Simple Hello World module");

#endif  //ifndef COM_KMODULE_H
