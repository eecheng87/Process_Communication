# Process_Communication
 Implement transfer information between the kernel and the user space processes



## Overview 
![](https://i.imgur.com/GHl4VBK.png)

## Behavior
- Several com_apps send registration
request to register with kernel
module via the socket
- After receiving registration request,
com_kmodule creates a mailbox for
each com_app and returns ack
message
- Mailboxes are used to manage
message data from com_apps
- com_app can communicate with
each other through com_kmodule

## Two mode

- Queued

    - It is like that there is a FIFO queue in kernel module. When
the queue is empty, the module does not provide any message
data to the application. When the queue is not empty and the
application wants to receive the message data, then the module
provides the application with the oldest message data and
removes this message data from the queue.
    - If new message data arrives in kernel module and the queue is
not full, this new message data is stored in the queue. If new
message data arrives and queue is full, this message data is
lost.(Our homework can store at most 3 message data)

- Unqueued

    - Unqueued application does not use the FIFO mechanism. The
application does not consume the message data in kernel
module during reception of message data – instead, a message
data may be read multiple times by an application.
    - If no message data is sent for the application after the mailbox
is created, the module does not provide any message data to
the application.
    - Message data is overwritten by newly arrived message data.
    
    
## How to build
- `make` in topest folder
- `make ins` under kernel folder
- `make rm` for remove kernel

## Correctness

- pass all testbench from `OSLab`

## Input example

- Execute program:

`./com_app [mail_number] [mode]`

- Receive message

`Recv [id]`

- Send message

`Send [id] [message data]`
