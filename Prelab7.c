/*
 * File:   Prelab7.c
 * Author: Josea
 *
 * Created on 5 de abril de 2022, 10:10 PM
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

uint8_t contador = 0;

//Subrutina de interrupción puerto B

void __interrupt() isr (void){
    
    if(INTCONbits.RBIF){                // Fue interrupci n del PORTB?
            
        if (!b0)             
            contador += 1;              // Incremento del contador
            INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupci n?
        if (!b1)
            contador -= 1;              // Decremento del contador
            INTCONbits.RBIF = 0; 
    }
    return;
}

void setup(void);                       //Prototipo de funcion

void main(void) {
    setup();
  

    while(1){
        PORTA = contador;               //siempre actualizamos el valor del contador en el puerto
    }
    return;
}


 void setup(void){
    ANSEL = 0;
    ANSELH = 0;                 // Usamos solo I/O digitales
    
    TRISA = 0x00;               // PORTA como salida
    PORTA = 0;                  // Limpiamos PORTA
  
    
    TRISBbits.TRISB0 = 1;       // RB0 como entrada (configurada con bits de control)
    TRISBbits.TRISB1 = 1;       // RB1 como entrada (configurada con bits de control)
    OPTION_REGbits.nRBPU = 0;   // Habilitamos resistencias de pull-up del PORTB
    WPUBbits.WPUB0 = 1;         // Habilitamos resistencia de pull-up de RB0
    WPUBbits.WPUB1 = 1;         // Habilitamos resistencia de pull-up de RB1
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.RBIE = 1;        // Habilitamos interrupciones del PORTB
    IOCBbits.IOCB0 = 1;         // Habilitamos interrupcion por cambio de estado ?para RB0
    IOCBbits.IOCB1 = 1;         // Habilitamos interrupcion por cambio de estado ?para RB1
    INTCONbits.RBIF = 0;        // Limpiamos bandera de interrupcion

    
    OSCCONbits.IRCF = 0b0110;   // Oscilador a 4mhz
    OSCCONbits.SCS = 1;        // Habilitado el oscilador interno
}