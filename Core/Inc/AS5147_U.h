#ifndef AS5147_U_SPI_H
#define AS5147_U_SPI_H


#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Register addresses (verify against the AS5147U datasheet register table) */
#define AS5147_U_NOP 0x0000
#define AS5147_U_ERRFL 0x0001
#define AS5147_U_PROG 0x0003
#define AS5147_U_DIA 0x3FF5
#define AS5147_U_AGC 0x3FF9
#define AS5147_U_VEL 0x3FFC
#define AS5147_U_MAG 0x3FFD
#define AS5147_U_ANGLEUNC 0x3FFE
#define AS5147_U_ANGLECOM 0x3FFF
#define AS5147_U_ZPOSM 0x0016
#define AS5147_U_ZPOSL 0x0017
#define AS5147_U_DATA_MASK 0x3FFF

/* Command frame bits (16-bit mode, no CRC) */
#define AS5147_U_READ_WRITE (1U<<14)

/* Read data frame bits (16-bit mode, no CRC) */
#define AS5147_U_RESP_EF (1U<<14)
#define AS5147_U_RESP_WARN (1U<<15)

typedef struct {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef	*cs_port;
	uint16_t 		cs_pin;
	uint16_t		last_rx;
} as5147_u_t;

typedef enum {
	AS5147_U_OK = 0,
	AS5147_U_ERR_SPI,
	AS5147_U_ERR_CRC,	/* reserved for 24-bit CRC mode, unused in 16-bit mode */
	AS5147_U_ERR_EF
} as5147_u_status_t;

as5147_u_status_t  AS5147_U_READ(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata);
as5147_u_status_t  AS5147_U_READ_ANGLE(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata);
//as5147_u_status_t  AS5147_U_WRITE(as5147_u_t *device, uint16_t regaddy, uint16_t txdata);


#endif
