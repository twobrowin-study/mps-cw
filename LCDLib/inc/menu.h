/*============================================================================================
 *
 *  (C) 2013, LDM-SYSTEMS
 *
 *  Демонстрационный проект для отладочной платы LDM-K1986BE92QI
 *
 *  Данное ПО предоставляется "КАК ЕСТЬ", т.е. исключительно как пример, призванный облегчить
 *  пользователям разработку их приложений для процессоров Milandr K1986BE92QI. Компания LDM-SYSTEMS
 *  не несет никакой ответственности за возможные последствия использования данного, или
 *  разработанного пользователем на его основе, ПО.
 *
 *--------------------------------------------------------------------------------------------
 *
 *  Файл Menu.h: Иерархическое текстовое меню
 *
 *============================================================================================*/

#ifndef __MENU_H
#define __MENU_H

#include <stdio.h>

#include "types.h"

#define  MAX_MENU_LEVELS 2

#define countof(a) (sizeof(a) / sizeof(*(a)))

// Описание меню 

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

void Menu_Init(void);
void DisplayMenuTitle(const char *ptr);
void DisplayMenu(void);
/* Ждет нажатия SEL и возвращаемся в главное меню */
void BackToMenuOnSel(void);
/* Функция-диспетчер */
void ReadKey(void);

#endif/* __MENU_H */

/*============================================================================================
 * Конец файла Menu.h
 *============================================================================================*/

