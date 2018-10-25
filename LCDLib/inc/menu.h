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
 *--------------------------------------------------------------------------------------------
 *
 *  ���� Menu.h: ������������� ��������� ����
 *
 *============================================================================================*/

#ifndef __MENU_H
#define __MENU_H

#include <stdio.h>

#include "types.h"

#define  MAX_MENU_LEVELS 2

#define countof(a) (sizeof(a) / sizeof(*(a)))

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

void Menu_Init(void);
void DisplayMenuTitle(const char *ptr);
void DisplayMenu(void);
/* ���� ������� SEL � ������������ � ������� ���� */
void BackToMenuOnSel(void);
/* �������-��������� */
void ReadKey(void);

#endif/* __MENU_H */

/*============================================================================================
 * ����� ����� Menu.h
 *============================================================================================*/

