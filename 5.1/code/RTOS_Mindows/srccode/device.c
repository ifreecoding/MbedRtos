
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* 消息缓冲池 */

TASKSWITCHMSGSTR gstrTsMsgStr;      /* 任务切换信息 */

U16 gusPenColor;                    /* 画笔颜色 */
U16 gusBackGroundColor;             /* 背景色 */

U8 gaucFont[] =                     /* 字体点阵, 0~9, A~Z, a~z */
{
    /* 0 */
    0x00, 0x00, 0x07, 0xF0, 0x08, 0x08, 0x10, 0x04,
    0x10, 0x04, 0x08, 0x08, 0x07, 0xF0, 0x00, 0x00,

    /* 1 */
    0x00, 0x00, 0x08, 0x04, 0x08, 0x04, 0x1F, 0xFC,
    0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,

    /* 2 */
    0x00, 0x00, 0x0E, 0x0C, 0x10, 0x14, 0x10, 0x24,
    0x10, 0x44, 0x11, 0x84, 0x0E, 0x0C, 0x00, 0x00,

    /* 3 */
    0x00, 0x00, 0x0C, 0x18, 0x10, 0x04, 0x11, 0x04,
    0x11, 0x04, 0x12, 0x88, 0x0C, 0x70, 0x00, 0x00,

    /* 4 */
    0x00, 0x00, 0x00, 0xE0, 0x03, 0x20, 0x04, 0x24,
    0x08, 0x24, 0x1F, 0xFC, 0x00, 0x24, 0x00, 0x00,

    /* 5 */
    0x00, 0x00, 0x1F, 0x98, 0x10, 0x84, 0x11, 0x04,
    0x11, 0x04, 0x10, 0x88, 0x10, 0x70, 0x00, 0x00,

    /* 6 */
    0x00, 0x00, 0x07, 0xF0, 0x08, 0x88, 0x11, 0x04,
    0x11, 0x04, 0x18, 0x88, 0x00, 0x70, 0x00, 0x00,

    /* 7 */
    0x00, 0x00, 0x1C, 0x00, 0x10, 0x00, 0x10, 0xFC,
    0x13, 0x00, 0x1C, 0x00, 0x10, 0x00, 0x00, 0x00,

    /* 8 */
    0x00, 0x00, 0x0E, 0x38, 0x11, 0x44, 0x10, 0x84,
    0x10, 0x84, 0x11, 0x44, 0x0E, 0x38, 0x00, 0x00,

    /* 9 */
    0x00, 0x00, 0x07, 0x00, 0x08, 0x8C, 0x10, 0x44,
    0x10, 0x44, 0x08, 0x88, 0x07, 0xF0, 0x00, 0x00,

    /* A */
    0x00, 0x04, 0x00, 0x3C, 0x03, 0xC4, 0x1C, 0x40,
    0x07, 0x40, 0x00, 0xE4, 0x00, 0x1C, 0x00, 0x04,

    /* B */
    0x10, 0x04, 0x1F, 0xFC, 0x11, 0x04, 0x11, 0x04,
    0x11, 0x04, 0x0E, 0x88, 0x00, 0x70, 0x00, 0x00,

    /* C */
    0x03, 0xE0, 0x0C, 0x18, 0x10, 0x04, 0x10, 0x04,
    0x10, 0x04, 0x10, 0x08, 0x1C, 0x10, 0x00, 0x00,

    /* D */
    0x10, 0x04, 0x1F, 0xFC, 0x10, 0x04, 0x10, 0x04,
    0x10, 0x04, 0x08, 0x08, 0x07, 0xF0, 0x00, 0x00,

    /* E */
    0x10, 0x04, 0x1F, 0xFC, 0x11, 0x04, 0x11, 0x04,
    0x17, 0xC4, 0x10, 0x04, 0x08, 0x18, 0x00, 0x00,

    /* F */
    0x10, 0x04, 0x1F, 0xFC, 0x11, 0x04, 0x11, 0x00,
    0x17, 0xC0, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00,

    /* G */
    0x03, 0xE0, 0x0C, 0x18, 0x10, 0x04, 0x10, 0x04,
    0x10, 0x44, 0x1C, 0x78, 0x00, 0x40, 0x00, 0x00,

    /* H */
    0x10, 0x04, 0x1F, 0xFC, 0x10, 0x84, 0x00, 0x80,
    0x00, 0x80, 0x10, 0x84, 0x1F, 0xFC, 0x10, 0x04,

    /* I */
    0x00, 0x00, 0x10, 0x04, 0x10, 0x04, 0x1F, 0xFC,
    0x10, 0x04, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00,

    /* J */
    0x00, 0x03, 0x00, 0x01, 0x10, 0x01, 0x10, 0x01,
    0x1F, 0xFE, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00,

    /* K */
    0x10, 0x04, 0x1F, 0xFC, 0x11, 0x04, 0x03, 0x80,
    0x14, 0x64, 0x18, 0x1C, 0x10, 0x04, 0x00, 0x00,

    /* L */
    0x10, 0x04, 0x1F, 0xFC, 0x10, 0x04, 0x00, 0x04,
    0x00, 0x04, 0x00, 0x04, 0x00, 0x0C, 0x00, 0x00,

    /* M */
    0x10, 0x04, 0x1F, 0xFC, 0x1F, 0x00, 0x00, 0xFC,
    0x1F, 0x00, 0x1F, 0xFC, 0x10, 0x04, 0x00, 0x00,

    /* N */
    0x10, 0x04, 0x1F, 0xFC, 0x0C, 0x04, 0x03, 0x00,
    0x00, 0xE0, 0x10, 0x18, 0x1F, 0xFC, 0x10, 0x00,

    /* O */
    0x07, 0xF0, 0x08, 0x08, 0x10, 0x04, 0x10, 0x04,
    0x10, 0x04, 0x08, 0x08, 0x07, 0xF0, 0x00, 0x00,

    /* P */
    0x10, 0x04, 0x1F, 0xFC, 0x10, 0x84, 0x10, 0x80,
    0x10, 0x80, 0x10, 0x80, 0x0F, 0x00, 0x00, 0x00,

    /* Q */
    0x07, 0xF0, 0x08, 0x18, 0x10, 0x24, 0x10, 0x24,
    0x10, 0x1C, 0x08, 0x0A, 0x07, 0xF2, 0x00, 0x00,

    /* R */
    0x10, 0x04, 0x1F, 0xFC, 0x11, 0x04, 0x11, 0x00,
    0x11, 0xC0, 0x11, 0x30, 0x0E, 0x0C, 0x00, 0x04,

    /* S */
    0x00, 0x00, 0x0E, 0x1C, 0x11, 0x04, 0x10, 0x84,
    0x10, 0x84, 0x10, 0x44, 0x1C, 0x38, 0x00, 0x00,

    /* T */
    0x18, 0x00, 0x10, 0x00, 0x10, 0x04, 0x1F, 0xFC,
    0x10, 0x04, 0x10, 0x00, 0x18, 0x00, 0x00, 0x00,

    /* U */
    0x10, 0x00, 0x1F, 0xF8, 0x10, 0x04, 0x00, 0x04,
    0x00, 0x04, 0x10, 0x04, 0x1F, 0xF8, 0x10, 0x00,

    /* V */
    0x10, 0x00, 0x1E, 0x00, 0x11, 0xE0, 0x00, 0x1C,
    0x00, 0x70, 0x13, 0x80, 0x1C, 0x00, 0x10, 0x00,

    /* W */
    0x1F, 0xC0, 0x10, 0x3C, 0x00, 0xE0, 0x1F, 0x00,
    0x00, 0xE0, 0x10, 0x3C, 0x1F, 0xC0, 0x00, 0x00,

    /* X */
    0x10, 0x04, 0x18, 0x0C, 0x16, 0x34, 0x01, 0xC0,
    0x01, 0xC0, 0x16, 0x34, 0x18, 0x0C, 0x10, 0x04,

    /* Y */
    0x10, 0x00, 0x1C, 0x00, 0x13, 0x04, 0x00, 0xFC,
    0x13, 0x04, 0x1C, 0x00, 0x10, 0x00, 0x00, 0x00,

    /* Z */
    0x08, 0x04, 0x10, 0x1C, 0x10, 0x64, 0x10, 0x84,
    0x13, 0x04, 0x1C, 0x04, 0x10, 0x18, 0x00, 0x00,

    /* a */
    0x00, 0x00, 0x00, 0x98, 0x01, 0x24, 0x01, 0x44,
    0x01, 0x44, 0x01, 0x44, 0x00, 0xFC, 0x00, 0x04,

    /* b */
    0x10, 0x00, 0x1F, 0xFC, 0x00, 0x88, 0x01, 0x04,
    0x01, 0x04, 0x00, 0x88, 0x00, 0x70, 0x00, 0x00,

    /* c */
    0x00, 0x00, 0x00, 0x70, 0x00, 0x88, 0x01, 0x04,
    0x01, 0x04, 0x01, 0x04, 0x00, 0x88, 0x00, 0x00,

    /* d */
    0x00, 0x00, 0x00, 0x70, 0x00, 0x88, 0x01, 0x04,
    0x01, 0x04, 0x11, 0x08, 0x1F, 0xFC, 0x00, 0x04,

    /* e */
    0x00, 0x00, 0x00, 0xF8, 0x01, 0x44, 0x01, 0x44,
    0x01, 0x44, 0x01, 0x44, 0x00, 0xC8, 0x00, 0x00,

    /* f */
    0x00, 0x00, 0x01, 0x04, 0x01, 0x04, 0x0F, 0xFC,
    0x11, 0x04, 0x11, 0x04, 0x11, 0x00, 0x18, 0x00,

    /* g */
    0x00, 0x00, 0x00, 0xD6, 0x01, 0x29, 0x01, 0x29,
    0x01, 0x29, 0x01, 0xC9, 0x01, 0x06, 0x00, 0x00,

    /* h */
    0x10, 0x04, 0x1F, 0xFC, 0x00, 0x84, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x04, 0x00, 0xFC, 0x00, 0x04,

    /* i */
    0x00, 0x00, 0x01, 0x04, 0x19, 0x04, 0x19, 0xFC,
    0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,

    /* j */
    0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x01, 0x01,
    0x19, 0x01, 0x19, 0xFE, 0x00, 0x00, 0x00, 0x00,

    /* k */
    0x10, 0x04, 0x1F, 0xFC, 0x00, 0x24, 0x00, 0x40,
    0x01, 0xB4, 0x01, 0x0C, 0x01, 0x04, 0x00, 0x00,

    /* l */
    0x00, 0x00, 0x10, 0x04, 0x10, 0x04, 0x1F, 0xFC,
    0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,

    /* m */
    0x01, 0x04, 0x01, 0xFC, 0x01, 0x04, 0x01, 0x00,
    0x01, 0xFC, 0x01, 0x04, 0x01, 0x00, 0x00, 0xFC,

    /* n */
    0x01, 0x04, 0x01, 0xFC, 0x00, 0x84, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x04, 0x00, 0xFC, 0x00, 0x04,

    /* o */
    0x00, 0x00, 0x00, 0xF8, 0x01, 0x04, 0x01, 0x04,
    0x01, 0x04, 0x01, 0x04, 0x00, 0xF8, 0x00, 0x00,

    /* p */
    0x01, 0x01, 0x01, 0xFF, 0x00, 0x85, 0x01, 0x04,
    0x01, 0x04, 0x00, 0x88, 0x00, 0x70, 0x00, 0x00,

    /* q */
    0x00, 0x00, 0x00, 0x70, 0x00, 0x88, 0x01, 0x04,
    0x01, 0x04, 0x01, 0x05, 0x01, 0xFF, 0x00, 0x01,

    /* r */
    0x01, 0x04, 0x01, 0x04, 0x01, 0xFC, 0x00, 0x84,
    0x01, 0x04, 0x01, 0x00, 0x01, 0x80, 0x00, 0x00,

    /* s */
    0x00, 0x00, 0x00, 0xCC, 0x01, 0x24, 0x01, 0x24,
    0x01, 0x24, 0x01, 0x24, 0x01, 0x98, 0x00, 0x00,

    /* t */
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x07, 0xF8,
    0x01, 0x04, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00,

    /* u */
    0x01, 0x00, 0x01, 0xF8, 0x00, 0x04, 0x00, 0x04,
    0x00, 0x04, 0x01, 0x08, 0x01, 0xFC, 0x00, 0x04,

    /* v */
    0x01, 0x00, 0x01, 0x80, 0x01, 0x70, 0x00, 0x0C,
    0x00, 0x10, 0x01, 0x60, 0x01, 0x80, 0x01, 0x00,

    /* w */
    0x01, 0xF0, 0x01, 0x0C, 0x00, 0x30, 0x01, 0xC0,
    0x00, 0x30, 0x01, 0x0C, 0x01, 0xF0, 0x01, 0x00,

    /* x */
    0x00, 0x00, 0x01, 0x04, 0x01, 0x8C, 0x00, 0x74,
    0x01, 0x70, 0x01, 0x8C, 0x01, 0x04, 0x00, 0x00,

    /* y */
    0x01, 0x01, 0x01, 0x81, 0x01, 0x71, 0x00, 0x0E,
    0x00, 0x18, 0x01, 0x60, 0x01, 0x80, 0x01, 0x00,

    /* z */
    0x00, 0x00, 0x01, 0x84, 0x01, 0x0C, 0x01, 0x34,
    0x01, 0x44, 0x01, 0x84, 0x01, 0x0C, 0x00, 0x00
};

