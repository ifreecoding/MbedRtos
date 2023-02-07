
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "stm32f10x.h"
#include "wanlix.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


#define ROOTTASKSTACK       400     /* 根任务栈大小, 单位: 字节 */
#define TASKSTACK           400     /* 任务栈大小, 单位: 字节 */


#define KEYNULL             0x0     /* 没有按键按下 */


/* LCD颜色 */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F
#define LCDCOLORGREY        0x9CD3
#define LCDCOLORYELLOW1     0xF5CD
#define LCDCOLORYELLOW2     0xFFE0


#define LED_DARK            0       /* LED熄灭 */
#define LED_LIGHT           1       /* LED点亮 */


/* 灯编号定义 */
typedef enum lampno
{
    MASTER_RED = 0,                 /* 主通道红灯 */
    MASTER_YELLOW,                  /* 主通道黄灯 */
    MASTER_GREEN,                   /* 主通道绿灯 */
    SLAVE_RED,                      /* 从通道红灯 */
    SLAVE_GREEN,                    /* 从通道绿灯 */
    LEDNUM                          /* 灯总数 */
}LAMPNO;


/*********************************** 变量声明 *************************************/
extern U8 gaucRootTaskStack[ROOTTASKSTACK];
extern U16 gusPenColor;
extern U16 gusBackGroundColor;


/* 设置LCD画笔颜色, usColor: 画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* 获取LCD画笔颜色, 返回值为画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETPENCOLOR                  gusPenColor

/* 设置LCD背景色, usColor: 背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* 获取LCD背景色, 返回值为背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


extern U8 gaucCheckPsgKeyStack[TASKSTACK];
extern U8 gaucCrossStateStack[TASKSTACK];
extern U8 gaucLedDisplayStack[TASKSTACK];
extern W_TCB* gpstrCheckPsgKeyTcb;
extern W_TCB* gpstrCrossStateTcb;
extern W_TCB* gpstrLedDisplayTcb;
extern U32 guiSystemTick;


/*********************************** 函数声明 *************************************/
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

