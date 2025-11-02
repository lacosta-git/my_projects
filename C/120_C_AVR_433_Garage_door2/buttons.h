// Defines
#define B_READ_DIVIDER	10  // Pull time = 1 ms * READ_DIVIDER
#define B_PRESSED	0		// Short to GND
#define B_PRESSED_FILTER 50	// Filter time = pull time * filter. Def 500 [ms]

// Functions
void b_buttons_init(void);
void b_buttons_time_tick_ms(void);
void b_buttons_handler(void);
