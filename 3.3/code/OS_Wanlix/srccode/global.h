
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "stm32f10x.h"
#include "wanlix.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


/* LCD颜色 */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* 存储任务切换信息的结构 */
typedef struct lcdtaskswitchmsg
{
    U32 uiTask;
    U32 uiTime;
}LCDTASKSWITCHMSG;


/*********************************** 变量声明 *************************************/
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


/*********************************** 函数声明 *************************************/
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


#endif

