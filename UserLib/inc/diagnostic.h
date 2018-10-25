/*!
\file
  \bref Заголовочный файл меню диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит подключение всех небходимых библиотек для работы меню диагностического режима,
    а также описание функий обеспечения работы меню диагностического режима
*/
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>

#include "defs.h"

END_STATUS menu_init(void);
END_STATUS menu_release(void);

#endif // DIAGNOSTIC_H
