
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* ��Ϣ����� */

TASKSWITCHMSGSTR gstrTsMsgStr;      /* �����л���Ϣ */

U16 gusPenColor;                    /* ������ɫ */
U16 gusBackGroundColor;             /* ����ɫ */

U8 gaucFont[] =                     /* �������, 0~9, A~Z, a~z */
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

U8 gaucInvertFont[] =               /* �������, ˳ʱ����ת90��, 0~9, A~Z, a~z */
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
��������: ��ʼ�����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* ��ʼ����Ϣ����� */
    DEV_BufferInit(&gstrBufPool);

    /* ��ʼ�������л���Ϣ�ṹ */
    DEV_TaskSwitchMsgStrInit();

    /* ������ӡ���к��ź��� */
    gpstrSerialMsgQue = MDS_QueCreate((M_QUE*)NULL, QUEPRIO);
    gpstrLcdMsgSem = MDS_SemCreate((M_SEM*)NULL, SEMBIN | SEMPRIO, SEMEMPTY);

    /* ���������ź���, ���������ȼ��̳й��� */
    gpstrSemMut = MDS_SemCreate((M_SEM*)NULL, SEMPRIINH | SEMMUT | SEMPRIO,
                                SEMFULL);

#ifdef MDS_INCLUDETASKHOOK

    /* �ҽ������Ӻ��� */
    MDS_TaskCreateHookAdd(TEST_TaskCreatePrint);
    MDS_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
    MDS_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/***********************************************************************************
��������: ��ʼ��Ӳ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* ��ʼ��Ӳ���汾��IO */
    DEV_HardwareVersionInit();

    /* ��ʼ������ */
    DEV_UartInit();

    /* ��ʼ��LCDҺ���� */
    DEV_LcdInit();

    /* �����л���ʾͼ�γ�ʼ�� */
    DEV_TaskSwitchDisplayInit();
}

/***********************************************************************************
��������: ��ʼ������.
��ڲ���: none.
�� �� ֵ: none.
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
��������: �򴮿ڷ���һ���ַ�.
��ڲ���: ucChar: ������ַ�.
�� �� ֵ: none.
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
��������: ���ַ�����ӡ���ڴ�. �ú�����ڲ�����printf����һ��.
��ڲ���: pvStringPt: ��һ��������ָ��, ����Ϊ�ַ���ָ��.
          ...: ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* ����buf, ���������Ҫ��ӡ���ַ� */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* ���ַ�����ӡ���ڴ� */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* ���buf���� */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* ��buf������� */
    (void)MDS_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
��������: �򴮿ڴ�ӡ��Ϣ. ���ڴ���ָ����ַ�򴮿�������ӡһ�����ȵ�����, ���ô��ڲ�ѯ
          ��ʽ��ӡ.
