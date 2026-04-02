/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include<string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define EMERG GPIO_PIN_14
#define TASK1 GPIO_PIN_10
#define TASK2 GPIO_PIN_11
#define TASK3 GPIO_PIN_12

//#define	LED_PORT GPIOG

//interrupt
#define EMERG_BIT 0x01
#define TASK1_BIT 0x02
#define TASK2_BIT 0x04
#define TASK3_BIT 0x08

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

//osThreadId defaultTaskHandle;
//osTimerId myTimer01Handle;
//osMutexId myMutex01Handle;
/* USER CODE BEGIN PV */
TaskHandle_t MyTask1Handle;
TaskHandle_t MyTask2Handle;
TaskHandle_t MyTask3Handle;
TaskHandle_t Emerg_TaskHandle,ControlTaskHandle,UARTHandle;
//TaskHandle_t Timer_BouncingTask_Handle;
//TimerHandle_t TimerHandle;


EventGroupHandle_t EventGroupHandle;


//for mutex
SemaphoreHandle_t MutexHandle;
//SemaphoreHandle_t UART_Mutex;

//for semaphore
//SemaphoreHandle_t Timer_sem;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
//void StartDefaultTask(void const * argument);
//void Callback01(void const * argument);

/* USER CODE BEGIN PFP */
void Emerg_Task(void *arg);
void Control_Task(void * arg);
void MyTask1(void *arg);
void MyTask2(void *arg);
void MyTask3(void *arg);
void UART_Task(void * arg);
//void Timer_BouncingTask(void *args);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)ptr,(uint16_t)len,HAL_MAX_DELAY);
	return len;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(GPIO_Pin == GPIO_PIN_0)
		xEventGroupSetBitsFromISR(EventGroupHandle,EMERG_BIT,&xHigherPriorityTaskWoken);
	else if(GPIO_Pin == GPIO_PIN_1)
		xEventGroupSetBitsFromISR(EventGroupHandle,TASK1_BIT,&xHigherPriorityTaskWoken);
	else if(GPIO_Pin == GPIO_PIN_2)
			xEventGroupSetBitsFromISR(EventGroupHandle,TASK2_BIT,&xHigherPriorityTaskWoken);
	else if(GPIO_Pin == GPIO_PIN_3)
			xEventGroupSetBitsFromISR(EventGroupHandle,TASK3_BIT,&xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void Toggle_LED(uint16_t pin){
	//mutex lock
	xSemaphoreTake(MutexHandle, portMAX_DELAY);
	//LED Toggle
	HAL_GPIO_TogglePin(GPIOG, pin);


	//mutex unlock
	xSemaphoreGive(MutexHandle);
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
  /* USER CODE BEGIN 2 */

  //to create Even Group
  EventGroupHandle = xEventGroupCreate();
  MutexHandle = xSemaphoreCreateMutex();

  // TimerHandle =  xTimerCreate("Timer_task", pdMS_TO_TICKS(50),pdFALSE, 0, Timer_BouncingTask);



 // xTimerStart(TimerHandle, 0);

  xTaskCreate(Control_Task, "Control Task", 128, NULL, 2, &ControlTaskHandle);
  xTaskCreate(Emerg_Task, "Emergency Task", 128, NULL, 2, &Emerg_TaskHandle);
  xTaskCreate(MyTask1, "Task1", 128, NULL, 1, &MyTask1Handle);
  xTaskCreate(MyTask2, "Task2", 128, NULL, 1, &MyTask2Handle);
  xTaskCreate(MyTask3, "Task3", 128, NULL, 1, &MyTask3Handle);
  xTaskCreate(UART_Task, "UART Task", 256, NULL, 1, &UARTHandle);

  vTaskSuspend(MyTask1Handle);
  vTaskSuspend(MyTask2Handle);
  vTaskSuspend(MyTask3Handle);
  vTaskSuspend(Emerg_TaskHandle);

  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
//  osMutexDef(myMutex01);
//  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
//  osTimerDef(myTimer01, Callback01);
//  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  //osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PG10 PG11 PG12 PG13
                           PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Control_Task(void * arg)
{
	for(;;){
		uint16_t bit = xEventGroupWaitBits(EventGroupHandle,  EMERG_BIT | TASK1_BIT | TASK2_BIT | TASK3_BIT, pdTRUE, pdFALSE,portMAX_DELAY);
		switch(bit)
		{
			case 0x01:
				vTaskResume(Emerg_TaskHandle);
				vTaskSuspend(MyTask1Handle);
				vTaskSuspend(MyTask2Handle);
				vTaskSuspend(MyTask3Handle);
				break;

			case 0x02:
				vTaskResume(MyTask1Handle);
				vTaskSuspend(MyTask2Handle);
				vTaskSuspend(MyTask3Handle);
				vTaskSuspend(Emerg_TaskHandle);
				break;

			case 0x04:
				vTaskResume(MyTask2Handle);
				vTaskSuspend(MyTask1Handle);
				vTaskSuspend(MyTask3Handle);
				vTaskSuspend(Emerg_TaskHandle);
				break;

			case 0x08:
				vTaskResume(MyTask3Handle);
				vTaskSuspend(MyTask1Handle);
				vTaskSuspend(MyTask2Handle);
				vTaskSuspend(Emerg_TaskHandle);
				break;
		}
	}
}


void Emerg_Task(void *agr){
	for(;;){
		//function call
		Toggle_LED(EMERG);
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}
void MyTask1(void *arg)
{
	for(;;)
	{
		//function call
		Toggle_LED(TASK1);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void MyTask2(void *arg){
	for(;;){
		//function call
		Toggle_LED(TASK2);
		vTaskDelay(pdMS_TO_TICKS(1000));

	}
}
void MyTask3(void * arg){
	for(;;){
		//function call
		Toggle_LED(TASK3);
		vTaskDelay(pdMS_TO_TICKS(1500));

	}
}
void UART_Task(void *args)
{
	uint8_t STATUS[500];

		for(;;){
			//STATUS= xEventGroupWaitBits(EventGroupHandle, EMERGENCY_TASK_BIT | TASK1_BIT | TASK2_BIT | TASK3_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
			vTaskList(STATUS);
			HAL_UART_Transmit(&huart1,(uint8_t *)STATUS,strlen(STATUS), portMAX_DELAY );
			vTaskDelay(pdMS_TO_TICKS(500));

		   /* if(STATUS == EMERG_BIT)
		    	HAL_UART_Transmit(&huart1, "Emergency_task\r\n",17, portMAX_DELAY);
		    else if(STATUS == TASK1_BIT)
		   		HAL_UART_Transmit(&huart1, "task1\r\n",8, portMAX_DELAY);
		    else if(STATUS == TASK2_BIT)
		   		HAL_UART_Transmit(&huart1, "task2\r\n",8, portMAX_DELAY);
		    else if(STATUS == TASK3_BIT)
		   	    HAL_UART_Transmit(&huart1, "task3\r\n",8, portMAX_DELAY);
		    vTaskList(buf);*/
		}
}


/*void Timer_BouncingTask(void *args)
{
	BaseType_t xHigherPriorityTaskWaken = pdFALSE;

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)==1)
		xEventGroupSetBits(EventGroupHandle,EMERGENCY_TASK_BIT,&xHigherPriorityTaskWaken);
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==1)
		xEventGroupSetBits(EventGroupHandle,EMERGENCY_TASK_BIT,&xHigherPriorityTaskWaken);
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1)
		xEventGroupSetBits(EventGroupHandle,EMERGENCY_TASK_BIT,&xHigherPriorityTaskWaken);
	else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)==1)
		xEventGroupSetBits(EventGroupHandle,EMERGENCY_TASK_BIT,&xHigherPriorityTaskWaken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWaken);
}*/

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
//////  /* Infinite loop */
//////  for(;;)
//////  {
//////    osDelay(1);
//////  }
//  /* USER CODE END 5 */
//}

/* Callback01 function */
//void Callback01(void const * argument)
//{
//  /* USER CODE BEGIN Callback01 */
//////
//  /* USER CODE END Callback01 */
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
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
#ifdef USE_FULL_ASSERT
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
