// w_socket_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "main.h"


SOCKET gsocket_fd;                  /* socket��� */
U32 gsocket_status;                 /* socket����״̬ */

QUE grx_que;                        /* ��Ϣ���ն��� */
QUE gtx_que;                        /* ��Ϣ���Ͷ��� */

MSG_BUF grx_buf;                    /* ��Ϣ���ջ��� */
MSG_BUF gtx_buf;                    /* ��Ϣ���ͻ��� */


/***********************************************************************************
��������: ������.
��ڲ���: argc: ���������ַ���������.
          argv: _TCHAR*��ָ�����飬�ֱ�ָ���������и����ַ���.
�� �� ֵ: RTN_SUCD: �ɹ�.
          RTN_FAIL: ʧ��.
***********************************************************************************/
int _tmain(int argc, _TCHAR* argv[])
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    char* pserver_ip;
    U32 server_port;
    U32 client_port;

    /* ��ڲ������ */
    if(4 != argc)
    {
        printf("please input 3 papameters: server ip, server port, client port.\n");

        return RTN_FAIL;
    }

    /* ��ȡ���� */
#define STRING_BUF_LEN  16
    char string_buf[STRING_BUF_LEN];

    /* �����ַ���ת��Ϊխ�ַ��� */
    if(-1 == wcstombs(string_buf, argv[1], STRING_BUF_LEN))
    {
        printf("convert ip failed!\n");

        return RTN_FAIL;
    }

    pserver_ip = string_buf;
    server_port = wcstol(argv[2], NULL, 10);
    client_port = wcstol(argv[3], NULL, 10);

    /* ����ʼ���� */
    printf("windows program is running!\n");

    /* socket����ǰ��Ϊδ����״̬ */
    gsocket_status = SOCKET_DOWN;

    /* ��ʼ�����ն��� */
    queue_init(&grx_que);

    /* ��ʼ�����Ͷ��� */
    queue_init(&gtx_que);

    /* ����rx_thread�߳� */
    if(0 == CreateThread(NULL, 0, rx_thread, NULL, 0, NULL))
    {
        printf("create rx_thread failed!\n");
    }

    /* ����handle_thread�߳� */
    if(0 == CreateThread(NULL, 0, handle_thread, NULL, 0, NULL))
    {
        printf("create handle_thread failed!\n");
    }

    /* ����tx_thread�߳� */
    if(0 == CreateThread(NULL, 0, tx_thread, NULL, 0, NULL))
    {
        printf("create tx_thread failed!\n");
    }

    /* ���socket���� */
    while(1)
    {
        /* socket���ӶϿ����������� */
        if(SOCKET_DOWN == gsocket_status)
        {
            /* ����TCP���� */
            socket_tcp_connect(pserver_ip, server_port, client_port);
        }

        /* �ӳ�1s���ٳ��� */
        Sleep(1000);
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: ����socket tcp����.
��ڲ���: pserver_ip: ��ŷ�����IP�Ļ����ַ.
          server_port: �������˿ں�.
          client_port: �ͻ��˶˿ں�.
�� �� ֵ: RTN_SUCD: ���ӳɹ�.
          RTN_FAIL: ����ʧ��.
***********************************************************************************/
U32 socket_tcp_connect(char* pserver_ip, U32 server_port, U32 client_port)
{
    /* ����winsock�� */
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);

    if(0 != WSAStartup(sockVersion, &wsaData))
    {
        printf("WSA startup failed!\n");

        return RTN_FAIL;
    }

    /* ����socket, ʹ��TCPЭ�� */
    gsocket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == gsocket_fd)
    {
        printf("create socket tcp error!\n");

        return RTN_FAIL;
    }

    /* ���ÿͻ��˲���, �󶨱������� */
    SOCKADDR_IN client;
    client.sin_family = AF_INET;                        /* IPЭ���� */
    client.sin_port = htons(client_port);               /* �ͻ��˶˿ں� */
    client.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    /* �ͻ���IP */

    /* �󶨿ͻ��˲��� */
    if(SOCKET_ERROR != bind(gsocket_fd, (SOCKADDR*)&client, sizeof(SOCKADDR)))
    {
        printf("bind socket tcp successfully!\n");
    }
    else
    {
        printf("bind socket tcp error!\n");

        goto RTN;
    }

    /* ���ӷ������� */
    SOCKADDR_IN serv_addr;
    serv_addr.sin_family = AF_INET;                         /* IPЭ���� */
    serv_addr.sin_port = htons(server_port);                /* �������˿ں� */
    serv_addr.sin_addr.S_un.S_addr = inet_addr(pserver_ip); /* ������IP */

    /* ��û�����ӵ�������, ��ú����ᱻ���� */
    if(SOCKET_ERROR != connect(gsocket_fd, (SOCKADDR*)&serv_addr, sizeof(SOCKADDR)))
    {
        printf("socket tcp connect successfully!\n");
    }
    else
    {
        printf("socket tcp connect error!\n");

        goto RTN;
    }

    /* socket���Ӻ���Ϊ������״̬ */
    gsocket_status = SOCKET_UP;

    return RTN_SUCD;

