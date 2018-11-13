/*!
\file
  \bref Пункты меню диагностического режима
  \authors Дубровин Егор гр. ИУ6-72, LDM-SYSTEMS (2013 г.)
  \date Октябрь 2018 года
  \attention Этот файл написан в кодировке Windows CP1251!

  Файл содержит описания функций, вызываемых для вывода на ЖК дисплей
    функций диагностического режима
 */
#include "cp1251_diagnostic_menu_items.h"

 #include "usb.h"

/*!
  \bref Вывести на дсплей текущие настройки МК

  Для выхода в меню следует нажать кнопку "Выбор"
 */
void DiagnoseSettings(void) {
  /* Выводим заглавие */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("Текущие настройки МК");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* Вывод на экран всех настроек построчно */
  // Строка Текущее время
  uint y = CurrentFont->Height + 4;
  DisplayMenuItemStringPrefix(y, "Время: ", time_as_string(current_time % day_proportion));

  // Время начала интервала
  y += CurrentFont->Height + 2;
  DisplayMenuItemStringPrefix(y, "Начало в: ", time_as_string(interval_start));

  // Время конца интервала
  y += CurrentFont->Height + 2;
  DisplayMenuItemStringPrefix(y, "Конец  в: ", time_as_string(interval_end));

  // Сообщение для пользователя
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "Нажмите ВЫБОР");

  /* Возвращение в меню по нажатию кнопки "Выбор" */
  BackToMenuOnSel();
}


/*!
  \brief      Диагностика сервера

  Для выхода в меню следует нажать кнопку "Выбор"
 */
void DiagnoseServer(void) {
  /* Выводим заглавие */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("Дигностика сервера");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* Вывод строки о наличии подключения */
  uint y = CurrentFont->Height + 4;
  if (is_usb_connected()) { // Подключение установлено
    DisplayMenuItemString(y, "Связь установлена");

    /* Вывод строки об отправке сообщения серверу */
    send_via_usb_cdc(current_settings());
    y += CurrentFont->Height + 2;
    DisplayMenuItemString(y, "Пакет отправлен");

    /* Вывод строки о получении ответа */
    uint trys = 100; // Количество попыток получить пакет
    while ((time_controll() != END_OK) && (trys)) {
      trys -= 1;
      delay(10);
    }
    y += CurrentFont->Height + 2;
    if (trys) { // Пакет получен
      DisplayMenuItemString(y, "Пакет принят");

      /* Вывод сообщения о завершении диагностики */
      y += CurrentFont->Height + 2;
      DisplayMenuItemString(y, "Диагностика завершена");
    }
    else // Пакет так и не получен
      DisplayMenuItemString(y, "Пакет не принят");
  }
  else // Подключение не установлено
    DisplayMenuItemString(y, "Связь не установлена");

  /* Сообщение для пользователя */
  y += CurrentFont->Height + 2;
  DisplayMenuItemString(y, "Нажмите ВЫБОР");

  /* Возвращение в меню по нажатию кнопки "Выбор" */
  BackToMenuOnSel();
}
