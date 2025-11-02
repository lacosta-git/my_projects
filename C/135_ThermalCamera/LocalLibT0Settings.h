/*
 * LocalLibT0Settings.h
 *
 * Created: 2014-03-01 18:46:19
 *  Author: Tomasz Szkudlarek
 */ 

#define T0_NormalOperation		TCCR0A=(0<<COM0A0)|(0<<COM0A1)|(0<<COM0B0)|(0<<COM0B1)|(0<<WGM00)|(0<<WGM01)
#define T0_PRESCLER_1024		TCCR0B=(1<<CS02)|(0<<CS01)|(1<<CS00)|(0<<WGM02)
#define T0_20msValue			(uint8_t)(256-156)

#ifndef LOCALLIBT0SETTINGS_H_
#define LOCALLIBT0SETTINGS_H_

void t0_timer_init(void);

#endif /* LOCALLIBT0SETTINGS_H_ */