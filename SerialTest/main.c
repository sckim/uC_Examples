/*
 * UART_simple.c
 *
 *  Created on: 2019. 5. 28.
 *      Author: Soochan Kim
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

char str[80];

void InitUART(unsigned long iBaudrate) {

	// UCSRnA 레지스터를 초기화시킨다.
	// 0번째 비트, 즉 MPCMn 를 0으로 세트 (USARTn을 멀티 프로세서 통신모드로 설정)
	UCSR0A = 0x00;

	// UCSRnB 레지스터를 이용하여 송신 및 수신 사용설정을 한다.
	// Rx, Tx enable
	// TXENn (USARTn모듈의 송신부 동작 enable)
	// RXENn (USARTn모듈의 수신부 동작 enable)
	UCSR0B = (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	// UCRnC 레지스터를 이용하여 모드(동기/비동기), 패리티모드, 정지비트,
	// 비동기 방식, No Parity bit, 1 Stop bit, 8bits
	UCSR0C |= (1 << UCSZ01);
	UCSR0C |= (1 << UCSZ00);

	// See http://wormfood.net/avrbaudcalc.php
	// UBRRnH(L) 레지스터를 이용한 송수신 보레이트 설정
	UBRR0H = 0x00;
	switch (iBaudrate) {
	case 9600:
		//UBRR0L = 95; // 14.7456 MHz -> 9600 bps
		UBRR0L = 103; // 16 MHz -> 9600 bps
		break;
	case 19200:
//		UBRR0L = 47; // 14.7456 MHz -> 19200 bps
		UBRR0L = 51; // 16 MHz -> 9600 bps
		break;
	case 115200:
		//UBRR0L = 7;  // 14.7456 MHz -> 115200 bps
		UBRR0L = 8;  // 16 MHz -> 115200 bps
		break;
	default: //9600
		UBRR0L = 103;
	}
}

void UART_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	//while (!( UCSR0A & (1 << UDRE0)))
	//	;
	loop_until_bit_is_set(UCSR0A, UDRE0);

	UDR0 = data;
}

unsigned char UART_Receive(void) {
	/* Wait for data to be received */
	while (!(UCSR0A & (1 << RXC0)))
		;
//	loop_until_bit_is_set(UCSR0A, RXC0);

	return UDR0;
}

void UART_putString(char *data) {
	while (*data != '\0')
		UART_Transmit(*data++);
}

void uart_printf(char data, FILE * stream)
{
	UART_Transmit(data);
}

unsigned char uart_scanf(FILE * stream)
{
    return UART_Receive();
}

//새로운 standard io를 지정한다.
static FILE std_output = FDEV_SETUP_STREAM(uart_printf, NULL, _FDEV_SETUP_WRITE);
static FILE std_input = FDEV_SETUP_STREAM(NULL, uart_scanf, _FDEV_SETUP_READ);

void InitADC() {
	// For Aref=AVcc;
	ADMUX = (1 << REFS0);
	//Rrescalar div factor =128
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ReadADC(uint8_t ch) {
	//Select ADC Channel ch must be 0-7
	ADMUX &= ~(0x07);
	ADMUX |= (ch & 0x07);

	//Start Single conversion
	ADCSRA |= (1 << ADSC);

	//Wait for conversion to complete
	while (!(ADCSRA & (1 << ADIF)))
	;

	//Clear ADIF by writing one to it
	//Note you may be wondering why we have write one to clear it
	//This is standard way of clearing bits in io as said in datasheets.
	//The code writes '1' but it result in setting bit to '0' !!!
	ADCSRA |= (1 << ADIF);

	return (ADC);
}

int main(void) {
	char recieved_byte;
	char text[] = "\r\nWelcome! Serial communication world!!\r\n Good Luck\r\n";
	char echo[] = "HKNU >> ";
	int i;
	unsigned int adc_value;

	//stdout과 stdin에 사용자가 정의한 함수로 설정
	//아래 2줄은 main 함수 밖에 작성하면 오류가 발생하므로 주의
	stdout = &std_output;
	stdin = &std_input;

	InitUART(19200);
	InitADC();

	i = 0;
//	while (text[i] != '\0') {
//		UART_Transmit(text[i++]);
//	}
	printf("\r%s", text);

//	UART_putString(echo);
	puts(echo);


	for (;;) {
		_delay_ms(1000);
		adc_value = ReadADC(0);
		printf("ADC[0] = %4d\r\n", adc_value);
		adc_value = ReadADC(1);
		printf("ADC[1] = %4d\r\n", adc_value);
	}
	return 0; // never reached
}
