/*!
\file
  \bref Сеть окружения контроллера CAN
  \author Дубровин Егор гр. ИУ6-72
  \date Ноябрь 2018 года

  Файл содержит реализацию методов поддержки CAN и организации сети датчиков вокруг МК
 */
#include "can.h"


/* Внутренняя функция инициализации интерфейса */
static uint32_t can_initialize_if(uint32_t can_init_type);


/*!
  \brief      Инициализация CAN

  \return     Результат выполнения

  \attention  Предполагается, что HCLK = 72 МГц
 */
uint32_t can_init(void) {
  /* Разрешение тактирования CAN1 и опорного порта A */
  RST_CLK_PCLKcmd((RST_CLK_PCLK_CAN1 | RST_CLK_PCLK_PORTA), ENABLE);

  /*! Установ частоты синхронизации CAN1 равным HCLK */
  CAN_BRGInit(MDR_CAN1, CAN_HCLKdiv1);

  /* ****************
   * Самотестирование
   * ****************
  */
  CAN_TxMsgTypeDef TxMsg;
  CAN_RxMsgTypeDef RxMsg;
  uint32_t i = 0;

  /* Инициализация интерфейса в режие самотестирования */
  if (can_initialize_if(CAN_INIT_SELFTEST) != END_OK)
    return END_NOT_OK;

  /* Установ данных для передачи */
  TxMsg.IDE     = CAN_ID_EXT;
  TxMsg.DLC     = 0x08;
  TxMsg.PRIOR_0 = DISABLE;
  TxMsg.ID      = MC_CAN_ADDR;
  TxMsg.Data[1] = current_time;
  TxMsg.Data[0] = 0x89ABCDEF;

  /* Передача */
  CAN_Transmit(MDR_CAN1, CAN_TX_BUF, &TxMsg);

  /* Ожидание окончания передачи */
  i = 0;
  while(((CAN_GetStatus(MDR_CAN1) & CAN_STATUS_TX_READY) != RESET) && (i != 0xFFF))
    i++;
  CAN_ITClearRxTxPendingBit(MDR_CAN1, CAN_TX_BUF, CAN_STATUS_TX_READY);

  /* Ожидание приёма */
  i = 0;
  while(((CAN_GetStatus(MDR_CAN1) & CAN_STATUS_RX_READY) == RESET) && (i != 0xFFF))
    i++;

  /* Чтение полченных данных из буфера */
  CAN_GetRawReceivedData(MDR_CAN1, CAN_ST_RX_BUF, &RxMsg);

  /* Освобождение буфера приёма и отключение CAN1 */
  CAN_ITClearRxTxPendingBit(MDR_CAN1, CAN_ST_RX_BUF, CAN_STATUS_RX_READY);
  CAN_Cmd(MDR_CAN1, DISABLE);

  /* Проверка заголовков принятого пакета */
  if(RxMsg.Rx_Header.IDE != TxMsg.IDE)
    return END_NOT_OK;
  if(RxMsg.Rx_Header.DLC != TxMsg.DLC)
    return END_NOT_OK;
  if(RxMsg.Rx_Header.ID != MC_CAN_ADDR)
    return END_NOT_OK;
  if(RxMsg.Data[1] != TxMsg.Data[1])
    return END_NOT_OK;
  if(RxMsg.Data[0] != TxMsg.Data[0])
    return END_NOT_OK;

  /* Самотестирование успешно пройдено! */

  /* ******************************
   * Нормальная инициализация CAN1
   * ******************************
  */
  PORT_InitTypeDef PORTA_InitStructure;

  /********************************
   * Инициализация настроек PORTA 
   ********************************
   */

  /* Конфигурация контакта 6 PORTA для приёма от CAN1 */
  PORTA_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PORTA_InitStructure.PORT_Pin   = PORT_Pin_6;
  PORTA_InitStructure.PORT_OE    = PORT_OE_OUT;
  PORTA_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORTA_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(MDR_PORTA, &PORTA_InitStructure);

  /* Конфигурация контакта 7 PORTA для передачи к CAN1 */
  PORTA_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
  PORTA_InitStructure.PORT_Pin   = PORT_Pin_7;
  PORTA_InitStructure.PORT_OE    = PORT_OE_IN;
  PORTA_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
  PORTA_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(MDR_PORTA, &PORTA_InitStructure);

  /* Инициализация интерфейса в нормальном режиме */
  if (can_initialize_if(CAN_INIT_SELFTEST) != END_OK)
    return END_NOT_OK;

  return END_OK;
}


/*!
  \brief      Получает данные от сенсора по CAN

  \param[in]  addr  Адрес датчика
  \param[out] data  Указатель на возвращаемые данные

  \return     Результат выполнения
 */
