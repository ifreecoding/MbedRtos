
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define TICK                100                         /* Tick时间, 单位: ms */


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC 命令地址 */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC 数据地址 */


/* LCD显示宏定义, 屏的左上角为坐标原点, X轴方向向右, Y轴方向向下 */
#define LCDXLEN             240                         /* LCD X轴长度, 单位: 像素 */
#define LCDYLEN             400                         /* LCD Y轴长度, 单位: 像素 */


/* 按键定义 */
#define PSGKEY1             0x1                         /* 行人按键1 */
#define PSGKEY2             0x2                         /* 行人按键2 */

/* 按键IO定义 */
#define IOPSGKEY1           GPIO_Pin_14                 /* 行人按键1 */
#define IOPSGKEY2           GPIO_Pin_15                 /* 行人按键2 */

/* 按键管脚在GPIO端口的偏移值 */
#define PSGKEY1OFFSET       14                          /* 行人按键1 */
#define PSGKEY2OFFSET       15                          /* 行人按键2 */


/*********************************** 变量声明 *************************************/
extern U8 gaucFont[];
extern U8 gaucInvertFont[];
extern U8 gaucRound[];
extern U8 gaucArrow1[];
extern U8 gaucArrow2[];


/*********************************** 函数声明 *************************************/
extern void DEV_PsgKeyInit(void);
extern void DEV_TickTimerInit(void);
extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern void DEV_LcdCrossInit(void);
extern U8 DEV_ReadHardwareVersion(void);


#endif

