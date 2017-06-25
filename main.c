/*
 * teclaTiempo.c
 *
 * Created: 25/06/2017 09:29:30 a. m.
 * Author : LuisEduardo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t mSeg;
static volatile uint16_t mdSeg;
static volatile int flag500;
static volatile int errorFlag = 0;
static volatile int pressedFlag = 0;

char UART0_getchar( void );
void itoa( char *str, uint16_t num, uint8_t base );
void countVars_init( void );
void enterVars_init( void );
void Timer0_init( void );
void UART0_init( void );
void UART0_putchar( char dato );
void UART0_puts( char *str );


char strMseg[20];
char strMdseg[20];
int main(void)
{
	Timer0_init();
    UART0_init();

    while (1) 
    {
    	UART0_puts("\n\rPresiona <enter> para iniciar");
    	while(  UART0_getchar() != 13 );
    	enterVars_init();
    	while( !flag500 );
    	UART0_puts("\n\rPresiona una tecla");
    	countVars_init();
    	UART0_getchar();
    	pressedFlag = 1;
    	if( errorFlag )
    	{
    		UART0_puts("\n\rError, intentelo de nuevo.");
    	}else
    	{
    		
    		itoa(strMseg, mSeg,10);
    		itoa(strMdseg, mdSeg,10);
    		UART0_puts("\n\rTecla presionada en: ");
    		UART0_puts(strMseg);
    		UART0_putchar('.');
    		UART0_puts(strMdseg);
    	}
    }
}
void Timer0_init( void )
{
	TCCR0B = (3<<CS00);
	TCNT0 = 0;
	OCR0A = 25-1;
	TIMSK0 = (1<<OCIE0A);
	TCCR0A = (2<<WGM00);
	sei();
}
void enterVars_init( void )
{
	mdSeg = 0;
	mSeg = 0;
	flag500 = 0;
}
void countVars_init( void )
{
	pressedFlag = 0;
	errorFlag = 0;
}
void UART0_init( void )
{
	UBRR0 = 103;
	UCSR0A = (1<<U2X0);
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	UCSR0C = (3<<UCSZ00);
}
void UART0_puts( char *str )
{
	while( *str )
		UART0_putchar( *str++ );
}
void UART0_putchar( char dato )
{
	while( !(UCSR0A&(1<<UDRE0)) );
	UDR0 = dato;
}
char UART0_getchar( void )
{
	while( !(UCSR0A&(1<<RXC0)) );
	return UDR0;
}
void itoa( char *str, uint16_t num, uint8_t base)
{
	char auxNum;
	uint16_t aux, i = 0, j = 0;
	do{
		aux = num/base;
		( (num%=base)>9 )? (str[j++] = num+'7') : (str[j++] = num+'0');
		num = aux;
	}while( num );
	str[j--] = '\0';

	while( i<j )
	{
		auxNum = str[i];
		str[i++] = str[j];
		str[j--] = auxNum; 
	}
}
ISR( TIMER0_COMPA_vect )
{
	mdSeg++;
	if( mdSeg>10 )	//0.001mseg en mdSeg 
	{	
		mdSeg -=10;		
		mSeg++;		//1mseg 
	}

	if( mSeg >= 500 && !flag500)
	{
		flag500 = 1;
	}

	if( pressedFlag && (mdSeg <=9)&& (mSeg <=999) )
	{
		TCCR0B = 0;
		errorFlag = 0;
	}else
	{
		errorFlag = 1;
	}
} 

