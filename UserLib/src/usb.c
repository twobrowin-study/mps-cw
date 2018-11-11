/*!
\file
  \bref Подключение по USB
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит реализацию соединения МК с сервером по каналу USB в режиме Device
 */
#include "usb.h"

#include <string.h>


/// Буфер входных данных USB CDC
static uint8_t recv_buf[MDR_USB_CDC_MAX_PACKET_SIZE];


/*!
  \bref Инициализация USB
  \return Статус завершения
  
  Оганизация тактирования доп. блоков для работы USB и инициализация USB CDC
 */
END_STATUS usb_init(void) {
  RST_CLK_PCLKcmd(
    RST_CLK_PCLK_RST_CLK |
    RST_CLK_PCLK_BKP |
    RST_CLK_PCLK_POWER,
    ENABLE
  );

  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
  EEPROM_SetLatency(EEPROM_Latency_2);

  mdr_usb_cdc_init();

  clean_usb_cdc_recv_buf();
  check_usb_cdc();

  return END_OK;
}


/*!
  \brief      Получить актуальную вход USB CDC во внутреннюю переменную

  \return     Статус завершения

  Это контрольная точка, которая определяет далее работу со входом USB CDC
 */
END_STATUS check_usb_cdc(void) {
  if(mdr_usb_cdc_is_recv_buf_setted() == MDR_USB_CDC_Fail)
    mdr_usb_cdc_set_recv_buf(recv_buf);
  return END_OK;
}


/*!
  \brief      Получить входной буфер USB CDC

  \return     Буфер USB CDC как сторка

  Принимаемые данные жёстко ограничиваются в 26 символов согласно протоколу передачи
 */
char* get_recv_via_usb_cdc(void) {
  recv_buf[26] = '\0';
  return (char*) recv_buf;
}


/*!
  \brief      Очистка буфера USB CDC

  \return     Статус завершения
 */
END_STATUS clean_usb_cdc_recv_buf(void) {
  recv_buf[0] = '\0';
  return END_OK;
}


/*!
  \brief      Отправляет через USB CDC

  \param[in]  str   Строка для передачи

  \return     Результат завершения функции
 */
END_STATUS send_via_usb_cdc(const char *str) {
  if(mdr_usb_cdc_is_sending() == MDR_USB_CDC_Fail) 
    mdr_usb_cdc_send(strlen(str), (uint8_t*) str);

  return END_OK;
}
