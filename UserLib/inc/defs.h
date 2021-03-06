/*!
\file
  \bref Файл описания всех определителй программы
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит описание всех перечисляемых типов и макросов программы
 */
#ifndef DEFS_H
#define DEFS_H

/// Перечисляемый тип статуса завершения работы вызываемой функции
/// \details Используется как возвращаемый тип вместо void
enum END_STATUS_ENUM {
  END_OK = 0, ///< Указывает, что завершение успешное
  END_NOT_OK, ///< Указывает, что завершение не успешное
  END_GOOD, ///< Указывает, что завершение не плохое
  END_ERROR = 2567 ///< Указывает, что завершение ошибочное
};

/// Перечисляемый тип разрешения завершения работы диагностического режима
/// \details Используется в диагностическом меню
enum DIAGNOSTIC_STATUS {
  DIGANOSTIC_ON, ///< Указывает, что диагностический режим включён
  DIGANOSTIC_STOP ///< Указывает, что диагностический режим необходимо завершить
};

/// Перечисляемый тип, обозначающий режим работы CAN
/// \details    Используется при инициализации CAN
enum CAN_INIT_TYPE {
  CAN_INIT_COMMON, ///< Указывает, что иниициализация происходит в нормальном режиме
  CAN_INIT_SELFTEST ///< Указывает, что иниициализация происходит в режиме самотестирования
};

/// Перечисляемый тип, обозначающий режим вызова сборщика строки данных датчика
/// \details    Используется при форматизации даных датчика
enum SENSOR_FORMAT_MODE {
  SENSOR_FORMAT_MODE_USB, ///< Указывает, что форматизация происходит для режима USB
  SENSOR_FORMAT_MODE_DIAGNOSTIC ///< Указывает, что форматизация происходит для режима диагностики
};

#endif // DEFS_H
