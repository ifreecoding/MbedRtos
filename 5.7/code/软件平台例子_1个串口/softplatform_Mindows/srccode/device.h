
#ifndef  __DEVICE_H__
#define  __DEVICE_H__


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC 命令地址 */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC 数据地址 */


/* LCD显示宏定义, 屏的左上角为坐标原点, X轴方向向右, Y轴方向向下 */
#define LCDXLEN             240                         /* LCD X轴长度, 单位: 像素 */
#define LCDYLEN             400                         /* LCD Y轴长度, 单位: 像素 */


extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern U8* DEV_CalcFontAddr(U8 ucAscii);
extern U8* DEV_CalcInvertFontAddr(U8 ucAscii);
extern U8 DEV_ReadHardwareVersion(void);


#endif

