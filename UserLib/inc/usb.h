/*!
\file
  \bref Заголовочный файл работы с сервером по каналу USB
  \authors Дубровин Егор гр. ИУ6-72, JTzar http://forum.milandr.ru
  \date Ноябрь 2018 года

  Файл содержит макросы и публичные функции для подключения МК как
    USB Device СDC с использованием библиотеки USBLib
 */
#ifndef USB_H
#define USB_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>
#include <MDR32F9Qx_rst_clk.h>

#include "mdr_usb_cdc.h"
#include "ring_buffer.h"

#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_power.h"

// Макросы определения частотных характеристик для работы USB
#define MDR_USB_PLL                 6
#define MDR_USB_HSE_VALUE           HSE_Value

#include "defs.h"

END_STATUS usb_init(void);
END_STATUS check_usb(void);

#endif // USB_H
