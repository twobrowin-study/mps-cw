/*!
\file
  \bref Сеть окружения контроллера CAN
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь-Декабрь 2018 года

  Файл содержит описание инициализации счётчика реального времени и суточных интервалов
 */
#ifndef CAN_H
#define CAN_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_can.h>

#include "defs.h"
#include "time.h"

/// Адрес МК
#define MC_CAN_ADDR 0x001

/// Маска фильтра адреса МК
#define MC_CAN_ADDR_MASK 0x001

/// Макрос адреса датчика
#define CAN_SENSOR_ADDR(a) ((a&0xf)<<8)

#ifdef CAN_SENSORS_SELFTEST
  /// Маска фильтра адреса датчика
  #define CAN_SENSOR_ADDR_MASK 0x700

  /// Макрос номера CAN буфера датчика
  #define CAN_SENSOR_BUF(a) ((a&0x7) + CAN_RX_BUF)
#endif

/// Макрос проверки существования адреса
#define IS_SENSOR_ADDR(a) ((SENSORS_COUNT >= a) && (a > 0))

/// Номера буферов CAN для приёма и передачи
#define CAN_TX_BUF 0
#define CAN_RX_BUF 1

uint32_t can_init(void);
uint32_t get_sensor_data(uint32_t addr, uint32_t* data);

#endif // CAN_H
