/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "u8g2_porting.h"
// #include "mui.h"
// #include "mui_u8g2.h"
#include "stdio.h"
#include "encoder_simu.h"
#include "ulog.h"
#include "string.h"
#include "menu.h"
#include "menu_u8g2.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8g2_t u;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE BEGIN PFP */

void logger_fn1(ulog_level_t severity, char *msg)
{
  char endl = '\n';
  uint16_t len = strlen(msg);
  for (int i = 0; i < len; ++i)
  {
    while (!LL_USART_IsActiveFlag_TXE(USART1));
    LL_USART_TransmitData8(USART1, msg[i]);
  }
  while (!LL_USART_IsActiveFlag_TXE(USART1));
  LL_USART_TransmitData8(USART1, endl);
}

void btn_handle_press(button_t *btn)
{
    extern void up(void);
    extern void down(void);
    extern void enter(void);
    extern void home(void);

    if ('U' == btn->name[0])
    {
      up();
    }
    else if ('D' == btn->name[0])
    {
      down();
    }
    else if ('L' == btn->name[0])
    {
      home();
    }
    else if ('R' == btn->name[0])
    {
      enter();
    }
}

struct stm32_gpio_btn
{
  button_t btn;
  GPIO_TypeDef *port;
  uint32_t pin;
};

struct stm32_gpio_btn btn_map[] =
{
  {.port= GPIOB, .pin = GPIO_PIN_8},
  {.port= GPIOB, .pin = GPIO_PIN_3},
  {.port= GPIOB, .pin = GPIO_PIN_5},
  {.port= GPIOB, .pin = GPIO_PIN_4},
};

int pin_read(button_t *btn)
{
    struct stm32_gpio_btn *gpio_btn = NULL;
    if (NULL == btn)
    {
        return 0;
    }

    gpio_btn = container_of(btn, struct stm32_gpio_btn, btn);
    return HAL_GPIO_ReadPin(gpio_btn->port, gpio_btn->pin);
}

int regitster_button(void)
{
    button_cfg_t cfg = BTN_CFG_DEFAULE;

    cfg.pin_read = pin_read;
    cfg.debounce = 1;
    button_init(&(btn_map[0].btn), "U", cfg, BTN_FLAG_START);
    button_init(&(btn_map[1].btn), "D", cfg, BTN_FLAG_START);
    button_init(&(btn_map[2].btn), "L", cfg, BTN_FLAG_START);
    button_init(&(btn_map[3].btn), "R", cfg, BTN_FLAG_START);

    button_attach(&(btn_map[0].btn), BTN_PRESS_DOWN, btn_handle_press);
    button_attach(&(btn_map[1].btn), BTN_PRESS_DOWN, btn_handle_press);
    button_attach(&(btn_map[2].btn), BTN_PRESS_DOWN, btn_handle_press);
    button_attach(&(btn_map[3].btn), BTN_PRESS_DOWN, btn_handle_press);

    return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  u8g2_oled_init(&u);
  menu_api_register_handle((void *)(&u));

  ULOG_INIT();
  ULOG_SUBSCRIBE(logger_fn1, ULOG_DEBUG_LEVEL);

  ULOG_DEBUG("Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd");
  ULOG_DEBUG("Encoder Simulator v1.0");

  regitster_button();

  while (1)
  {

    // HAL_Delay(500);
    // HAL_GPIO_TogglePin(LED_SYS_GPIO_Port, LED_SYS_Pin);

    menu();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
