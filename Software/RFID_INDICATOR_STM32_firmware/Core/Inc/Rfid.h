/*
 * Rfid.h
 *
 *  Created on: Jul 25, 2025
 *      Author: Akash sujanapal
 */

#ifndef INC_RFID_H_
#define INC_RFID_H_


#define BLT_LED_ON()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define BLT_LED_OFF()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define BLT_LED_TOGGLE()  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)

#define CPU_LED_ON()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
#define CPU_LED_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)

#define NTW_LED_ON()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define NTW_LED_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)

#define RF_LED_ON()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define RF_LED_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)

#define ERROR_LED_ON()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)
#define ERROR_LED_OFF()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)

void parse_structured_command(char *data);
void Rfid_Begin(void);
void Rfid_Run(void);


#endif /* INC_RFID_H_ */
