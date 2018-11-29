/*!
\file
  \bref Заголовочный файл форматизации времени
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит описание функций форматизации времени и данных от сервера
 */
#ifndef TIME_FORMAT_H
#define TIME_FORMAT_H

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "time.h"
#include "can.h"

/* Функции для вывода информации в меню тестирования */
char* time_as_string(uint time);

/* Функции для разбора времени суток и интервалов */
uint deformat_dai(
  const char* data,
  char*  dd,
  uint* ctb,
  char*  is,
  uint* isb,
  char*  ie,
  uint* ieb);
uint upscalestrlen(char *str, uint len, char ch);

/* Функции для передачи по USB */
char* current_settings();
char* sensor_data(uint adr, uint data);

#endif // TIME_FORMAT_H
