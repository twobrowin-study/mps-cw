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
#include "format.h"

/// Текущее время
#define current_date BKP_RTC_GetCounter()
/// Начало суточного интервала
#define interval_start MDR_BKP->REG_00
/// Конец суточного интеравла
#define interval_end MDR_BKP->REG_01
/// Величина пропорции времени суток в секундах
#define get_time_from_date(n) ((n) % 86400)
/// Величина пропорции 15 минут в секундах
#define fifteen_minutes_proportion(n) ((n) % 60)

uint32_t time_init(void);
uint32_t time_scale(void);
uint32_t time_controll(void);
uint32_t time_update(uint32_t time, uint32_t binterval_start, uint32_t binterval_end);

#endif // TIME_H
