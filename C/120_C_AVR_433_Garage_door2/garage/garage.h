
// Garage
typedef struct {
	uint8_t state;					// Garage system state
	uint8_t door;  					// Garage door status   OPEN/OPENING/CLOSED/CLOSING
	uint8_t i_door_status;			// Garage door status detection  OPEN/CLOSED
	uint8_t i_obstacle_detected;	// Obstacle detection	TRUE/FALSE
	uint8_t i_sw_up_pressed;		// Manual UP switch pressed   TRUE/FALSE
	uint8_t i_sw_down_pressed;		// Manual DOWN switch pressed   TRUE/FALSE
	uint8_t o_door_close;			// Garage door closing relay   ON/OFF
	uint8_t o_door_open;			// Garage door opening relay   ON/OFF
	uint8_t beep;					// Beep function ON/OFF
	uint16_t beep_on_time_ms;		// Beep ON time in ms  MAX: 32 000 ms
	uint16_t beep_off_time_ms;		// Beep OFF time in ms  MAX: 32 000 ms
	uint8_t o_beep;					// Beep output ON/OFF
	uint8_t light;					// Warning light function ON/OFF
	uint16_t light_on_time_ms;		// Warning light ON time in ms MAX: 32 000 ms
	uint16_t light_off_time_ms;		// Warning light OFF time in ms MAX: 32 000 ms
	uint8_t o_light;				// Warning light output ON/OFF
} s_garage;

// Garage
#define G_TIME_DIV_SEC	1000

#define G_MAX_CLOSING_TIME_MS_MAX	32000
#define G_MAX_CLOSING_TIME_MS_DEF	15000
#define G_MAX_CLOSING_TIME_MS_MIN	1000

#define G_MAX_OPENING_TIME_MS_MAX	32000
#define G_MAX_OPENING_TIME_MS_DEF	15000
#define G_MAX_OPENING_TIME_MS_MIN	1000

#define G_MAX_DOOR_OPEN_TIME_SEC_MAX	3600
#define G_MAX_DOOR_OPEN_TIME_SEC_DEF	600
#define G_MAX_DOOR_OPEN_TIME_SEC_MIN	10

#define G_WARNING_TIME_MS_MAX		32000
#define G_WARNING_TIME_MS_DEF		3000
#define G_WARNING_TIME_MS_MIN		100

#define G_WARNING_BEEP_ON_TIME_MS_MAX	30000
#define G_WARNING_BEEP_ON_TIME_MS_DEF	100
#define G_WARNING_BEEP_ON_TIME_MS_MIN	0

#define G_WARNING_BEEP_OFF_TIME_MS_MAX	30000
#define G_WARNING_BEEP_OFF_TIME_MS_DEF	900
#define G_WARNING_BEEP_OFF_TIME_MS_MIN	0

#define G_WARNING_LIGHT_ON_TIME_MS_MAX	30000
#define G_WARNING_LIGHT_ON_TIME_MS_DEF	500
#define G_WARNING_LIGHT_ON_TIME_MS_MIN	0

#define G_WARNING_LIGHT_OFF_TIME_MS_MAX	30000
#define G_WARNING_LIGHT_OFF_TIME_MS_DEF	500
#define G_WARNING_LIGHT_OFF_TIME_MS_MIN	0

#define G_WARNINGS_ENABLED_DELAY_MS_MAX	30000
#define G_WARNINGS_ENABLED_DELAY_MS_DEF	2000
#define G_WARNINGS_ENABLED_DELAY_MS_MIN	100

#define G_TIME_BETWEEN_COMMANDS_MS_MAX	30000
#define G_TIME_BETWEEN_COMMANDS_MS_DEF	1000
#define G_TIME_BETWEEN_COMMANDS_MS_MIN	100

#define G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_MAX	30000
#define G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_DEF	100
#define G_WARNING_DOOR_OPEN_BEEP_ON_TIME_MS_MIN	0

#define G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_MAX	30000
#define G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_DEF	3000
#define G_WARNING_DOOR_OPEN_BEEP_OFF_TIME_MS_MIN	0

#define G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_MAX	30000
#define G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_DEF	0
#define G_WARNING_DOOR_OPEN_LIGHT_ON_TIME_MS_MIN	0

#define G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_MAX	30000
#define G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_DEF	3000
#define G_WARNING_DOOR_OPEN_LIGHT_OFF_TIME_MS_MIN	0


// Garage state
#define	G_IDLE		0
#define G_OPEN		2
#define G_OPENING	4
#define G_CLOSED	6
#define G_CLOSING	8
#define G_WARNING	10
#define G_DELAY_AFTER_CLOSING	12

// Buttons
#define HOLD_TIME_MS			2000
#define MAX_HEART_BEAT_TIME		250
#define BUTTON_NONE		0
#define BUTTON_OPEN		2
#define BUTTON_CLOSE	4


// Functions
void garage_read_eeprom_settings(void);
void garage_time_tick(void);
void garage_init(void);
void garage_handler(void);
void garage_button_pressed(uint8_t);
void beep_handler(void);
void light_handler(void);
void garage_toggle_auto_closing(void);
void auto_closing_handler(void);

void setup_warnings (uint16_t, uint16_t, uint16_t, uint16_t);

