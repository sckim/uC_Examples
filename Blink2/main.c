/*
 * Blink2.c
 *
 * Created: 2019-11-21 오후 5:31:51
 * Author : Soochan Kim
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    /* Replace with your application code */
	DDRD = _BV(PD3);
    while (1) 
    {
		PORTD |= _BV(PD3);
		_delay_ms(100);
		PORTD &= ~_BV(PD3);
		_delay_ms(100);
    }
}

