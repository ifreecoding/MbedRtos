
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC 命令地址 */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC 数据地址 */


/* LCD显示宏定义, 屏的左上角为坐标原点, X轴方向向右, Y轴方向向下 */
#define LCDXLEN             240                         /* LCD X轴长度, 单位: 像素 */
#define LCDYLEN             400                         /* LCD Y轴长度, 单位: 像素 */


/* 任务切换LCD显示宏定义 */
#define TSDSPXSTART         20                          /* 任务切换显示区域X轴起始点, 单位: 像素 */
#define TSDSPYSTART         20                          /* 任务切换显示区域Y轴起始点, 单位: 像素 */
#define TSDSPXLEN           (LCDXLEN - TSDSPXSTART - 20)/* 任务切换显示区域X轴长度, 单位: 像素 */
#define TSDSPYLEN           (LCDYLEN - TSDSPYSTART - 10)/* 任务切换显示区域Y轴长度, 单位: 像素 */
#define TSDSPXWID           3                           /* 任务切换显示区域X轴宽度, 单位: 像素 */
#define TSDSPYWID           3                           /* 任务切换显示区域Y轴宽度, 单位: 像素 */

#define TSDSPWID            (TSDSPXLEN - 20)            /* 任务切换显示区域宽度, X轴方向, 单位: 像素 */
#define TSDSPLEN            (TSDSPYLEN - 10)            /* 任务切换显示区域长度, Y轴方向, 单位: 像素 */
#define TSDSPSWTITV         2                           /* 任务切换显示区域任务切换点间隔, 单位: 像素 */
#define TSDSPSWTTICKNUM     10                          /* 任务切换显示区域任务切换点之间的TICK数 */
#define TSDSPBTNUM          (300 / TSDSPSWTTICKNUM)     /* 任务切换显示区域2个时间刻度线之间的任务切换点数 */
#define TSDSPTIMEITV        (TSDSPSWTITV * TSDSPBTNUM)  /* 任务切换显示区域时间刻度线时间间隔, 单位: 像素 */
#define TSDSPTASKNUM        5                           /* 任务切换的任务个数 */
#define TSDSPTASKITV        (TSDSPWID / TSDSPTASKNUM)   /* 任务名称显示间隔, 单位: 像素 */
#define TSDSPTIMENUM        (TSDSPLEN / TSDSPTIMEITV)   /* 时间刻度线的数量 */
#define TSDSPTIMELEN        5                           /* 时间刻度线的长度, X轴方向, 单位: 像素 */
#define TSDSPTIMEWID        3                           /* 时间刻度线的宽度, Y轴方向, 单位: 像素 */
#define TSDSPLINEWID        3                           /* 任务切换线宽, 单位: 像素 */
#define TSDSPSWTNUM         (TSDSPLEN / TSDSPSWTITV)    /* 任务切换点的数量 */

#define TASKSWITCHMSGLEN    (TSDSPSWTNUM + 1)           /* 存放任务切换信息环形缓冲的长度 */


/* 图形更新标志 */
#define TSFLAGUPDATA        0       /* 任务切换图形需要更新 */
#define TSFLAGNOTUPDATA     1       /* 任务切换图形不需要更新 */


typedef struct taskswitchmsg
{
    U32 uiTask;         /* 任务编号 */
    U32 uiRunTime;      /* 任务运行的时间, 存放的是任务运行的切换点个数 */
    U32 uiTolalTime;    /* 所有任务已运行的总时间, 存放的是任务运行的切换点个数 */
}TASKSWITCHMSG;

typedef struct taskswitchmsgstr
{
    TASKSWITCHMSG astrMsg[TASKSWITCHMSGLEN];    /* 存放任务切换信息的环形缓冲 */
    U32 uiHead;                                 /* 环形缓冲头位置 */
    U32 uiTail;                                 /* 环形缓冲尾位置 */
    U32 uiFlag;                                 /* 图形是否更新的标志 */
}TASKSWITCHMSGSTR;


/*********************************** 变量声明 *************************************/
extern TASKSWITCHMSGSTR gstrTsMsgStr;
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
extern void DEV_TaskSwitchDisplayInit(void);
extern void DEV_TaskSwitchMsgStrInit(void);
extern void DEV_PrintTaskSwitchTime(U32 uiLastTime);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern U8 DEV_ReadHardwareVersion(void);


#endif

