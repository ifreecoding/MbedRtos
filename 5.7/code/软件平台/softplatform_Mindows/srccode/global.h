
#ifndef  __GLOBAL_H__
#define  __GLOBAL_H__


#include "mindows.h"


/* ����汾�� */
#define SOFTWARE_VER                    "001.001.002.000"


/* ��������ջ��С */
#define SCHED_TASK_STACK                1024
#define RX_TASK_STACK                   512
#define TX_TASK_STACK                   512
#define PRINT_TASK_STACK                512
#define PROCESS_TASK_STACK              1024

/* �����������ȼ� */
#define SCHED_TASK_PRIO                 1
#define RX_TASK_PRIO                    4
#define TX_TASK_PRIO                    5
#define PRINT_TASK_PRIO                 6
#define PROCESS_TASK_PRIO               3


#define BUF_MAX_LEN                     256         /* ���������֧�ֵ���󻺳�ĳ��� */


#define PRINT_ALL                       0xFFFFFFFF  /* ȫ�� */

/* ��ӡ���� */
#define PRINT_SUGGEST                   0x1         /* ��ʾ */
#define PRINT_NORMAL                    0x2         /* һ�� */
#define PRINT_IMPORTANT                 0x4         /* ��Ҫ */

/* ��ӡģ�� */
#define PRINT_BUF                       0x00000001  /* bufģ�� */
#define PRINT_COM                       0x00000002  /* ͨ��ģ�� */
#define PRINT_TASK                      0x00000004  /* ����ģ�� */
#define PRINT_RX                        0x00000008  /* ����ģ�� */
#define PRINT_TX                        0x00000010  /* ����ģ�� */
#define PRINT_SCHED                     0x00000020  /* ����ģ�� */
#define PRINT_PROCESS                   0x00000040  /* ����ģ�� */
#define PRINT_PRINT                     0x00000080  /* ��ӡģ�� */


/* nΪ���뵽�Ļ��峤��, ������Ϊ�ɴ�����ݵĻ��峤�� */
#define GET_BUF_LEN(n)                  (n - (sizeof(MSG_HEAD)))


/* ��������ṹ */
typedef struct slist
{
    struct slist* next;
}SLIST;


typedef struct buf_pool         /* ����ؽṹ */
{
    SLIST list;                 /* ���������ĸ��ڵ� */
    M_SEM* psem;                /* ����ػ�������Ķ������ź��� */
    U32 len;                    /* ������л���ĳ��� */
    U32 s_num;                  /* ��ջ�о�̬����Ļ�������� */
    U32 d_num;                  /* �ӻ�����ж�̬����Ļ�������� */
}BUF_POOL;

typedef struct buf_node         /* ����ڵ�ṹ */
{
    M_DLIST list;               /* �ҽӵ�����صĻ������� */
    BUF_POOL* phead;            /* ָ�򻺳�ص�ָ�� */
}BUF_NODE;

typedef struct msg_buf          /* ��Ϣ����ṹ */
{
    BUF_NODE node;              /* ����ڵ� */
    U8 buf[BUF_MAX_LEN];        /* ��Ϣ���� */
}MSG_BUF;

typedef struct msg_head         /* ��Ϣͷ�ṹ */
{
    U32 len;                    /* ��Ϣ���� */
    /* �û����Ը���ͨ��Э���޸ĸýṹ�� */
}MSG_HEAD;

typedef struct msg_str
{
    MSG_HEAD head;                      /* ��Ϣͷ�ṹ */
    U8 buf[GET_BUF_LEN(BUF_MAX_LEN)];   /* �����Ϣ�Ļ��� */
}MSG_STR;


typedef struct task_str         /* ����ṹ */
{
    SLIST list;                 /* ����������ӽڵ� */
    M_QUE* pque;                /* ���������񴫵���Ϣ�Ķ��� */
    M_TCB* ptcb;                /* ����TCB */
}TASK_STR;

typedef struct task_pool        /* ����ؽṹ */
{
    SLIST list;                 /* �����������ڵ� */
    M_SEM* psem;                /* ����������в������ź��� */
    U32 s_num;                  /* ��̬����handle��������� */
    U32 d_num;                  /* ��̬����handle��������� */
}TASK_POOL;


extern void software_init(void);
extern void hardware_init(void);
extern void buf_pool_init(void);
extern BUF_NODE* buf_malloc(U32 len);
extern void buf_free(BUF_NODE* pbuf);
extern BUF_NODE* buf_malloc_without_print(U32 len);
extern void buf_free_without_print(BUF_NODE* pbuf);
extern void slist_init(SLIST* plist);
extern void slist_node_add(SLIST* plist, SLIST* pnode);
extern SLIST* slist_node_delete(SLIST* plist);
extern void msg_rx_task(void* pvPara);
extern void msg_tx_task(void* pvPara);
extern void msg_sched_task(void* pvPara);
extern void msg_print_task(void* pvPara);
extern void create_sched_task(void);
extern void msg_process_task(void* pvPara);
extern void create_rx_task(void);
extern void create_tx_task(void);
extern void create_print_task(void);
extern U32 create_process_task(TASK_STR* ptask);
extern void print_msg(U32 level, U32 module, U8* fmt, ...);
extern void print_level_set(U32 level);
extern void print_level_clr(U32 level);
extern U32 print_level_get(void);
extern void print_module_set(U32 module);
extern void print_module_clr(U32 module);
extern U32 print_module_get(void);
extern void task_create(void);
extern TASK_STR* process_task_malloc(void);
extern void process_task_free(TASK_STR* ptask);
extern void msg_rx_isr(void);
extern void msg_tx_isr(void);
extern U32 data_receive(U8* pbuf, U32 len);
extern U32 data_send(U8* pbuf, U32 len);
extern void uart1_init(void);
extern void uart2_init(void);
extern void msg_rx_ring_buf_init(void);
extern void msg_tx_ring_buf_init(void);
extern U32 rx_message_packet(U8* pbuf, MSG_STR* ppacket_buf, U32 data_len,
                             U32* pused_len);
extern void send_msg_to_sched_task(BUF_NODE* pbuf);
extern BUF_NODE* sched_task_receive_msg(void);
extern void send_msg_to_rx_task(BUF_NODE* pbuf);
extern BUF_NODE* rx_task_receive_msg(void);
extern void send_msg_to_tx_task(BUF_NODE* pbuf);
extern BUF_NODE* tx_task_receive_msg(void);
extern void send_msg_to_process_task(TASK_STR* ptask, BUF_NODE* pbuf);
extern BUF_NODE* process_task_receive_msg(TASK_STR* ptask);
extern void send_msg_to_print_task(BUF_NODE* pbuf);
extern BUF_NODE* print_task_receive_msg(void);


#endif

