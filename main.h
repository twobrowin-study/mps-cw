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

#include "defs.h"
#include "menu.h"
#include "delay.h"

END_STATUS init(void);

#endif // MAIN_H
