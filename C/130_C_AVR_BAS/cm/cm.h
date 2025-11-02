// Command Manager

#include "../main.h"
#include "../interfaces/ifc.h"

#define MAX_CMD_LEN 100

uint8_t cm_cmd_builder(char);
uint8_t cm_is_new_cmd_ready(void);
uint8_t cm_excute_cmd(void);

