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
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>

#include "defs.h"
#include "diagnostic.h"
#include "diagnostic_start.h"
#include "delay.h"
#include "time.h"
#include "usb.h"
#include "can.h"

uint init(void);
uint rst_clk_pll_init(void);

#endif // MAIN_H
