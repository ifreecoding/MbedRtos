
#include "main.h"


S32 gsocket_fd;                     /* socket句柄 */
U32 gsocket_status;                 /* socket连接状态 */

pthread_t grx_thread;               /* rx_thread线程pid */
pthread_t ghandle_thread;           /* handle_thread线程pid */
pthread_t gtx_thread;               /* tx_thread线程pid */

QUE grx_que;                        /* 消息接收队列 */
QUE gtx_que;                        /* 消息发送队列 */

MSG_BUF grx_buf;                    /* 消息接收缓冲 */
MSG_BUF gtx_buf;                    /* 消息发送缓冲 */


/***********************************************************************************
函数功能: 主函数.
入口参数: argc: 命令行中字符串的数量.
          argv: char*型指针数组，分别指向命令行中各个字符串.
返 回 值: RTN_SUCD: 成功.
          RTN_FAIL: 失败.
***********************************************************************************/
int main(int argc, char* argv[])
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    U8* pserver_ip;
    U32 server_port;
    U32 client_port;

    /* 入口参数检查 */
    if(4 != argc)
    {
        printf("please input 3 papameters: server ip, server port, client port.\n");

        return RTN_FAIL;
    }

    /* 获取参数 */
    pserver_ip = argv[1];
    server_port = atoi(argv[2]);
    client_port = atoi(argv[3]);

    /* 程序开始运行 */
    printf("linux program is running!\n");

    /* socket连接前置为未连接状态 */
    gsocket_status = SOCKET_DOWN;

    /* 初始化接收队列 */
    queue_init(&grx_que);

    /* 初始化发送队列 */
    queue_init(&gtx_que);

    /* 创建rx_thread线程 */
    if(0 != pthread_create(&grx_thread, NULL, rx_thread, NULL))
    {
        printf("create rx_thread failed!\n");
    }

    /* 创建handle_thread线程 */
    if(0 != pthread_create(&ghandle_thread, NULL, handle_thread, NULL))
    {
        printf("create handle_thread failed!\n");
    }

    /* 创建tx_thread线程 */
    if(0 != pthread_create(&gtx_thread, NULL, tx_thread, NULL))
    {
        printf("create tx_thread failed!\n");
    }

    /* 检查socket链接 */
    while(1)
    {
        /* socket链接断开则重新连接 */
        if(SOCKET_DOWN == gsocket_status)
        {
            /* 建立TCP连接 */
            socket_tcp_connect(pserver_ip, server_port, client_port);
        }

        /* 延迟1s后再尝试 */
        sleep(1);
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 建立socket tcp连接.
入口参数: pserver_ip: 存放服务器IP的缓冲地址.
          server_port: 服务器端口号.
          client_port: 客户端端口号.
返 回 值: RTN_SUCD: 连接成功.
          RTN_FAIL: 连接失败.
***********************************************************************************/
U32 socket_tcp_connect(U8* pserver_ip, U32 server_port, U32 client_port)
{
    struct sockaddr_in serv_addr;
    struct sockaddr_in clin_addr;
    S32 opt_val;

    /* 创建socket, 使用TCP协议 */
    gsocket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == gsocket_fd)
    {
        printf("create socket tcp error!\n");

        return RTN_FAIL;
    }

    /* 设置socket选项, 规避无法重新建立socket的bug */
    opt_val = 1;
    if(0 != setsockopt(gsocket_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, sizeof(opt_val)))
    {
        printf("setsockopt error!\n");

        goto RTN;
    }

    /* 设置客户端参数, 绑定本机参数 */
    memset(&clin_addr, 0, sizeof(clin_addr));
    clin_addr.sin_family = AF_INET;                     /* IP协议族 */
    clin_addr.sin_port = htons(client_port);            /* 客户端端口号 */
    clin_addr.sin_addr.s_addr = htonl(INADDR_ANY);      /* 客户端IP */

    /* 绑定客户端参数 */
    if(0 == bind(gsocket_fd, (struct sockaddr*)&clin_addr, sizeof(clin_addr)))
    {
        printf("bind socket tcp successfully!\n");
    }
    else
    {
        printf("bind socket tcp error!\n");

        goto RTN;
    }

    /* 设置服务器参数, 连接服务器 */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                     /* IP协议族 */
    serv_addr.sin_port = htons(server_port);            /* 服务器端口号 */
    serv_addr.sin_addr.s_addr = inet_addr(pserver_ip);  /* 服务器IP */

    /* 连接服务器端 */
    if(0 == connect(gsocket_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)))
    {
        printf("socket tcp connect successfully!\n");
    }
    else
    {
        printf("socket tcp connect error!\n");

        goto RTN;
    }

    /* socket连接后置为已连接状态 */
    gsocket_status = SOCKET_UP;

    return RTN_SUCD;

