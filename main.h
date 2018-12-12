/*!
\file
  \bref Заголовочный основной файл программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь-Декабрь 2018 года

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

#ifndef SENSORS_COUNT
/// Количество датчиков если не задано
#define SENSORS_COUNT 6
#endif

/// Макрос переключение светодиода
#define LED_TOGGLE(Pin) (MDR_PORTC->RXTX ^= Pin)
/// Макрос включения светодиода
#define LED_ON(Pin) (MDR_PORTC->RXTX |= Pin)
/// Макрос переключение светодиода
#define LED_OFF(Pin) (MDR_PORTC->RXTX &= ~Pin)

/// Макрос светодиода 0
#define LED0 PORT_Pin_0
/// Макрос светодиода 1
#define LED1 PORT_Pin_1

uint32_t init(void);
uint32_t rst_clk_pll_init(void);
uint32_t sensors_data_transmit(void);

#endif // MAIN_H
