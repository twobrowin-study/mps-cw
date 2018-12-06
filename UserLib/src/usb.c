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
static uint8_t recv_buf[MDR_USB_CDC_MAX_PACKET_SIZE] = "";


/*!
  \bref Инициализация USB
  \return Статус завершения
  
  Оганизация тактирования доп. блоков для работы USB и инициализация USB CDC
 */
uint32_t usb_init(void) {
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
uint32_t check_usb_cdc(void) {
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

  Функция напрямую очищает 3 оконечную точку
 */
uint32_t clean_usb_cdc_recv_buf(void) {
  strcpy((char*) recv_buf, "");
  uint8_t no_buf[MDR_USB_CDC_MAX_PACKET_SIZE] = "";
  if(mdr_usb_cdc_is_recv_buf_setted() == MDR_USB_CDC_Fail)
    mdr_usb_cdc_set_recv_buf(no_buf);
  return END_OK;
}


/*!
  \brief      Отправляет через USB CDC

  \param[in]  str   Строка для передачи

  \return     Результат завершения функции
 */
uint32_t send_via_usb_cdc(const char *str) {
  if(mdr_usb_cdc_is_sending() == MDR_USB_CDC_Fail) {
    mdr_usb_cdc_send(strlen(str), (uint8_t*) str);
    return END_OK;
  }

  return END_NOT_OK;
}


/*!
  \brief      Состояние подключёния USB

  \return     Состояние подключёния USB

  Возвращает состояние линии USB - битов SCRXLS = MDR_USB->SLS[1:0]
    RESET = 0 - ложь
    LOW_SPEED_CONNECT = 1 - истина
    FULL_SPEED_CONNECT = 2 - истина
  При этом не гарантировано, что сервер запущен
 */
uint32_t is_usb_connected(void) {
  
  return MDR_USB->SLS & 0x3;
}