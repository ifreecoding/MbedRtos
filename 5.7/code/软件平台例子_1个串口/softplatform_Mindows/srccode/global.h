
#ifndef  __GLOBAL_H__
#define  __GLOBAL_H__


#include "mindows.h"


/* 软件版本号 */
#define SOFTWARE_VER                    "001.001.002.000"


/* 定义任务栈大小 */
#define SCHED_TASK_STACK                1024
#define RX_TASK_STACK                   512
#define TX_TASK_STACK                   512
#define PROCESS_TASK_STACK              1024

/* 定义任务优先级 */
#define SCHED_TASK_PRIO                 1
#define RX_TASK_PRIO                    4
#define TX_TASK_PRIO                    5
#define PROCESS_TASK_PRIO               3


#define LOCAL_ADDR                      5           /* 本机地址 */


#define BUF_MAX_LEN                     256         /* 缓冲池中所支持的最大缓冲的长度 */


#define PRINT_ALL                       0xFFFFFFFF  /* 全部 */

/* 打印级别 */
#define PRINT_SUGGEST                   0x1         /* 提示 */
#define PRINT_NORMAL                    0x2         /* 一般 */
#define PRINT_IMPORTANT                 0x4         /* 重要 */

/* 打印模块 */
#define PRINT_BUF                       0x00000001  /* buf模块 */
#define PRINT_COM                       0x00000002  /* 通信模块 */
#define PRINT_TASK                      0x00000004  /* 任务模块 */
#define PRINT_RX                        0x00000008  /* 接收模块 */
#define PRINT_TX                        0x00000010  /* 发送模块 */
#define PRINT_SCHED                     0x00000020  /* 调度模块 */
#define PRINT_PROCESS                   0x00000040  /* 处理模块 */
#define PRINT_PRINT                     0x00000080  /* 打印模块 */


#define MSG_TYPE_COM                    0           /* 通信消息 */
#define MSG_TYPE_PRT                    1           /* 打印消息 */


/* LCD颜色 */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* n为申请到的缓冲长度, 计算结果为可存放数据的缓冲长度 */
#define GET_BUF_LEN(n)                  (n - (sizeof(MSG_HEAD)))

/* n为申请到的缓冲长度, 计算结果为可存放用户数据的缓冲长度, 与USR_MSG结构体对应 */
#define GET_USR_BUF_LEN(n)              (GET_BUF_LEN(n) - 9)


/* n为用户层数据域长度, 计算结果为用户层消息总长度 */
#define USR_DATA_LEN_2_USR_TOTAL_LEN(n) (n + 10)

/* n为用户层数据域长度, 计算结果为用户层消息需要计算校验和的长度 */
#define USR_DATA_LEN_2_USR_CRC_LEN(n)   (n + 7)


/* 设置LCD画笔颜色, usColor: 画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* 获取LCD画笔颜色, 返回值为画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETPENCOLOR                  gusPenColor

/* 设置LCD背景色, usColor: 背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* 获取LCD背景色, 返回值为背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


/* 单向链表结构 */
typedef struct slist
{
    struct slist* next;
}SLIST;


typedef struct buf_pool         /* 缓冲池结构 */
{
    SLIST list;                 /* 缓冲池链表的根节点 */
    M_SEM* psem;                /* 缓冲池互斥操作的二进制信号量 */
    U32 len;                    /* 缓冲池中缓冲的长度 */
    U32 s_num;                  /* 从栈中静态申请的缓冲的数量 */
    U32 d_num;                  /* 从缓冲池中动态申请的缓冲的数量 */
}BUF_POOL;

typedef struct buf_node         /* 缓冲节点结构 */
{
    M_DLIST list;               /* 挂接到缓冲池的缓冲链表 */
    BUF_POOL* phead;            /* 指向缓冲池的指针 */
}BUF_NODE;

typedef struct msg_buf          /* 消息缓冲结构 */
{
    BUF_NODE node;              /* 缓冲节点 */
    U8 buf[BUF_MAX_LEN];        /* 消息缓冲 */
}MSG_BUF;

typedef struct msg_head         /* 消息头结构 */
{
    U32 len;                    /* 消息长度 */
    /* 用户可以根据通信协议修改该结构体 */
    U32 r_addr;                 /* 接收到的消息的地址 */
    U32 type;                   /* 消息类别 */
}MSG_HEAD;

typedef struct msg_str
{
    MSG_HEAD head;                      /* 消息头结构 */
    U8 buf[GET_BUF_LEN(BUF_MAX_LEN)];   /* 存放消息的缓冲 */
}MSG_STR;


/* 用户自定义消息结构 */
typedef __packed struct usr_msg
{
    U8 head;                    /* 消息头 */
    U8 crc;                     /* 校验和 */
    U8 s_addr;                  /* 源地址 */
    U8 d_addr;                  /* 目的地址 */
    U8 cmd;                     /* 命令 */
    U32 len;                    /* 长度 */
    U8 data[GET_USR_BUF_LEN(BUF_MAX_LEN)];  /* 数据缓冲 */
}USR_MSG;


typedef struct task_str         /* 任务结构 */
{
    SLIST list;                 /* 任务池链表子节点 */
    M_QUE* pque;                /* 用来向任务传递消息的队列 */
    M_TCB* ptcb;                /* 任务TCB */
}TASK_STR;

typedef struct task_pool        /* 任务池结构 */
{
    SLIST list;                 /* 任务池链表根节点 */
    M_SEM* psem;                /* 任务池链表串行操作的信号量 */
    U32 s_num;                  /* 静态创建handle任务的数量 */
    U32 d_num;                  /* 动态创建handle任务的数量 */
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

