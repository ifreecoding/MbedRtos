
#include <Winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <wchar.h>


/* ����������� */
typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;


/* ����ֵ */
#define RTN_SUCD                0       /* �ɹ� */
#define RTN_FAIL                1       /* ʧ�� */
#define QUE_RTN_NULL            2       /* ����Ϊ�� */


#define SOCKET_DOWN             0       /* δ����socket״̬ */
#define SOCKET_UP               1       /* �Ѵ���socket״̬ */


#define BUF_LEN                 1000    /* ��Ϣ���峤��, ��λ: �ֽ� */


/* ˫������ṹ */
typedef struct dlist
{
    struct dlist* head;
    struct dlist* tail;
}DLIST;

/* ���нṹ */
typedef struct que
{
    HANDLE sem_mut;     /* ���ڶ��в���������ź��� */
    HANDLE sem_cnt;     /* ���ڶ��в���ͬ�����ź��� */
    DLIST list;         /* ����������ڵ� */
}QUE;

/* ��Ϣ����ṹ */
typedef struct msg_buf
{
    DLIST list;         /* �����ӽڵ� */
    U32 len;            /* ���ݳ��� */
    char buf[BUF_LEN];  /* ���� */
}MSG_BUF;


extern U32 socket_tcp_connect(char* pserver_ip, U32 server_port, U32 client_port);
DWORD WINAPI rx_thread(LPVOID lpThreadParameter);
DWORD WINAPI handle_thread(LPVOID lpThreadParameter);
DWORD WINAPI tx_thread(LPVOID lpThreadParameter);
extern void msg_rx(void);
extern void msg_process(void);
extern void msg_tx(void);
extern void send_msg_to_que(QUE* pque, DLIST* plist);
extern DLIST* receive_msg_from_que(QUE* pque);
extern void dlist_init(DLIST* plist);
extern void dlist_node_add(DLIST* plist, DLIST* pnode);
extern DLIST* dlist_node_delete(DLIST* plist);
extern void queue_init(QUE* pque);
extern U32 queue_put(QUE* pque, DLIST* pque_node);
extern U32 queue_get(QUE* pque, DLIST** pque_node);

