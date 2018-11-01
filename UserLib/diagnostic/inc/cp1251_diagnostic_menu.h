/*!
\file
  \bref ���� ���������������� ������
  \authors �������� ���� ��. ��6-72, LDM-SYSTEMS (2013 �.)
  \date ������� 2018 ����
  \attention ���� ���� ������� � ��������� Windows CP1251!

  ���� �������� �������� ����������� � ���� ����������������� ������ �
    ����� � ����������� LCDLib
*/
#ifndef CP1251_DIAGNOSTIC_MENU_H
#define CP1251_DIAGNOSTIC_MENU_H

#include "defs.h"

#include "cp1251_diagnostic_menu_items.h"

#include <string.h>
#include <stdio.h>
#include "stddef.h"

#include "menu_items.h"
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"

#define  MAX_MENU_LEVELS 2

#define countof(a) (sizeof(a) / sizeof(*(a)))

typedef void (* tMenuFunc)(void);
typedef struct sMenuItem *tMenuItem;
typedef struct sMenu *tMenu;

/// ��������� �������� ������ ����
struct sMenuItem
{
  const char * psTitle;
  tMenuFunc pfMenuFunc;
  tMenu psSubMenu;
};

/// ��������� �������� ����
struct sMenu
{
  const char* psTitle;
  tMenuItem psItems;
  u32 nItems;
};

void MenuInit(void);
void DisplayMenuTitle(const char *ptr);

void DisplayMenuItemString(u32 y, const char *ptr);
void DisplayMenuItemStringPrefix(u32 y, const char *prefix, const char *ptr);
void DisplayMenuItemInt(u32 y, uint val, uint scal);
void DisplayMenuItemIntPrefix(u32 y, const char *prefix, uint val, uint scal);

void DisplayMenu(void);
void BackToMenuOnSel(void);
void ReadKey(void);

#endif // CP1251_DIAGNOSTIC_MENU_H
