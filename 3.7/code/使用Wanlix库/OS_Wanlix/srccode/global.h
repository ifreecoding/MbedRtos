
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "stm32f10x.h"
#include "wanlix.h"


/* ����汾�� */
#define SOFTWARE_VER            "001.001.002.000"


#define ROOTTASKSTACK       400     /* ������ջ��С, ��λ: �ֽ� */
#define TASKSTACK           400     /* ����ջ��С, ��λ: �ֽ� */


#define KEYNULL             0x0     /* û�а������� */


/* LCD��ɫ */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F
#define LCDCOLORGREY        0x9CD3
#define LCDCOLORYELLOW1     0xF5CD
#define LCDCOLORYELLOW2     0xFFE0


#define LED_DARK            0       /* LEDϨ�� */
#define LED_LIGHT           1       /* LED���� */


/* �Ʊ�Ŷ��� */
typedef enum lampno
{
    MASTER_RED = 0,                 /* ��ͨ����� */
    MASTER_YELLOW,                  /* ��ͨ���Ƶ� */
    MASTER_GREEN,                   /* ��ͨ���̵� */
    SLAVE_RED,                      /* ��ͨ����� */
    SLAVE_GREEN,                    /* ��ͨ���̵� */
    LEDNUM                          /* ������ */
}LAMPNO;


/*********************************** �������� *************************************/
extern U8 gaucRootTaskStack[ROOTTASKSTACK];
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


extern U8 gaucCheckPsgKeyStack[TASKSTACK];
extern U8 gaucCrossStateStack[TASKSTACK];
extern U8 gaucLedDisplayStack[TASKSTACK];
extern W_TCB* gpstrCheckPsgKeyTcb;
extern W_TCB* gpstrCrossStateTcb;
extern W_TCB* gpstrLedDisplayTcb;
extern U32 guiSystemTick;


/*********************************** �������� *************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern U8 DEV_ReadKey(void);
extern void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen);
extern void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern void DEV_LampSet(LAMPNO uiLampNo, U32 uiLampSta);
extern U8* DEV_CalcFontAddr(U8 ucAscii);
extern U8* DEV_CalcInvertFontAddr(U8 ucAscii);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_CrossVarInit(void);
extern void TEST_CheckPsgKeyTask(void* pvPara);
extern void TEST_CrossStateTask(void* pvPara);
extern void TEST_LedDisplayTask(void* pvPara);


#endif