U8 gaucInvertFont[] =               /* 字体点阵, 顺时针旋转90度, 0~9, A~Z, a~z */
{
    /* 0 */
    0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00, 0x00,

    /* 1 */
    0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x70, 0x10, 0x00, 0x00, 0x00,

    /* 2 */
    0x00, 0x00, 0x7E, 0x42, 0x20, 0x10, 0x08, 0x04,
    0x04, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00,

    /* 3 */
    0x00, 0x00, 0x38, 0x44, 0x42, 0x02, 0x02, 0x04,
    0x18, 0x04, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00,

    /* 4 */
    0x00, 0x00, 0x1E, 0x04, 0x04, 0x7E, 0x44, 0x44,
    0x24, 0x24, 0x14, 0x0C, 0x04, 0x00, 0x00, 0x00,

    /* 5 */
    0x00, 0x00, 0x38, 0x44, 0x42, 0x02, 0x02, 0x64,
    0x58, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00,

    /* 6 */
    0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x64,
    0x58, 0x40, 0x40, 0x24, 0x1C, 0x00, 0x00, 0x00,

    /* 7 */
    0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x08, 0x08, 0x44, 0x44, 0x7E, 0x00, 0x00, 0x00,

    /* 8 */
    0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x24, 0x18,
    0x24, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00,

    /* 9 */
    0x00, 0x00, 0x38, 0x24, 0x02, 0x02, 0x1A, 0x26,
    0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00, 0x00,

    /* A */
    0x00, 0x00, 0xE7, 0x42, 0x42, 0x44, 0x3C, 0x24,
    0x28, 0x28, 0x18, 0x10, 0x10, 0x00, 0x00, 0x00,

    /* B */
    0x00, 0x00, 0xF8, 0x44, 0x42, 0x42, 0x42, 0x44,
    0x78, 0x44, 0x44, 0x44, 0xF8, 0x00, 0x00, 0x00,

    /* C */
    0x00, 0x00, 0x38, 0x44, 0x42, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00,

    /* D */
    0x00, 0x00, 0xF8, 0x44, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x44, 0xF8, 0x00, 0x00, 0x00,

    /* E */
    0x00, 0x00, 0xFC, 0x42, 0x42, 0x40, 0x48, 0x48,
    0x78, 0x48, 0x48, 0x42, 0xFC, 0x00, 0x00, 0x00,

    /* F */
    0x00, 0x00, 0xE0, 0x40, 0x40, 0x40, 0x48, 0x48,
    0x78, 0x48, 0x48, 0x42, 0xFC, 0x00, 0x00, 0x00,

    /* G */
    0x00, 0x00, 0x38, 0x44, 0x44, 0x84, 0x8E, 0x80,
    0x80, 0x80, 0x44, 0x44, 0x3C, 0x00, 0x00, 0x00,

    /* H */
    0x00, 0x00, 0xE7, 0x42, 0x42, 0x42, 0x42, 0x7E,
    0x42, 0x42, 0x42, 0x42, 0xE7, 0x00, 0x00, 0x00,

    /* I */
    0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, 0x00,

    /* J */
    0xF0, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, 0x00, 0x00,

    /* K */
    0x00, 0x00, 0xEE, 0x44, 0x44, 0x48, 0x48, 0x50,
    0x70, 0x50, 0x48, 0x44, 0xEE, 0x00, 0x00, 0x00,

    /* L */
    0x00, 0x00, 0xFE, 0x42, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x40, 0x40, 0x40, 0xE0, 0x00, 0x00, 0x00,

    /* M */
    0x00, 0x00, 0xD6, 0x54, 0x54, 0x54, 0x54, 0x54,
    0x6C, 0x6C, 0x6C, 0x6C, 0xEE, 0x00, 0x00, 0x00,

    /* N */
    0x00, 0x00, 0xE2, 0x46, 0x46, 0x4A, 0x4A, 0x4A,
    0x52, 0x52, 0x62, 0x62, 0xC7, 0x00, 0x00, 0x00,

    /* O */
    0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00,

    /* P */
    0x00, 0x00, 0xE0, 0x40, 0x40, 0x40, 0x40, 0x7C,
    0x42, 0x42, 0x42, 0x42, 0xFC, 0x00, 0x00, 0x00,

    /* Q */
    0x00, 0x06, 0x38, 0x4C, 0xCA, 0xB2, 0x82, 0x82,
    0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00,

    /* R */
    0x00, 0x00, 0xE3, 0x42, 0x44, 0x44, 0x48, 0x48,
    0x7C, 0x42, 0x42, 0x42, 0xFC, 0x00, 0x00, 0x00,

    /* S */
    0x00, 0x00, 0x7C, 0x42, 0x42, 0x02, 0x04, 0x18,
    0x20, 0x40, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00,

    /* T */
    0x00, 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x92, 0xFE, 0x00, 0x00, 0x00,

    /* U */
    0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x42, 0xE7, 0x00, 0x00, 0x00,

    /* V */
    0x00, 0x00, 0x10, 0x10, 0x18, 0x28, 0x28, 0x24,
    0x24, 0x44, 0x42, 0x42, 0xE7, 0x00, 0x00, 0x00,

    /* W */
    0x00, 0x00, 0x44, 0x44, 0x44, 0x6C, 0xAA, 0xAA,
    0x92, 0x92, 0x92, 0x92, 0xD6, 0x00, 0x00, 0x00,

    /* X */
    0x00, 0x00, 0xE7, 0x42, 0x24, 0x24, 0x18, 0x18,
    0x18, 0x24, 0x24, 0x42, 0xE7, 0x00, 0x00, 0x00,

    /* Y */
    0x00, 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x28, 0x28, 0x44, 0x44, 0xEE, 0x00, 0x00, 0x00,

    /* Z */
    0x00, 0x00, 0xFC, 0x42, 0x42, 0x20, 0x20, 0x10,
    0x08, 0x08, 0x04, 0x84, 0x7E, 0x00, 0x00, 0x00,

    /* a */
    0x00, 0x00, 0x3F, 0x42, 0x42, 0x22, 0x1E, 0x42,
    0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* b */
    0x00, 0x00, 0x58, 0x64, 0x42, 0x42, 0x42, 0x64,
    0x58, 0x40, 0x40, 0x40, 0xC0, 0x00, 0x00, 0x00,

    /* c */
    0x00, 0x00, 0x1C, 0x22, 0x40, 0x40, 0x40, 0x22,
    0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* d */
    0x00, 0x00, 0x1B, 0x26, 0x42, 0x42, 0x42, 0x22,
    0x1E, 0x02, 0x02, 0x02, 0x06, 0x00, 0x00, 0x00,

    /* e */
    0x00, 0x00, 0x3C, 0x42, 0x40, 0x40, 0x7E, 0x42,
    0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* f */
    0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x7E, 0x10, 0x10, 0x11, 0x0F, 0x00, 0x00, 0x00,

    /* g */
    0x3C, 0x42, 0x42, 0x3C, 0x40, 0x38, 0x44, 0x44,
    0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* h */
    0x00, 0x00, 0xE7, 0x42, 0x42, 0x42, 0x42, 0x62,
    0x5C, 0x40, 0x40, 0x40, 0xC0, 0x00, 0x00, 0x00,

    /* i */
    0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x70, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00,

    /* j */
    0x78, 0x44, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x1C, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x00,

    /* k */
    0x00, 0x00, 0xEE, 0x44, 0x48, 0x68, 0x50, 0x48,
    0x4E, 0x40, 0x40, 0x40, 0xC0, 0x00, 0x00, 0x00,

    /* l */
    0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x70, 0x00, 0x00, 0x00,

    /* m */
    0x00, 0x00, 0xED, 0x49, 0x49, 0x49, 0x49, 0x49,
    0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* n */
    0x00, 0x00, 0xE7, 0x42, 0x42, 0x42, 0x42, 0x62,
    0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* o */
    0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* p */
    0xE0, 0x40, 0x78, 0x44, 0x42, 0x42, 0x42, 0x64,
    0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* q */
    0x07, 0x02, 0x1E, 0x22, 0x42, 0x42, 0x42, 0x22,
    0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* r */
    0x00, 0x00, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x32,
    0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* s */
    0x00, 0x00, 0x7C, 0x42, 0x02, 0x3C, 0x40, 0x42,
    0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* t */
    0x00, 0x00, 0x0C, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x7C, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* u */
    0x00, 0x00, 0x3B, 0x46, 0x42, 0x42, 0x42, 0x42,
    0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* v */
    0x00, 0x00, 0x10, 0x10, 0x28, 0x24, 0x24, 0x42,
    0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* w */
    0x00, 0x00, 0x44, 0x44, 0xAA, 0xAA, 0x92, 0x92,
    0xD7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* x */
    0x00, 0x00, 0x76, 0x24, 0x18, 0x18, 0x18, 0x24,
    0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* y */
    0xE0, 0x10, 0x10, 0x18, 0x28, 0x24, 0x24, 0x42,
    0xE7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* z */
    0x00, 0x00, 0x7E, 0x22, 0x10, 0x10, 0x08, 0x44,
    0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/***********************************************************************************
函数功能: 初始化软件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* 初始化消息缓冲池 */
    DEV_BufferInit(&gstrBufPool);

    /* 初始化任务切换信息结构 */
    DEV_TaskSwitchMsgStrInit();

    /* 创建打印队列和信号量 */
    gpstrSerialMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);
    gpstrLcdMsgSem = MDS_SemCreate((M_SEM*)NULL, SEMBIN | SEMPRIO, SEMEMPTY);

    /* 创建互斥信号量, 有任务优先级继承功能 */
    gpstrSemMut = MDS_SemCreate((M_SEM*)NULL, SEMPRIINH | SEMMUT | SEMPRIO,
                                SEMFULL);

