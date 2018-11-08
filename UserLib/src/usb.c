/*!
\file
  \bref Подключение по USB
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит реализацию соединения МК с сервером по каналу USB в режиме Device
 */
#include "usb.h"


/* Описания для работы USB */
#define BUFFER_LEN 2048
static uint8_t usb_buffer[BUFFER_LEN];
static RING_BUFFER(usb_rb, BUFFER_LEN);
static uint8_t  writing_to_zero_index;
static uint8_t  reading_from_zero_index;
static uint32_t reading_length;


/* Статические функции этого файла */
void mdr_usbclock_configure();
uint8_t get_write_buf(uint8_t **buf);
void written_buf(uint32_t size);
uint8_t get_read_buf(uint8_t **buf, uint32_t *length);
void readed_buf(uint32_t size);
void mdr_usb_cdc_on_data_recv(uint8_t data_len);
void mdr_usb_cdc_on_data_sent();


/*!
  \bref Инициализация USB
  \return Статус завершения
  
  Чётотам происходит
  \todo Описать подпрограмму
 */
END_STATUS usb_init(void) {
  mdr_usbclock_configure();
  mdr_usb_cdc_init();

  return END_OK;
}


/*!
  \brief      Проверка USB

  \return     Результат завершения функции

  Проверка состояния текущих входных и выходных буферов USB на наличие данных и принятие
    решения по распределению в соответствие с алгоритмом программы
 */
END_STATUS check_usb(void) {
  // while(1) {
    if(mdr_usb_cdc_is_recv_buf_setted() == MDR_USB_CDC_Fail) {
      uint8_t *write_buf;
      if(get_write_buf(&write_buf)) {
        mdr_usb_cdc_set_recv_buf(write_buf);
      }
    }

    if(mdr_usb_cdc_is_sending() == MDR_USB_CDC_Fail) {
      uint8_t *read_buf;
      if(get_read_buf(&read_buf, &reading_length)) {
        mdr_usb_cdc_send(reading_length, read_buf);
      }
    }
  // }

  return END_OK;
}


/*!
  \brief    Конфигурация тактирования блока USB
 */
void mdr_usbclock_configure() {
  RST_CLK_PCLKcmd(
    RST_CLK_PCLK_RST_CLK |
    RST_CLK_PCLK_BKP |
    RST_CLK_PCLK_POWER,
    ENABLE
  );

  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
  EEPROM_SetLatency(EEPROM_Latency_2);
}


/*!
  \brief    Получить статус чтения буфера

  \param    buf   Буфер записи

  \return   Результат прочтения буфера
 */
uint8_t get_write_buf(uint8_t **buf) {
  uint8_t res = 0;
  rb_index write_size, zero_size;
  rb_get4write(&usb_rb, &write_size, &zero_size);

  if(write_size >= MDR_USB_CDC_MAX_PACKET_SIZE) {
    writing_to_zero_index = 0;
    *buf = &usb_buffer[usb_rb.write];
    res = 1;
  }
  else if(zero_size >= MDR_USB_CDC_MAX_PACKET_SIZE) {
    writing_to_zero_index = 1;
    *buf = &usb_buffer[0];
    res = 1;
  }

  return res;
}


/*!
  \brief    Прочтённый буфер

  \param[in]  size  Размер
 */
void written_buf(uint32_t size) {
  if(writing_to_zero_index)
    rb_written(&usb_rb, 0, size);
  else
    rb_written(&usb_rb, size, 0);
}


/*!
  \brief    Получает буфер чтения.

  \param    buf   Буфер
  \param    length  Длина

  \return   Статус чтения буфера
 */
uint8_t get_read_buf(uint8_t **buf, uint32_t *length) {
  uint8_t res = 0;
  rb_index read_size, zero_size;
  rb_get4read(&usb_rb, &read_size, &zero_size);

  if(read_size > 0) {
    reading_from_zero_index = 0;
    *buf = &usb_buffer[usb_rb.read];
    *length = read_size > MDR_USB_CDC_MAX_PACKET_SIZE ?
      MDR_USB_CDC_MAX_PACKET_SIZE :
      read_size;
    res = 1;
  }
  else if(zero_size > 0) {
    reading_from_zero_index = 1;
    *buf = &usb_buffer[0];
    *length = zero_size > MDR_USB_CDC_MAX_PACKET_SIZE ?
      MDR_USB_CDC_MAX_PACKET_SIZE :
      zero_size;
    res = 1;
  }

  return res;
}



/*!
  \brief    Размер прочтённого буфера

  \param[in]  size  Размер
 */
void readed_buf(uint32_t size) {
  if(size > 0) {
    if(reading_from_zero_index)
      rb_readed(&usb_rb, 0, size);
    else
      rb_readed(&usb_rb, size, 0);
  }
}


/*!
  \brief    Вызываемая функция по получению данных

  \param[in]  data_len  Длина данных

  При получении данных по USB CDC и заверешении всех прочих задач вызывается и отправляет данные
 */
void mdr_usb_cdc_on_data_recv(uint8_t data_len) {
  uint8_t *write_buf;
  written_buf(data_len);
  if(get_write_buf(&write_buf)) {
    mdr_usb_cdc_set_recv_buf_isr(write_buf);
  }
}


/*!
  \brief    Вызываемая функция по выдаче данных

  Если в буфере накопились записи для передачи, вызывается и передаёт их
 */
void mdr_usb_cdc_on_data_sent() {
  uint8_t *read_buf;
  readed_buf(reading_length);
  if(get_read_buf(&read_buf, &reading_length)) {
    mdr_usb_cdc_send_isr(reading_length, read_buf);
  }
  else {
    reading_length=0;
  }
}

