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

END_STATUS delay_init(void);
END_STATUS delay(uint ms);

#endif // DELAY_H
