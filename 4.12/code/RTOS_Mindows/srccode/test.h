
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


/* LCD��ʾ�궨��, �������Ͻ�Ϊ����ԭ��, X�᷽������, Y�᷽������ */
#define LCDXLEN             240                         /* LCD X�᳤��, ��λ: ���� */
#define LCDYLEN             400                         /* LCD Y�᳤��, ��λ: ���� */


/******* ��������ĵ�Ԫ���ڴ��е�����, ����PIXELAMP���ӦLCD��Ļ�ϵ�ʵ������ ******/

/* Lcd��Ӧ���ڴ�ֲ��ṹ, ����������ʾ�����Ͻ�Ϊ����ԭ��, ����ΪX����������, ����ΪY
   ����������. ��Ϊ��������ʾ���͸�������ʾ��. ��������ʾ�����������Ϸ����, ������
   ��ʾ��������Ÿ�����ʾ����Ϣ */

/************************************************/
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*              ������              ||  ������  */
/*              ��ʾ��              ||  ��ʾ��  */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/*                                  ||          */
/************************************************/

/* ���طŴ��� */
#define PIXELAMP            16                      /* �ڴ���һ�����Ӧʵ�����صĵ��� */

#define LCDSCN_X_LEN        (LCDXLEN / PIXELAMP)    /* LCD��ĻX�᳤�� */
#define LCDSCN_Y_LEN        (LCDYLEN / PIXELAMP)    /* LCD��ĻY�᳤�� */

#define MAINSCN_X_LEN       11                      /* ������X�᳤�� */
#define MAINSCN_Y_LEN       LCDSCN_Y_LEN            /* ������Y�᳤�� */

#define MAINSCR_MIN_X       0                       /* ������X����С���� */
#define MAINSCR_MAX_X       (MAINSCN_X_LEN - 1)     /* ������X��������� */
#define MAINSCR_MIN_Y       0                       /* ������Y����С���� */
#define MAINSCR_MAX_Y       (LCDSCN_Y_LEN - 1)      /* ������Y��������� */

#define LINE_X_LEN          1                       /* �ָ���X�᳤�� */
#define LINE_Y_LEN          LCDSCN_Y_LEN            /* �ָ���Y�᳤�� */

#define LINE_MIN_X          MAINSCN_X_LEN           /* �ָ���X����С���� */
#define LINE_MIN_Y          0                       /* �ָ���Y����С���� */

#define SLAVESCN_X_LEN      (LCDSCN_X_LEN - MAINSCN_X_LEN - LINE_X_LEN) /* ������X�᳤�� */
#define SLAVESCN_Y_LEN      LCDSCN_Y_LEN                                /* ������Y�᳤�� */

#define SLAVESCR_MIN_X      (MAINSCN_X_LEN + LINE_X_LEN)    /* ������X����С���� */
#define SLAVESCR_MAX_X      (LCDSCN_X_LEN - 1)      /* ������X��������� */
#define SLAVESCR_MIN_Y      0                       /* ������Y����С���� */
#define SLAVESCR_MAX_Y      MAINSCR_MAX_Y           /* ������Y��������� */

#define NEXTFIG_X_LEN       3                       /* �¸���ʾͼ�ε�X�᳤�� */
#define NEXTFIG_Y_LEN       5                       /* �¸���ʾͼ�ε�Y�᳤�� */
#define NEXTFIG_MAX_LEN     NEXTFIG_Y_LEN           /* �¸���ʾͼ�����ĳ��� */

#define NEXTFIG_MIN_X       12                      /* �¸���ʾͼ��X����С���� */
#define NEXTFIG_MAX_X       (NEXTFIG_MIN_X + NEXTFIG_X_LEN - 1) /* �¸���ʾͼ��X��������� */
#define NEXTFIG_MIN_Y       5                       /* �¸���ʾͼ��Y����С���� */
#define NEXTFIG_MAX_Y       (NEXTFIG_MIN_X + NEXTFIG_Y_LEN - 1) /* �¸���ʾͼ��Y��������� */


/* ͼ����ʾ�����ض���, ��ÿ��ͼ�ε����Ͻ�Ϊ����ԭ��, ����ΪX����������, ����ΪY����
   ������, ͼ�����ʱ�����궨��, ��ʽΪ:
(���������Ͻ�X������, ���������Ͻ�Y������, X�᳤��, Y�᳤��),
(���������Ͻ�X������, ���������Ͻ�Y������, X�᳤��, Y�᳤��),
ͼ���������, (���Ͻ�X������, ���Ͻ�Y������, X�᳤��, Y�᳤��) * ͼ��������� */
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

/******* ��������ĵ�Ԫ���ڴ��е�����, ����PIXELAMP���ӦLCD��Ļ�ϵ�ʵ������ ******/


#define DELLINFLASHFRQ      2       /* ɾ������˸Ƶ��, ��λ: Hz */


/* ����ͻ�Ĳ���ģʽ */
#define MOVE_DOWN           0       /* ͼ������ */
#define MOVE_LEFT           1       /* ͼ������ */
#define MOVE_RIGHT          2       /* ͼ������ */
#define MOVE_RTT            3       /* ͼ����ת */

#define ROTATION_0          0       /* ͼ����ת0�� */
#define ROTATION_90         1       /* ͼ����ת90�� */
#define ROTATION_180        2       /* ͼ����ת180�� */
#define ROTATION_270        3       /* ͼ����ת270�� */


#define AUTODOWNTIME        4       /* �Զ������ߵ�ʱ����, ��λ: ÿ������������� */
#define NOTACTKEYTIME       2       /* ����Ӧ������ʱ����, ��λ: ÿ������������� */


/* ͼ������ */
typedef struct figRam
{
    U16 usFigNo;        /* ͼ�α�� */
    U16 usRtt;          /* ͼ����ת�ĽǶ� */
    U16 usClr;          /* ͼ����ɫ */
    U16 ausFigRam[NEXTFIG_MAX_LEN][NEXTFIG_MAX_LEN];    /* ���ͼ�ε�Ram */
}FIGRAM;

/* ͼ�����ڵ�λ�� */
typedef struct figpos
{
    S8 scLTX;           /* ͼ�����Ͻ�X������, ��ǰ���� */
    S8 scLTY;           /* ͼ�����Ͻ�Y������, ��ǰ���� */
    S8 scRBX;           /* ͼ�����½�X������, ��ǰ���� */
    S8 scRBY;           /* ͼ�����½�Y������, ��ǰ���� */
}FIGPOS;

/* ͼ�νṹ */
typedef struct figStr
{
    FIGRAM strRam;
    FIGPOS strPos;
}FIGSTR;

typedef struct strkeymsgque /* TCB���нṹ */
{
    M_DLIST strQueHead;     /* ���Ӷ��е����� */
    U32 uiKey;              /* ������Ϣ */
}STRKEYMSGQUE;

/*********************************** �������� *************************************/
extern FIGSTR gstrCurFig;
extern FIGRAM gstrNextFig;
extern U16 gausMainPrtRam[MAINSCN_X_LEN][MAINSCN_Y_LEN];


/*********************************** �������� *************************************/
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

