/*
 * Rfid.c
 *
 *  Created on: Jul 25, 2025
 *      Author: Akash sujanapal
 */

#include <string.h>
#include <stdio.h>
#include "main.h"
#include "Rfid.h"
#include "usart.h"


char rx_buffer[256];
char command[256];
volatile uint8_t rx_index = 0;

uint8_t boot = 0;
uint8_t offline = 1;
uint8_t rf_reader = 0;
uint8_t state = 0;
uint8_t error_flag = 0;



uint32_t ntw_lastTick = 0;
uint32_t rf_lastTick = 0;
uint32_t cpu_lastTick = 0;
uint32_t error_lastTick = 0;

uint32_t ntw_blinkDelay;
uint32_t rf_blinkDelay;

uint8_t ntw_ledState = 0;
uint8_t rf_ledState = 0;
uint8_t cpu_ledState = 0;
uint8_t error_ledstate = 0;

// ------------------- UART READING INTERRUPT FUNCTION --------------------------- //

void parse_structured_command(char *data)
{
    if (data[0] != '{') return;

    char *payload = data + 1;  // skip '{'
    char *end = strchr(payload, '}');
    if (end) *end = '\0';

    char *token = strtok(payload, ",");

    while (token != NULL)
    {
        char key[20], value[20];

        	if (sscanf(token, "\"%[^\"]\":\"%[^\"]\"", key, value) == 2)
        {
            // STATUS
            if (strcmp(key, "cpu_status") == 0)
            {
                if (strcmp(value, "BOOT") == 0)
                {
                	 boot = 0;

                }

                else if (strcmp(value, "BOOTED") == 0)
                {
                	boot = 1;

                }
            }

            // READING
            else if (strcmp(key, "reader_status") == 0)
            {
                if (strcmp(value, "INACTIVE") == 0)
                    rf_reader = 0;
                else if (strcmp(value, "ACTIVE") == 0)
                    rf_reader = 1;
            }
            // DATA READING
            else if (strcmp(key, "data_reading") == 0)
            {
                if (strcmp(value, "INACTIVE") == 0)
                	state = 0;
                else if (strcmp(value, "ACTIVE") == 0)
                	state = 1;
            }


            // NETWORK
            else if (strcmp(key, "network") == 0)
            {
                if (strcmp(value, "OFFLINE") == 0)
                {
                	offline = 1;
                }

                else if (strcmp(value, "LAN") == 0)
                {
                	offline = 0;
                	ntw_blinkDelay = 500;
                }
                else if (strcmp(value, "WIFI") == 0)
                {
                    offline = 0;
                    ntw_blinkDelay = 75;
                }

            }

            // FREQ
            else if (strcmp(key, "frequency") == 0)
            {
                if (strcmp(value, "1000") == 0)
                {
                	rf_blinkDelay = 500;
                }

                else if (strcmp(value, "500") == 0)
                {
                	rf_blinkDelay = 200;
                }
            }

            // ERROR
            else if (strcmp(key, "error") == 0)
            {
                if (strcmp(value, "INACTIVE") == 0)
                {
                  error_flag = 0;
                }

                else if (strcmp(value, "ACTIVE") == 0)
                {
                  error_flag = 1;
                }
             }
        }

        token = strtok(NULL, ",");
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (rx_buffer[rx_index] == '\n')
        {
            rx_buffer[rx_index] = '\0';
            strcpy(command, rx_buffer);


            parse_structured_command(command);
            HAL_UART_Transmit(&huart2, (uint8_t *)"OK\n", 3, HAL_MAX_DELAY);

            rx_index = 0;
            memset(rx_buffer, 0, sizeof(rx_buffer));
        }
        else
        {
            rx_index++;
            if (rx_index >= sizeof(rx_buffer)) rx_index = 0;
        }

        HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_buffer[rx_index], 1); // Re-enable UART interrupt
    }
}

// ------------------------ USER FUNCTIONS -------------------------------//

void Rfid_Begin(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_buffer[rx_index], 1); // Start UART receive interrupt


	for(uint8_t i = 0; i < 8; i ++)
	 {
		CPU_LED_ON();
		NTW_LED_ON();
		RF_LED_ON();
		BLT_LED_ON();
		HAL_Delay(200);

		CPU_LED_OFF();
		NTW_LED_OFF();
		RF_LED_OFF();
		BLT_LED_OFF();
		HAL_Delay(200);
	 }


}
void Rfid_Run(void)
{
	uint32_t currentTick = HAL_GetTick();

// --------------------- CPU INDICATOR ---------------------------------//

	if(!boot)
	{
		if (currentTick - cpu_lastTick >= 200)
		{
		    cpu_lastTick = currentTick;
			cpu_ledState = !cpu_ledState;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, cpu_ledState);
	    }
	}
	if(boot)
	{
		CPU_LED_ON();
	}

// ------------------ NETWORK INDICATOR ------------------------------ //

	if(offline)
	{
		NTW_LED_OFF();
	}

	if(!offline)
	{
		if (currentTick - ntw_lastTick >= ntw_blinkDelay)
		      {
			    ntw_lastTick = currentTick;
			    ntw_ledState = !ntw_ledState;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, ntw_ledState);
			  }
	}


// ------------------------ RF INDICATOR ------------------------------- //

    if(!rf_reader)
    {
    	RF_LED_OFF();

    }
    if(rf_reader == 1 && state == 0)
    {
    	RF_LED_ON();

    }
    if(rf_reader == 1 && state == 1)
    {
    	if (currentTick - rf_lastTick >= rf_blinkDelay)
    	{
    	    rf_lastTick = currentTick;
    		rf_ledState = !rf_ledState;
    		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, rf_ledState);

    	}

    }
 // ------------------------ ERROR DETECTOR ------------------------------- //

   if(!error_flag)
   {
	   ERROR_LED_OFF();
	   BLT_LED_OFF();
   }
   if(error_flag)
   {
	   if (currentTick - error_lastTick >= 500)
	     {
	       error_lastTick = currentTick;
           error_ledstate =!error_ledstate;

           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, error_ledstate);

	     }

   }

}

