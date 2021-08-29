#define F_CPU 16000000L
#define BAUDRATE 9600
#define ADDRESS 0x3C

#define DISPLAY_OFF 0xAE
#define	SETDISPLAYCLOCKDIV 0xD5 //Set display clock divide ratio/oscillator frequency
#define SETMULTIPLEX 0xA8 //set multiplex ratio
#define SETDISPLAYOFFSET 0xD3
#define SETSTARTLINE 0x40
#define CHARGEPUMP 0x8D
#define MEMORYMODE 0x20 //SET MEMORY ADRESS MODE, EN ESTE CASO NO ES NECESARIO YA VIENE POR DEFECTO EN HORIZONTAL - primero 0x20 y despues 0x00
#define SEGREMAP 0xA0
#define COMSCANDEC0 0xC0 //SCAN FROM COM[N-1] TO COM0 //N ES EL NUMERO DE MULTIPLEX RATIO, en este caso 64-1 = 63 = 0x3F 
#define COMSCANDEC8 0xC8 //SCAN FROM COM0 TO COM[N-1]
#define SETCOMPINS 0xDA //Set COM Pins Hardware Configuration
#define SETCONTRAST 0x81
#define SETVCOMDETECT 0xDB
#define DISPLAYALLON_RESUME 0xA4
#define NORMALDISPLAY 0xA6
#define DISPLAYON 0xAF
#define SETPRECHARGE 0xD9
#define SETCOLUMNADRESS 0x21
#define SETPAGEADRESS 0x22



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Prototipos
void InitDisplay();


//Prototipos TWI
void Start_TWI();
void Send_Address();
void Send_Data(uint8_t);
void Stop_TWI();
void Send_Command(uint8_t,uint8_t);
void Send_Command22(uint8_t,uint8_t);
void Write_Display(uint8_t,uint8_t);
void ClearDisplay();

typedef union{
	struct{
		unsigned char validHeader: 1;
		unsigned char validCmd: 1;
		unsigned char confirmBoton : 1;
		unsigned char botonpresionado : 1;
		unsigned char esperandoTwi : 1;
	}bit;
}_sFlag;

volatile _sFlag flags1;


//variables globales
volatile uint8_t timeoutBlink;
volatile uint8_t maxtimeBlink;
uint8_t timeBoton;
uint8_t buff[256];
uint8_t indL;
uint8_t indE;
uint8_t cksTx;
uint8_t cks;
uint8_t command;
uint8_t indcmd;
uint8_t status_TWI;
uint8_t estado_com_display;
uint16_t nbytes;
short indexHeader;


int main(void)
{
	cli();
	InitDisplay();

	estado_com_display = 0;
	timeoutBlink = 25;
	maxtimeBlink = timeoutBlink;
	timeBoton = 10;
	flags1.bit.botonpresionado = 0;
	flags1.bit.confirmBoton = 0;

	
	sei(); //habilita todas las interrupciones
	
	
	while (1)
	{
		//if(!(PIND & (1<<PIND2)) & !flags1.bit.botonpresionado){
			//flags1.bit.confirmBoton = 1;
			//if(!timeBoton){
				//flags1.bit.botonpresionado = 1;
				//flags1.bit.confirmBoton = 0;
				//PORTB |= (1<<PORTB5);
			//}
		//}
		
		//if(flags1.bit.botonpresionado){
		//if(!(PIND & (1<<PIND2))){
			//
			//
			//Send_Command(0x00,0xB3);
			//Send_Command(0x00,0x0F);
			//Send_Command(0x00,0x10);
			////Send_Command(0x00,0xE0);
			//
			//Send_Command(0x40,0xFF);
			//Send_Command(0x40,0x88);
			//Send_Command(0x40,0x88);
			//Send_Command(0x40,0x80);
			//Send_Command(0x40,0x80);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//
			//Send_Command(0x40,0x7f);
			//Send_Command(0x40,0x81);
			//Send_Command(0x40,0x81);
			//Send_Command(0x40,0x81);
			//Send_Command(0x40,0x81);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//
			//Send_Command(0x40,0x7f);
			//Send_Command(0x40,0x88);
			//Send_Command(0x40,0x88);
			//Send_Command(0x40,0x88);
			//Send_Command(0x40,0x7F);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
//
			//Send_Command(0x40,0xFf);
			//Send_Command(0x40,0x01);
			//Send_Command(0x40,0x01);
			//Send_Command(0x40,0x01);
			//Send_Command(0x40,0x01);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);
			//Send_Command(0x40,0x00);			
		//}
		
	}
}


void Send_Command(uint8_t c,uint8_t cmd){
	Start_TWI();
	Send_Address();
	Send_Data(c);
	Send_Data(cmd);
	Stop_TWI();
}

void Write_Display(uint8_t c,uint8_t cmd){
	Start_TWI();
	Send_Address();
	Send_Data(c);
	Send_Data(cmd);
	Stop_TWI();
}

void Send_Command22(uint8_t c,uint8_t cmd){
	//Start_TWI();
	//Send_Address();
	//Send_Data(c);
	Send_Data(cmd);
	//Stop_TWI();
}