#ifdef MDS_INCLUDETASKHOOK

    /* 挂接任务钩子函数 */
    MDS_TaskCreateHookAdd(TEST_TaskCreatePrint);
    MDS_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
    MDS_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/***********************************************************************************
函数功能: 初始化硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* 初始化硬件版本号IO */
    DEV_HardwareVersionInit();

    /* 初始化串口 */
    DEV_UartInit();

    /* 初始化LCD液晶屏 */
    DEV_LcdInit();

    /* 任务切换显示图形初始化 */
    DEV_TaskSwitchDisplayInit();
}

/***********************************************************************************
函数功能: 初始化串口.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_UartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - Hardware flow control disabled (RTS and CTS signals)
    */
    USART_StructInit(&USART_InitStructure);

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

/***********************************************************************************
函数功能: 向串口发送一个字符.
入口参数: ucChar: 输出的字符.
返 回 值: none.
***********************************************************************************/
void DEV_PutChar(U8 ucChar)
{
    /* Loop until USART1 DR register is empty */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        ;
    }

    USART_SendData(USART1, ucChar);
}

/***********************************************************************************
函数功能: 将字符串打印到内存. 该函数入口参数与printf函数一致.
入口参数: pvStringPt: 第一个参数的指针, 必须为字符串指针.
          ...: 其它参数.
返 回 值: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* 申请buf, 用来存放需要打印的字符 */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* 将字符串打印到内存 */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* 填充buf参数 */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* 将buf挂入队列 */
    (void)MDS_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
