/*
 * File:   Postlab7.c
 * Author: Josea
 *
 * Created on 9 de abril de 2022, 04:57 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory codeprotection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000    //definicion de frecuencia macro delay 4mhz


#define b0 PORTBbits.RB0
#define b1 PORTBbits.RB1

//definicion de variables
uint8_t contador = 0,selectorDSP = 0,DSP1 = 0, DSP2 = 0, DSP3 = 0, unidades = 0, decenas = 0, centenas = 0, temp = 0;

//esta sería nuestra nueva tabla
uint8_t DISPLAY[10]= {0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111};

//Subrutina de interrupciones_________________________________________________________________________________________

void __interrupt() isr (void){
    
    if(INTCONbits.RBIF){                // Fue interrupcion del PORTB?
            
        if (!b0)             
            contador += 1;              // Incremento del contador
            INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupci n?
        if (!b1)
            contador -= 1;              // Decremento del contador
            INTCONbits.RBIF = 0; 
    }
    
    if(T0IF){                           // Interrupcion del timer0
        if(selectorDSP < 4) 
           selectorDSP++;               // Incremento del selector para el multiplexado
        else
            selectorDSP = 0;            // solo tenemos 3 displays
        
        TMR0 = 249;
        INTCONbits.T0IF = 0;            // Reiniciamos el timer0
     
    }              
      

    return;
}
//_______________________________________________________________________________________________________________________


//Prototipos de funciones________________________________________________________________________________________________

void setup(void);                       
void DSPsetup(void);
void separacion(void);

//_______________________________________________________________________________________________________________________


//______________________________MAIN_____________________________________________________________________________________
void main(void) {
    setup();
  

    while(1){
        PORTA = contador;               //siempre actualizamos el valor del contador en el puerto
        //Multiplexado
        DSPsetup();
        separacion();
        switch(selectorDSP)
    {
        case 0:
            PORTD = 0;
            break;
        case 1:
            PORTD = 0;
            PORTC = DSP1;
            PORTDbits.RD0 = 1;
            break;
        case 2:
            PORTD = 0;
            PORTC = DSP2;
            PORTDbits.RD1 = 1;
            break;
        case 3:
            PORTD = 0;
            PORTC = DSP3;
            PORTDbits.RD2 = 1;
            break;
    }
     
    }
    return;
}


//_______________________________________________RUTINAS PARA DISPLAYS_________________________________________________
void separacion(void){
    temp = contador;
    
    if(temp >= 100)
        centenas = temp/100;
        temp = temp%100;
    if(temp >= 10)
        decenas = temp/10;
        temp = temp%10;
    if(temp >= 0)
        unidades = temp;
}

void DSPsetup(void){ 
        DSP1= DISPLAY[centenas];
        DSP2= DISPLAY[decenas];
        DSP3= DISPLAY[unidades];
    return;
}
//________________________________________________________________________________________________________________________

//______________________________________________CONFIGURACIONES___________________________________________________________
 void setup(void){
    ANSEL = 0;
    ANSELH = 0;                 // Usamos solo I/O digitales
    
    TRISA = 0x00;               // PORTA como salida
    PORTA = 0;                  // Limpiamos PORTA
  
    TRISC = 0x00;               // PORTC como salida
    PORTC = 0;                  // Limpiamos PORTC
    
    TRISD = 0x00;               // PORTD como salida
    PORTD = 0;                  // Limpiamos PORTD
    
    TRISBbits.TRISB0 = 1;       // RB0 como entrada (configurada con bits de control)
    TRISBbits.TRISB1 = 1;       // RB1 como entrada (configurada con bits de control)
    OPTION_REGbits.nRBPU = 0;   // Habilitamos resistencias de pull-up del PORTB
    WPUBbits.WPUB0 = 1;         // Habilitamos resistencia de pull-up de RB0
    WPUBbits.WPUB1 = 1;         // Habilitamos resistencia de pull-up de RB1
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.RBIE = 1;        // Habilitamos interrupciones del PORTB
    INTCONbits.T0IE = 1;        // habilitamos la interrupcion del timer0
    IOCBbits.IOCB0 = 1;         // Habilitamos interrupcion por cambio de estado ?para RB0
    IOCBbits.IOCB1 = 1;         // Habilitamos interrupcion por cambio de estado ?para RB1
    INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupcion
    INTCONbits.T0IF = 0;
    
    OSCCONbits.IRCF = 0b0101;   // Oscilador a 2mhz
    OSCCONbits.SCS = 1;        // Habilitado el oscilador interno
    
    //Timer 0 a 1ms
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 
    OPTION_REGbits.PSA = 0;   // bit 3  
    OPTION_REGbits.PS2 = 0;   // bits 2-0  
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;


}