void Start_TWI(){
	TWCR =	(1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI == 0x08 || status_TWI == 0x10){
		estado_com_display = 1;
	}
}
void Send_Address(){
	TWDR = (ADDRESS << 1);
	TWCR =(1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI==0x18 || status_TWI==0x20){
		estado_com_display = 2;
	}
}
void Send_Data(uint8_t cmd){
	TWDR = cmd;
	TWCR =	(1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI == 28 || status_TWI == 30){
		estado_com_display = 2;
	}
	else{
		estado_com_display = 4;
	}
}
void Stop_TWI(){
	TWCR =	(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)));
}

void ClearDisplay(){
	for(uint16_t i=0;i<1024; i++){
		Write_Display(0x40,0x00);
	}

}

void InitDisplay(){
	TWBR = 0x08; //bit rate register
	TWSR &=~ (1<<TWPS1) ;
	TWSR |= (1<<TWPS0) ;//bits del preescalador
	//Estos 3 datos son para el cálculo de la frec de transmisión, da 200KHz en esta config
	//TWCR |= (1<<TWEA);//habilita bit de acknowledge
	TWCR |= (1<<TWEN); //habilita el twi
	Start_TWI();
	Send_Address();
	Send_Data(0x00); //Le decimos a la pantalla que viene una lista de comandos de configuracion

	Send_Data(DISPLAY_OFF);//Apago display
	
	Send_Data(SETDISPLAYCLOCKDIV); //Establecer la velocidad del Oscilador
	Send_Data(0x80); //Establecer la velocidad del Oscilador
	
	Send_Data(SETMULTIPLEX); //establecemos multiplex 0xA8
	// Establecer el maximo de filas a 0x3F = 63
	// es decir, ira de 0 a 63, por tanto tenemos 64 filas de pixeles
	Send_Data(0x3F); //63
	
	Send_Data(SETDISPLAYOFFSET); //establecemos displey offset en 0x00
	Send_Data(0x00); //ponemos displey offset en 0x00
	
	Send_Data(SETSTARTLINE|0x0); //0x40 linea de inicio Poner el comienzo de linea a 0
	
	//Invertir el eje X de pantalla, por si esta girada.
	//Puedes cambiarlo por 0xA0 si necesitas cambiar la orientacion
	Send_Data(SEGREMAP|0x1); //0xA0 en este caso el resultado de la operacion da 0xA1
	
	//Invertir el eje Y de la patnalla
	//Puedes cambiarlo por 0xC0 si necesitas cambiar la orientacion
	Send_Data(COMSCANDEC0);// 0xC0 O 0xC8
	
	Send_Data(SETCOMPINS); //Set COM Pins Hardware Configuration
	Send_Data(0x12); // Sequential COM pin configuration
	
	Send_Data(SETCONTRAST); //set contrast
	Send_Data(0x9F);// value contrast //Este valor tiene que estar entre 0x00 (min) y 0xFF (max)
	
	Send_Data(SETPRECHARGE); //0xD9
	Send_Data(0x22);
	
	Send_Data(SETVCOMDETECT); //0xDB
	Send_Data(0x40);
	
	Send_Data(DISPLAYALLON_RESUME); //0xA4 ACTIVA LA SALIDA DE LO QUE HAY EN LA GDDRAM
	Send_Data(NORMALDISPLAY); //0xA6 MODO NORMAL DE DISPLAY
	//Como extra, establecemos el rango de columnas y paginas
	Send_Data(SETCOLUMNADRESS);//Columnas de 0 a 127
	Send_Data(0x00);
	Send_Data(0x7F);
	Send_Data(SETPAGEADRESS);//Paginas de 0 a 7
	Send_Data(0x00);
	Send_Data(0x07);

	Stop_TWI(); //CIERRO COMUNICACION
	
	
	ClearDisplay();
	Start_TWI();
	Send_Address();
	Send_Data(0x00);
	Send_Data(DISPLAYON);
	_delay_ms(100);
	Write_Display(0x00,0xA4);
	_delay_ms(1000);
	Write_Display(0x00,0xA5);
	_delay_ms(1000);
	Write_Display(0x00,0xA4);
	_delay_ms(1000);
	
		Send_Command(0x00,0xB3);
		Send_Command(0x00,0x0F);
		Send_Command(0x00,0x10);
		//Send_Command(0x00,0xE0);
				
		Send_Command(0x40,0xFF);
		Send_Command(0x40,0x88);
		Send_Command(0x40,0x88);
		Send_Command(0x40,0x80);
		Send_Command(0x40,0x80);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
				
		Send_Command(0x40,0x7f);
		Send_Command(0x40,0x81);
		Send_Command(0x40,0x81);
		Send_Command(0x40,0x81);
		Send_Command(0x40,0x81);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
				
		Send_Command(0x40,0x7f);
		Send_Command(0x40,0x88);
		Send_Command(0x40,0x88);
		Send_Command(0x40,0x88);
		Send_Command(0x40,0x7F);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);

		Send_Command(0x40,0xFf);
		Send_Command(0x40,0x01);
		Send_Command(0x40,0x01);
		Send_Command(0x40,0x01);
		Send_Command(0x40,0x01);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
		Send_Command(0x40,0x00);
				
}