RTN:

    /* �ر�socket���� */
    closesocket(gsocket_fd);

    return RTN_FAIL;
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: lpThreadParameter: �̲߳���ָ��.
�� �� ֵ: ��������ֵ.
***********************************************************************************/
DWORD WINAPI rx_thread(LPVOID lpThreadParameter)
{
    while(1)
    {
        /* �������� */
        msg_rx();
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: lpThreadParameter: �̲߳���ָ��.
�� �� ֵ: ��������ֵ.
***********************************************************************************/
DWORD WINAPI handle_thread(LPVOID lpThreadParameter)
{
    while(1)
    {
        /* ������Ϣ */
        msg_process();
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: lpThreadParameter: �̲߳���ָ��.
�� �� ֵ: ��������ֵ.
***********************************************************************************/
DWORD WINAPI tx_thread(LPVOID lpThreadParameter)
{
    while(1)
    {
        /* �������� */
        msg_tx();
    }
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx(void)
{
    U32 rtn;

    /* ����������״̬��������� */
    if(SOCKET_UP == gsocket_status)
    {
        /* ��������, ���û������, ���̻߳ᱻ���������� */
        rtn = recv(gsocket_fd, grx_buf.buf, BUF_LEN, 0);

        /* ���մ��� */
        if(0 == rtn) /* ������������ӶϿ�, ����յ�0�ֽ����� */
        {
            printf("receive 0 byte data! break from host!\n");

            /* �ر�socket���� */
            closesocket(gsocket_fd);

            /* socket��Ϊδ����״̬ */
            gsocket_status = SOCKET_DOWN;

            return;
        }

        /* ���������ݵĳ��� */
        grx_buf.len = rtn;

        printf("socket_receive %d bytes data from host successfully!\n", rtn);

        /* �����յ�����Ϣ������Ϣ���ն��� */
        send_msg_to_que(&grx_que, &grx_buf.list);

        return;
    }
    else /* ����δ����״̬���ӳ�1s */
    {
        Sleep(1000);
    }
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_process(void)
{
    DLIST* pbuf;

    /* ����Ϣ���ն��л�ȡ���յ�����Ϣ */
    pbuf = receive_msg_from_que(&grx_que);
    if(NULL == pbuf)
    {
        return;
    }

    /* �����ջ����ڵ���Ϣ���Ƶ����ͻ����� */
    memcpy(gtx_buf.buf, grx_buf.buf, grx_buf.len);
    gtx_buf.len = grx_buf.len;

    /* ����Ҫ���͵���Ϣ������Ϣ���Ͷ��� */
    send_msg_to_que(&gtx_que, &gtx_buf.list);
}

/***********************************************************************************
��������: ������Ϣ, ����Ϣ��������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx(void)
{
    MSG_BUF* pbuf;
    U32 rtn;
    U32 send_len;

    /* ����Ϣ���Ͷ��л�ȡ��Ҫ���͵���Ϣ */
    pbuf = (MSG_BUF*)receive_msg_from_que(&gtx_que);
    if(NULL == pbuf)
    {
        return;
    }

    send_len = 0;

    /* ����������״̬�������� */
    if(SOCKET_UP == gsocket_status)
    {
        /* ѭ����������, ֱ�����ݷ������ */
        while(1)
        {
            /* �������� */
            rtn = send(gsocket_fd, pbuf->buf + send_len, gtx_buf.len - send_len, 0);

            /* ���ʹ��� */
            if(SOCKET_ERROR == rtn)
            {
                printf("send error! data length is %d!\n", gtx_buf.len);

                /* �ر�socket���� */
                closesocket(gsocket_fd);

                /* socket��Ϊδ����״̬ */
                gsocket_status = SOCKET_DOWN;

                return;
            }

            /* �ѷ��͵����ݳ��� */
            send_len += rtn;

            /* ���ݷ������ */
            if(gtx_buf.len == send_len)
            {
                printf("send %d bytes data to host successfully!\n", gtx_buf.len);

                return;
            }
            /* ����û�з������, �������� */
            else if(send_len < gtx_buf.len)
            {
                printf("send part of data! %d/%d already send!\n", send_len,
                       gtx_buf.len);
            }
        }
    }
}

/***********************************************************************************
��������: ����з�����Ϣ.
��ڲ���: pque: ����ָ��.
          plist: ������е���Ϣָ��.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_que(QUE* pque, DLIST* plist)
{
    printf("send message to queue!\n");

    /* ����Ϣ������� */
    queue_put(pque, plist);

    /* �ͷ��ź���, ����Ӷ����л�ȡ��Ϣ���߳� */
    if(0 == ReleaseSemaphore(pque->sem_cnt, 1, NULL))
    {
        printf("post sem failed!\n");

        return;
    }
}

/***********************************************************************************
��������: �Ӷ��н�����Ϣ.
��ڲ���: pque: ����ָ��.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
DLIST* receive_msg_from_que(QUE* pque)
{
    DLIST* pbuf;
    U32 rtn;

    /* ���ܻ�ȡ�������ź�����˵������������Ϣ, ��ʼ������Ϣ */
    if(WAIT_OBJECT_0 != WaitForSingleObject(pque->sem_cnt, INFINITE))
    {
        printf("wait sem failed!\n");

        return NULL;
    }

    /* ��ȡ�������ź���, ˵������������Ϣ, �Ӷ����л�ȡ��Ϣ */
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
��������: ��ʼ��˫������.
��ڲ���: plist: ������ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void dlist_init(DLIST* plist)
{
    /* �������ͷβ��ָ��սڵ� */
    plist->head = (DLIST*)NULL;
    plist->tail = (DLIST*)NULL;
}

/***********************************************************************************
��������: ��˫���������һ���ڵ�, ������β������.
��ڲ���: plist: ������ڵ�ָ��.
          pnode: ����Ľڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void dlist_node_add(DLIST* plist, DLIST* pnode)
{
    /* ����ǿ� */
    if(NULL != plist->tail)
    {
        /* �½ڵ��ͷָ��ԭβ�ڵ� */
        pnode->head = plist->head;

        /* �½ڵ��βָ����ڵ� */
        pnode->tail = plist;

        /* ԭβ�ڵ��βָ���½ڵ� */
        plist->head->tail = pnode;

        /* ���ڵ��ͷָ���¼���Ľڵ� */
        plist->head = pnode;
    }
    else /* ����Ϊ�� */
    {
        /* �½ڵ��ͷβ��ָ����ڵ� */
        pnode->head = plist;
        pnode->tail = plist;

        /* ���ڵ��ͷβ��ָ���½ڵ� */
        plist->head = pnode;
        plist->tail = pnode;
    }
}

/***********************************************************************************
��������: ��˫������ɾ��һ���ڵ�, ������ͷ��ɾ��.
��ڲ���: plist: ������ڵ�ָ��.
�� �� ֵ: ɾ���Ľڵ�ָ��, ������Ϊ���򷵻�NULL.
***********************************************************************************/
DLIST* dlist_node_delete(DLIST* plist)
{
    DLIST* ptemp_node;

    /* �����еĵ�һ���ڵ� */
    ptemp_node = plist->tail;

    /* ����ǿ� */
    if(NULL != ptemp_node)
    {
        /* �������ж���ڵ� */
        if(plist->head != plist->tail)
        {
            /* ���ڵ��βָ��ڶ����ڵ� */
            plist->tail = ptemp_node->tail;

            /* �ڶ����ڵ��ͷָ����ڵ� */
            ptemp_node->tail->head = plist;
        }
        else /* ������ֻ��һ���ڵ� */
        {
            /* ȡ���ڵ������Ϊ�� */
            plist->head = (DLIST*)NULL;
            plist->tail = (DLIST*)NULL;
        }

        /* ����ȡ���Ľڵ�ָ�� */
        return ptemp_node;
    }
    else /* ����Ϊ�շ���NULL */
    {
        return (DLIST*)NULL;
    }
}

/***********************************************************************************
��������: ��ʼ������.
��ڲ���: pque: ����ָ��.
�� �� ֵ: none.
***********************************************************************************/
void queue_init(QUE* pque)
{
    /* ��ʼ�����л����ź��� */
    if(NULL == (pque->sem_mut = CreateSemaphore(NULL, 1, 100, NULL)))
    {
        printf("queue sem_mut init failed!\n");
    }

    /* ��ʼ�����м����ź��� */
    if(NULL == (pque->sem_cnt = CreateSemaphore(NULL, 0, 100, NULL)))
    {
        printf("queue sem_cnt init failed!\n");
    }

    /* ��ʼ�����и��ڵ� */
    dlist_init(&pque->list);
}

/***********************************************************************************
��������: ��һ���ڵ�������.
��ڲ���: pque: ����ָ��.
          pque_node: Ҫ����Ķ��нڵ�ָ��.
�� �� ֵ: RTN_SUCD: ���ڵ������гɹ�.
          RTN_FAIL: ���ڵ�������ʧ��.
***********************************************************************************/
U32 queue_put(QUE* pque, DLIST* pque_node)
{
    /* ��ڲ������ */
    if((NULL == pque) || (NULL == pque_node))
    {
        return RTN_FAIL;
    }

    /* ��ȡ�ź���, ��֤��������Ĵ����� */
    if(WAIT_OBJECT_0 != WaitForSingleObject(pque->sem_mut, INFINITE))
    {
        printf("wait sem failed!\n");

        return RTN_FAIL;
    }

    /* ���ڵ������� */
    dlist_node_add(&pque->list, pque_node);

    /* �ͷ��ź��� */
    if(0 == ReleaseSemaphore(pque->sem_mut, 1, NULL))
    {
        printf("post sem failed!\n");

        return RTN_FAIL;
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: �Ӷ�����ȡ��һ�����нڵ�.
��ڲ���: pque: ���и��ڵ�ָ��.
          pque_node: ��Ŷ��нڵ�ָ���ָ��.
�� �� ֵ: RTN_SUCD: �Ӷ���ȡ���ڵ�ɹ�.
          RTN_FAIL: �Ӷ���ȡ���ڵ�ʧ��.
          QUE_RTN_NULL: ����Ϊ��, �޷�ȡ���ڵ�.
***********************************************************************************/
U32 queue_get(QUE* pque, DLIST** pque_node)
{
    DLIST* pnode;

    /* ��ڲ������ */
    if((NULL == pque) || (NULL == pque_node))
    {
        return RTN_FAIL;
    }

    /* ��ȡ�ź���, ��֤��������Ĵ����� */
    if(WAIT_OBJECT_0 != WaitForSingleObject(pque->sem_mut, INFINITE))
    {
        printf("wait sem failed!\n");

        return RTN_FAIL;
    }

    /* �Ӷ���ȡ���ڵ� */
    pnode = dlist_node_delete(&pque->list);

    /* �ͷ��ź��� */
    if(0 == ReleaseSemaphore(pque->sem_mut, 1, NULL))
    {
        printf("post sem failed!\n");

        return RTN_FAIL;
    }

    /* ���в�Ϊ��, ����ȡ���ڵ� */
    if(NULL != pnode)
    {
        *pque_node = pnode;

        return RTN_SUCD;
    }
    else /* ����Ϊ��, �޷�ȡ���ڵ� */
    {
        printf("the queue is empty!\n");

        *pque_node = NULL;

        return QUE_RTN_NULL;
    }
}

