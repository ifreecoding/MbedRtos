
#include <Winsock2.h>
#pragma comment(lib,"ws2_32")
#include <stdio.h>
#include <wchar.h>


/* 定义变量类型 */
typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;


/* 返回值 */
#define RTN_SUCD                0       /* 成功 */
#define RTN_FAIL                1       /* 失败 */
#define QUE_RTN_NULL            2       /* 队列为空 */


#define SOCKET_DOWN             0       /* 未创建socket状态 */
#define SOCKET_UP               1       /* 已创建socket状态 */


#define BUF_LEN                 1000    /* 消息缓冲长度, 单位: 字节 */


/* 双向链表结构 */
typedef struct dlist
{
    struct dlist* head;
    struct dlist* tail;
}DLIST;

/* 队列结构 */
typedef struct que
{
    HANDLE sem_mut;     /* 用于队列操作互斥的信号量 */
    HANDLE sem_cnt;     /* 用于队列操作同步的信号量 */
    DLIST list;         /* 队列链表根节点 */
}QUE;

/* 消息缓冲结构 */
typedef struct msg_buf
{
    DLIST list;         /* 缓冲子节点 */
    U32 len;            /* 数据长度 */
    char buf[BUF_LEN];  /* 缓冲 */
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

