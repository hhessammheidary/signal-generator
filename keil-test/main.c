#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "LCD16x2Lib/LCD.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void SysTick_Handler(void);
void display_error(void);
void display_options(void);
int read_keypad(void);
static void MX_ADC1_Init(void);
void SystemClock_Config(void);
static void MX_USART1_UART_Init(void);
void dec_to_bi(int dec, int bits[]);

ADC_HandleTypeDef hadc;
UART_HandleTypeDef huart1;

int main(void)
{
	SystemCoreClockUpdate();	
	SystemClock_Config();
	HAL_Init();
	MX_USART1_UART_Init();
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//PA1
	GPIO_InitTypeDef PinConfig;
	PinConfig.Pin = GPIO_PIN_1;
	PinConfig.Mode = GPIO_MODE_OUTPUT_PP;
	PinConfig.Pull = GPIO_NOPULL;
	PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &PinConfig);
	//PA2
	//GPIO_InitTypeDef PinConfig;
	PinConfig.Pin = GPIO_PIN_2;
	PinConfig.Mode = GPIO_MODE_OUTPUT_PP;
	PinConfig.Pull = GPIO_NOPULL;
	PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &PinConfig);
	//PB0
	//GPIO_InitTypeDef PinConfig;
	PinConfig.Pin = GPIO_PIN_0;
	PinConfig.Mode = GPIO_MODE_INPUT;
	PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &PinConfig);
	//PB1
	//GPIO_InitTypeDef PinConfig;
	PinConfig.Pin = GPIO_PIN_1;
	PinConfig.Mode = GPIO_MODE_INPUT;
	PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &PinConfig);
	//PB2
	//GPIO_InitTypeDef PinConfig;
	PinConfig.Pin = GPIO_PIN_2;
	PinConfig.Mode = GPIO_MODE_INPUT;
	PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &PinConfig);
	//PA0 pot
	PinConfig.Pin = GPIO_PIN_0;
  PinConfig.Mode = GPIO_MODE_ANALOG;
  PinConfig.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &PinConfig);
	
	LCD_Init();
  MX_ADC1_Init();
	
	
	
	LCD_Puts(0, 0, "99243033, 99243044");
	HAL_Delay(1000);
	display_options();
	
	int option = 0;
	int option_bit[8];
	int pot_value_time = 0;
	char pot_value_time_str[20];
	int pot_time_bit[12];
	int pot_value_frq = 0;
	char pot_value_frq_str[20];
	int pot_frq_bit[12];
	//read keypad for get input from keypad
	while(1){
		option = read_keypad();
		if(option == 1 || option == 2 || option == 3 || option == 4
			|| option == 5 || option == 6){
				break;
			}
		else{
			display_error();
		}
	}
	dec_to_bi(option, option_bit);

	
	LCD_Clear();
	LCD_Puts(0, 0, "set pot for time and press # and for see time press 1");
	HAL_Delay(100);
	while(option != 12){
		option = read_keypad();
		pot_value_time = get_pot_value_time();
		sprintf(pot_value_time_str, "%d", pot_value_time);
		LCD_Clear();
		LCD_Puts(0, 0, "time:");
		LCD_Puts(8, 0, pot_value_time_str);
	}
	dec_to_bi(pot_value_time, pot_time_bit);
	option = -1;
	LCD_Clear();
	LCD_Puts(0, 0, "set pot for frequence and press # and for see frequence press 1");
	HAL_Delay(100);
	while(option != 12){
		option = read_keypad();
		pot_value_frq = get_pot_value_frq();
		sprintf(pot_value_frq_str, "%d", pot_value_frq);
		LCD_Clear();
		LCD_Puts(0, 0, "frequence:");
		LCD_Puts(13, 0, pot_value_frq_str);
	}
	dec_to_bi(pot_value_frq, pot_frq_bit); 
	
		/*HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(200);*/
	return 0;
}



void SysTick_Handler(void)
{
  HAL_IncTick();
}

void display_options(){
	LCD_Clear();
	LCD_Puts(0, 0, "1)Sin 2)Square 3)Triangular 4)Sin2 5)Step 6)Sawtooth");
}

void display_error(){
	LCD_Clear();
	LCD_Puts(0, 0, "ERROR");
	HAL_Delay(100);
	display_options();
}

int read_keypad(){
	int value = 0;
	while (1)
	{
		// PA0:0 PA1:0
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){//1
			value = 1;
			break;
		}
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){//2
			value = 2;
			break;
		}	
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)){//3
			value = 3;
			break;
		}
		
		// PA0:1 PA1:0
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){//4
			value = 4;
			break;
		}
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){//5
			value = 5;
			break;
		}	
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)){//6
			value = 6;
			break;
		}
		
		// PA0:0 PA1:1
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){//7
			value = 7;
			break;
		}
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){//8
			value = 8;
			break;
		}	
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)){//9
			value = 9;
			break;
		}
		
		// PA0:1 PA1:1
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)){//*
			value = 10;
			break;
		}
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)){//0
			value = 11;
			break;
		}	
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)){//#
			value = 12;
			break;
		}
	}
	return value;
}


int get_pot_value_time(){
	HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  uint32_t potentiometerValue = HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);
	potentiometerValue = (potentiometerValue * 9500 / 4095) + 500;
	return potentiometerValue;
}

int get_pot_value_frq(){
	HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  uint32_t potentiometerValue = HAL_ADC_GetValue(&hadc);
  HAL_ADC_Stop(&hadc);
	potentiometerValue = potentiometerValue * 1000 / 4095;
	return potentiometerValue;
}



void SystemClock_Config()
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig;

  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.ScanConvMode = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc);

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset = 0;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);
}



void dec_to_bi(int dec, int bits[]){
	int num = dec;
	int i;
	for(i = 0; num > 0; i++){    
		bits[i] = num % 2;    
		num = num / 2;    
	}    
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif


