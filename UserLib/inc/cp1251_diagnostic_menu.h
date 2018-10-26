/*!
\file
  \bref Меню диагностического режима
  \authors Дубровин Егор гр. ИУ6-72, LDM-SYSTEMS (2013 г.)
  \date Октябрь 2018 года
  \attention Этот файл написан в кодировке Windows CP1251!

  Файл содержит описания подключения к меню диангностического режима и
    связи с библиотекой LCDLib
*/
#ifndef CP1251_DIAGNOSTIC_MENU_H
#define CP1251_DIAGNOSTIC_MENU_H

#include "defs.h"

#include "types.h"

#define  MAX_MENU_LEVELS 2

#define countof(a) (sizeof(a) / sizeof(*(a)))

typedef void (* tMenuFunc)(void);
typedef struct sMenuItem *tMenuItem;
typedef struct sMenu *tMenu;

/// Структура описания пункта меню
struct sMenuItem
{
  const char * psTitle;
  tMenuFunc pfMenuFunc;
  tMenu psSubMenu;
};

/// Структура описания меню
struct sMenu
{
  const char* psTitle;
  tMenuItem psItems;
  u32 nItems;
};

void MenuInit(void);
void DisplayMenuTitle(const char *ptr);
void DisplayMenu(void);
void BackToMenuOnSel(void);
void ReadKey(void);

#endif // CP1251_DIAGNOSTIC_MENU_H