uint32_t get_sensor_data(uint32_t addr, uint32_t* data) {
  uint32_t i = 0;

  /* Проверка существования адреса датчика */
  if (! IS_CAN_SENSOR_ADDR (addr))
    return END_ERROR;

  /* Переменные сообщений CAN */
  CAN_TxMsgTypeDef TxMsg;
  CAN_RxMsgTypeDef RxMsg;

  /* Установ данных для передачи */
  TxMsg.IDE     = CAN_ID_EXT;
  TxMsg.DLC     = 0x08;
  TxMsg.PRIOR_0 = DISABLE;
  TxMsg.ID      = addr;
  TxMsg.Data[1] = 0;
  TxMsg.Data[0] = 0;//current_time; // Текущее время передаётся согласно протоколу

  /* Передача */
  CAN_Transmit(MDR_CAN1, CAN_TX_BUF, &TxMsg);

  // /* Ожидание окончания передачи */
  // i = 0;
  // while(((CAN_GetStatus(MDR_CAN1) & CAN_STATUS_TX_READY) != RESET) && (i != 0xFFF))
  //   i++;
  // CAN_ITClearRxTxPendingBit(MDR_CAN1, CAN_TX_BUF, CAN_STATUS_TX_READY);

  // /* Ожидание приёма */
  // i = 0;
  // while(((CAN_GetStatus(MDR_CAN1) & CAN_STATUS_RX_READY) == RESET) && (i != 0xFFF))
  //   i++;

  /* Чтение полченных данных из буфера */
  CAN_GetRawReceivedData(MDR_CAN1, addr, &RxMsg);

  /* Освобождение буфера приёма и отключение CAN1 */
  CAN_ITClearRxTxPendingBit(MDR_CAN1, addr, CAN_STATUS_RX_READY);

  /* Возврат данных */
  *data = RxMsg.Rx_Header.ID;

  return END_OK;
}


/*!
  \brief      Инициализация интерфейса CAN1

  \param[in]  can_init_type  Тип инициализации CAN (см. defs.h)

  \return     Результат выполнения

  \attention  Настройка датчиков должна совпадать с этими настройками

  Скорость = 72 МГц / ((BRP+1) * (PSEG + CAN_SEG1 + CAN_SEG2 + 1))
 */
uint32_t can_initialize_if(uint32_t can_init_type) {
  CAN_InitTypeDef  sCAN;
  CAN_FilterInitTypeDef sFilter;

  /* Освобождение регистров CAN1 */
  CAN_DeInit(MDR_CAN1);

  /* Инициализация стурктуры параметров CAN */
  CAN_StructInit (&sCAN);

  /* Настройка на необходимый режим */
  sCAN.CAN_ROP  = 
  sCAN.CAN_SAP  = 
  sCAN.CAN_STM  = can_init_type == CAN_INIT_SELFTEST ? ENABLE : DISABLE;

  /* Настройка скорости рабты передатчика */
  sCAN.CAN_ROM  = DISABLE;
  sCAN.CAN_PSEG = CAN_PSEG_Mul_2TQ;
  sCAN.CAN_SEG1 = CAN_SEG1_Mul_3TQ;
  sCAN.CAN_SEG2 = CAN_SEG2_Mul_4TQ;
  sCAN.CAN_SJW  = CAN_SJW_Mul_1TQ;
  sCAN.CAN_SB   = CAN_SB_1_SAMPLE;
  sCAN.CAN_BRP  = 71;
  
  /* Инициализация CAN1 и включение его работы */
  CAN_Init (MDR_CAN1, &sCAN);
  CAN_Cmd(MDR_CAN1, ENABLE);

  /* Выключение всех прерываний CAN1 */
  CAN_ITConfig( MDR_CAN1, CAN_IT_GLBINTEN | CAN_IT_RXINTEN | CAN_IT_TXINTEN |
                CAN_IT_ERRINTEN | CAN_IT_ERROVERINTEN, DISABLE);

  /* Включение передающего буфера CAN1 */
  CAN_TxITConfig( MDR_CAN1, (1<<CAN_TX_BUF), ENABLE);

  // if (can_init_type == CAN_INIT_SELFTEST) {
  //   /* Включение принимающего буфера CAN1 */
    CAN_RxITConfig( MDR_CAN1 ,(1<<CAN_ST_RX_BUF), ENABLE);
    CAN_Receive(MDR_CAN1, CAN_ST_RX_BUF, DISABLE);
  //   /* Включение принимающего буфера */
    
  //   /* Инициализация  фильтра */
  //   sFilter.Mask_ID = CAN_STDID_TO_EXTID(CAN_ID_STD);
  //   sFilter.Filter_ID = CAN_STDID_TO_EXTID(CAN_ID_STD);
  //   CAN_FilterInit(MDR_CAN1, CAN_ST_RX_BUF, &sFilter);
  // }
  // // else {
    uint32_t i;
    /* Проход по всем датчикам */
    for (i = 1; i <= SENSORS_COUNT; i++) {
      /* Включение прерывания CAN1 от принимающего буфера */
      CAN_RxITConfig( MDR_CAN1 ,(1<<i), ENABLE);
      
      /* Включение принимающего буфера */
      CAN_Receive(MDR_CAN1, i, DISABLE);
      
      /* Инициализация  фильтра */
      sFilter.Mask_ID = CAN_SENSOR_ADDR(i);
      sFilter.Filter_ID = CAN_SENSOR_ADDR(i);
      CAN_FilterInit(MDR_CAN1, i, &sFilter);
    }
  // }

  return END_OK;
}