RTN:

    /* 关闭socket连接 */
    close(gsocket_fd);

    return RTN_FAIL;
}

/***********************************************************************************
函数功能: 消息接收线程.
入口参数: pattr: 线程参数指针.
返 回 值: 函数返回值.
***********************************************************************************/
void* rx_thread(void* pattr)
{
    while(1)
    {
        /* 接收数据 */
        msg_rx();
    }
}

/***********************************************************************************
函数功能: 消息处理线程.
入口参数: pattr: 线程参数指针.
返 回 值: 函数返回值.
***********************************************************************************/
void* handle_thread(void* pattr)
{
    while(1)
    {
        /* 处理消息 */
        msg_process();
    }
}

/***********************************************************************************
函数功能: 消息发送线程.
入口参数: pattr: 线程参数指针.
返 回 值: 函数返回值.
***********************************************************************************/
void* tx_thread(void* pattr)
{
    while(1)
    {
        /* 发送数据 */
        msg_tx();
    }
}

/***********************************************************************************
函数功能: 接收消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx(void)
{
    ssize_t rtn;

    /* 处于已连接状态则接收数据 */
    if(SOCKET_UP == gsocket_status)
    {
        /* 接收数据, 如果没有数据, 该线程会被阻塞在这里 */
        rtn = recv(gsocket_fd, grx_buf.buf, BUF_LEN, 0);

        /* 接收错误 */
        if(-1 == rtn)
        {
            printf("socket_receive error!\n");

            /* 关闭socket连接 */
            close(gsocket_fd);

            /* socket置为未创建状态 */
            gsocket_status = SOCKET_DOWN;

            return;
        }
        else if(0 == rtn) /* 与服务器端连接断开, 会接收到0字节数据 */
        {
            printf("receive 0 byte data! break from host!\n");

            /* 关闭socket连接 */
            close(gsocket_fd);

            /* socket置为未创建状态 */
            gsocket_status = SOCKET_DOWN;

            return;
        }

        /* 填充接收数据的长度 */
        grx_buf.len = rtn;

        printf("socket_receive %d bytes data from host successfully!\n", rtn);

        /* 将接收到的消息放入消息接收队列 */
        send_msg_to_que(&grx_que, &grx_buf.list);

        return;
    }
    else /* 处于未连接状态则延迟1s */
    {
        sleep(1);
    }
}

/***********************************************************************************
函数功能: 处理消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_process(void)
{
    DLIST* pbuf;

    /* 从消息接收队列获取接收到的消息 */
    pbuf = receive_msg_from_que(&grx_que);
    if(NULL == pbuf)
    {
        return;
    }

    /* 将接收缓冲内的消息复制到发送缓冲内 */
    memcpy(gtx_buf.buf, grx_buf.buf, grx_buf.len);
    gtx_buf.len = grx_buf.len;

    /* 将需要发送的消息放入消息发送队列 */
    send_msg_to_que(&gtx_que, &gtx_buf.list);
}

