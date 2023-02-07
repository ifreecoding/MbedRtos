
#ifndef  __GLOBAL_H__
#define  __GLOBAL_H__


#include "mindows.h"


/* ����汾�� */
#define SOFTWARE_VER                    "001.001.002.000"


/* ��������ջ��С */
#define SCHED_TASK_STACK                1024
#define RX_TASK_STACK                   512
#define TX_TASK_STACK                   512
#define PROCESS_TASK_STACK              1024

/* �����������ȼ� */
#define SCHED_TASK_PRIO                 1
#define RX_TASK_PRIO                    4
#define TX_TASK_PRIO                    5
#define PROCESS_TASK_PRIO               3


#define LOCAL_ADDR                      5           /* ������ַ */


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


#define MSG_TYPE_COM                    0           /* ͨ����Ϣ */
#define MSG_TYPE_PRT                    1           /* ��ӡ��Ϣ */


/* LCD��ɫ */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* nΪ���뵽�Ļ��峤��, ������Ϊ�ɴ�����ݵĻ��峤�� */
#define GET_BUF_LEN(n)                  (n - (sizeof(MSG_HEAD)))

/* nΪ���뵽�Ļ��峤��, ������Ϊ�ɴ���û����ݵĻ��峤��, ��USR_MSG�ṹ���Ӧ */
#define GET_USR_BUF_LEN(n)              (GET_BUF_LEN(n) - 9)


/* nΪ�û��������򳤶�, ������Ϊ�û�����Ϣ�ܳ��� */
#define USR_DATA_LEN_2_USR_TOTAL_LEN(n) (n + 10)

/* nΪ�û��������򳤶�, ������Ϊ�û�����Ϣ��Ҫ����У��͵ĳ��� */
#define USR_DATA_LEN_2_USR_CRC_LEN(n)   (n + 7)


/* ����LCD������ɫ, usColor: ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* ��ȡLCD������ɫ, ����ֵΪ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETPENCOLOR                  gusPenColor

/* ����LCD����ɫ, usColor: ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* ��ȡLCD����ɫ, ����ֵΪ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


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
    U32 r_addr;                 /* ���յ�����Ϣ�ĵ�ַ */
    U32 type;                   /* ��Ϣ��� */
}MSG_HEAD;

typedef struct msg_str
{
    MSG_HEAD head;                      /* ��Ϣͷ�ṹ */
    U8 buf[GET_BUF_LEN(BUF_MAX_LEN)];   /* �����Ϣ�Ļ��� */
}MSG_STR;


/* �û��Զ�����Ϣ�ṹ */
typedef __packed struct usr_msg
{
    U8 head;                    /* ��Ϣͷ */
    U8 crc;                     /* У��� */
    U8 s_addr;                  /* Դ��ַ */
    U8 d_addr;                  /* Ŀ�ĵ�ַ */
    U8 cmd;                     /* ���� */
    U32 len;                    /* ���� */
    U8 data[GET_USR_BUF_LEN(BUF_MAX_LEN)];  /* ���ݻ��� */
}USR_MSG;


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
extern void display_string(U8* pstring, U32 len, U32 x, U32 y);
extern void set_pen_color(U16 color);
extern void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen);
extern void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
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
extern void create_sched_task(void);
extern void msg_process_task(void* pvPara);
extern void create_rx_task(void);
extern void create_tx_task(void);
extern U32 create_process_task(TASK_STR* ptask);
extern void print_init(void);
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
extern void msg_rx_ring_buf_init(void);
extern void msg_tx_ring_buf_init(void);
extern void send_msg_to_sched_task(BUF_NODE* pbuf);
extern BUF_NODE* sched_task_receive_msg(void);
extern void send_msg_to_rx_task(BUF_NODE* pbuf);
extern BUF_NODE* rx_task_receive_msg(void);
extern void send_msg_to_tx_task(BUF_NODE* pbuf);
extern BUF_NODE* tx_task_receive_msg(void);
extern void send_msg_to_process_task(TASK_STR* ptask, BUF_NODE* pbuf);
extern BUF_NODE* process_task_receive_msg(TASK_STR* ptask);
extern U32 check_msg_len(U32 msg_len, U32 data_len);
extern U8 calc_msg_crc(U8* pbuf, U32 len);
extern U8 check_msg_addr(U8 addr);


#endif

