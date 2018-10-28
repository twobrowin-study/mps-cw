/*!
\file
  \bref Файл описания всех определителй программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит описание всех перечисляемых типов и макросов программы
*/
#ifndef DEFS_H
#define DEFS_H

/// Определение типа uint
/// \details Тип беззнакового целого 32-х битного числа
typedef unsigned int uint;

/// Перечисляемый тип статуса завершения работы вызываемой функции
/// \details Используется как возвращаемый тип вместо void
enum END_STATUS_ENUM {
  END_OK, ///< Указывает, что завершение успешное
  END_NOT_OK, ///< Указывает, что завершение не успешное
  END_ERROR = 2567 ///< Указывает, что завершение ошибочное
};

/// Переопределение название статуса возврата
#define END_STATUS enum END_STATUS_ENUM

/// Перечисляемый тип разрешения завершения работы диагностического режима
/// \details Используется в диагностическом меню
enum DIAGNOSTIC_STATUS {
  DIGANOSTIC_ON, ///< Указывает, что диагностический режим включён
  DIGANOSTIC_STOP ///< Указывает, что диагностический режим необходимо завершить
};

#endif // DEFS_H