/***********************************************************************************
函数功能: 发送消息, 将消息发给主机.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx(void)
{
    MSG_BUF* pbuf;
    ssize_t rtn;
    U32 send_len;

    /* 从消息发送队列获取需要发送的消息 */
    pbuf = (MSG_BUF*)receive_msg_from_que(&gtx_que);
    if(NULL == pbuf)
    {
        return;
    }

    send_len = 0;

    /* 处于已连接状态则发送数据 */
    if(SOCKET_UP == gsocket_status)
    {
        /* 循环发送数据, 直至数据发送完毕 */
        while(1)
        {
            /* 发送数据 */
            rtn = send(gsocket_fd, pbuf->buf + send_len, gtx_buf.len - send_len, 0);

            /* 发送错误 */
            if(-1 == rtn)
            {
                printf("send error! data length is %d!\n", gtx_buf.len);

                /* 关闭socket连接 */
                close(gsocket_fd);

                /* socket置为未创建状态 */
                gsocket_status = SOCKET_DOWN;

                return;
            }

            /* 已发送的数据长度 */
            send_len += rtn;

            /* 数据发送完毕 */
            if(gtx_buf.len == send_len)
            {
                printf("send %d bytes data to host successfully!\n", gtx_buf.len);

                return;
            }
            /* 数据没有发送完毕, 继续发送 */
            else if(send_len < gtx_buf.len)
            {
                printf("send part of data! %d/%d already send!\n", send_len,
                       gtx_buf.len);
            }
        }
    }
}

/***********************************************************************************
函数功能: 向队列发送消息.
入口参数: pque: 队列指针.
          plist: 加入队列的消息指针.
返 回 值: none.
***********************************************************************************/
void send_msg_to_que(QUE* pque, DLIST* plist)
{
    printf("send message to queue!\n");

    /* 将消息放入队列 */
    queue_put(pque, plist);

    /* 释放信号量, 激活从队列中获取消息的线程 */
    if(0 != sem_post(&pque->sem_cnt))
    {
        printf("post sem failed!\n");

        return;
    }
}

/***********************************************************************************
函数功能: 从队列接收消息.
入口参数: pque: 队列指针.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
DLIST* receive_msg_from_que(QUE* pque)
{
    DLIST* pbuf;
    U32 rtn;

    /* 若能获取到计数信号量则说明队列中有消息, 开始处理消息 */
    if(0 != sem_wait(&pque->sem_cnt))
    {
        printf("wait sem failed!\n");

        return NULL;
    }

    /* 获取到计数信号量, 说明队列中有消息, 从队列中获取消息 */
    rtn = queue_get(pque, &pbuf);
    if(RTN_SUCD != rtn)
    {
        printf("get message error %d!\n", rtn);

        pbuf = NULL;
    }

    printf("get message from queue!\n");

    return pbuf;
}

/***********************************************************************************
函数功能: 初始化双向链表.
入口参数: plist: 链表根节点指针.
返 回 值: none.
***********************************************************************************/
void dlist_init(DLIST* plist)
{
    /* 空链表的头尾都指向空节点 */
    plist->head = (DLIST*)NULL;
    plist->tail = (DLIST*)NULL;
}

/***********************************************************************************
函数功能: 向双向链表添加一个节点, 从链表尾部加入.
入口参数: plist: 链表根节点指针.
          pnode: 加入的节点指针.
返 回 值: none.
***********************************************************************************/
void dlist_node_add(DLIST* plist, DLIST* pnode)
{
    /* 链表非空 */
    if(NULL != plist->tail)
    {
        /* 新节点的头指向原尾节点 */
        pnode->head = plist->head;

        /* 新节点的尾指向根节点 */
        pnode->tail = plist;

        /* 原尾节点的尾指向新节点 */
        plist->head->tail = pnode;

        /* 根节点的头指向新加入的节点 */
        plist->head = pnode;
    }
    else /* 链表为空 */
    {
        /* 新节点的头尾都指向根节点 */
        pnode->head = plist;
        pnode->tail = plist;

        /* 根节点的头尾都指向新节点 */
        plist->head = pnode;
        plist->tail = pnode;
    }
}