函数功能: 向串口打印消息. 从内存中指定地址向串口连续打印一定长度的数据, 采用串口查询
          方式打印.
入口参数: pucBuf: 需要打印数据存放的缓冲地址.
          uiLen: 需要打印数据的长度, 单位: 字节.
返 回 值: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* 采用查询的方式发送数据 */
    while(uiCounter < uiLen)
    {
        /* 向串口发一个字符 */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
函数功能: 初始化LCD液晶屏.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdInit(void)
{
    U8 ucHwVer;

    /* 读取硬件版本号 */
    ucHwVer = DEV_ReadHardwareVersion();

    DEV_LcdGpioCfg();
    DEV_LcdFsmcCfg();
    DEV_LcdReset();
    DEV_LcdLightOn();

    DEV_LcdWriteCmd(0x11);      //SLP OUT
    DEV_DelayMs(1);

    DEV_LcdWriteCmd(0x3A);      //Set_pixel_format
    DEV_LcdWriteData(0x55);     //0x55: 16bit/pixel, 65k; 0x66: 18bit/pixel, 262k;

    DEV_LcdWriteCmd(0x36);      //Set_address_mode

    if(0 == ucHwVer)            //液晶屏使用S6D04D1X21驱动器
    {
        DEV_LcdWriteData(0x78);
    }
    else if(1 == ucHwVer)       //液晶屏使用HX8352C驱动器
    {
        DEV_LcdWriteData(0x3A);
        DEV_LcdWriteCmd(0x55);
    }

    DEV_LcdWriteCmd(0x29);      //Set_display_on
    DEV_DelayMs(1);
}

/***********************************************************************************
函数功能: 初始化控制LCD的GPIO.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdGpioCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*  LED灯, RESET输出管脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FLM输入管脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/***********************************************************************************
函数功能: 初始化LCD使用的FSMC管脚和FSMC时序.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdFsmcCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef p;

    /* Enable FSMC pin Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* config tft data lines base on FSMC data lines,
     * FSMC-D0~D15: PD 0 1 4 5 7 8 9 10 11 14 15, PE 7 8 9 10 11 12 13 14 15
     */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7
                                  | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14
                                  | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
                                  | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    p.FSMC_AddressSetupTime = 0x02;
    p.FSMC_AddressHoldTime = 0x00;
    p.FSMC_DataSetupTime = 0x05;
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;

    p.FSMC_AccessMode = FSMC_AccessMode_B;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/***********************************************************************************
函数功能: 复位LCD液晶屏.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdReset(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
}

/***********************************************************************************
函数功能: 点亮LCD液晶屏背光灯.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdLightOn(void)
{
    /* 打开LCD背光灯 */
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
函数功能: 关闭LCD液晶屏背光灯.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_LcdLightOff(void)
{
    /* 关闭LCD背光灯 */
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
函数功能: 向LCD写命令.
入口参数: usCmd: 写入的命令.
返 回 值: none.
***********************************************************************************/
void DEV_LcdWriteCmd(U16 usCmd)
{
    *(volatile U16*)(FSMCCMDADDR) = usCmd;
}

/***********************************************************************************
函数功能: 向LCD写数据.
入口参数: usData: 写入的数据.
返 回 值: none.
***********************************************************************************/
void DEV_LcdWriteData(U16 usData)
{
    *(volatile U16*)(FSMCDATAADDR) = usData;
}

/***********************************************************************************
函数功能: 设定LCD一块区域, 后续写操作将向该区域写入数据, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usXEnd: X轴结束坐标点.
          usYStart: Y轴开始坐标点.
          usYEnd: Y轴结束坐标点.
返 回 值: none.
***********************************************************************************/
void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd)
{
    /* 设定X轴范围 */
    DEV_LcdWriteCmd(0x2B);
    DEV_LcdWriteData(usXStart >> 8);
    DEV_LcdWriteData(usXStart & 0xFF);
    DEV_LcdWriteData(usXEnd >> 8);
    DEV_LcdWriteData(usXEnd & 0xFF);

    /* 设定Y轴范围 */
    DEV_LcdWriteCmd(0x2A);
    DEV_LcdWriteData(usYStart >> 8);
    DEV_LcdWriteData(usYStart & 0xFF);
    DEV_LcdWriteData(usYEnd >> 8);
    DEV_LcdWriteData(usYEnd & 0xFF);

    /* 写入写屏命令, 后续写入的数据会显示在屏上 */
    DEV_LcdWriteCmd(0x2C);
}

/***********************************************************************************
函数功能: 在LCD中指定的位置填充矩形图案, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点.
          usXLen: Y轴长度.
          usYLen: Y轴长度.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen)
{
    U32 i;

    /* 设定写入的范围 */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* 写入数据 */
    for(i = 0; i < usXLen * usYLen; i++)
    {
        *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
    }
}

