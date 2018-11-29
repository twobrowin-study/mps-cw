/*!
\file
  \bref �������� ������� ���� ���������������� ������
  \authors �������� ���� ��. ��6-72, LDM-SYSTEMS (2013 �.)
  \date ������� 2018 ����
  \attention ���� ���� ������� � ��������� Windows CP1251!

  ���� �������� �������� ������������ ��������� � ������� ������� ������� ����
    ���������������� ������
 */
#ifndef CP1251_DIAGNOSTIC_MENU_ITEMS_H
#define CP1251_DIAGNOSTIC_MENU_ITEMS_H

#include <string.h>
#include "stddef.h"

#include "defs.h"
#include "time.h"
#include "format.h"
#include "cp1251_diagnostic_menu.h"
#include "delay.h"

#include "menu_items.h"
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"

void DiagnoseSettings(void);
void DiagnoseServer(void);
void SensorMenu(uint addr);

#endif // CP1251_DIAGNOSTIC_MENU_ITEMS_H
