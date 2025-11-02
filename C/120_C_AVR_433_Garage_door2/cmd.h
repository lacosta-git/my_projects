
#include "main.h"


#define C_INPUT_BUF_SIZE	200  // Input buffer size in chars
#define C_CMD_MAX_SIZE		100	 // Max size of single command
#define C_MS		0
#define C_SEC		1

void c_exe_cmd(void);
void c_cmd_tick_ms(void);
void c_cmd_handler(void);
void c_extract_digits_txt(char*, char*);
void c_clear_code(void);
void c_validate_save_update_code(uint16_t, char *);

