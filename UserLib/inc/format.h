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
char* time_as_string(uint32_t time);

/* Функции для разбора времени суток и интервалов */
uint32_t deformat_dai(
  const char* data,
  char*  dd,
  uint32_t* ctb,
  char*  is,
  uint32_t* isb,
  char*  ie,
  uint32_t* ieb);
uint32_t upscalestrlen(char *str, uint32_t len, char ch);

/* Функции для передачи по USB */
char* current_settings();
char* sensor_data(uint32_t adr, uint32_t data, uint32_t mode);

#endif // TIME_FORMAT_H
