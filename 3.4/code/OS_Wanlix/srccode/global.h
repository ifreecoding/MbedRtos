
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "stm32f10x.h"
#include "wanlix.h"


/* ����汾�� */
#define SOFTWARE_VER            "001.001.002.000"


#define TASKSTACK           400     /* ����ջ��С, ��λ: �ֽ� */


/* LCD��ɫ */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* �洢�����л���Ϣ�Ľṹ */
typedef struct lcdtaskswitchmsg
{
    U32 uiTask;
    U32 uiTime;
}LCDTASKSWITCHMSG;


/*********************************** �������� *************************************/
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];
extern W_TCB* gpstrTask1Tcb;
extern W_TCB* gpstrTask2Tcb;
extern W_TCB* gpstrTask3Tcb;
extern U16 gusPenColor;
extern U16 gusBackGroundColor;


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
extern void DEV_TaskPrintMsg(U32 uiTask);
extern void DEV_AddTaskTime(U32 uiTime);
extern U32 DEV_GetTaskTime(void);
extern void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen);
extern void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern U8* DEV_CalcFontAddr(U8 ucAscii);
extern U8* DEV_CalcInvertFontAddr(U8 ucAscii);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void);
extern void TEST_TestTask2(void);
extern void TEST_TestTask3(void);


#endif

