#include "com_kmodule.h"

static int __init com_kmodule_init(void)
{
    printk(KERN_INFO "!!Enter module. Hello world!\n");
    socketptr = netlink_kernel_create(&init_net,NETLINK_TEST, &cfg);
    return 0;
}
static void nl_recv_msg (struct sk_buff *skb)
{
    int pid, reg_id=0;
    struct nlmsghdr *nlh = NULL;
    char msg_tmp[300];
    char msg_cpy[300]; // copy for msg_tmp
    char ret_msg[20]; // fail or success
    struct sk_buff *skb_out;
    if(skb == NULL)
    {
        printk("skb is NULL \n");
        return ;
    }
    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;
    printk(KERN_INFO "%s\n",(char*)NLMSG_DATA(nlh));
    char *pch;
    strcpy(msg_tmp,(char*)NLMSG_DATA(nlh));
    strcpy(msg_cpy,msg_tmp);
    pch = kstrtok(msg_tmp,"=");
    if(strcmp(pch,"Registration. id")==0)
    {
        // Regist request
        printk(KERN_INFO "msg_tmp: %s\n",msg_tmp);
        pch = kstrtok(NULL,","); // id
        printk(KERN_INFO "pch: %s\n",pch);
        reg_id = katoi(pch);
        printk(KERN_INFO "katoi: %d\n",reg_id);
        pch = kstrtok(NULL,"=");
        pch = kstrtok(NULL,""); // type
        printk(KERN_INFO "pch: %s\n",pch);

        // filling info
        if(isReg[reg_id]==0)
        {
            // unregisted
            isReg[reg_id] = 1;
            if(strcmp(pch,"queued")==0)
            {
                // type is queued
                MB[reg_id].type = 1;
            }
            else
            {
                // type is unqueued
                MB[reg_id].type = 0;
            }
            MB[reg_id].msg_data_count = 0;
            // success
            strcpy(ret_msg,"Success");
        }
        else
        {
            // already registered, return fail
            strcpy(ret_msg,"Fail");
        }

        int msg_size = strlen(ret_msg);
        skb_out = nlmsg_new(msg_size,0);
        nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);
        NETLINK_CB(skb_out).dst_group = 0;
        strncpy(nlmsg_data(nlh),ret_msg,msg_size);
        int res = nlmsg_unicast(socketptr,skb_out,pid);
        return;
    }

    // check `Send` identifier
    strcpy(msg_tmp,msg_cpy);
    //printk(KERN_INFO "CHECK %s\n",msg_tmp);
    pch = kstrtok(msg_tmp," ");
    if(strcmp(pch,"Send")==0)
    {
        /*
            send_state = 0 means sending data fail, e.g. data > 256 or full or
            unregiser
        */
        int send_mail_id, send_state;
        char send_mail_data[256];
        pch = kstrtok(NULL," ");
        send_mail_id = katoi(pch);
        pch = kstrtok(NULL,"");

        if(strlen(pch)>255||isReg[send_mail_id]==0||MB[send_mail_id].msg_data_count>=3)
        {
            // sending fail
            send_state = 0;
        }
        else
        {
            // sending success
            send_state = 1;
            strcpy(send_mail_data,pch);
            if(MB[send_mail_id].type==0)
            {
                // unqueued
                MB[send_mail_id].msg_data_head = (struct msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                MB[send_mail_id].msg_data_count = 1;
                strcpy(MB[send_mail_id].msg_data_head->buf,send_mail_data);
            }
            else
            {
                // queued
                if(MB[send_mail_id].msg_data_count==0)
                {
                    MB[send_mail_id].msg_data_head = (struct msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                    strcpy(MB[send_mail_id].msg_data_head->buf,send_mail_data);
                    MB[send_mail_id].msg_data_tail = MB[send_mail_id].msg_data_head;
                }
                else
                {
                    MB[send_mail_id].msg_data_tail->next = (struct msg_data*)kmalloc(sizeof(struct msg_data),GFP_KERNEL);
                    MB[send_mail_id].msg_data_tail = MB[send_mail_id].msg_data_tail->next;
                    strcpy(MB[send_mail_id].msg_data_tail->buf,send_mail_data);
                }
                MB[send_mail_id].msg_data_count += 1;
            }
        }


        if(send_state==0)
        {
            // sending fail
            strcpy(ret_msg,"Fail");
        }
        else
        {
            // sending success
            strcpy(ret_msg,"Success");
        }
        int msg_size = strlen(ret_msg);
        skb_out = nlmsg_new(msg_size,0);
        nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);
        NETLINK_CB(skb_out).dst_group = 0;
        strncpy(nlmsg_data(nlh),ret_msg,msg_size);
        int res = nlmsg_unicast(socketptr,skb_out,pid);
        return;
    }

    // check `Recv` identifier
    strcpy(msg_tmp,msg_cpy);
    pch = kstrtok(msg_tmp," ");
    if(strcmp(pch,"Recv")==0)
    {
        int recv_mail_id;
        char recv_ret_msg[300];
        pch = kstrtok(NULL,"");
        recv_mail_id = katoi(pch);
        if(recv_mail_id>1000||recv_mail_id<1||isReg[recv_mail_id]==0||MB[recv_mail_id].msg_data_count==0)
        {
            // recv fail
            // id is not exist , no message data to recv
            strcpy(recv_ret_msg,"Fail");
        }
        else
        {
            if(MB[recv_mail_id].type==0)
            {
                // unqueued
                strcpy(recv_ret_msg,MB[recv_mail_id].msg_data_head->buf);
            }
            else
            {
                // queued
                struct msg_data *rcv_tmp = MB[recv_mail_id].msg_data_head;
                if(MB[recv_mail_id].msg_data_count==1)
                {
                    MB[recv_mail_id].msg_data_head = NULL;
                    MB[recv_mail_id].msg_data_tail = NULL;
                }
                else
                {
                    MB[recv_mail_id].msg_data_head = MB[recv_mail_id].msg_data_head->next;
                }
                strcpy(recv_ret_msg,rcv_tmp->buf);
                MB[recv_mail_id].msg_data_count-=1;
                kfree(rcv_tmp);
            }
        }
        int rcvmsg_size = strlen(recv_ret_msg);
        skb_out = nlmsg_new(rcvmsg_size,0);
        nlh = nlmsg_put(skb_out,0,0,NLMSG_DONE,rcvmsg_size,0);
        NETLINK_CB(skb_out).dst_group = 0;
        strncpy(nlmsg_data(nlh),recv_ret_msg,rcvmsg_size);
        int res = nlmsg_unicast(socketptr,skb_out,pid);
    }
    return;
}
int katoi(char *str)
{
    int ans=0;
    int i;
    for(i = 0; i<strlen(str); ++i)
    {
        ans *= 10;
        ans += str[i]-'0';
    }

    return ans;
}
char* kstrtok(char *s1, const char *delimit)
{
    static char *lastToken = NULL; /* UNSAFE SHARED STATE! */
    char *tmp;

    /* Skip leading delimiters if new string. */
    if ( s1 == NULL )
    {
        s1 = lastToken;
        if (s1 == NULL)         /* End of story? */
            return NULL;
    }
    else
    {
        s1 += strspn(s1, delimit);
    }

    /* Find end of segment */
    tmp = strpbrk(s1, delimit);
    if (tmp)
    {
        /* Found another delimiter, split string and save state. */
        *tmp = '\0';
        lastToken = tmp + 1;
    }
    else
    {
        /* Last segment, remember that. */
        lastToken = NULL;
    }

    return s1;
}

static void __exit com_kmodule_exit(void)
{
    printk(KERN_INFO "Exit module. Bye~\n");
    sock_release(socketptr->sk_socket);
}



module_init(com_kmodule_init);
module_exit(com_kmodule_exit);
