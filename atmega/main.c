#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ov7670.h"

#define SPEAKER_PORT PORTA
#define SPEAKER_DDR DDRA
#define SPEAKER_PIN 5

const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;
 
const int buzzerPin = 8;
const int ledPin1 = 12;
const int ledPin2 = 13;
 
int counter = 0;
int pixel_count = 0;

void PLAYNOTE(float duration, float frequency)
{

	long int i, cycles;
	float half_period;
	float wavelength;

	wavelength = (1 / frequency) * 1000;
	cycles = duration / wavelength;
	half_period = wavelength / 2;

	SPEAKER_DDR |= (1 << SPEAKER_PIN);

	for (i = 0; i < cycles; i++)
	{
		_delay_ms(half_period);
		SPEAKER_PORT |= (1 << SPEAKER_PIN);
		_delay_ms(half_period);
		SPEAKER_PORT &= ~(1 << SPEAKER_PIN);
	}

	return;
}

void beep(float frequency, float duration){
	PLAYNOTE(duration,frequency);
}

/* star wars */
void loop()
{
	_delay_ms(500);
  //Play first section
  firstSection();
 
  //Play second section
  secondSection();
 
  //Variant 1
  beep(f, 250);  
  beep(gS, 500);  
  beep(f, 350);  
  beep(a, 125);
  beep(cH, 500);
  beep(a, 375);  
  beep(cH, 125);
  beep(eH, 650);
 
  _delay_ms(500);
 
  //Repeat second section
  secondSection();
 
  //Variant 2
  beep(f, 250);  
  beep(gS, 500);  
  beep(f, 375);  
  beep(cH, 125);
  beep(a, 500);  
  beep(f, 375);  
  beep(cH, 125);
  beep(a, 650);  
 
  _delay_ms(650);
}
 
void firstSection()
{
  beep(a, 500);
  beep(a, 500);    
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);  
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  _delay_ms(500);
 
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);  
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
 
  _delay_ms(500);
}
 
void secondSection()
{
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);    
  beep(fSH, 250);
 
  _delay_ms(325);
 
  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);  
  beep(cSH, 175);  
  beep(cH, 125);  
  beep(b, 125);  
  beep(cH, 250);  
 
  _delay_ms(350);
}

void play_song(){

	PLAYNOTE(400, 880); // Musical note 880 Hz

	PLAYNOTE(400, 932);

	PLAYNOTE(400, 988);

	PLAYNOTE(400, 1047);

	PLAYNOTE(400, 1109);
}
/* if the pixels are in range then play the song*/
// 2 bytes pixel
void checkAndPlay(uint8_t b1, uint8_t b2){
	float r,g,b;
	/*r,g,b must be in the range of [0,1]*/
	/* r,g,b bits are in datasheet */
	// 32 = 2 ^ 5 (5 bits of data)
	r = (b1 >> 3) / 32.0;
	/* green have 6 bits of data*/
	g = (((b1 << 3) >> 5) + (b2 >> 5) ) / 64.0;
	b = ((b2 << 3) >> 5) / 32.0;

	// this values are from the trainer
	if( r < 0.2 && r > 0.1 && g < 0.2 && b > 0.3 && b < 0.4){
		pixel_count ++;
	}
	if(pixel_count == 50){
		loop();
	}
}

/* Configuration: this lets you easily change between different resolutions
 * You must only uncomment one
 * no more no less*/
//#define useVga
//#define useQvga
#define useQqvga

static void captureImg(uint16_t wg,uint16_t hg){
	uint16_t lg2;
#ifdef useQvga
	uint8_t buf[640];
#elif defined(useQqvga)
	uint8_t buf[320];
#endif
	//Wait for vsync it is on pin 3 (counting from 0) portD
	while(!(PORTD&8));//wait for high
	while((PORTD&8));//wait for low
#ifdef useVga
	while(hg--){
		lg2=wg;
		while(lg2--){
			while((PORTD&4));//wait for low
			UDR0=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
		}
	}
#elif defined(useQvga)
	/*We send half of the line while reading then half later */
	while(hg--){
		uint8_t*b=buf,*b2=buf;
		lg2=wg/2;
		while(lg2--){
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			UDR0=*b2++;
			while(!(PORTD&4));//wait for high
		}
		/* Finish sending the remainder during blanking */
		lg2=wg/2;
		while(!( UCSR0A & (1<<UDRE0)));//wait for byte to transmit
		while(lg2--){
			UDR0=*b2++;
			while(!( UCSR0A & (1<<UDRE0)));//wait for byte to transmit
		}
	}
#else
	/* This code is very similar to qvga sending code except we have even more blanking time to take advantage of */
	while(hg--){
		uint8_t*b=buf,*b2=buf;
		lg2=wg/5;
		while(lg2--){
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
			while((PORTD&4));//wait for low
			*b++=(PINC&15)|(PORTD&240);
			while(!(PORTD&4));//wait for high
		}
		checkAndPlay(b[lg2],b[lg2 + 1]);
		/* Finish sending the remainder during blanking */
		lg2=320-(wg/5);
		
	}
#endif
}
int main(void){
	cli();//disable interrupts
	play_song();
	/* Setup the 8mhz PWM clock 
	 * This will be on pin 11*/
	DDRB|=(1<<3);//pin 11
	ASSR &= ~(_BV(EXCLK) | _BV(AS2));
	TCCR2A=(1<<COM2A0)|(1<<WGM21)|(1<<WGM20);
	TCCR2B=(1<<WGM22)|(1<<CS20);
	OCR2A=0;//(F_CPU)/(2*(X+1))
	DDRC&=~15;//low d0-d3 camera
	DDRD&=~252;//d7-d4 and interrupt pins
	_delay_ms(3000);
	//set up twi for 100khz
	TWSR&=~3;//disable prescaler for TWI
	TWBR=72;//set to 100khz
	//enable serial
	UBRR0H=0;
	UBRR0L=1;//0 = 2M baud rate. 1 = 1M baud. 3 = 0.5M. 7 = 250k 207 is 9600 baud rate.
	UCSR0A|=2;//double speed aysnc
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);//Enable receiver and transmitter
	UCSR0C=6;//async 1 stop bit 8bit char no parity bits
	camInit();
#ifdef useVga
	setRes(VGA);
	setColorSpace(BAYER_RGB);
	wrReg(0x11,25);
#elif defined(useQvga)
	setRes(QVGA);
	setColorSpace(YUV422);
	wrReg(0x11,12);
#else
	setRes(QQVGA);
	setColorSpace(YUV422);
	wrReg(0x11,3);
#endif
	/* If you are not sure what value to use here for the divider (register 0x11)
	 * Values I have found to work raw vga 25 qqvga yuv422 12 qvga yuv422 21
	 * run the commented out test below and pick the smallest value that gets a correct image */
	while (1){
		/* captureImg operates in bytes not pixels in some cases pixels are two bytes per pixel
		 * So for the width (if you were reading 640x480) you would put 1280 if you are reading yuv422 or rgb565 */
		/*uint8_t x=63;//Uncomment this block to test divider settings note the other line you need to uncomment
		  do{
		  wrReg(0x11,x);
		  _delay_ms(1000);*/
#ifdef useVga
		captureImg(640,480);
#elif defined(useQvga)
		captureImg(320*2,240);
#else
		captureImg(160*2,120);
#endif
		//}while(--x);//Uncomment this line to test divider settings
	}
}
