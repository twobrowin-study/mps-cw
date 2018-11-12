/*!
\file
  \bref Старт диагностического режима
  \author Дубровин Егор гр. ИУ6-72
  \date Октябрь 2018 года

  Файл содержит реализации подпрограмм для запуска диагностического режима по
    нажатию кнопки "Выбор".
    Здесь инициализируется таймер 2 и во время его прерывания по перполнению
      вызывается диагностическиий режим при нажатой кнопке "Выбор"
 */
#include "diagnostic_start.h"


/*!
  \bref Инициализация поддрежки запуска диагностического режима
  \return Статус завершения
  \attention Рассчитывается, что частота процессорного ядра составляет 72 МГц

  Содержит инизиализацию таймера 2 и разрешения прерывания для него, обработчик
    которого запускает диагностический режим.
    Настройка осуществляется для вызова обработчика прерывания 20 раз в секунду.
 */
END_STATUS diagnostic_start_init(void) {
  TIMER_CntInitTypeDef timer_struct;

  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);

  /// После деления на 8 частота на входе предделителя таймера - 9 МГц
  TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv8);

  timer_struct.TIMER_ARR_UpdateMode 	= TIMER_ARR_Update_Immediately;
  timer_struct.TIMER_BRK_Polarity 		= TIMER_BRKPolarity_NonInverted;
  timer_struct.TIMER_CounterDirection = TIMER_CntDir_Up;
  timer_struct.TIMER_CounterMode 			= TIMER_CntMode_ClkFixedDir;
  timer_struct.TIMER_ETR_FilterConf 	= TIMER_Filter_1FF_at_TIMER_CLK;
  timer_struct.TIMER_ETR_Polarity 		= TIMER_ETRPolarity_NonInverted;
  timer_struct.TIMER_ETR_Prescaler 		= TIMER_ETR_Prescaler_None;
  timer_struct.TIMER_EventSource 			= TIMER_EvSrc_None;
  timer_struct.TIMER_FilterSampling 	= TIMER_FDTS_TIMER_CLK_div_1;
  timer_struct.TIMER_IniCounter 			= 0;

  /// Получим 20 Гц при помощи предделителя на 32000 и подсчтётом до 14 перед прерыванием
  timer_struct.TIMER_Period 					= 32000-1;
  timer_struct.TIMER_Prescaler 				= 14;

  TIMER_CntInit(MDR_TIMER2, &timer_struct);

  /* Установка прерывания таймера 2 */
  NVIC_SetPriority (TIMER2_IRQn, 15); // Самый низкий приоритет прерывания
  NVIC_EnableIRQ(TIMER2_IRQn);
  TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE);

  TIMER_Cmd(MDR_TIMER2, ENABLE);

  return END_OK;
}


/*!
  \bref Функция инициализации кнопки "Выбор"
  \return Статус завершения

  Содержит функции инициализации PC2 для обеспечения прослушивания кнопки "Выбор"
 */
END_STATUS sel_butt_init(void) {
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC, ENABLE);
  PORT_InitTypeDef GPIOInitStruct;
  PORT_StructInit(&GPIOInitStruct);
  GPIOInitStruct.PORT_Pin        = PORT_Pin_2;
  GPIOInitStruct.PORT_OE         = PORT_OE_IN;
  GPIOInitStruct.PORT_SPEED      = PORT_SPEED_SLOW;
  GPIOInitStruct.PORT_MODE       = PORT_MODE_DIGITAL;
  PORT_Init(MDR_PORTC, &GPIOInitStruct);

  return END_OK;
}


/*!
  \bref Обработчик прерывания таймера 2

  Содержит проверку на нажатие кнопки "Выбор" и вызов диагностического
    режима при нажатии
  Для работы с USB перед входом в диагностический режим все прерывания разрешеются
 */
void Timer2_IRQHandler(void)
{
	if (TIMER_GetITStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) == SET)
  {
    TIMER_ClearITPendingBit(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
    MDR_PORTC->RXTX ^= 2;
    if( PORT_ReadInputDataBit (MDR_PORTC, PORT_Pin_2) == 0 ) {
      // Контрольная точка USB и времени до начала диагностики
      time_controll();

      diagnostic_release();
      
      // Реинициализация меню диагностического режима после завершения работы в нём
      diagnostic_init();

      // Контрольная точка USB и времени после окончания диагностики
      time_controll();
    }
  }
}