/***********************************************************************************
函数功能: 沿X轴方向画一条与X轴平行的直线, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点, 为线宽度的中间线.
          usLen: 长度, X轴方向.
          usWid: 宽度, Y轴方向, 应该是奇数.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart, usYStart - usWid / 2, usLen, usWid);
}

/***********************************************************************************
函数功能: 沿Y轴方向画一条与Y轴平行的直线, 使用画笔颜色, 单位: 像素.
入口参数: usXStart: X轴开始坐标点, 为线宽度的中间线.
          usYStart: Y轴开始坐标点.
          usLen: 长度, Y轴方向.
          usWid: 宽度, X轴方向, 应该是奇数.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart - usWid / 2, usYStart, usWid, usLen);
}

/***********************************************************************************
函数功能: 在LCD中指定的位置填充图案, 必须是矩形的图案, X轴必须是8的倍数, 单位: 像素.
入口参数: usXStart: X轴开始坐标点.
          usYStart: Y轴开始坐标点.
          usXLen: Y轴长度.
          usYLen: Y轴长度.
          pucPic: 填充图案存放的内存地址, 内存中的1代表使用画笔颜色, 0保持背景色.
返 回 值: none.
***********************************************************************************/
void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic)
{
    U32 i;
    S32 j;

    /* 设定写入的范围 */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* 按字节写入数据 */
    for(i = 0; i < usXLen * usYLen / 8; i++)
    {
        /* 判断字节中每个bit */
        for(j = 7; j >= 0; j--)
        {
            /* 只有bit为1才写入画笔颜色 */
            if(0x01 == ((pucPic[i] >> j) & 0x01))
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
            }
            else /* 写入背景色 */
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETBACKGROUNDCOLOR;
            }
        }
    }
}

