#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>              
#include <stdbool.h>            

/* HAL_SPI_Enable() is to drive the CS Pin to the eve HIGH */
void HAL_SPI_Enable(void);

/* HAL_SPI_Disable() is to drive the CS Pin to the eve LOW */
void HAL_SPI_Disable(void);

/* HAL_SPI_Write(uint8_t data) does a single byte SPI write transfer */
uint8_t HAL_SPI_Write(uint8_t data);

/* HAL_SPI_WriteBuffer does a buffer based SPI Write transfer */
void HAL_SPI_WriteBuffer(uint8_t *Buffer, uint32_t Length);

/* HAL_SPI_WriteBuffer does a buffer based SPI Read transfer */
void HAL_SPI_ReadBuffer(uint8_t *Buffer, uint32_t Length);

/* Stall the cpu for X milliseconds */
void HAL_Delay(uint32_t milliSeconds);

/* Gives an opertunity to reset the EVE hardware */
void HAL_Eve_Reset_HW(void);

/* Cleans up and resources allocated */
void HAL_Close(void);


#ifdef __cplusplus
}
#endif

