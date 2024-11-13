/*
 * File:   Basic Circuit.c
 * Author: Hadi
 *
 * Created on June 3, 2024, 9:44 PM
 */


// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled) //off ///////////////
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 4000000UL // needed for the delays, set to 4 MHz crystal frequency

#include <xc.h>
#include <stdio.h>
#include "lcd_x8.h"
#include "my_adc.h"
#include "my_ser.h"

void initPorts(void);
void delay_ms(unsigned int n);

void main(void) {
    INTCON = 0;
    initPorts();
    setupSerial();
    char buffer[64];
    init_adc_no_lib();
   lcd_init();
    lcd_putc('\f');

    lcd_gotoxy(1, 1);
    lcd_puts(" Anwar & Abood");
    sprintf(buffer, " Anwar & Abood\r\n");
    send_string_no_lib(buffer);
    
  delay_ms(5000);

    lcd_putc('\f');

    char x;
    
    int counter = 0;
    int currentState = 0;
    int prevState = 0;
    while (1) { 
        lcd_gotoxy(1, 2);
    lcd_puts(" Anwar & Abood");
        if(PORTBbits.RB7 == 0){
            delay_ms(100);
            
            counter = 0;
        }
        
        char ir_sensor = PORTAbits.RA3 == 0 ? '0' : '1'; // sensor on RA3 if RA3=0 sensor well be 0 else sensor is 1
        int adc_value1 = read_adc_raw_no_lib(0);// channel 0
        float temp = (adc_value1 / 1023.0) * 500.0;
        
        if(temp > 45.0){
            PORTDbits.RD0 = 1; // buzzer on RD0
            sprintf(buffer, "Temp Alert!   ");
            lcd_gotoxy(1,1);
            lcd_puts(buffer);
        }
        else{
            currentState = ir_sensor;
            if(prevState == 1 & ir_sensor == '1'){
                counter++; // count every time when the sensor sense any thing
                 // tern buzzer on
            }
            else if(ir_sensor=='0'){
                PORTDbits.RD0 = 1;
            }
            else {
                 PORTDbits.RD0 = 0; //turn off the buzzer
            }
            if(ir_sensor == '0'){
                prevState = 1;

            }
            else{
                prevState = 0;

            }
           
            sprintf(buffer, "Counter : %d   ", counter);
            lcd_gotoxy(1,1);
            lcd_puts(buffer);
        }
        if (is_byte_available()) {
            x = read_byte_no_lib();
            sprintf(buffer, "%c", x);
            lcd_gotoxy(14, 1);
            lcd_puts(buffer);
            
            if (x == 'b') {
               
              PORTDbits.RD0 = 1;
              delay_ms(5000);
              send_string_no_lib("\n buzzer on"); 
            }
           // send_string_no_lib(buffer);
            PORTDbits.RD0 = 0;
        }
        
        
        delay_ms(100);
     
    }
}
void initPorts(void) {
    ADCON1 = 0x0D;
    LATA = LATB = LATC = LATD = LATE = 0;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISD = TRISE = 0;
    TRISC = 0b10000000;
}

/*void initPorts(void) {
    ADCON1 = 0x0D; // two analog input , AN0 and AN1 
    LATA = LATB = LATC = LATD = LATE = 0; //all output
    TRISA = 0xFF; // all prota input
    TRISB = 0xFF; // protb all input exept RB1 //0xFF
   // TRISBbits.RB1=0;
    //PORTBbits.RB1=0;
    TRISD = TRISE = 0; // portd and porte all output
    TRISC = 0b10000000; //portc is output and RX set to 1
}*/

void delay_ms(unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        __delaywdt_ms(1);
        CLRWDT(); // Clear Watchdog Timer to prevent reset
    }
}
/*void delay_ms(unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        __delaywdt_ms(1);
    }
}*/
