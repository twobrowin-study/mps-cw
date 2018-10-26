/*!
\file
  \bref ���� ���������������� ������
  \authors �������� ���� ��. ��6-72, LDM-SYSTEMS (2013 �.)
  \date ������� 2018 ����
  \attention ���� ���� ������� � ��������� Windows 1251!

  ���� �������� ���� ���������������� ������ � ������ �������� ��� ������.
    ���� ������� �� ������ ��������� LCDLib
*/
#include "cp1251_diagnostic_menu.h"

#include <string.h>
#include "stddef.h"

#include "menu_items.h"
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"

/*!
  \groupdef menu_static (���������� ������� ����)

  @{
*/
/*!
  \bref ��������� ������ � ����
  @{
*/
static void DisplayMenuItemString(u32 y, const char *ptr);
static void IdleFunc(void);
static void SelFunc(void);
static void UpFunc(void);
static void DownFunc(void);
static void ReturnFunc(void);
static void StopDiagnosticFunc(void);
/*! }@ */


/*!
  \bref ���������� ��� ������ ����
  @{
*/
static u32 MenuItemIndex = 0, nMenuLevel = 0;
static tMenuItem psMenuItem, psCurrentMenuItem;
static tMenu psPrevMenu[MAX_MENU_LEVELS];
static tMenu psCurrentMenu;
static enum DIAGNOSTIC_STATUS diagnostic_status;
/*! }@ */


/*!
  \bref �������� ���� ������� �������� ��
  @{
*/
struct sMenuItem CurrentSettingsMenuItems[] = {
  {"���� ���� ����� �����", IdleFunc, NULL},
  {"�������", ReturnFunc, NULL}};
struct sMenu CurrentSettingsMenu = {"������� ��������� ��", CurrentSettingsMenuItems, countof(CurrentSettingsMenuItems)};
/*! }@ */


/*!
  \bref ������� ���� ���������������� �����
  @{
*/
struct sMenuItem DiagnosticMainMenuItems[] = {
  {"������� ��������� ��", IdleFunc, &CurrentSettingsMenu},
  {"����������� ��������", IdleFunc, NULL},
  {"���������� �������", IdleFunc, NULL},
  {"���������", StopDiagnosticFunc, NULL}};
struct sMenu DiagnosticMainMenu = {"��������������� �����", DiagnosticMainMenuItems, countof(DiagnosticMainMenuItems)};
/*! }@ */


/*!
  \bref ������� ������ �� �������
*/
void DisplayMenuItemString(u32 y, const char *ptr) {
    u32 x;

    LCD_PUTS(0, y, "                                        ");
    x = (MAX_X - (CurrentFont->Width * strlen(ptr))) / 2;
    LCD_PUTS(x, y, ptr);
}


/*!
  \bref ������ �������
*/
void IdleFunc(void) { }


/*!
  \bref ������� �� ������� �/��� ����� ���������������� �����������
*/
void SelFunc(void) {
    psCurrentMenuItem = psMenuItem;

    if(psMenuItem->psSubMenu != NULL)
		{
        MenuItemIndex = 0;
        psCurrentMenu = psMenuItem->psSubMenu;
        psMenuItem = &(psCurrentMenu->psItems)[MenuItemIndex];
        DisplayMenu();
        nMenuLevel++;
        psPrevMenu[nMenuLevel] = psCurrentMenu;
    }
    psCurrentMenuItem->pfMenuFunc();
}


/*!
  \bref ������� �� ���� ����� �����
*/
void UpFunc(void) {
    // ����������� �������� ������ ���� ��� ������������
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;
    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    // ����������� ������ ������ ���� (�� �����)
    if(MenuItemIndex > 0)
        MenuItemIndex--;
    else
        MenuItemIndex = psCurrentMenu->nItems - 1;


    // ����������� ������ ������ ���� ��� ����������
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref ������� �� ���� ����� ����
*/
void DownFunc(void) {
    // ����������� �������� ������ ���� ��� ������������
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;

    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    // ����������� ������ ������ ���� (�� �����)
    if(MenuItemIndex >= ((psCurrentMenu->nItems)-1))
        MenuItemIndex = 0;
    else
        MenuItemIndex++;
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);

    // ����������� ������ ������ ���� ��� ����������
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref ������� � ������� ����
*/
void ReturnFunc(void) {
    if(nMenuLevel == 0)
        nMenuLevel++;

    psCurrentMenu = psPrevMenu[nMenuLevel-1];
    psMenuItem = &psCurrentMenu->psItems[0];
    MenuItemIndex = 0;
    nMenuLevel--;

    DisplayMenu();
}


/*!
  \bref �������� ����� ����������
*/
void StopDiagnosticFunc(void) {
  diagnostic_status = DIGANOSTIC_STOP;
  ResetLCD();
  LCD_CLS();
}

/*! }@ */

/*!
  \groupdef menu_nonestatic (������������ ������� ����)

  @{
*/
/*!
\bref ������������� ����
*/
void MenuInit(void) {
  diagnostic_status = DIGANOSTIC_ON;
  MenuItemIndex = 0;
  nMenuLevel = 0;
  psCurrentMenu = &DiagnosticMainMenu;
  psPrevMenu[nMenuLevel] = psCurrentMenu;
  psMenuItem = DiagnosticMainMenuItems;
  CurrentFont = &Font_6x8;
}


/*!
  \bref ������� �������� ����
*/
void DisplayMenuTitle(const char *ptr) {
    u32 x, y;

    x = (MAX_X - (CurrentFont->Width * strlen(ptr))) / 2;
    LCD_PUTS(x, 0, ptr);

    y = CurrentFont->Height + 1;
    CurrentMethod = MET_OR;
    LCD_Line(0, y, MAX_X, y);
}

/*!
  \bref ������� ���� �� �������
*/
void DisplayMenu(void) {
    u32 y, index;
    tMenuItem psMenuItem2;

    LCD_CLS();
    CurrentMethod = MET_AND;

    // ���������� ��������� ����
    DisplayMenuTitle(psCurrentMenu->psTitle);
  // ���������� ������ ����
    for (index = 0, y = CurrentFont->Height + 4;
                index < psCurrentMenu->nItems;
                index++, y += CurrentFont->Height + 2) {
        psMenuItem2 = &(psCurrentMenu->psItems[index]);
        DisplayMenuItemString(y, psMenuItem2->psTitle);
    }

    // ���������� ������� �����
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref �������� ������� ������ "�����" � ������� � ���� ����
*/
void BackToMenuOnSel(void) {
  WAIT_UNTIL_KEY_PRESSED(SEL);
  DisplayMenu();
}


/*!
  \bref �������-��������� ������� ������
*/
void ReadKey(void) {
    u32 key;

    while (diagnostic_status == DIGANOSTIC_ON) {
        key = GetKey();
        switch (key) {
            case SEL:   SelFunc();  break;
            case UP:    UpFunc();   break;
            case DOWN:  DownFunc(); break;
        }
        WAIT_UNTIL_KEY_RELEASED(key);
    }
}

/*! }@ */