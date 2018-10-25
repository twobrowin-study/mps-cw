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
 *  ���� Menu_leds.c: �������-����������� ���� - ����������
 *
 *============================================================================================*/

#include "menu.h"
#include "menu_items.h"
#include "leds.h"
#include "lcd.h"
#include "text.h"
#include "joystick.h"


void LightsOnFunc(void) {
  u32 tck, tck_led;

  /* ������� ��������� �� ����� � ���� key up */
  LCD_CLS();
  CurrentMethod = MET_AND;
  CurrentFont = &Font_6x8;
  LCD_PUTS(0, 0,                       "    ���  ��������   ");
  LCD_PUTS(0, CurrentFont->Height + 2, "     ������� SEL    ");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* ��������� "����������" */
  CurrentLights = __SHLC(0xFDF05380, LED0_OFS);
  for (tck = 0, tck_led = 0; !KEY_PRESSED(SEL); tck++) {
    if (tck == tck_led) {
      tck_led += 35000;
      ShiftLights();
    }
  }
  /* ������ SEL - ��������� ���������� � ������������ � ������� ���� */
  CurrentLights = 0x0;
  ShiftLights();
  DisplayMenu();
}

/*============================================================================================
 * ����� ����� Menu_leds.c
 *============================================================================================*/