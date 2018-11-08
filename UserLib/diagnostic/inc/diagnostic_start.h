/*!
\file
  \bref Заголовочный файл поддержки старта диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит описание функций поддрежки старта диагностического режима.
    Эта функция должна быть вызвана независимо от функций из diagnostic.h
 */
#ifndef DIAGNOSTIC_START_H
#define DIAGNOSTIC_START_H


#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_timer.h>


#include "defs.h"
#include "diagnostic.h"


END_STATUS diagnostic_start_init(void);
END_STATUS sel_butt_init(void);


#endif // DIAGNOSTIC_START_H
