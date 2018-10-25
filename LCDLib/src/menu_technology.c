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

void TechnologyText(u32 nam);

/* ����� */
#ifndef __CMCARM_DEMO__

static char LongMasLDM[4] = {19, 13, 10, 23};

static char BookLDM[19][22] = {
        {"LDM-SYSTEMS          "},
        {" ���������� ��������-"},
        {"����������� � �����- "},
        {"�������� ����������� "},
        {"���������.           "},
        {" ��������� �������   "},
        {"���������� ���� ��   "},
        {"���� ������������:   "},
        {"MILANDR, MultiClet,  "},
        {"����� (������),      "},
        {"ATMEL (���)          "},
        {"� ���� ALTERA, XILINX"},
        {"(���).               "},
        {" ���������� ��������-"},
        {"��� ������ �� �����- "},
        {"����� ������� ��     "},
        {"��������� ��������-  "},
        {"����.                "},
        {"www.LDM-SYSTEMS.ru   "}};




static char BookMILAND[13][22] = {
        {"��� ��� �������      "},
        {" ���������� ��������-"},
        {"����������� � �����- "},
        {"�������� �����-      "},
        {"����������� �������- "},
        {"��� ����, ���������- "},
        {"������ �� ����������-"},
        {"��� � �������� � ����"},
        {"������� ������������ "},
        {"� ���������� (����-  "},
        {"����������� �������, "},
        {"����������� � �. �.)."},
        {"www.milandr.ru       "}};

static char BookMELT[10][22] = {
        {"��� ����             "},
        {"���������� ���������-"},
        {"���� �����������:    "},
        {"1) ���������� ��;    "},
        {"2) ��������� �������;"},
        {"3) �������� � ���;   "},
        {"4) LED ����������;   "},
        {"5) ������� ��� �����-"},
        {"   ����� �������.    "},
        {"www.melt.com.ru      "}};




static char BookPHYTON[23][22] = {
        {"��� ����� �����      "},
        {" ���������������� �� "},
        {"���������� � ��������"},
        {"����������������     "},
        {"������� ��� �����-   "},
        {"������������.        "},
        {"|                    "},
        {"�������� �����������:"},
        {"1) ������������� �   "},
        {"   JTAG ���������;   "},
        {"2) ����������-����-  "},
        {"   ������ ������     "},
        {"   (����������);     "},
        {"3) �����-���������� �"},
        {"   �����-����������� "},
        {"   ��;               "},
        {"4) �������������     "},
        {"   �������������;    "},
        {"5) �������������-    "},
        {"   �����������;      "},
        {"6) �������� ��������-"},
        {"  �� �� �� ���������."},
        {"www.phyton.ru        "}};



#else /* __CMCARM_DEMO__ defined */

static char LongMasLDM[4] = {2, 2, 2, 2};
static char BookLDM[2][22] = {
        {"LDM-SYSTEMS          "},
        {"www.LDM-SYSTEMS.ru   "}};

static char BookMILAND[2][22] = {
        {"��� ��� �������      "},
        {"www.milandr.ru       "}};

static char BookMELT[2][22] = {
        {"��� ����             "},
        {"www.melt.com.ru      "}};

static char BookPHYTON[2][22] = {
        {"��� ����� �����      "},
        {"www.phyton.ru        "}};

#endif /* __CMCARM_DEMO__ */


void LDM(void)
{
  TechnologyText(0);
}

void MILAND(void)
{
  TechnologyText(1);
}

void MELT(void)
{
  TechnologyText(2);
}

void PHYTON(void)
{
  TechnologyText(3);
}



void TechnologyText(u32 nam){
    u32 top_ind, key, i, LineD, LinS;

    // ���������� ���������� ����� �� ������ � ���������� ����� ���������
    if(LongMasLDM[nam] > 8)
    {
      LineD = 8;
      LinS = LongMasLDM[nam] - LineD;
    }
    else
    {
      LineD = LongMasLDM[nam];
      LinS = 0;
    }

    /* ������� ����� � ���� key up */
    LCD_CLS();
    CurrentMethod = MET_AND;
    WAIT_UNTIL_KEY_RELEASED(SEL);

    /* ���� ��������� ������� ������ */
    for (top_ind = 0, key = NOKEY; key != SEL; ){
        /* ������� ����� � ���� ������� ������ */
        for (i = 0; i < LongMasLDM[nam]; i++)
        {
          switch (nam)
          {
            case 0:
              LCD_PUTS(0, (CurrentFont->Height) * i, BookLDM[top_ind + i]);
            break;
            case 1:
              LCD_PUTS(0, (CurrentFont->Height) * i, BookMILAND[top_ind + i]);
            break;
            case 2:
              LCD_PUTS(0, (CurrentFont->Height) * i, BookMELT[top_ind + i]);
            break;
            case 3:
              LCD_PUTS(0, (CurrentFont->Height) * i, BookPHYTON[top_ind + i]);
            break;
          }
        }
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
