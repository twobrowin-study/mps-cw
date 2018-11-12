/*!
\file
  \bref Заголовочный файл временных констант МК
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит описание инициализации счётчика реального времени и суточных интервалов
 */
#ifndef TIME_H
#define TIME_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_bkp.h>

#include "defs.h"
#include "usb.h"
#include "time_format.h"

/// Текущее время
#define current_time BKP_RTC_GetCounter()
/// Начало суточного интервала
#define interval_start MDR_BKP->REG_00
/// Конец суточного интеравла
#define interval_end MDR_BKP->REG_01
/// Величина пропорции времени суток в секундах
#define day_proportion 86400

END_STATUS time_init(void);
END_STATUS time_scale(void);
END_STATUS time_controll(void);
END_STATUS time_update(uint time, uint binterval_start, uint binterval_end);

#endif // TIME_H
