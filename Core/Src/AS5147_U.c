	#include "AS5147_U.h"


	static as5147_u_status_t SPI_SEND_RET(as5147_u_t *device, uint32_t sender, uint32_t *receiver);
	static uint8_t crc_generator(uint16_t sender);
	static as5147_u_status_t unpack_reply(uint32_t frame, uint16_t *data);

	as5147_u_status_t AS5147_U_READ(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata){

	uint16_t header = 0;
	uint16_t empty_nop = 0;
	uint32_t reader = 0;
	uint32_t message_1 = 0;
	uint8_t crc_1 = 0;
	uint32_t message_2 = 0;
	uint8_t crc_2=0;


	header = regaddy | AS5147_U_READ_WRITE;
	empty_nop = AS5147_U_NOP | AS5147_U_READ_WRITE;

	crc_1 = crc_generator(header);
	crc_2 = crc_generator(empty_nop);

	message_1 = message_1 | (header<<8);
	message_1 = message_1 | crc_1;

	message_2 = message_2 | (empty_nop<<8);
	message_2 = message_2 | crc_2;


	as5147_u_status_t status_1 = SPI_SEND_RET(device, message_1, &reader);

	if(status_1 != AS5147_U_OK){
		return status_1;
	}

	as5147_u_status_t status_2 = SPI_SEND_RET(device, message_2, &reader);

	if(status_2 != AS5147_U_OK){
		return status_2;
	}

	uint16_t reply = 0;
	as5147_u_status_t status_3 = unpack_reply(reader, &reply);
	if (status_3 != AS5147_U_OK){
		return status_3;
	}

	if ((reply & AS5147_U_RESP_EF) != 0) {
	    return AS5147_U_ERR_EF;
	}

	*prxdata = AS5147_U_DATA_MASK & reply;

	return AS5147_U_OK;
	}


	as5147_u_status_t AS5147_U_READ_ANGLE(as5147_u_t *device, uint16_t regaddy, uint16_t *prxdata){

		uint16_t header = 0;
		uint32_t reader = 0;
		uint32_t message = 0;
		uint8_t crc = 0;

		header = regaddy | AS5147_U_READ_WRITE;
		crc = crc_generator(header);


		message = message | (header<<8);
		message = message | crc;

		as5147_u_status_t status_1 = SPI_SEND_RET(device, message, &reader);

		if(status_1 != AS5147_U_OK){
				return status_1;
		}

		uint16_t reply = 0;
		as5147_u_status_t status_2 = unpack_reply(reader, &reply);
		if (status_2 != AS5147_U_OK){
			return status_2;
		}

		if ((reply & AS5147_U_RESP_EF) != 0) {
		    return AS5147_U_ERR_EF;
		}

		*prxdata = AS5147_U_DATA_MASK & reply;

		return AS5147_U_OK;
		}


	//as5147_u_status_t AS5147_U_WRITE(as5147_u_t *device, uint16_t regaddy, uint16_t txdata){

	//}

	static uint8_t crc_generator(uint16_t sender){
		uint8_t crc = AS5147_U_CRC_INIT;

		for(int j = 0; j<2; j++){
			if(j==0){
				crc = crc ^ (sender>>8);
			}else{
				crc = crc ^ sender;
			}

		for(int i = 0; i<8; i++){
				if (crc & (1U << 7)){
					crc = crc << 1;
					crc = crc ^ AS5147_U_CRC_POLY;
				}
				else{
					crc = crc << 1;
				}
			}
		}
		crc = crc ^ AS5147_U_FV;
		return crc;
	}

	static as5147_u_status_t SPI_SEND_RET(as5147_u_t *device, uint32_t sender, uint32_t *receiver){
		uint16_t tx[2];
		uint16_t rx[2];
		tx[0] = (sender>>16);
		tx[1] = (sender & 0xFFFF);
		*receiver = 0;

		HAL_GPIO_WritePin(device->cs_port, device->cs_pin,GPIO_PIN_RESET);

		HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(device->hspi, (uint8_t *)tx, (uint8_t *)rx,2 , HAL_MAX_DELAY);
		*receiver = ((uint32_t)rx[0] << 16) | rx[1];
		device->last_rx = *receiver;
		HAL_GPIO_WritePin(device->cs_port, device->cs_pin, GPIO_PIN_SET);

		if(status != HAL_OK){
			return AS5147_U_ERR_SPI;
		}

		return AS5147_U_OK;
	}

	static as5147_u_status_t unpack_reply(uint32_t frame, uint16_t *data){
		uint16_t reply = (uint16_t)((frame >> 8) & 0xFFFF);
		uint8_t rx_crc = (uint8_t)(frame & 0xFF);

		if (crc_generator(reply) != rx_crc){
			return AS5147_U_ERR_CRC;
		}

		*data = reply;
		return AS5147_U_OK;
	}