��ڲ���: pucBuf: ��Ҫ��ӡ���ݴ�ŵĻ����ַ.
          uiLen: ��Ҫ��ӡ���ݵĳ���, ��λ: �ֽ�.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* ���ò�ѯ�ķ�ʽ�������� */
    while(uiCounter < uiLen)
    {
        /* �򴮿ڷ�һ���ַ� */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
��������: ��ʼ��LCDҺ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdInit(void)
{
    U8 ucHwVer;

    /* ��ȡӲ���汾�� */
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

    if(0 == ucHwVer)            //Һ����ʹ��S6D04D1X21������
    {
        DEV_LcdWriteData(0x78);
    }
    else if(1 == ucHwVer)       //Һ����ʹ��HX8352C������
    {
        DEV_LcdWriteData(0x3A);
        DEV_LcdWriteCmd(0x55);
    }

    DEV_LcdWriteCmd(0x29);      //Set_display_on
    DEV_DelayMs(1);
}

/***********************************************************************************
��������: ��ʼ������LCD��GPIO.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdGpioCfg(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /*  LED��, RESET����ܽ� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* FLM����ܽ� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/***********************************************************************************
��������: ��ʼ��LCDʹ�õ�FSMC�ܽź�FSMCʱ��.
��ڲ���: none.
�� �� ֵ: none.
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
��������: ��λLCDҺ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdReset(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    DEV_DelayMs(1);
}

/***********************************************************************************
��������: ����LCDҺ���������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdLightOn(void)
{
    /* ��LCD����� */
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
��������: �ر�LCDҺ���������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdLightOff(void)
{
    /* �ر�LCD����� */
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

/***********************************************************************************
��������: ��LCDд����.
��ڲ���: usCmd: д�������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdWriteCmd(U16 usCmd)
{
    *(volatile U16*)(FSMCCMDADDR) = usCmd;
}

/***********************************************************************************
��������: ��LCDд����.
��ڲ���: usData: д�������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdWriteData(U16 usData)
{
    *(volatile U16*)(FSMCDATAADDR) = usData;
}

/***********************************************************************************
��������: �趨LCDһ������, ����д�������������д������, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usXEnd: X����������.
          usYStart: Y�Ὺʼ�����.
          usYEnd: Y����������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd)
{
    /* �趨X�᷶Χ */
    DEV_LcdWriteCmd(0x2B);
    DEV_LcdWriteData(usXStart >> 8);
    DEV_LcdWriteData(usXStart & 0xFF);
    DEV_LcdWriteData(usXEnd >> 8);
    DEV_LcdWriteData(usXEnd & 0xFF);

    /* �趨Y�᷶Χ */
    DEV_LcdWriteCmd(0x2A);
    DEV_LcdWriteData(usYStart >> 8);
    DEV_LcdWriteData(usYStart & 0xFF);
    DEV_LcdWriteData(usYEnd >> 8);
    DEV_LcdWriteData(usYEnd & 0xFF);

    /* д��д������, ����д������ݻ���ʾ������ */
    DEV_LcdWriteCmd(0x2C);
}

/***********************************************************************************
��������: ��LCD��ָ����λ��������ͼ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����.
          usXLen: Y�᳤��.
          usYLen: Y�᳤��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen)
{
    U32 i;

    /* �趨д��ķ�Χ */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* д������ */
    for(i = 0; i < usXLen * usYLen; i++)
    {
        *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
    }
}

/***********************************************************************************
��������: ��X�᷽��һ����X��ƽ�е�ֱ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����, Ϊ�߿�ȵ��м���.
          usLen: ����, X�᷽��.
          usWid: ���, Y�᷽��, Ӧ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart, usYStart - usWid / 2, usLen, usWid);
}

/***********************************************************************************
��������: ��Y�᷽��һ����Y��ƽ�е�ֱ��, ʹ�û�����ɫ, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����, Ϊ�߿�ȵ��м���.
          usYStart: Y�Ὺʼ�����.
          usLen: ����, Y�᷽��.
          usWid: ���, X�᷽��, Ӧ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid)
{
    DEV_LcdDrawBox(usXStart - usWid / 2, usYStart, usWid, usLen);
}

/***********************************************************************************
��������: ��LCD��ָ����λ�����ͼ��, �����Ǿ��ε�ͼ��, X�������8�ı���, ��λ: ����.
��ڲ���: usXStart: X�Ὺʼ�����.
          usYStart: Y�Ὺʼ�����.
          usXLen: Y�᳤��.
          usYLen: Y�᳤��.
          pucPic: ���ͼ����ŵ��ڴ��ַ, �ڴ��е�1����ʹ�û�����ɫ, 0���ֱ���ɫ.
�� �� ֵ: none.
***********************************************************************************/
void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic)
{
    U32 i;
    S32 j;

    /* �趨д��ķ�Χ */
    DEV_LcdSetBlock(usXStart, usXStart + usXLen - 1, usYStart, usYStart + usYLen - 1);

    /* ���ֽ�д������ */
    for(i = 0; i < usXLen * usYLen / 8; i++)
    {
        /* �ж��ֽ���ÿ��bit */
        for(j = 7; j >= 0; j--)
        {
            /* ֻ��bitΪ1��д�뻭����ɫ */
            if(0x01 == ((pucPic[i] >> j) & 0x01))
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETPENCOLOR;
            }
            else /* д�뱳��ɫ */
            {
                *(volatile U16*)(FSMCDATAADDR) = LCDGETBACKGROUNDCOLOR;
            }
        }
    }
}

