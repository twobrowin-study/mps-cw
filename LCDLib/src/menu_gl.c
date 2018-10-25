/*============================================================================================
 *
 *  (C) 2013, LDM-SYSTEMS
 *
 *  ���������������� ������ ��� ���������� ����� LDM-K1986BE92QI
 *
 *  ������ �� ��������������� "��� ����", �.�. ������������� ��� ������, ���������� ���������
 *  ������������� ���������� �� ���������� ��� ����������� Milandr K1986BE92QI. �������� LDM-SYSTEMS
 *  �� ����� ������� ��������������� �� ��������� ����������� ������������� �������, ���
 *  �������������� ������������� �� ��� ������, ��.
 *
 *  ������������ ��� Keil 5 DeamonClub.RU
 *
 *--------------------------------------------------------------------------------------------
 *
 *  ���� Menu_gl.�: �������-����������� ���� - �������
 *
 *============================================================================================*/
#include "menu.h"
#include "menu_items.h"
#include "gl.h"
#include "lcd.h"
#include "joystick.h"
#include "systick.h"
#include "text.h"

/* ��������� ������� - ��������� ����������� ���������� */

/* ��������� ���������� �������������� ������������ */
static void DisplayTriangle (u32 x, u32 y, u32 side) {
    LCD_Line(x, y, x + side, y);
    LCD_Line(x + side, y, x + side, y + side);
    LCD_Line(x, y, x + side, y + side);
}


/* ��������� �������� �������������� ������������ */
static void DisplayFillingTriangle (u32 x, u32 y, u32 side) {
    u32 i;
    for (i = 0; i < side + 1; i++)
        LCD_Line(x + i, y + i, x + side, y + i);
}


/* ��������� ����� */
void DisplayFillingCircle (u32 x, u32 y, u32 radius) {
    u32 i;
    for (i = 0; i < radius + 1; i++)
        LCD_Circle(x, y, i);
}


/* ��������� ���������� �������������� */
void DisplayRectangle (u32 x, u32 y, u32 side_x, u32 side_y) {
    LCD_Line(x, y, x + side_x, y);
    LCD_Line(x, y, x, y + side_y);
    LCD_Line(x + side_x, y, x + side_x, y + side_y);
    LCD_Line(x, y + side_y, x + side_x, y + side_y);
}


/* ��������� ����� (�������� ��������������) */
void DisplayFillingRectangle (u32 x, u32 y, u32 side_x, u32 side_y) {
    u32 i;
    for (i = 0; i < side_y; i++)
        LCD_Line(x, y + i, x + side_x, y + i);
}


/* �������-����������� ���� */

void ElementsFunc(void) {
#define base_x 25
#define base_y 14
#define side   16

#define offs_x 8
#define offs_y 5

#define circle_radius side / 2
#define circle_x base_x + side + offs_x + circle_radius
#define circle_y base_y + circle_radius
#define circle_filling_y circle_y + circle_radius * 2 + offs_y

#define rectangle_x base_x + side * 2 + offs_x * 2
#define block_y  base_y + side + offs_y

    /* ������� ��������� */
    LCD_CLS();
    CurrentMethod = MET_OR;
    DisplayMenuTitle("���������");

    /* ������ ��������� � ���� key up */
    DisplayTriangle (base_x, base_y, side);
    DisplayFillingTriangle (base_x, base_y + side + offs_y, side);

    LCD_Circle(circle_x, circle_y, circle_radius);
    DisplayFillingCircle (circle_x, circle_filling_y, circle_radius);

    DisplayRectangle (rectangle_x, base_y, side * 2, side);
    DisplayFillingRectangle (rectangle_x, block_y, side * 2, side);

    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ���� ������� SEL � ������������ � ������� ���� */
    BackToMenuOnSel();
}


void IndicatorsFunc(void) {
#define side_x 4

    s32 i, sign;

    /* ������� ��������� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_OR;
    DisplayMenuTitle("����������");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ������� progress bar */
    LCD_PUTS(24,18,"0");
    DisplayRectangle (32, 14, 64, 14);
    LCD_PUTS(97,18,"100");
    /* ������� ����� */
    LCD_Circle(63, 63, 30);
    LCD_Line(63, 34, 63, 41);
    LCD_Line(35, 53, 42, 56);
    LCD_Line(91, 53, 84, 56);
    LCD_Line(45, 39, 51, 45);
    LCD_Line(81, 39, 75, 45);

    /* ��������� �������� - �� 0 �� 100% � ������� */
    /* ��������� �������� ������� �����            */
    CurrentMethod = MET_XOR;
    for (i = 0, sign = 1; !KEY_PRESSED(SEL); ) {
        s32 ni = i + side_x * sign;
        DisplayFillingRectangle (i + 34, 16, side_x - 1, 11);
        if (ni > 60 || ni < 0)
            sign = -sign;
        else
            i = ni;
        SysTickDelay(300);

        /* ������ ������� */
        LCD_Line(86, 46, 63, 63);
        SysTickDelay(150);
        LCD_Line(86, 46, 63, 63);
        SysTickDelay(150);
        LCD_Line(84, 44, 63, 63);
        SysTickDelay(150);
        LCD_Line(84, 44, 63, 63);
        SysTickDelay(150);
    }

    /* ������ SEL - ������������ � ������� ���� */
    DisplayMenu();
}

/*============================================================================================
 * ����� ����� Menu_gl.�
 *============================================================================================*/