/***********************************************************************************
函数功能: 计算存放字符的地址, 只能是0~9, A~Z, a~z之间的字符.
入口参数: ucAscii: 字符的ASCII码.
返 回 值: 字符的存储地址, NULL为无效地址.
***********************************************************************************/
U8* DEV_CalcFontAddr(U8 ucAscii)
{
    /* 无效字符 */
    if(!(((ucAscii >= '0') && (ucAscii <= '9')) || ((ucAscii >= 'A') && (ucAscii <= 'Z'))
         || ((ucAscii >= 'a') && (ucAscii <= 'z'))))
    {
        return NULL;
    }

    /* 0~9之间的字符 */
    if(ucAscii <= '9')
    {
        return &gaucFont[(ucAscii - '0') * 16];
    }
    /* A~Z之间的字符 */
    else if(ucAscii <= 'Z')
    {
        return &gaucFont[((ucAscii - 'A') * 16) + (16 * 10)];
    }
    /* a~z之间的字符 */
    else
    {
        return &gaucFont[((ucAscii - 'a') * 16) + (16 * (10 + 26))];
    }
}

/***********************************************************************************
函数功能: 计算存放旋转字符的地址, 只能是0~9, A~Z, a~z之间的字符.
入口参数: ucAscii: 字符的ASCII码.
返 回 值: 字符的存储地址, NULL为无效地址.
***********************************************************************************/
U8* DEV_CalcInvertFontAddr(U8 ucAscii)
{
    /* 无效字符 */
    if(!(((ucAscii >= '0') && (ucAscii <= '9')) || ((ucAscii >= 'A') && (ucAscii <= 'Z'))
         || ((ucAscii >= 'a') && (ucAscii <= 'z'))))
    {
        return NULL;
    }

    /* 0~9之间的字符 */
    if(ucAscii <= '9')
    {
        return &gaucInvertFont[(ucAscii - '0') * 16];
    }
    /* A~Z之间的字符 */
    else if(ucAscii <= 'Z')
    {
        return &gaucInvertFont[((ucAscii - 'A') * 16) + (16 * 10)];
    }
    /* a~z之间的字符 */
    else
    {
        return &gaucInvertFont[((ucAscii - 'a') * 16) + (16 * (10 + 26))];
    }
}

/***********************************************************************************
函数功能: 任务切换显示图形初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_TaskSwitchDisplayInit(void)
{
    U32 i;

    /* LCD清屏 */
    LCDSETPENCOLOR(LCDCOLORWHITE);
    DEV_LcdDrawBox(0, 0, LCDXLEN, LCDYLEN);

    /********** 画X和Y轴坐标 **********/
    /* 设置画笔颜色和背景色 */
    LCDSETPENCOLOR(LCDCOLORBLACK);
    LCDSETBACKGROUNDCOLOR(LCDCOLORWHITE);

    /* 画X轴和Y轴 */
    DEV_LcdDrawXLine(TSDSPXSTART, TSDSPYSTART, TSDSPXLEN, TSDSPXWID);
    DEV_LcdDrawYLine(TSDSPXSTART, TSDSPYSTART, TSDSPYLEN, TSDSPYWID);

    /* 显示空闲任务名 */
    DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + TSDSPTASKITV - 8,
                   (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2,
                   16,
                   8,
                   DEV_CalcInvertFontAddr('I'));

    DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + TSDSPTASKITV - 8,
                   (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2 + 8,
                   16,
                   8,
                   DEV_CalcInvertFontAddr('D'));

    /* 显示用户创建的任务名 */
    for(i = 0; i < TSDSPTASKNUM - 2; i++)
    {
        DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + (TSDSPTASKITV * (i + 2)) - 8,
                       (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2,
                       16,
                       8,
                       DEV_CalcInvertFontAddr('T'));

        DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + (TSDSPTASKITV * (i + 2)) - 8,
                       (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2 + 8,
                       16,
                       8,
                       DEV_CalcInvertFontAddr('1' + i));
    }

    /* 显示根任务名 */
    DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + (TSDSPTASKITV * TSDSPTASKNUM) - 8,
                   (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2,
                   16,
                   8,
                   DEV_CalcInvertFontAddr('R'));

    DEV_LcdDrawPic(TSDSPXSTART + (TSDSPYWID / 2) + (TSDSPTASKITV * TSDSPTASKNUM) - 8,
                   (TSDSPYSTART - (TSDSPXWID / 2) - 16) / 2 + 8,
                   16,
                   8,
                   DEV_CalcInvertFontAddr('T'));

    /* 显示任务线 */
    for(i = 0; i < TSDSPTASKNUM; i++)
    {
        DEV_LcdDrawYLine(TSDSPXSTART + (TSDSPTASKITV * (i + 1)),
                         TSDSPYSTART,
                         TSDSPLEN,
                         1);
    }

    /* 显示时间刻度 */
    for(i = 0; i < TSDSPTIMENUM; i++)
    {
        DEV_LcdDrawXLine(TSDSPXSTART,
                         TSDSPYSTART + (TSDSPTIMEITV * (i + 1)),
                         TSDSPTIMELEN,
                         TSDSPTIMEWID);
    }

    /* 显示时间 */
    DEV_PrintTaskSwitchTime(TSDSPTIMENUM * TSDSPBTNUM);
}

