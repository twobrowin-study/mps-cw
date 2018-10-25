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
 *  ���� Menu_text.c: �������-����������� ���� - ����� ������
 *
 *============================================================================================*/

#include <string.h>

#include "lcd.h"
#include "menu.h"
#include "menu_items.h"
#include "text.h"
#include "joystick.h"


void FontFunc(void) {
    FONT *OldFont = CurrentFont;

    /* ������� ��������� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("������� �������");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ������� ������� */
    LCD_PUTS(0, 12, "�����6X8");

#ifndef __CMCARM_DEMO__

    CurrentFont = &Font_12x16;
    LCD_PUTS(0, 22, "�����12X16");

    CurrentFont = &Font_7x10_thin;
    LCD_PUTS(0, 40, "�����7X10");

    CurrentFont = &Font_7x10_bold;
    LCD_PUTS(0, 52, "�����7X10 ������");

#endif /* __CMCARM_DEMO__ */

    /* ���� ������� SEL � ������������ � ������� ���� */
    CurrentFont = OldFont;
    BackToMenuOnSel();
}


void StyleFunc(void) {
    FONT *OldFont = CurrentFont;

    /* ������� ��������� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("������� ������");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ������� ������ */
    do {
        LCD_PUTS_Ex(0, 12, "���������", StyleBlink);
        if (KEY_PRESSED(SEL)) break;
        LCD_PUTS_Ex(0, 32, "����������", StyleFlipFlop);
        if (KEY_PRESSED(SEL)) break;
        LCD_PUTS_Ex(0, 52, "��������", StyleVibratory);
    } while (!KEY_PRESSED(SEL));

    /* ������ SEL - ������������ � ������� ���� */
    CurrentFont = OldFont;
    DisplayMenu();
}


void ShiftFunc(void) {
    FONT *OldFont = CurrentFont;

    /* ������� ��������� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    CurrentFont = &Font_6x8;
    DisplayMenuTitle("������� ������");
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* TODO - ������� ������ */
    do {

    } while (!KEY_PRESSED(SEL));

    /* ������ SEL - ������������ � ������� ���� */
    CurrentFont = OldFont;
    DisplayMenu();
}

/* ����� ��� ������������ "����������� �����" */
static char LongMasBook = 10;
static char Book[10][22] = {
        {"���������������� ��- "},
        {"��� K1986BE92QI ����-"},
        {"���� ������������-   "},
        {"������ �� ���������� "},
        {"Flash ������� ����-  "},
        {"���� � ��������� ��  "},
        {"���� ���������������-"},
        {"�������� �����������-"},
        {"�� RISC ����         "},
        {"Cortex-M3.           "}};

void BookFunc(void){
    u32 top_ind, key, i, LineD, LinS;

    // ���������� ���������� ����� �� ������ � ���������� ����� ���������
    if(LongMasBook > 8)
    {
      LineD = 8;
      LinS = LongMasBook - LineD;
    }
    else
    {
      LineD = LongMasBook;
      LinS = 0;
    }

    /* ������� ����� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ���� ��������� ������� ������ */
    for (top_ind = 0, key = NOKEY; key != SEL; ){
        /* ������� ����� � ���� ������� ������ */
        for (i = 0; i < LineD; i++)
            LCD_PUTS(0, (CurrentFont->Height) * i, Book[top_ind + i]);
        WAIT_UNTIL_ANY_KEY;

        /* ��������� ������� ������ */
        key = GetKey();
        switch (key) {
            /* �������������� ����� */
            case UP:
                if (top_ind > 0)
                    top_ind--;
                break;
            /* �������������� ���� */
            case DOWN:
                if (top_ind < LinS)
                    top_ind++;
                break;
    }
  }

  /* ������ SEL - ������������ � ������� ���� */
  WAIT_UNTIL_KEY_RELEASED(key);
  DisplayMenu();
}


void AboutFunc(void) {

    /* ������� ����� About */
    LCD_CLS();
    CurrentMethod = MET_AND;

    LCD_PUTS(0, 0,                             "   LDM-K1986BE92QI     ");
    LCD_PUTS(0, CurrentFont->Height + 1,       "  ����������  �����    ");
    LCD_PUTS(0, (CurrentFont->Height) * 2 + 2, "                    ");
    LCD_PUTS(0, (CurrentFont->Height) * 3 + 2, "                    ");
    LCD_PUTS(0, (CurrentFont->Height) * 4 + 3, "  ���� ������  v.1.0 ");
    LCD_PUTS(0, (CurrentFont->Height) * 5 + 4, "   LDM-SYSTEMS 2013     ");
    LCD_PUTS(0, (CurrentFont->Height) * 6 + 5, "  www.ldm-systems.ru    ");

    /* ���� ������� SEL � ������������ � ������� ���� */
    BackToMenuOnSel();
}

/*============================================================================================
 * ����� ����� Menu_text.c
 *============================================================================================*/