/***********************************************************************************
函数功能: 从双向链表删除一个节点, 从链表头部删除.
入口参数: plist: 链表根节点指针.
返 回 值: 删除的节点指针, 若链表为空则返回NULL.
***********************************************************************************/
DLIST* dlist_node_delete(DLIST* plist)
{
    DLIST* ptemp_node;

    /* 链表中的第一个节点 */
    ptemp_node = plist->tail;

    /* 链表非空 */
    if(NULL != ptemp_node)
    {
        /* 链表中有多个节点 */
        if(plist->head != plist->tail)
        {
            /* 根节点的尾指向第二个节点 */
            plist->tail = ptemp_node->tail;

            /* 第二个节点的头指向根节点 */
            ptemp_node->tail->head = plist;
        }
        else /* 链表中只有一个节点 */
        {
            /* 取出节点后链表为空 */
            plist->head = (DLIST*)NULL;
            plist->tail = (DLIST*)NULL;
        }

        /* 返回取出的节点指针 */
        return ptemp_node;
    }
    else /* 链表为空返回NULL */
    {
        return (DLIST*)NULL;
    }
}

/***********************************************************************************
函数功能: 初始化队列.
入口参数: pque: 队列指针.
返 回 值: none.
***********************************************************************************/
void queue_init(QUE* pque)
{
    /* 初始化队列互斥信号量 */
    if(0 != sem_init(&pque->sem_mut, 0, 1))
    {
        printf("queue sem_mut init failed!\n");
    }

    /* 初始化队列计数信号量 */
    if(0 != sem_init(&pque->sem_cnt, 0, 0))
    {
        printf("queue sem_cnt init failed!\n");
    }

    /* 初始化队列根节点 */
    dlist_init(&pque->list);
}

/***********************************************************************************
函数功能: 将一个节点加入队列.
入口参数: pque: 队列指针.
          pque_node: 要加入的队列节点指针.
返 回 值: RTN_SUCD: 将节点加入队列成功.
          RTN_FAIL: 将节点加入队列失败.
***********************************************************************************/
U32 queue_put(QUE* pque, DLIST* pque_node)
{
    /* 入口参数检查 */
    if((NULL == pque) || (NULL == pque_node))
    {
        return RTN_FAIL;
    }

    /* 获取信号量, 保证链表操作的串行性 */
    if(0 != sem_wait(&pque->sem_mut))
    {
        printf("wait sem failed!\n");

        return RTN_FAIL;
    }

    /* 将节点加入队列 */
    dlist_node_add(&pque->list, pque_node);

    /* 释放信号量 */
    if(0 != sem_post(&pque->sem_mut))
    {
        printf("post sem failed!\n");

        return RTN_FAIL;
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 从队列中取出一个队列节点.
入口参数: pque: 队列根节点指针.
          pque_node: 存放队列节点指针的指针.
返 回 值: RTN_SUCD: 从队列取出节点成功.
          RTN_FAIL: 从队列取出节点失败.
          QUE_RTN_NULL: 队列为空, 无法取出节点.
***********************************************************************************/
U32 queue_get(QUE* pque, DLIST** pque_node)
{
    DLIST* pnode;

    /* 入口参数检查 */
    if((NULL == pque) || (NULL == pque_node))
    {
        return RTN_FAIL;
    }

    /* 获取信号量, 保证链表操作的串行性 */
    if(0 != sem_wait(&pque->sem_mut))
    {
        printf("wait sem failed!\n");

        return RTN_FAIL;
    }

    /* 从队列取出节点 */
    pnode = dlist_node_delete(&pque->list);

    /* 释放信号量 */
    if(0 != sem_post(&pque->sem_mut))
    {
        printf("post sem failed!\n");

        return RTN_FAIL;
    }

    /* 队列不为空, 可以取出节点 */
    if(NULL != pnode)
    {
        *pque_node = pnode;

        return RTN_SUCD;
    }
    else /* 队列为空, 无法取出节点 */
    {
        printf("the queue is empty!\n");

        *pque_node = NULL;

        return QUE_RTN_NULL;
    }
}