/***********************************************************************************
��������: �������ַ��ĵ�ַ, ֻ����0~9, A~Z, a~z֮����ַ�.
��ڲ���: ucAscii: �ַ���ASCII��.
�� �� ֵ: �ַ��Ĵ洢��ַ, NULLΪ��Ч��ַ.
***********************************************************************************/
U8* DEV_CalcFontAddr(U8 ucAscii)
{
    /* ��Ч�ַ� */
    if(!(((ucAscii >= '0') && (ucAscii <= '9')) || ((ucAscii >= 'A') && (ucAscii <= 'Z'))
         || ((ucAscii >= 'a') && (ucAscii <= 'z'))))
    {
        return NULL;
    }

    /* 0~9֮����ַ� */
    if(ucAscii <= '9')
    {
        return &gaucFont[(ucAscii - '0') * 16];
    }
    /* A~Z֮����ַ� */
    else if(ucAscii <= 'Z')
    {
        return &gaucFont[((ucAscii - 'A') * 16) + (16 * 10)];
    }
    /* a~z֮����ַ� */
    else
    {
        return &gaucFont[((ucAscii - 'a') * 16) + (16 * (10 + 26))];
    }
}

/***********************************************************************************
��������: ��������ת�ַ��ĵ�ַ, ֻ����0~9, A~Z, a~z֮����ַ�.
��ڲ���: ucAscii: �ַ���ASCII��.
�� �� ֵ: �ַ��Ĵ洢��ַ, NULLΪ��Ч��ַ.
***********************************************************************************/
U8* DEV_CalcInvertFontAddr(U8 ucAscii)
{
    /* ��Ч�ַ� */
    if(!(((ucAscii >= '0') && (ucAscii <= '9')) || ((ucAscii >= 'A') && (ucAscii <= 'Z'))
         || ((ucAscii >= 'a') && (ucAscii <= 'z'))))
    {
        return NULL;
    }

    /* 0~9֮����ַ� */
    if(ucAscii <= '9')
    {
        return &gaucInvertFont[(ucAscii - '0') * 16];
    }
    /* A~Z֮����ַ� */
    else if(ucAscii <= 'Z')
    {
        return &gaucInvertFont[((ucAscii - 'A') * 16) + (16 * 10)];
    }
    /* a~z֮����ַ� */
    else
    {
        return &gaucInvertFont[((ucAscii - 'a') * 16) + (16 * (10 + 26))];
    }
}

/***********************************************************************************
��������: �����л���ʾͼ�γ�ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_TaskSwitchDisplayInit(void)
{
    U32 i;

    /* LCD���� */
    LCDSETPENCOLOR(LCDCOLORWHITE);
    DEV_LcdDrawBox(0, 0, LCDXLEN, LCDYLEN);

    /********** ��X��Y������ **********/
    /* ���û�����ɫ�ͱ���ɫ */
    LCDSETPENCOLOR(LCDCOLORBLACK);
    LCDSETBACKGROUNDCOLOR(LCDCOLORWHITE);

    /* ��X���Y�� */
    DEV_LcdDrawXLine(TSDSPXSTART, TSDSPYSTART, TSDSPXLEN, TSDSPXWID);
    DEV_LcdDrawYLine(TSDSPXSTART, TSDSPYSTART, TSDSPYLEN, TSDSPYWID);

    /* ��ʾ���������� */
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

    /* ��ʾ�û������������� */
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

    /* ��ʾ�������� */
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

    /* ��ʾ������ */
    for(i = 0; i < TSDSPTASKNUM; i++)
    {
        DEV_LcdDrawYLine(TSDSPXSTART + (TSDSPTASKITV * (i + 1)),
                         TSDSPYSTART,
                         TSDSPLEN,
                         1);
    }

    /* ��ʾʱ��̶� */
    for(i = 0; i < TSDSPTIMENUM; i++)
    {
        DEV_LcdDrawXLine(TSDSPXSTART,
                         TSDSPYSTART + (TSDSPTIMEITV * (i + 1)),
                         TSDSPTIMELEN,
                         TSDSPTIMEWID);
    }

    /* ��ʾʱ�� */
    DEV_PrintTaskSwitchTime(TSDSPTIMENUM * TSDSPBTNUM);
}

