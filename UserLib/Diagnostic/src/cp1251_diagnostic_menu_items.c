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

/*!
  \bref Вывести на дсплей текущие настройки МК

  Для выхода в меню следует нажать кнопку "Выбор"
*/
void DiagnoseSettings(void) {
  /* Выводим заглавие */
  LCD_CLS();
  CurrentMethod = MET_AND;
  DisplayMenuTitle("Шрифты дофига");
  WAIT_UNTIL_KEY_RELEASED(SEL);

  /* Вывод на экран всех настроек построчно */
  // Строка Текущее время
  uint y = CurrentFont->Height + 4;
  DisplayMenuItemStringPrefix(y, "Время : ", time_as_string(current_time % day_proportion));

  //  Время начала интервала
  y += CurrentFont->Height + 2;
  DisplayMenuItemIntPrefix(y, "Время : ", current_time % day_proportion, 10);

  /* Возвращение в меню по нажатию кнопки "Выбор" */
  BackToMenuOnSel();
}
