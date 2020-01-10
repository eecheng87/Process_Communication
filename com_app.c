#include "com_app.h"

int main(int argc, char *argv[])
{

    char reg_msg[30] = "Registration. id=";
    sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(sock_fd<0)
        return -1;
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    //memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    memset(&msg, 0, sizeof(msg));
    // processing registration console
    strcat(reg_msg,argv[1]);
    strcat(reg_msg,", type=");
    strcat(reg_msg,argv[2]);

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), reg_msg);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&(dest_addr);
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;




    sendmsg(sock_fd,&msg,0);
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    recvmsg(sock_fd,&msg,0);
    // ack message after registration
    printf("%s\n",(char*)NLMSG_DATA(nlh));
    if(strcmp((char*)NLMSG_DATA(nlh),"Fail")==0)return 0;




    char cmdline[300];
    char *pc;
    while(1)
    {
        char*p;
        //scanf("%[^\n]s",cmdline);
        fgets(cmdline,300,stdin);
        if(p=strchr(cmdline,'\n'))
        {
            *p = 0;
        }
        else
        {
            scanf("%*[^\n]");
            scanf("%*c");
        }
        //printf("HI: %s\n",cmdline);
        /*
            critical!!!
            maybe cmdline should remove '\n' -> check later
        */
        pc = strtok(cmdline,"\n");
        if(strcmp(pc,"Recv")==0)
        {
            strcat(cmdline," ");
            strcat(cmdline,argv[1]);
        }

        nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();
        nlh->nlmsg_flags = 0;
        strcpy(NLMSG_DATA(nlh), cmdline);
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&(dest_addr);
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        sendmsg(sock_fd,&msg,0);
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        recvmsg(sock_fd,&msg,0);
        printf("%s\n",(char*)NLMSG_DATA(nlh));
        free(nlh);
    }
    close(sock_fd);
    return 0;
}