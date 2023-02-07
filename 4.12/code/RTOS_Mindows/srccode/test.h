
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


/* LCD显示宏定义, 屏的左上角为坐标原点, X轴方向向右, Y轴方向向下 */
#define LCDXLEN             240                         /* LCD X轴长度, 单位: 像素 */
#define LCDYLEN             400                         /* LCD Y轴长度, 单位: 像素 */


/******* 下述定义的单元是内存中的像素, 乘以PIXELAMP后对应LCD屏幕上的实际像素 ******/

/* Lcd对应的内存分布结构, 以主窗口显示区左上角为坐标原点, 向右为X轴增长方向, 向下为Y
   轴增长方向. 分为主窗口显示区和副窗口显示区. 主窗口显示区用来存放游戏界面, 副窗口
   显示区用来存放辅助显示的信息 */

/************************************************/
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*              主窗口              ||  副窗口  */
/*              显示区              ||  显示区  */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/************************************************/

/* 像素放大倍数 */
#define PIXELAMP            16                      /* 内存中一个点对应实际像素的点数 */

#define LCDSCN_X_LEN        (LCDXLEN / PIXELAMP)    /* LCD屏幕X轴长度 */
#define LCDSCN_Y_LEN        (LCDYLEN / PIXELAMP)    /* LCD屏幕Y轴长度 */

#define MAINSCN_X_LEN       11                      /* 主窗口X轴长度 */
#define MAINSCN_Y_LEN       LCDSCN_Y_LEN            /* 主窗口Y轴长度 */

#define MAINSCR_MIN_X       0                       /* 主窗口X轴最小坐标 */
#define MAINSCR_MAX_X       (MAINSCN_X_LEN - 1)     /* 主窗口X轴最大坐标 */
#define MAINSCR_MIN_Y       0                       /* 主窗口Y轴最小坐标 */
#define MAINSCR_MAX_Y       (LCDSCN_Y_LEN - 1)      /* 主窗口Y轴最大坐标 */

#define LINE_X_LEN          1                       /* 分隔线X轴长度 */
#define LINE_Y_LEN          LCDSCN_Y_LEN            /* 分隔线Y轴长度 */

#define LINE_MIN_X          MAINSCN_X_LEN           /* 分隔线X轴最小坐标 */
#define LINE_MIN_Y          0                       /* 分隔线Y轴最小坐标 */

#define SLAVESCN_X_LEN      (LCDSCN_X_LEN - MAINSCN_X_LEN - LINE_X_LEN) /* 副窗口X轴长度 */
#define SLAVESCN_Y_LEN      LCDSCN_Y_LEN                                /* 副窗口Y轴长度 */

#define SLAVESCR_MIN_X      (MAINSCN_X_LEN + LINE_X_LEN)    /* 副窗口X轴最小坐标 */
#define SLAVESCR_MAX_X      (LCDSCN_X_LEN - 1)      /* 副窗口X轴最大坐标 */
#define SLAVESCR_MIN_Y      0                       /* 副窗口Y轴最小坐标 */
#define SLAVESCR_MAX_Y      MAINSCR_MAX_Y           /* 副窗口Y轴最大坐标 */

#define NEXTFIG_X_LEN       3                       /* 下个显示图形的X轴长度 */
#define NEXTFIG_Y_LEN       5                       /* 下个显示图形的Y轴长度 */
#define NEXTFIG_MAX_LEN     NEXTFIG_Y_LEN           /* 下个显示图形最长轴的长度 */

#define NEXTFIG_MIN_X       12                      /* 下个显示图形X轴最小坐标 */
#define NEXTFIG_MAX_X       (NEXTFIG_MIN_X + NEXTFIG_X_LEN - 1) /* 下个显示图形X轴最大坐标 */
#define NEXTFIG_MIN_Y       5                       /* 下个显示图形Y轴最小坐标 */
#define NEXTFIG_MAX_Y       (NEXTFIG_MIN_X + NEXTFIG_Y_LEN - 1) /* 下个显示图形Y轴最大坐标 */


