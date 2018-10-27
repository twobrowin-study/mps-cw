/*!
\file
  \bref Меню диагностического режима
  \authors Дубровин Егор гр. ИУ6-72, LDM-SYSTEMS (2013 г.)
  \date Октябрь 2018 года
  \attention Этот файл написан в кодировке Windows 1251!

  Файл содержит меню диагностического режима и тексты процедур его работы.
    Файл написан на основе библиотки LCDLib
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
  \groupdef menu_static (Внутренние функции меню)

  @{
*/
/*!
  \bref Процедуры работы с меню
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
  \bref Переменные для работы меню
  @{
*/
static u32 MenuItemIndex = 0, nMenuLevel = 0;
static tMenuItem psMenuItem, psCurrentMenuItem;
static tMenu psPrevMenu[MAX_MENU_LEVELS];
static tMenu psCurrentMenu;
static enum DIAGNOSTIC_STATUS diagnostic_status;
/*! }@ */


/*!
  \bref Описание меню текущих настроек МК
  @{
*/
struct sMenuItem SensorSettingsMenuItems[] = {
  #ifdef SENSORS_COUNT
    #if(SENSORS_COUNT > 0)
      { "Датчик 1", IdleFunc, NULL },
    #endif
    #if(SENSORS_COUNT > 1)
      { "Датчик 2", IdleFunc, NULL },
    #endif
    #if(SENSORS_COUNT > 2)
      { "Датчик 3", IdleFunc, NULL },
    #endif
    #if(SENSORS_COUNT > 3)
      { "Датчик 4", IdleFunc, NULL },
    #endif
    #if(SENSORS_COUNT > 4)
      { "Датчик 5", IdleFunc, NULL },
    #endif
    #if(SENSORS_COUNT > 5)
      { "Датчик 6", IdleFunc, NULL },
    #endif
  #endif
  {"Возврат", ReturnFunc, NULL}};
struct sMenu SensorSettingsMenu = {"Выбор датчика", SensorSettingsMenuItems, countof(SensorSettingsMenuItems)};
/*! }@ */


/*!
  \bref Главное меню диагностического режма
  @{
*/
struct sMenuItem DiagnosticMainMenuItems[] = {
  {"Текущие настройки МК", IdleFunc, &SensorSettingsMenu},
  {"Диагностика датчиков", IdleFunc, NULL},
  {"Дигностика сервера", IdleFunc, NULL},
  {"Завершить", StopDiagnosticFunc, NULL}};
struct sMenu DiagnosticMainMenu = {"Диагностический режим", DiagnosticMainMenuItems, countof(DiagnosticMainMenuItems)};
/*! }@ */


/*!
  \bref Вывести строку на дисплей
*/
void DisplayMenuItemString(u32 y, const char *ptr) {
    u32 x;

    LCD_PUTS(0, y, "                                        ");
    x = (MAX_X - (CurrentFont->Width * strlen(ptr))) / 2;
    LCD_PUTS(x, y, ptr);
}


/*!
  \bref Пустая функция
*/
void IdleFunc(void) { }


/*!
  \bref Переход на подменю и/или вызов соответствующего обработчика
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
  \bref Переход на один пункт вверх
*/
void UpFunc(void) {
    // Отображение текущего пункта меню как невыбранного
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;
    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    // Определение нового пункта меню (по циклу)
    if(MenuItemIndex > 0)
        MenuItemIndex--;
    else
        MenuItemIndex = psCurrentMenu->nItems - 1;


    // Отображение нового пункта меню как выбранного
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref Переход на один пункт вниз
*/
void DownFunc(void) {
    // Отображение текущего пункта меню как невыбранного
    psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
    CurrentMethod = MET_AND;

    DisplayMenuItemString((MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), psMenuItem->psTitle);

    // Определение нового пункта меню (по циклу)
    if(MenuItemIndex >= ((psCurrentMenu->nItems)-1))
        MenuItemIndex = 0;
    else
        MenuItemIndex++;
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);

    // Отображение нового пункта меню как выбранного
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref Возврат в главное меню
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
  \bref Звершить режим дигностики
*/
void StopDiagnosticFunc(void) {
  diagnostic_status = DIGANOSTIC_STOP;
  ResetLCD();
  LCD_CLS();
}

/*! }@ */

/*!
  \groupdef menu_nonestatic (Интерфейсные функции меню)

  @{
*/
/*!
\bref Инициализация меню
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
  \bref Вывести название меню
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
  \bref Вывести меню на дисплей
*/
void DisplayMenu(void) {
    u32 y, index;
    tMenuItem psMenuItem2;

    LCD_CLS();
    CurrentMethod = MET_AND;

    // Отображаем заголовок меню
    DisplayMenuTitle(psCurrentMenu->psTitle);
  // Отображаем пункты меню
    for (index = 0, y = CurrentFont->Height + 4;
                index < psCurrentMenu->nItems;
                index++, y += CurrentFont->Height + 2) {
        psMenuItem2 = &(psCurrentMenu->psItems[index]);
        DisplayMenuItemString(y, psMenuItem2->psTitle);
    }

    // Определяем текущий пункт
    psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
    CurrentMethod = MET_NOT_XOR;
    LCD_PUTS(0, (MenuItemIndex * (CurrentFont->Height + 2) + CurrentFont->Height + 4), "                                        ");
}


/*!
  \bref Ожидание нежатия кнопки "Выбор" и возврат в меню выше
*/
void BackToMenuOnSel(void) {
  WAIT_UNTIL_KEY_PRESSED(SEL);
  DisplayMenu();
}


/*!
  \bref Функция-диспетчер нажатых кнопок
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
