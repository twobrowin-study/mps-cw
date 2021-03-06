/*!
\file
  \bref Заголовочный файл диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит подключение всех небходимых библиотек для работы меню диагностического режима,
    а также описание функий обеспечения работы меню диагностического режима
 */
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H


#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>


#include "defs.h"
#include "cp1251_diagnostic_menu.h"


// Библиотеки подключения к LCDLib
#include "menu_items.h"
#include "types.h"
#include "lcd.h"
#include "gl.h"
#include "text.h"
#include "joystick.h"
#include "leds.h"


uint32_t diagnostic_init(void);
uint32_t diagnostic_release(void);


#endif // DIAGNOSTIC_H
