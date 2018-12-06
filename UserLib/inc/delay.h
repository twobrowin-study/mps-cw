/*!
\file
  \bref Заголовочный файл фунционирования точной задержки
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит подключение всех небходимых библиотек для работы точной задержки,
    а также описание методов точной задержки
 */
#ifndef DELAY_H
#define DELAY_H

#include <MDR32F9Qx_config.h>
#include <system_MDR32F9Qx.h>

#include "defs.h"

uint32_t delay_init(void);
uint32_t delay(uint32_t ms);

#endif // DELAY_H
