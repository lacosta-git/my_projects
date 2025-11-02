
#define SRV_0_DEGREE	16000 		// 1ms
#define SRV_180_DEGREE	32000		// 2ms
#define SVR_1_DEGREE	89			// ticks


void srv_attach(uint8_t *, uint8_t, uint8_t);
void srv_handler(void);
void srv_set_angle(uint8_t, uint8_t);
uint16_t srv_next_reload(void); // Function returns number of next timer ticks

