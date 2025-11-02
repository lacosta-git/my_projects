/*
 * Command execution
 *
 *  Created on: 10.06.2019
 *      Author: lapiUser
 */

#include "../main.h"
#include "../interfaces/ifc.h"

#define CE_EXECUTION  0
#define CE_PROGRAMMING 1

#define CE_DETECT_LINE_NUMBER 10
#define CE_FIND_END_OF_LINE 20
#define CE_COMPARE_LINE_NUMBERS 30
#define CE_INSERT_NEW_LINE 50
#define CE_UPDATE_CURRENT_LINE 60
#define CE_END_OF_EDIT 100

#define CE_DL_DETECT_LINE_NUMBER 10
#define CE_DL_FIND_END_OF_LINE 20
#define CE_DL_COMPARE 30
#define CE_DL_DEL_LINE 40
#define CE_DL_END 50



uint8_t cmd_exe_parser(char*);
