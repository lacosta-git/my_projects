/*
 * at45db0xx.h
 *
 *  Created on: 23 paü 2016
 *      Author: autoMaster
 */

/* Definition
 *
 */

#define PAGE_MIN	0
#define PAGE_MAX	2047

// COMmands
#define COM_CONTINOUS_ARRAY_READ	0x0B
#define COM_MAIN_MEMORY_PAGE_READ	0xD2
#define COM_BUFFER1_READ			0xD4
#define COM_BUFFER2_READ			0xD6
#define COM_BUFFER1_WRITE			0x84
#define COM_BUFFER2_WRITE			0x87
#define COM_BUFFER1_TO_MAIN_MEM_WITH_ERASE 	0x83
#define COM_BUFFER2_TO_MAIN_MEM_WITH_ERASE 	0x86
#define COM_BUFFER1_TO_MAIN_MEM 	0x88
#define COM_BUFFER2_TO_MAIN_MEM 	0x89
#define COM_PAGE_ERASE				0x81
#define COM_MAIN_MEM_TO_BUFFER1		0x53
#define COM_MAIN_MEM_TO_BUFFER2		0x55

#define COM_STATUS_REGISTER_READ	0xD7
#define SR_READY_MASK				0x80
#define SR_READY_BIT				0x07  // 7...0
#define SR_COMP_MASK				0x40
#define SR_COMP_BIT					0x06  // 7...0
#define SR_PROTECT_MASK				0x02
#define SR_PROTECT_BIT				0x01  // 7...0
#define SR_PAGE_SIZE_MASK			0x01
#define SR_PAGE_SIZE_BIT			0x00  // 7...0

#define COM_DEVICE_ID_READ			0x9F

// Functions
void chipErase(void);
void readFlash(char*, uint32_t, uint8_t);
void writeFlash(char*, uint32_t, uint8_t);


