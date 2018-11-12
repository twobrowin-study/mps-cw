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

char* time_as_string(uint time);
END_STATUS deformat_dai(
  const char* data,
  char*  dd,
  uint* ctb,
  char*  is,
  uint* isb,
  char*  ie,
  uint* ieb);
char* current_settings();
END_STATUS upscalestrlen(char *str, uint len, char ch);

#endif // TIME_FORMAT_H
