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
 *  ���� Menu.c: ������������� ��������� ����
 *
 *============================================================================================*/

#include <string.h>
#include "stddef.h"

#include "menu.h"
#include "menu_items.h"
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"

#ifdef NODIAGNOSTICMENU
// ��������� ��������� ������� � ������������
static void DisplayMenuItemString(u32 y, const char *ptr);
static void IdleFunc(void);
static void SelFunc(void);
static void UpFunc(void);
static void DownFunc(void);
static void ReturnFunc(void);

// �������� ����

typedef void (* tMenuFunc)(void);
typedef struct sMenuItem *tMenuItem;
typedef struct sMenu *tMenu;

static u32 MenuItemIndex = 0, nMenuLevel = 0;
//static u32 ItemNumb[MAX_MENU_LEVELS];

static tMenuItem psMenuItem, psCurrentMenuItem;
static tMenu psPrevMenu[MAX_MENU_LEVELS];
static tMenu psCurrentMenu;

struct sMenuItem
{
  const char * psTitle;
  tMenuFunc pfMenuFunc;
  tMenu psSubMenu;
};

struct sMenu
{
  const char* psTitle;
  tMenuItem psItems;
  u32 nItems;
};

// ���� ������� �������
struct sMenuItem TextMenuItems[] = {{"������", FontFunc, NULL},
                                    {"�����", StyleFunc, NULL},
                                    {"������", BookFunc, NULL},
                                    {"�������", ReturnFunc, NULL}};
struct sMenu TextMenu = {"�����", TextMenuItems, countof(TextMenuItems)};

struct sMenuItem GraphicMenuItems[] = {{"���������", ElementsFunc, NULL},
                                       {"����������", IndicatorsFunc, NULL},
                                       {"�������", ReturnFunc, NULL}};
struct sMenu GraphicMenu = {"�������", GraphicMenuItems, countof(GraphicMenuItems)};

struct sMenuItem LEDsMenuItems[] = {{"��������", LightsOnFunc, NULL},
                                    {"�������", ReturnFunc, NULL}};
struct sMenu LEDsMenu = {"����������", LEDsMenuItems, countof(LEDsMenuItems)};

struct sMenuItem TechnologyMenuItems[] = {{"LDM-SYSTEMS", LDM, NULL},
                                    {"��� ��� �������", MILAND, NULL},
                                    {"��� ����� �����", PHYTON, NULL},
                                    {"��� ����", MELT, NULL},
                                    {"�������", ReturnFunc, NULL}};
struct sMenu TechnologyMenu = {"����������", TechnologyMenuItems, countof(TechnologyMenuItems)};

// ������� ����
struct sMenuItem MainMenuItems[] = {
  {"�����", IdleFunc, &TextMenu},
  {"�������", IdleFunc, &GraphicMenu},
  {"����������", IdleFunc, &LEDsMenu},
  {"����������", IdleFunc, &TechnologyMenu},
  {"� ���������", AboutFunc, NULL}};
struct sMenu MainMenu = {"������� ����", MainMenuItems, countof(MainMenuItems)};


// ��������� ������� ��������� ����

void DisplayMenuItemString(u32 y, const char *ptr) {
    u32 x;

    LCD_PUTS(0, y, "                                        ");
    x = (MAX_X - (CurrentFont->Width * strlen(ptr))) / 2;
    LCD_PUTS(x, y, ptr);
}


// ��������� �����������

void IdleFunc(void)
{

}


// SEL - ������� �� ������� �/��� ����� ���������������� �����������
void SelFunc(void)
{
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


// UP - ������� �� ���� ����� �����
void UpFunc(void)
{
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

//    ItemNumb[nMenuLevel] = MenuItemIndex;
}


// DOWN - ������� �� ���� ����� ����
void DownFunc(void)
{
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

//    ItemNumb[nMenuLevel] = MenuItemIndex;
}


// ������� � ������� ����
void ReturnFunc(void)
{
    if(nMenuLevel == 0)
        nMenuLevel++;

    psCurrentMenu = psPrevMenu[nMenuLevel-1];
    psMenuItem = &psCurrentMenu->psItems[0];
//    ItemNumb[nMenuLevel] = 0;
    MenuItemIndex = 0;
    nMenuLevel--;

    DisplayMenu();
}


// ������������ �������

void DisplayMenuTitle(const char *ptr)
{
    u32 x, y;

    x = (MAX_X - (CurrentFont->Width * strlen(ptr))) / 2;
    LCD_PUTS(x, 0, ptr);

    y = CurrentFont->Height + 1;
    CurrentMethod = MET_OR;
    LCD_Line(0, y, MAX_X, y);
}


void Menu_Init(void)
{
    psCurrentMenu = &MainMenu;
    psPrevMenu[nMenuLevel] = psCurrentMenu;
    psMenuItem = MainMenuItems;
    CurrentFont = &Font_6x8;
}


void DisplayMenu(void)
{
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


// ���� ������� SEL � ������������ � ������� ����
void BackToMenuOnSel(void)
{
  WAIT_UNTIL_KEY_PRESSED(SEL);
  DisplayMenu();
}


// �������-���������
void ReadKey(void)
{
    u32 key;

    while (1) {
        key = GetKey();
        switch (key) {
            case SEL:   SelFunc();  break;
            case UP:    UpFunc();   break;
            case DOWN:  DownFunc(); break;
        }
        WAIT_UNTIL_KEY_RELEASED(key);
    }
}
#endif // NODIAGNOSTICMENU

/*============================================================================================
 * ����� ����� Menu.c
 *============================================================================================*/
