
#ifndef  WANLIX_H
#define  WANLIX_H


#define ROOTTASKSTACK       400     /* ������ջ��С, ��λ: �ֽ� */


/* ����������� */
typedef char                U8;     /* ��������char��Ϊ�޷����� */
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef signed char         S8;
typedef short               S16;
typedef int                 S32;
typedef void                (*VFUNC)(void);


#ifndef NULL
 #define NULL               ((void*)0)
#endif


/* TCB�б��ݼĴ�����Ľṹ��, ������ʱ��������ǰ���ļĴ��� */
typedef struct stackreg
{
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
    U32 uiXpsr;
}STACKREG;

/* TCB�ṹ�� */
typedef struct w_tcb
{
    STACKREG strStackReg;           /* ���ݼĴ����� */
}W_TCB;


/*********************************** �������� *************************************/
extern W_TCB* WLX_TaskCreate(VFUNC vfFuncPointer, U8* pucTaskStack,
                             U32 uiStackSize);
extern void WLX_TaskSwitch(W_TCB* pstrTcb);


#endif

