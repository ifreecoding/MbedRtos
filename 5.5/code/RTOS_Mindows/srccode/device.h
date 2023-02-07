
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC 命令地址 */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC 数据地址 */


/* LCD显示宏定义, 屏的左上角为坐标原点, X轴方向向右, Y轴方向向下 */
#define LCDXLEN             240                         /* LCD X轴长度, 单位: 像素 */
#define LCDYLEN             400                         /* LCD Y轴长度, 单位: 像素 */


/* CPU占有率LCD显示宏定义 */
#define CSDSPXSTART         130                         /* CPU占有率显示区域X轴起始点, 单位: 像素 */
#define CSDSPYSTART         30                          /* CPU占有率显示区域Y轴起始点, 单位: 像素 */
#define CSDSPXLEN           (LCDXLEN - CSDSPXSTART - 10)/* CPU占有率显示区域X轴长度, 单位: 像素 */
#define CSDSPYLEN           (LCDYLEN - CSDSPYSTART - 10)/* CPU占有率显示区域Y轴长度, 单位: 像素 */
#define CSDSPXWID           3                           /* CPU占有率显示区域X轴宽度, 单位: 像素 */
#define CSDSPYWID           3                           /* CPU占有率显示区域Y轴宽度, 单位: 像素 */

#define CSDSPWID            (CSDSPXLEN - 20)            /* CPU占有率显示区域宽度, X轴方向, 单位: 像素 */
#define CSDSPLEN            (CSDSPYLEN - 10)            /* CPU占有率显示区域长度, Y轴方向, 单位: 像素 */
#define CSDSPSWTITV         1                           /* CPU占有率显示区域统计点间隔, 单位: 像素 */
#define CSDSPSWTTICKNUM     10                          /* CPU占有率显示区域统计点之间的TICK数 */
#define CSDSPBTNUM          (500 / CSDSPSWTTICKNUM)     /* CPU占有率显示区域2个时间刻度线之间的任务切换点数 */
#define CSDSPTIMEITV        (CSDSPSWTITV * CSDSPBTNUM)  /* CPU占有率显示区域时间刻度线时间间隔, 单位: 像素 */
#define CSDSPTIMENUM        (CSDSPLEN / CSDSPTIMEITV)   /* 时间刻度线的数量 */
#define CSDSPTIMELEN        5                           /* 时间刻度线的长度, X轴方向, 单位: 像素 */
#define CSDSPTIMEWID        3                           /* 时间刻度线的宽度, Y轴方向, 单位: 像素 */
#define CSDSPLINEWID        3                           /* CPU占有率线宽, 单位: 像素 */
#define CSDSPSWTNUM         (CSDSPLEN / CSDSPSWTITV / CSDSPSWTTICKNUM)   /* CPU占有率点的数量 */

#define CPUSHAREMSGLEN      (CSDSPSWTNUM + 1)           /* 存放CPU占有率信息环形缓冲的长度 */

#define CSTASKXSTART        100                         /* CPU占有率任务显示区域X轴起始点, 单位: 像素 */
#define CSTASKYSTART        30                          /* CPU占有率任务显示区域Y轴起始点, 单位: 像素 */
#define CSVALYSTART         100                         /* CPU占有率数值显示区域Y轴起始点, 单位: 像素 */


/* 存储CPU占有率信息的结构 */
typedef struct cpusharemsgstr
{
    U32 astrMsg[CPUSHAREMSGLEN];    /* 存放CPU占有率信息的环形缓冲 */
    U32 uiHead;                     /* 环形缓冲头位置 */
    U32 uiTail;                     /* 环形缓冲尾位置 */
}CPUSHAREMSGSTR;


/*********************************** 变量声明 *************************************/
extern CPUSHAREMSGSTR gstrCsMsgStr;
extern U8 gaucFont[];
extern U8 gaucInvertFont[];


/*********************************** 函数声明 *************************************/
extern void DEV_UartInit(void);
extern void DEV_PutChar(U8 ucChar);
extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern void DEV_CpuShareDisplayInit(void);
extern void DEV_CpuShareMsgStrInit(void);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern U8 DEV_ReadHardwareVersion(void);


#endif

