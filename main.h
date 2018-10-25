/*!
\file
  \bref Заголовочный основной файл программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит подключение всех небходимых библиотек для работы основного алгоритма
*/
#ifndef MAIN_H
#define MAIN_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>

#include "defs.h"
#include "diagnostic.h"
#include "delay.h"

/// Подключение библиотеки поддержки LCD
#include <stdio.h>
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"
#include "menu.h"
#include "image.h"

END_STATUS init(void);
END_STATUS rst_clk_pll_init(void);

#endif // MAIN_H
