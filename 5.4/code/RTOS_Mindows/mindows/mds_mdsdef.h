
#ifndef  MDS_MDSDEF_H
#define  MDS_MDSDEF_H


/* ����������� */
typedef char                        U8;     /* ��������char��Ϊ�޷����� */
typedef unsigned short              U16;
typedef unsigned int                U32;
typedef signed char                 S8;
typedef short                       S16;
typedef int                         S32;
typedef void                        (*VFUNC)(void*);
typedef void                        (*VFUNC1)(U8);

#ifndef NULL
 #define NULL                       ((void*)0)
#endif


/* ����ֵ */
#define RTN_SUCD                    0       /* �ɹ� */
#define RTN_FAIL                    1       /* ʧ�� */


/* THUMBָ�USR����ģʽ���� */
#define MODE_USR                    0x01000000


/* �ֽڶ������� */
#define ALIGN4MASK                  0xFFFFFFFC  /* 4�ֽڶ��� */
#define ALIGN8MASK                  0xFFFFFFF8  /* 8�ֽڶ��� */


/* ��ȡָ����ַ�е����� */
#define ADDRVAL(addr)               (*((volatile U32*)(addr)))


/* �������ȼ���Ŀ */
#define PRIORITY256                 256
#define PRIORITY128                 128
#define PRIORITY64                  64
#define PRIORITY32                  32
#define PRIORITY16                  16
#define PRIORITY8                   8


/* ���ȱ�궨�� */
#if PRIORITY256 == PRIORITYNUM
 #define PRIOFLAGGRP1               32
 #define PRIOFLAGGRP2               4
#elif PRIORITY128 == PRIORITYNUM
 #define PRIOFLAGGRP1               16
 #define PRIOFLAGGRP2               2
#elif PRIORITY64 == PRIORITYNUM
 #define PRIOFLAGGRP1               8
 #define PRIOFLAGGRP2               1
#elif PRIORITY32 == PRIORITYNUM
 #define PRIOFLAGGRP1               4
 #define PRIOFLAGGRP2               1
#elif PRIORITY16 == PRIORITYNUM
 #define PRIOFLAGGRP1               2
 #define PRIOFLAGGRP2               1
#elif PRIORITY8 == PRIORITYNUM
 #define PRIOFLAGGRP1               1
 #define PRIOFLAGGRP2               1
#else
 #error The priotity must be defined between 8 and 256, and must be the power of 2.
#endif


/* TCB�б��ݼĴ�����Ľṹ��, ������ʱ��������ǰ���ļĴ��� */
typedef struct stackreg
{
    U32 uiR0;
    U32 uiR1;
    U32 uiR2;
    U32 uiR3;
    U32 uiR4;
    U32 uiR5;
    U32 uiR6;
    U32 uiR7;
    U32 uiR8;
    U32 uiR9;
    U32 uiR10;
    U32 uiR11;
    U32 uiR12;
    U32 uiR13;
    U32 uiR14;
    U32 uiR15;
    U32 uiXpsr;
    U32 uiExc_Rtn;
}STACKREG;

typedef struct m_dlist              /* ����ṹ */
{
    struct m_dlist* pstrHead;       /* ͷָ�� */
    struct m_dlist* pstrTail;       /* βָ�� */
}M_DLIST;

typedef struct m_tcbque             /* TCB���нṹ */
{
    M_DLIST strQueHead;             /* ���Ӷ��е����� */
    struct m_tcb* pstrTcb;          /* TCBָ�� */
}M_TCBQUE;

typedef struct m_taskopt            /* ������� */
{
    U8 ucTaskSta;                   /* ��������״̬ */
    U32 uiDelayTick;                /* �ӳ�ʱ�� */
}M_TASKOPT;

typedef struct m_prioflag               /* ���ȼ���־�� */
{
#if PRIORITYNUM >= PRIORITY128
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 aucPrioFlagGrp2[PRIOFLAGGRP2];
    U8 ucPrioFlagGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 ucPrioFlagGrp2;
#else
    U8 ucPrioFlagGrp1;
#endif
}M_PRIOFLAG;

typedef struct m_taskschedtab           /* ������ȱ� */
{
    M_DLIST astrList[PRIORITYNUM];      /* �������ȼ����ڵ� */
    M_PRIOFLAG strFlag;                 /* ���ȼ���־ */
}M_TASKSCHEDTAB;

/* SEM�ṹ�� */
typedef struct m_sem
{
    M_TASKSCHEDTAB strSemTab;       /* �ź������ȱ� */
    U32 uiCounter;                  /* �ź�������ֵ */
    U32 uiSemOpt;                   /* �ź������� */
    U8* pucSemMem;                  /* �����ź���ʱ���ڴ��ַ */
    struct m_tcb* pstrSemTask;      /* ��ȡ�������ź��������� */
}M_SEM;

/* TCB�ṹ�� */
typedef struct m_tcb
{
    STACKREG strStackReg;           /* ���ݼĴ����� */
    M_TCBQUE strTcbQue;             /* TCB�ṹ���� */
    M_TCBQUE strSemQue;             /* sem����� */
    M_SEM* pstrSem;                 /* ����������ź���ָ�� */
    U8* pucTaskName;                /* ��������ָ�� */
    U8* pucTaskStack;               /* ��������ʱ��ջ��ַ */
    U32 uiTaskFlag;                 /* �����־ */
    U8 ucTaskPrio;                  /* �������ȼ� */
#ifdef MDS_TASKPRIOINHER
    U8 ucTaskPrioBackup;            /* �������ȼ��̳к�, ��������ԭ�е����ȼ� */
#endif
    M_TASKOPT strTaskOpt;           /* ������� */
    U32 uiStillTick;                /* �ӳٽ�����ʱ�� */
}M_TCB;


#endif