/***********************************************************************************
��������: ��ʼ�������л���Ϣ�ṹ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_TaskSwitchMsgStrInit(void)
{
    gstrTsMsgStr.uiHead = 0;
    gstrTsMsgStr.uiTail = 0;

    gstrTsMsgStr.uiFlag = TSFLAGNOTUPDATA;
}

/***********************************************************************************
��������: ���������л���Ϣ.
��ڲ���: pucTaskName: ����������Ƶ��ַ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_SaveTaskSwitchMsg(U8* pucTaskName)
{
    U32 uiTask;
    U32 uiPreTail;

    /* ȷ�������� */
    if(0 == strcmp(ROOTTASKNAME, pucTaskName))
    {
        /* ��������ΪΪ��������� */
        uiTask = TSDSPTASKNUM;
    }
    else if(0 == strcmp(IDLETASKNAME, pucTaskName))
    {
        /* ����������1 */
        uiTask = 1;
    }
    else
    {
        /* ����������Ϊ�������еı��+1 */
        uiTask = pucTaskName[4] - '0' + 1;
    }

    /* �洢�����л���Ϣ */
    gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTask = uiTask;
    gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTolalTime = MDS_GetSystemTick() / TSDSPSWTTICKNUM; /* ��LCD���ص������, �����л�ͼ��ֻ��
                                                                                                      ��ȷ��TSDSPSWTTICKNUM��tick */

    /* ���ٴ洢��2����Ϣ */
    if(gstrTsMsgStr.uiHead != gstrTsMsgStr.uiTail)
    {
        /* ���㵹���ڶ����洢������Ϣ��λ�� */
        uiPreTail = (gstrTsMsgStr.uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

        /* �洢�ϸ����������ʱ�� */
        gstrTsMsgStr.astrMsg[uiPreTail].uiRunTime = gstrTsMsgStr.astrMsg[gstrTsMsgStr.uiTail].uiTolalTime
                                                    - gstrTsMsgStr.astrMsg[uiPreTail].uiTolalTime;
    }

    /* ���»��λ�����βλ�� */
    gstrTsMsgStr.uiTail++;
    gstrTsMsgStr.uiTail %= TASKSWITCHMSGLEN;

    /* ���λ��������� */
    if(gstrTsMsgStr.uiHead == gstrTsMsgStr.uiTail)
    {
        /* ���»��λ�����ͷλ�� */
        gstrTsMsgStr.uiHead++;
        gstrTsMsgStr.uiHead %= TASKSWITCHMSGLEN;
    }

    /* ��Ϣ�Ѿ��������, ��Ϊ��Ҫ���±�־ */
    gstrTsMsgStr.uiFlag = TSFLAGUPDATA;

    /* ���λ������Ѿ��������, �ͷ�LCD��ӡ�ź��� */
    (void)MDS_SemGive(gpstrLcdMsgSem);
}