/* 图形显示的像素定义, 以每个图形的左上角为坐标原点, 向右为X轴增长方向, 向下为Y轴增
   长方向, 图像出现时的坐标定义, 格式为:
(主窗口左上角X轴坐标, 主窗口左上角Y轴坐标, X轴长度, Y轴长度),
(主窗口左上角X轴坐标, 主窗口左上角Y轴坐标, X轴长度, Y轴长度),
图形组件数量, (左上角X轴坐标, 左上角Y轴坐标, X轴长度, Y轴长度) * 图形组件数量 */
#define PIXEL_BOX_FIG                   {5, 0, 2, 2, 1, 0, 0, 2, 2}
#define PIXEL_I_FIG_0_180               {5, 0, 1, 5, 1, 0, 0, 1, 5}
#define PIXEL_I_FIG_90_270              {5, 0, 5, 1, 1, 0, 0, 5, 1}
#define PIXEL_MOUNTAIN_FIG_0            {5, 0, 3, 2, 2, 1, 0, 1, 1, 0, 1, 3, 1}
#define PIXEL_MOUNTAIN_FIG_90           {5, 0, 2, 3, 2, 1, 1, 1, 1, 0, 0, 1, 3}
#define PIXEL_MOUNTAIN_FIG_180          {5, 0, 3, 2, 2, 1, 1, 1, 1, 0, 0, 3, 1}
#define PIXEL_MOUNTAIN_FIG_270          {5, 0, 2, 3, 2, 0, 1, 1, 1, 1, 0, 1, 3}
#define PIXEL_Z_FIG_0_180               {5, 0, 2, 3, 2, 0, 1, 1, 2, 1, 0, 1, 2}
#define PIXEL_Z_FIG_90_270              {5, 0, 3, 2, 2, 0, 0, 2, 1, 1, 1, 2, 1}
#define PIXEL_Z_REVERSE_FIG_0_180       {5, 0, 2, 3, 2, 0, 0, 1, 2, 1, 1, 1, 2}
#define PIXEL_Z_REVERSE_FIG_90_270      {5, 0, 3, 2, 2, 1, 0, 2, 1, 0, 1, 2, 1}
#define PIXEL_L_FIG_0                   {5, 0, 2, 3, 2, 0, 0, 1, 3, 1, 2, 1, 1}
#define PIXEL_L_FIG_90                  {5, 0, 3, 2, 2, 0, 0, 3, 1, 0, 1, 1, 1}
#define PIXEL_L_FIG180                  {5, 0, 2, 3, 2, 0, 0, 1, 1, 1, 0, 1, 3}
#define PIXEL_L_FIG270                  {5, 0, 3, 2, 2, 2, 0, 1, 1, 0, 1, 3, 1}
#define PIXEL_L_REVERSE_FIG_0           {5, 0, 2, 3, 2, 0, 2, 1, 1, 1, 0, 1, 3}
#define PIXEL_L_REVERSE_FIG_90          {5, 0, 3, 2, 2, 0, 0, 1, 1, 0, 1, 3, 1}
#define PIXEL_L_REVERSE_FIG_180         {5, 0, 2, 3, 2, 1, 0, 1, 1, 0, 0, 1, 3}
#define PIXEL_L_REVERSE_FIG_270         {5, 0, 3, 2, 2, 2, 1, 1, 1, 0, 0, 3, 1}

/******* 上述定义的单元是内存中的像素, 乘以PIXELAMP后对应LCD屏幕上的实际像素 ******/


#define DELLINFLASHFRQ      2       /* 删除行闪烁频率, 单位: Hz */


/* 检查冲突的操作模式 */
#define MOVE_DOWN           0       /* 图形下移 */
#define MOVE_LEFT           1       /* 图形左移 */
#define MOVE_RIGHT          2       /* 图形右移 */
#define MOVE_RTT            3       /* 图形旋转 */

#define ROTATION_0          0       /* 图形旋转0度 */
#define ROTATION_90         1       /* 图形旋转90度 */
#define ROTATION_180        2       /* 图形旋转180度 */
#define ROTATION_270        3       /* 图形旋转270度 */


#define AUTODOWNTIME        4       /* 自动向下走的时间间隔, 单位: 每个按键检查周期 */
#define NOTACTKEYTIME       2       /* 不响应按键的时间间隔, 单位: 每个按键检查周期 */


/* 图形数据 */
typedef struct figRam
{
    U16 usFigNo;        /* 图形编号 */
    U16 usRtt;          /* 图形旋转的角度 */
    U16 usClr;          /* 图形颜色 */
    U16 ausFigRam[NEXTFIG_MAX_LEN][NEXTFIG_MAX_LEN];    /* 存放图形的Ram */
}FIGRAM;

/* 图形所在的位置 */
typedef struct figpos
{
    S8 scLTX;           /* 图形左上角X轴坐标, 当前坐标 */
    S8 scLTY;           /* 图形左上角Y轴坐标, 当前坐标 */
    S8 scRBX;           /* 图形右下角X轴坐标, 当前坐标 */
    S8 scRBY;           /* 图形右下角Y轴坐标, 当前坐标 */
}FIGPOS;

/* 图形结构 */
typedef struct figStr
{
    FIGRAM strRam;
    FIGPOS strPos;
}FIGSTR;

typedef struct strkeymsgque /* TCB队列结构 */
{
    M_DLIST strQueHead;     /* 连接队列的链表 */
    U32 uiKey;              /* 按键信息 */
}STRKEYMSGQUE;

/*********************************** 变量声明 *************************************/
extern FIGSTR gstrCurFig;
extern FIGRAM gstrNextFig;
extern U16 gausMainPrtRam[MAINSCN_X_LEN][MAINSCN_Y_LEN];


/*********************************** 函数声明 *************************************/
extern void TEST_ScreenInit(void);
extern void TEST_ImageDraw(U8 ucX, U8 ucY, U8 ucXLen, U8 ucYLen, U16* pusRam);
extern void TEST_GameStart(void);
extern U32 TEST_NextFigStart(void);
extern void TEST_MainScnRamInit(void);
extern U32 TEST_CurFigStructInit(void);
extern void TEST_InitFigPos(FIGSTR* pstrFig);
extern U32 TEST_GetCurFigPosAfterRtt(FIGSTR* pstrOldFig, FIGSTR* pstrNewFig);
extern void TEST_NextFigStructInit(void);
extern void TEST_MoveFigRamInit(FIGRAM* pstrRam);
extern U8* TEST_GetFigPixelPointer(FIGRAM* pstrRam);
extern U32 TEST_CopyCurFigToMainScn(void);
extern void TEST_KeyUpProcess(void);
extern void TEST_KeyDownProcess(void);
extern void TEST_KeyLeftProcess(void);
extern void TEST_KeyRightProcess(void);
extern void TEST_KeyRestartProcess(void);
extern U8 TEST_KeyPressCheck(U32 uiKey);
extern void TEST_MainScnDownOneLine(U32 uiy);
extern U32 TEST_MoveFigOverlapCheck(U8 ucMode);
extern U32 TEST_RotationFigOverlapCheck(void);
extern U32 TEST_DelOneLineCheck(U32 uiy);
extern void TEST_OneLineFlash(U32 uiy, U32 uiStart);


#endif