/***********************************************************************************
函数功能: 初始化任务切换信息结构.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_TaskSwitchMsgStrInit(void)
{
    gstrTsMsgStr.uiHead = 0;
    gstrTsMsgStr.uiTail = 0;

    gstrTsMsgStr.uiFlag = TSFLAGNOTUPDATA;
}

/***********************************************************************************
函数功能: 保存任务切换信息.
入口参数: pucTaskName: 存放任务名称的字符串指针.
返 回 值: none.
***********************************************************************************/
void DEV_SaveTaskSwitchMsg(U8* pucTaskName)
{
    U32 uiTask;
    U32 uiPreTail;

    /* 确定任务编号 */
    if(0 == strcmp(ROOTTASKNAME, pucTaskName))
    {
        /* 根任务编号为为最大任务数 */
        uiTask = TSDSPTASKNUM;
    }
    else if(0 == strcmp(IDLETASKNAME, pucTaskName))
    {
        /* 空闲任务编号1 */
        uiTask = 1;
    }
    else
    {
        /* 其它任务编号为任务名中的编号+1 */
        uiTask = pucTaskName[4] - '0' + 1;
    }

    /* 存储任务切换信息 */
    gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTask = uiTask;
    gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTolalTime = MDS_GetSystemTick() / TSDSPSWTTICKNUM; /* 受LCD像素点的限制, 任务切换图形只能
                                                                                                      精确到TSDSPSWTTICKNUM个tick */

    /* 至少存储了2个信息 */
    if(gstrTsMsgStr.uiHead != gstrTsMsgStr.uiTail)
    {
        /* 计算倒数第二个存储任务信息的位置 */
        uiPreTail = (gstrTsMsgStr.uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

        /* 存储上个任务的运行时间 */
        gstrTsMsgStr.astrMsg[uiPreTail].uiRunTime = gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTolalTime
                                                    - gstrTsMsgStr.astrMsg[uiPreTail].uiTolalTime;
    }

    /* 更新环形缓冲区尾位置 */
    gstrTsMsgStr.uiTail++;
    gstrTsMsgStr.uiTail %= TASKSWITCHMSGLEN;

    /* 环形缓冲区已满 */
    if(gstrTsMsgStr.uiHead == gstrTsMsgStr.uiTail)
    {
        /* 更新环形缓冲区头位置 */
        gstrTsMsgStr.uiHead++;
        gstrTsMsgStr.uiHead %= TASKSWITCHMSGLEN;
    }

    /* 信息已经更新完毕, 置为需要更新标志 */
    gstrTsMsgStr.uiFlag = TSFLAGUPDATA;

    /* 环形缓冲区已经更新完毕, 释放LCD打印信号量 */
    (void)MDS_SemGive(gpstrLcdMsgSem);
}

/***********************************************************************************
函数功能: 将任务切换信息打印到LCD屏幕上.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_PrintTaskSwitchMsgToLcd(void)
{
    TASKSWITCHMSGSTR strTsMsgStr;
    U32 uiHead;
    U32 uiTail;
    U32 uiDisplayLen;
    U32 uiLeftDisplayLen;
    U32 uiNextTask;
    U32 i;

    /* 任务切换图形不需要更新 */
    if(TSFLAGUPDATA != gstrTsMsgStr.uiFlag)
    {
        return;
    }

    /* 为避免该函数在操作全局变量gstrTsMsgStr结构时被其它任务改写, 此处在锁中断状态
       下将gstrTsMsgStr结构保存到局部变量strTsMsgStr结构中, 此后该函数对strTsMsgStr
       结构进行操作 */
    (void)MDS_IntLock();

    /* 复制任务切换信息 */
    strTsMsgStr = gstrTsMsgStr;

    /* 信息已经更新完毕, 置为不需要更新标志 */
    gstrTsMsgStr.uiFlag = TSFLAGNOTUPDATA;

    (void)MDS_IntUnlock();

    /* 清除任务切换显示区域 */
    LCDSETPENCOLOR(LCDCOLORWHITE);
    DEV_LcdDrawBox(TSDSPXSTART + TSDSPTIMELEN + 1,
                   TSDSPYSTART + (TSDSPXWID / 2) + 1,
                   TSDSPXLEN - TSDSPTIMELEN - 1,
                   TSDSPYLEN - (TSDSPXWID / 2) - 1);

    /* 设置画笔颜色和背景色 */
    LCDSETPENCOLOR(LCDCOLORBLACK);
    LCDSETBACKGROUNDCOLOR(LCDCOLORWHITE);

    /* 显示任务线 */
    for(i = 0; i < TSDSPTASKNUM; i++)
    {
        DEV_LcdDrawYLine(TSDSPXSTART + (TSDSPTASKITV * (i + 1)),
                         TSDSPYSTART,
                         TSDSPLEN,
                         1);
    }

    uiHead = strTsMsgStr.uiHead;
    uiTail = strTsMsgStr.uiTail;

    /* 环形缓冲区尾指向最后存储的任务信息位置 */
    uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

    /* 计算需要打印的图形长度 */
    uiDisplayLen = (strTsMsgStr.astrMsg[uiTail].uiTolalTime
                    - strTsMsgStr.astrMsg[uiHead].uiTolalTime) * TSDSPSWTITV;

    /* 需要打印的图像长度达到了坐标轴刻度可显示的长度 */
    if(uiDisplayLen >= TSDSPTIMENUM * TSDSPTIMEITV)
    {
        /* 取最大打印长度 */
        uiDisplayLen = TSDSPTIMENUM * TSDSPTIMEITV;

        /* 更新显示时间 */
        DEV_PrintTaskSwitchTime(strTsMsgStr.astrMsg[uiTail].uiTolalTime);
    }

    /* 指向前一个已运行完的任务 */
    uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

    /* 打印任务切换图形. 若全部任务打印的长度小于刻度范围则第一个任务对齐最左边刻度
       线, 超过刻度范围则最后一个任务右对齐最右边刻度线. 从最后一个任务从右向左打印 */
    while(0 != uiDisplayLen)
    {
        /* 计算一个任务可打印的长度 */
        if(uiDisplayLen >= strTsMsgStr.astrMsg[uiTail].uiRunTime * TSDSPSWTITV)
        {
            uiLeftDisplayLen = strTsMsgStr.astrMsg[uiTail].uiRunTime * TSDSPSWTITV;
        }
        else
        {
            uiLeftDisplayLen = uiDisplayLen;
        }

        uiDisplayLen -= uiLeftDisplayLen;

        /* 打印任务运行的直线 */
        DEV_LcdDrawYLine(TSDSPXSTART + strTsMsgStr.astrMsg[uiTail].uiTask * TSDSPTASKITV,
                         TSDSPYSTART + uiDisplayLen,
                         uiLeftDisplayLen,
                         TSDSPLINEWID);

        /* 还有需要打印的任务 */
        if(0 != uiDisplayLen)
        {
            uiNextTask = strTsMsgStr.astrMsg[uiTail].uiTask;

            /* 指向前一个任务 */
            uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

            /* 根据任务在LCD屏的位置画2个任务之间切换的连线 */
            if(strTsMsgStr.astrMsg[uiTail].uiTask < uiNextTask)
            {
                /* 打印2个任务之间切换的连线 */
                DEV_LcdDrawXLine(TSDSPXSTART + strTsMsgStr.astrMsg[uiTail].uiTask * TSDSPTASKITV,
                                 TSDSPYSTART + uiDisplayLen,
                                 (uiNextTask - strTsMsgStr.astrMsg[uiTail].uiTask) * TSDSPTASKITV,
                                 TSDSPLINEWID);
            }
            else
            {
                /* 打印2个任务之间切换的连线 */
                DEV_LcdDrawXLine(TSDSPXSTART + uiNextTask * TSDSPTASKITV,
                                 TSDSPYSTART + uiDisplayLen,
                                 (strTsMsgStr.astrMsg[uiTail].uiTask - uiNextTask) * TSDSPTASKITV,
                                 TSDSPLINEWID);
            }
        }
    }
}