/***********************************************************************************
��������: �������л���Ϣ��ӡ��LCD��Ļ��.
��ڲ���: none.
�� �� ֵ: none.
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

    /* �����л�ͼ�β���Ҫ���� */
    if(TSFLAGUPDATA != gstrTsMsgStr.uiFlag)
    {
        return;
    }

    /* Ϊ����ú����ڲ���ȫ�ֱ���gstrTsMsgStr�ṹʱ�����������д, �˴������ж�״̬
       �½�gstrTsMsgStr�ṹ���浽�ֲ�����strTsMsgStr�ṹ��, �˺�ú�����strTsMsgStr
       �ṹ���в��� */
    (void)MDS_IntLock();

    /* ���������л���Ϣ */
    strTsMsgStr = gstrTsMsgStr;

    /* ��Ϣ�Ѿ��������, ��Ϊ����Ҫ���±�־ */
    gstrTsMsgStr.uiFlag = TSFLAGNOTUPDATA;

    (void)MDS_IntUnlock();

    /* ��������л���ʾ���� */
    LCDSETPENCOLOR(LCDCOLORWHITE);
    DEV_LcdDrawBox(TSDSPXSTART + TSDSPTIMELEN + 1,
                   TSDSPYSTART + (TSDSPXWID / 2) + 1,
                   TSDSPXLEN - TSDSPTIMELEN - 1,
                   TSDSPYLEN - (TSDSPXWID / 2) - 1);

    /* ���û�����ɫ�ͱ���ɫ */
    LCDSETPENCOLOR(LCDCOLORBLACK);
    LCDSETBACKGROUNDCOLOR(LCDCOLORWHITE);

    /* ��ʾ������ */
    for(i = 0; i < TSDSPTASKNUM; i++)
    {
        DEV_LcdDrawYLine(TSDSPXSTART + (TSDSPTASKITV * (i + 1)),
                         TSDSPYSTART,
                         TSDSPLEN,
                         1);
    }

    uiHead = strTsMsgStr.uiHead;
    uiTail = strTsMsgStr.uiTail;

    /* ���λ�����βָ�����洢��������Ϣλ�� */
    uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

    /* ������Ҫ��ӡ��ͼ�γ��� */
    uiDisplayLen = (strTsMsgStr.astrMsg[uiTail].uiTolalTime
                    - strTsMsgStr.astrMsg[uiHead].uiTolalTime) * TSDSPSWTITV;

    /* ��Ҫ��ӡ��ͼ�񳤶ȴﵽ��������̶ȿ���ʾ�ĳ��� */
    if(uiDisplayLen >= TSDSPTIMENUM * TSDSPTIMEITV)
    {
        /* ȡ����ӡ���� */
        uiDisplayLen = TSDSPTIMENUM * TSDSPTIMEITV;

        /* ������ʾʱ�� */
        DEV_PrintTaskSwitchTime(strTsMsgStr.astrMsg[uiTail].uiTolalTime);
    }

    /* ָ��ǰһ��������������� */
    uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

    /* ��ӡ�����л�ͼ��. ��ȫ�������ӡ�ĳ���С�ڿ̶ȷ�Χ���һ�������������߿̶�
       ��, �����̶ȷ�Χ�����һ�������Ҷ������ұ߿̶���. �����һ��������������ӡ */
    while(0 != uiDisplayLen)
    {
        /* ����һ������ɴ�ӡ�ĳ��� */
        if(uiDisplayLen >= strTsMsgStr.astrMsg[uiTail].uiRunTime * TSDSPSWTITV)
        {
            uiLeftDisplayLen = strTsMsgStr.astrMsg[uiTail].uiRunTime * TSDSPSWTITV;
        }
        else
        {
            uiLeftDisplayLen = uiDisplayLen;
        }

        uiDisplayLen -= uiLeftDisplayLen;

        /* ��ӡ�������е�ֱ�� */
        DEV_LcdDrawYLine(TSDSPXSTART + strTsMsgStr.astrMsg[uiTail].uiTask * TSDSPTASKITV,
                         TSDSPYSTART + uiDisplayLen,
                         uiLeftDisplayLen,
                         TSDSPLINEWID);

        /* ������Ҫ��ӡ������ */
        if(0 != uiDisplayLen)
        {
            uiNextTask = strTsMsgStr.astrMsg[uiTail].uiTask;

            /* ָ��ǰһ������ */
            uiTail = (uiTail + TASKSWITCHMSGLEN - 1) % TASKSWITCHMSGLEN;

            /* ����������LCD����λ�û�2������֮���л������� */
            if(strTsMsgStr.astrMsg[uiTail].uiTask < uiNextTask)
            {
                /* ��ӡ2������֮���л������� */
                DEV_LcdDrawXLine(TSDSPXSTART + strTsMsgStr.astrMsg[uiTail].uiTask * TSDSPTASKITV,
                                 TSDSPYSTART + uiDisplayLen,
                                 (uiNextTask - strTsMsgStr.astrMsg[uiTail].uiTask) * TSDSPTASKITV,
                                 TSDSPLINEWID);
            }
            else
            {
                /* ��ӡ2������֮���л������� */
                DEV_LcdDrawXLine(TSDSPXSTART + uiNextTask * TSDSPTASKITV,
                                 TSDSPYSTART + uiDisplayLen,
                                 (strTsMsgStr.astrMsg[uiTail].uiTask - uiNextTask) * TSDSPTASKITV,
                                 TSDSPLINEWID);
            }
        }
    }
}

