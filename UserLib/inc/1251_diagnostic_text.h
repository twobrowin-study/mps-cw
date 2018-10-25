/*!
\file
  \bref Текст меню диагностичесого режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года
  \attention Этот файл написано в кодировке Windows 1251!

  Файл содержит реализации подпрограмм меню диагностического режима
*/
#ifndef DIAGNOSTIC_TEXT_H
#define DIAGNOSTIC_TEXT_H

// Подключение библиотеки поддержки LCD
#include <stdio.h>
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"
#include "menu.h"
#include "image.h"

/// Пустая функция
static void IdleFunc(void) {}

/// Главное меню диагностического режима
struct sMenuItem DiagnosticMainMenuItems[] = {
  {"Один", IdleFunc, NULL},
  {"Два", IdleFunc, NULL},
  {"3", IdleFunc, NULL},
  {"Четырнадцать", IdleFunc, NULL},
  {"Пятьдесят", IdleFunc, NULL}};
struct sMenu DiagnosticMainMenu = {"Диагностический режим", DiagnosticMainMenuItems, countof(DiagnosticMainMenuItems)};

#endif // DIAGNOSTIC_TEXT_H
