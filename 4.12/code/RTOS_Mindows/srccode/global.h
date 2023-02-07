
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* ����汾�� */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              50      /* BUFPOOL���� */
#define MSGBUFLEN               100     /* ÿ��BUF�ĳ���, ��λ: �ֽ� */


/* �������� */
#define KEYUP                   0x1     /* ���ϰ��� */
#define KEYDOWN                 0x2     /* ���°��� */
#define KEYLEFT                 0x4     /* ���󰴼� */
#define KEYRIGHT                0x8     /* ���Ұ��� */
#define KEYRESTART              0x10    /* ���¿�ʼ���� */

#define KEYNULL                 0       /* û�а������� */
#define KEYNUM                  5       /* �������� */

/* ����IO���� */
#define IOKEYUP                 GPIO_Pin_11 /* ���ϰ��� */
#define IOKEYDOWN               GPIO_Pin_12 /* ���°��� */
#define IOKEYLEFT               GPIO_Pin_10 /* ���󰴼� */
#define IOKEYRIGHT              GPIO_Pin_13 /* ���Ұ��� */
#define IOKEYRESTART            GPIO_Pin_15 /* ���¿�ʼ���� */

/* �����ܽ���GPIO�˿ڵ�ƫ��ֵ */
#define KEYUPOFFSET             11      /* ���ϰ��� */
#define KEYDOWNOFFSET           12      /* ���°��� */
#define KEYLEFTOFFSET           10      /* ���󰴼� */
#define KEYRIGHTOFFSET          13      /* ���Ұ��� */
#define KEYRESTARTOFFSET        15      /* ���¿�ʼ���� */


/* ��Ϸ״̬ */
#define GAMESTA_STOP            0       /* ��Ϸ����ֹͣ״̬ */
#define GAMESTA_DROP            1       /* ͼ�������������������½� */
#define GAMESTA_DELLINE         2       /* ͼ������һ��, ����ɾ��һ��ͼ�� */


/* ͼ�α�Ŷ��� */
typedef enum figno
{
    BOX_FIG = 0,        /* ���� */
    I_FIG,              /* I�� */
    MOUNTAIN_FIG,       /* ɽ�� */
    Z_FIG,              /* Z�� */
    Z_REVERSE_FIG,      /* ����Z�� */
    L_FIG,              /* L�� */
    L_REVERSE_FIG,      /* ����L�� */
    FIGNUM              /* ͼ������ */
}FIGNO;


/* ��Ϣ����ṹ */
typedef struct msgbuf
{
    M_DLIST strList;                    /* �������� */
    U8 ucLength;                        /* ��Ϣ���� */
    U8 aucBuf[MSGBUFLEN];               /* ��Ϣ���� */
}MSGBUF;

/* ��Ϣ����ؽṹ */
typedef struct bufpool
{
    M_DLIST strFreeList;                /* ����ؿ������� */
    M_SEM strSem;                       /* ������ź��� */
    MSGBUF astrBufPool[BUFPOOLNUM];     /* ����� */
}BUFPOOL;


/* LCD��ɫ */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F
#define LCDCOLORBLUE2       0x051F
#define LCDCOLORGREY        0xF7DE
#define LCDCOLORMAGENTA     0xF81F
#define LCDCOLORCYAN        0x7FFF
#define LCDCOLORYELLOW      0xFFE0


/*********************************** �������� *************************************/
extern BUFPOOL gstrBufPool;
extern U16 gusPenColor;
extern U16 gusBackGroundColor;
extern M_QUE* gpstrSerialMsgQue;
extern M_TCB* gpstrSerialTaskTcb;
extern M_QUE* gpstrKeyMsgQue;
extern M_SEM* gpstrFigSem;
extern U8 gucGameSta;


/* ����LCD������ɫ, usColor: ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* ��ȡLCD������ɫ, ����ֵΪ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETPENCOLOR                  gusPenColor

/* ����LCD����ɫ, usColor: ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* ��ȡLCD����ɫ, ����ֵΪ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


/*********************************** �������� *************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutStrToMem(U8* pvStringPt, ...);
extern void DEV_PrintMsg(U8* pucBuf, U32 uiLen);
extern void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen);
extern void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawTxt(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern U8 DEV_ReadKey(void);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool);
extern void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode);
extern U32 DEV_GetRandomFig(void);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_FlushScnTask(void* pvPara);
extern void TEST_KeyTask(void* pvPara);
extern void TEST_ProcessTask(void* pvPara);
extern void TEST_SerialPrintTask(void* pvPara);
extern void TEST_TaskCreatePrint(M_TCB* pstrTcb);
extern void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb);
extern void TEST_TaskDeletePrint(M_TCB* pstrTcb);


#endif

