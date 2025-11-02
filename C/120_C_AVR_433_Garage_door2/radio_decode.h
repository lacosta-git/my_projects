
// BIT DETECTION
#define CODE_IDLE 	0
#define CODE_COUNT_LOW 	2
#define CODE_BIT_DETECT 10


// PILOT CODES ASSIGMENT - DEFAULT CODES
#define PILOT_CODE_OPEN   "101010101010101011110011"  // Default codes to changed via RS232 as required
#define PILOT_CODE_CLOSE  "101010101010101000111111"

#define PILOT_CODE_OPEN_2   "1110101111111001101111010001011101110111"
#define PILOT_CODE_CLOSE_2  "1110101111111001101111010001011101100110"

#define PILOT_CODE_OPEN_3   "1110101111111001110000101000111101110000"
#define PILOT_CODE_CLOSE_3  "1110101111111001110000101000111101100001"

#define CODE_VALID_TIME_MS	200  // Detected code validity time in 1ms

void verify_code(void);
void radio_code_time_tick(void);
void data_pin_handler(void);
void radio_decode_handler(void);
void rc_init(void);
void rc_read_key_codes(void);