/***********************************************************************************
��������: ��ӡ�����л�ʱ����Ϣ, ��λ: tick.
��ڲ���: uiLastTime: ��ʾ�����һ��ʱ��ֵ, ���ʱ��ֵ��Ӧ���������л������.
�� �� ֵ: none.
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
        /* ������ʾʱ���λ�� */
        digits_num = 0;
        time = (uiLastTime - (TSDSPTIMENUM - i) * TSDSPBTNUM) * TSDSPSWTTICKNUM;

        do
        {
            time /= 10;
            digits_num++;
        }while(0 != time);

        /* ��ʾʱ���ÿλ���� */
        time = (uiLastTime - (TSDSPTIMENUM - i) * TSDSPBTNUM) * TSDSPSWTTICKNUM;

        for(j = 1; j <= digits_num; j++)
        {
            /* ������ʾ�����λ */
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
��������: ��ʼ����Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* ��ʼ������ؿ������� */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* ����������ź���, ���ڱ�֤�Ի��������Ĵ��в��� */
    (void)MDS_SemCreate(&pstrBufPool->strSem, SEMBIN | SEMPRIO, SEMFULL);

    /* �������������������� */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
��������: ����Ϣ�������������Ϣ����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: �������Ϣ����ָ��.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    /* ���ж��������ڴ� */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* �ӿ����������һ������ */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);
    }
    else /* �������������ڴ� */
    {
        /* ��ȡ�ź���, ��֤�Ի��������Ĵ��в��� */
        (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

        /* �ӿ����������һ������ */
        pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

        /* �ͷ��ź��� */
        (void)MDS_SemGive(&pstrBufPool->strSem);
    }

    return pstrBuf;
}

/***********************************************************************************
��������: �ͷŽ���Ϣ�����ͷŻ���Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
          pstrQueNode: �ͷŵĻ���ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    /* ��ȡ�ź���, ��֤�Ի��������Ĵ��в��� */
    (void)MDS_SemTake(&pstrBufPool->strSem, SEMWAITFEV);

    /* ���ͷŵĻ���ҵ��������� */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    /* �ͷ��ź��� */
    (void)MDS_SemGive(&pstrBufPool->strSem);
}

/***********************************************************************************
��������: ��ʼ��Ӳ���汾��IO.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_HardwareVersionInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* ����Ӳ���汾��IO�Ĺܽ�Ϊ����״̬, ������ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/***********************************************************************************
��������: ��ȡӲ���汾��.
��ڲ���: none.
�� �� ֵ: Ӳ���汾��, 0~7.
***********************************************************************************/
U8 DEV_ReadHardwareVersion(void)
{
    U16 usHwVer;

    /* ��ȡӲ���汾��IO */
    usHwVer = GPIO_ReadInputData(GPIOC);

    /* PC12~PC10��ӦӲ���汾�ŵ�3��IO */
    return (usHwVer >> 10) & 0x07;
}

