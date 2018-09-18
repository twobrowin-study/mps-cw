//Содержимое файла main.c

#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>

//  Прототип функции задержки, реализованной ниже
void Delay(int waitTicks);


//  Точка входа, отсюда начинается исполнение программы
int main()
{
  // Заводим структуру конфигурации вывода(-ов) порта GPIO
  PORT_InitTypeDef GPIOInitStruct;
	
  //  Включаем тактирование порта C
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC, ENABLE);
	
  //  Инициализируем структуру конфигурации вывода(-ов) порта значениями по умолчанию
  PORT_StructInit(&GPIOInitStruct);
  
  //  Изменяем значения по умолчанию на необходимые нам настройки
  GPIOInitStruct.PORT_Pin        = PORT_Pin_0;
  GPIOInitStruct.PORT_OE         = PORT_OE_OUT;
  GPIOInitStruct.PORT_SPEED      = PORT_SPEED_SLOW;
  GPIOInitStruct.PORT_MODE       = PORT_MODE_DIGITAL;
  
  //  Применяем заполненную нами структуру для PORTC.
  PORT_Init(MDR_PORTC, &GPIOInitStruct);

  //  Запускаем бесконечный цикл обработки - Основной цикл	
  while (1)
  {
    // Считываем состояние вода PC0
    // Если на выводе логический "0", то выставляем вывод в логическую "1"
    if (PORT_ReadInputDataBit (MDR_PORTC, PORT_Pin_0) == 0)
    {	
	PORT_SetBits(MDR_PORTC, PORT_Pin_0);	// LED
    }	
    
    //  Задержка
    Delay(1000000);

    // Считываем состояние вода PC0
    // Если на выводе = "1", то выставляем "0"
    if (PORT_ReadInputDataBit (MDR_PORTC, PORT_Pin_0) == 1)
    {
	PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
    };
    
    //  Задержка    
    Delay(1000000);
  }      
}

//  Простейшая функция задержки, позднее мы заменим ее на реализацию через таймер
void Delay(int waitTicks)
{
  int i;
  for (i = 0; i < waitTicks; i++)
  {
    __NOP();
  }	
}
