	#include "AS5147_U.h"


	static as5147_u_status_t SPI_SEND_RET(as5147_u_t *device, uint16_t sender, uint16_t *receiver);

	as5147_u_status_t AS5147_U_READ(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata){

	uint16_t header = 0;
	uint16_t empty_nop = 0;
	uint16_t reader = 0;


	header = regaddy | AS5147_U_READ_WRITE;
	empty_nop = AS5147_U_NOP | AS5147_U_READ_WRITE;

	as5147_u_status_t status_1 = SPI_SEND_RET(device, header, &reader);

	if(status_1 != AS5147_U_OK){
		return status_1;
	}

	as5147_u_status_t status_2 = SPI_SEND_RET(device, empty_nop, &reader);


	if(status_2 != AS5147_U_OK){
		return status_2;
		}

		if ((reader & AS5147_U_RESP_EF) != 0) {
		    return AS5147_U_ERR_EF;
		}

		*prxdata = AS5147_U_DATA_MASK & reader;

		return AS5147_U_OK;
	}


	as5147_u_status_t AS5147_U_READ_ANGLE(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata){

		uint16_t header = 0;
		uint16_t reader = 0;
		header = regaddy | AS5147_U_READ_WRITE;

		as5147_u_status_t status = SPI_SEND_RET(device, header, &reader);


		if(status != AS5147_U_OK){
				return status;
				}

		if ((reader & AS5147_U_RESP_EF) != 0) {
				    return AS5147_U_ERR_EF;
				}

		*prxdata = AS5147_U_DATA_MASK & reader;

		return AS5147_U_OK;
		}


	//as5147_u_status_t AS5147_U_WRITE(as5147_u_t *device, uint16_t regaddy, uint16_t txdata){

	//}


	static as5147_u_status_t SPI_SEND_RET(as5147_u_t *device, uint16_t sender, uint16_t *receiver){

		HAL_GPIO_WritePin(device->cs_port, device->cs_pin,GPIO_PIN_RESET);

		HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(device->hspi, (uint8_t *)&sender, (uint8_t *)receiver,1 , HAL_MAX_DELAY);
		device->last_rx = *receiver;
		HAL_GPIO_WritePin(device->cs_port, device->cs_pin,GPIO_PIN_SET);

		if(status != HAL_OK){
			return AS5147_U_ERR_SPI;
		}

		return AS5147_U_OK;
	}
