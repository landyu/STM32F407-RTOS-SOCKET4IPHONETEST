/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tcpip.h"
//#include "httpserver-socket.h"
#include "Myserver.h"
#include "serial_debug.h"
#include "Printf.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*--------------- LCD Messages ---------------*/
#define MESSAGE1   "  SZW-STM32F407ZG   "
#define MESSAGE2   "  STM32F-4 Series   "
#define MESSAGE3   "Basic WebServer Demo"
#define MESSAGE4   "  www.armjishu.com  "

/*--------------- Tasks Priority -------------*/
#define DHCP_TASK_PRIO   ( tskIDLE_PRIORITY + 2 )      
#define LED_TASK_PRIO    ( tskIDLE_PRIORITY + 1 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void LCD_LED_Init(void);
void ToggleLed1(void * pvParameters);
void ToggleLed2(void * pvParameters);
void ToggleLed3(void * pvParameters);
void ToggleLed4(void * pvParameters);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured to 
       168 MHz, this is done through SystemInit() function which is called from
       startup file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  

#ifdef SERIAL_DEBUG
  //DebugComPort_Init();
#endif
  
  Printf_Init();

  /*Initialize LCD and Leds */ 
  LCD_LED_Init();
  
  /* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
  ETH_BSP_Config();

  printf("\n\r\r\n ARMJISHU.COM神舟王STM32F407ZGT开发板 FreeRTOS V6.1.0 httpserver_netconn 例程\r\n"); 
    
  /* Initilaize the LwIP stack */
  printf("\n\r LwIP_Init\r\n");
  LwIP_Init();
  
  /* Initialize webserver demo */
  my_server_socket_init();
  printf("\n\r FreeRTOS V6.1.0 my_server_socket_init: sys_thread_new HTTP\r\n");

#ifdef USE_DHCP
  /* Start DHCPClient */
  printf("\n\r FreeRTOS V6.1.0 xTaskCreate LwIP_DHCP_task\r\n");
  xTaskCreate(LwIP_DHCP_task, "DHCPClient", configMINIMAL_STACK_SIZE * 2, NULL,DHCP_TASK_PRIO, NULL);
#endif
    
  /* Start toogleLed4 task : Toggle LED4  every 250ms */
 // printf("\n\r FreeRTOS V6.1.0 xTaskCreate ToggleLed4\r\n");
  //xTaskCreate(ToggleLed4, "LED4", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);

  /* Start toogleLed1 task : Toggle LED1  every 250ms */
 // printf("\n\r FreeRTOS V6.1.0 xTaskCreate ToggleLed1\r\n");
 // xTaskCreate(ToggleLed1, "SZWLED1", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);
    
  /* Start toogleLed2 task : Toggle LED2  every 250ms */
  printf("\n\r FreeRTOS V6.1.0 xTaskCreate ToggleLed2\r\n");
  xTaskCreate(ToggleLed2, "SZWLED2", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);

  /* Start toogleLed3 task : Toggle LED3  every 250ms */
 // printf("\n\r FreeRTOS V6.1.0 xTaskCreate ToggleLed3\r\n");
  //xTaskCreate(ToggleLed3, "SZWLED3", configMINIMAL_STACK_SIZE, NULL, LED_TASK_PRIO, NULL);


  /* Start scheduler */
  printf("\n\r FreeRTOS V6.1.0 Start scheduler\r\n");
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

/**
  * @brief  Toggle Led1 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed1(void * pvParameters)
{
  for( ;; )
  {
    /* Toggle LED4 each 100ms */
    STM_EVAL_LEDToggle(LED1);
    vTaskDelay(100);
  }
}

/**
  * @brief  Toggle Led2 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed2(void * pvParameters)
{
  for( ;; )
  {
    /* Toggle LED4 each 150ms */
    STM_EVAL_LEDToggle(LED2);
    vTaskDelay(150);
  }
}

/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed3(void * pvParameters)
{
  for( ;; )
  {
    /* Toggle LED4 each 200ms */
    STM_EVAL_LEDToggle(LED3);
    vTaskDelay(200);
  }
}


/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
void ToggleLed4(void * pvParameters)
{
  for( ;; )
  {
    /* toggle LED4 each 250ms */
    STM_EVAL_LEDToggle(LED4);
    vTaskDelay(250);
  }
}

/**
  * @brief  Initializes the LCD and LEDs resources.
  * @param  None
  * @retval None
  */
void LCD_LED_Init(void)
{
#ifdef USE_LCD
  /* Initialize the STM324xG-EVAL's LCD */
  STM324xG_LCD_Init();
#endif

  /* Initialize STM324xG-EVAL's LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
#ifdef USE_LCD
  /* Clear the LCD */
  LCD_Clear(Black);

  /* Set the LCD Back Color */
  LCD_SetBackColor(Black);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Display message on the LCD*/
  LCD_DisplayStringLine(Line0, (uint8_t*)MESSAGE1);
  LCD_DisplayStringLine(Line1, (uint8_t*)MESSAGE2);
  LCD_DisplayStringLine(Line2, (uint8_t*)MESSAGE3);
  LCD_DisplayStringLine(Line3, (uint8_t*)MESSAGE4);  
  LCD_DisplayWelcomeStr(Line4); 
#endif
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
