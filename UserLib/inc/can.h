/*!
\file
  \bref Сеть окружения контроллера CAN
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит описание инициализации счётчика реального времени и суточных интервалов
 */
#ifndef CAN_H
#define CAN_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_can.h>

#include "defs.h"
#include "time.h"

/// Адрес МК
#define MC_CAN_ADDR 0x001

#ifdef SENSORS_COUNT
  /// Макрос адреса датчика
  #define CAN_SENSOR_ADDR(a) ((a & 0xf)<<16)
  
  /// Макрос проверки существования адреса
  #define IS_CAN_SENSOR_ADDR(a) (SENSORS_COUNT >= a)
#endif

/// Номера буферов CAN для приёма и передачи
#define CAN_RX_BUF 0
#define CAN_TX_BUF 1

uint can_init(void);
uint get_sensor_data(uint addr, uint* data);

#endif // CAN_H