/***********************************************************************************
函数功能: 打印任务切换时间信息, 单位: tick.
入口参数: uiLastTime: 显示的最后一个时间值, 这个时间值对应的是任务切换点个数.
返 回 值: none.
***********************************************************************************/
void DEV_PrintTaskSwitchTime(U32 uiLastTime)
{
    U32 digits_num;
    U32 time;
    U32 remainder;
    U32 i;
    U32 j;

    for(i = 0; i <= TSDSPTIMENUM; i++)
    {
        /* 计算显示时间的位数 */
        digits_num = 0;
        time = (uiLastTime - (TSDSPTIMENUM - i) * TSDSPBTNUM) * TSDSPSWTTICKNUM;

        do
        {
            time /= 10;
            digits_num++;
        }while(0 != time);

        /* 显示时间的每位数字 */
        time = (uiLastTime - (TSDSPTIMENUM - i) * TSDSPBTNUM) * TSDSPSWTTICKNUM;

        for(j = 1; j <= digits_num; j++)
        {
            /* 计算显示的最低位 */
            remainder = time % 10;
            time /= 10;

            DEV_LcdDrawPic((TSDSPXSTART - (TSDSPYWID / 2) - 16) / 2,
                           TSDSPYSTART + (TSDSPTIMEITV * i) + (digits_num * 8 / 2) - (j * 8) + 1,
                           16,
                           8,
                           DEV_CalcInvertFontAddr('0' + remainder));
        }
    }
}

/***********************************************************************************
函数功能: 初始化消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* 初始化缓冲池空闲链表 */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* 创建缓冲池信号量, 用于保证对缓冲池链表的串行操作 */
    (void)MDS_SemCreate(&pstrBufPool->strSem, SEMBIN | SEMPRIO, SEMFULL);

    /* 将各个缓冲挂入空闲链表 */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
函数功能: 从消息缓冲池中申请消息缓冲.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: 分配的消息缓冲指针.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    /* 在中断中申请内存 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* 从空闲链表分配一个缓冲 */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);
    }
    else /* 在任务中申请内存 */
    {
        /* 获取信号量, 保证对缓冲池链表的串行操作 */
        (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

        /* 从空闲链表分配一个缓冲 */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

        /* 释放信号量 */
        (void)MDS_SemGive(&pstrBufPool->strSem);
    }

    return pstrBuf;
}

/***********************************************************************************
函数功能: 释放将消息缓冲释放回消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
          pstrQueNode: 释放的缓冲节点指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    /* 获取信号量, 保证对缓冲池链表的串行操作 */
    (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

    /* 将释放的缓冲挂到空闲链表 */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    /* 释放信号量 */
    (void)MDS_SemGive(&pstrBufPool->strSem);
}

/***********************************************************************************
函数功能: 初始化硬件版本号IO.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareVersionInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置硬件版本号IO的管脚为输入状态, 弱下拉 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************************
函数功能: 读取硬件版本号.
入口参数: none.
返 回 值: 硬件版本号, 0~7.
***********************************************************************************/
U8 DEV_ReadHardwareVersion(void)
{
    U16 usHwVer;

    /* 读取硬件版本号IO */
    usHwVer = GPIO_ReadInputData(GPIOC);

    /* PC12~PC10对应硬件版本号的3个IO */
    return (usHwVer >> 10) & 0x07;
}

