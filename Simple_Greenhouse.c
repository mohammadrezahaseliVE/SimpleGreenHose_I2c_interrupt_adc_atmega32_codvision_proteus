/*******************************************************
Project : Simple GreenHouse Controll
Version : 2.0.0
Date    : 05/13/2022
Author  : Mohammad Reza Haseli 
Comments: this project will control Tempreture(LM35) and Light(Torch) as adc in intrupt mode
and also displaye the current sitiution on two lcd that connect to micro in I2C mode(PCF8574A) 


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

// Declare Function
#include <mega32.h>
#include <stdio.h>
#include <delay.h>
#include <i2c.h>
#include <LCD_i2c.h>

// Declare Output PortB
#define greenLED_1  PORTB.0
#define greenLED_2  PORTB.2
#define redLED_1  PORTB.1
#define redLED_2  PORTB.3
#define yellowLED_1  PORTB.4
#define yellowLED_2  PORTB.5

// Declare i2c addrees
#define LCD_1   0x7E
#define LCD_2   0x72


// Declare your global variables here
unsigned int adc_data_0;
unsigned int adc_data_1;
unsigned int adc_data_2;
unsigned int adc_data_3; 

int count = 0;
char data_0[16], data_1[16], data_2[16], data_3[16];


// Declare sensoers function
void AdcControl(int temp1, int temp2, int torch1, int torch2);

//Declare Display finction
void Displaye();

// ADC interrupt service routine
interrupt [ADC_INT] void adc_isr(void)
{
    //count will increse each time intrupt called and help to swith between adc pins (0-4)
    count++;
    if (count == 5) count = 0;

    if (count == 0)
    {
        ADMUX=0x01;
        adc_data_0 = ADCW;    
    }
    
    else if (count == 1)
    {
        ADMUX=0x02;
        adc_data_1 = ADCW;      
    }
    
    else if (count == 2)
    {
        ADMUX=0x03;
        adc_data_2 = ADCW;
        adc_data_2 = ((adc_data_2 * 4.8) / 10)+1;       
    }
    
    else if (count == 3)
    {
        ADMUX=0x00;
        adc_data_3 = ADCW;
        adc_data_3 = ((adc_data_3 * 4.8) / 10)+1;                
    } 
    
    //start conversion
    ADCSRA |= (1<<ADSC); 
    
    AdcControl(adc_data_2, adc_data_3, adc_data_0, adc_data_1);                         
}

void main(void)
{
    // Declare PORTA as input
    DDRA = 0x00;

    // Declare PORTB as input
    DDRB = 0xFF;

    // ADC initialization
    // ADC Clock frequency: 125.000 kHz
    // ADC Voltage Reference: AREF pin
    // ADC Auto Trigger Source: ADC Stopped
    ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
    SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
    ADCSRA |= (1<<ADSC); 
    // Bit-Banged I2C Bus initialization
    // I2C Port: PORTC
    // I2C SDA bit: 1
    // I2C SCL bit: 0
    // Bit Rate: 100 kHz
    // Note: I2C settings are specified in the
    // Project|Configure|C Compiler|Libraries|I2C menu.

    
    _i2c_init();
    _i2c_start();
    
    
    _i2c_write(LCD_2);
    lcd_init();
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("LCD_2 init :)");
    delay_ms(1000);
    lcd_clear();
     
    _i2c_write(LCD_1);
    _i2c_start();
    lcd_init();
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("LCD_1 init :)"); 
    delay_ms(1000);
    lcd_clear();
    delay_ms(1000);
        
    // Global enable interrupts
    
    #asm("sei")

while (1)
      {
        Displaye();                    
      }
}

void AdcControl(int temp1, int temp2, int torch1, int torch2)
{   

    //condition for temp1 
    if ((temp1 >= 20))
    {
        greenLED_1= 1;
        redLED_1 = 0;
    }
    else if (temp1 < 20)
    {
        greenLED_1= 0;
        redLED_1 = 1;
    }
    //condition for temp2
    if ((temp2 >= 20))
    {
        greenLED_2= 1;
        redLED_2 = 0;
    }
    else if (temp2 < 20)
    {
        greenLED_2= 0;
        redLED_2 = 1;
    }
    //condition for lightp1
    if ((torch1 >= 100))
    {
        yellowLED_1= 1;
    }
    else if (torch1 < 100)
    {
        yellowLED_1= 0;
    }
    //condition for light2 
     if ((torch2 >= 100))
    {
        yellowLED_2= 1;
    }
    else if (torch2 < 100)
    {
        yellowLED_2= 0;
    }  

}

void Displaye()
{
    _i2c_write(LCD_1);
    _i2c_start();
    //lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Temp = ");
    lcd_put_cur(0, 8);
    sprintf(data_2, "%02d", adc_data_2);
    lcd_send_string(data_2);
     
    
    
    lcd_put_cur(1, 0);
    lcd_send_string("Light = ");
    lcd_put_cur(1, 8);
    sprintf(data_0, "%04d", adc_data_0);
    lcd_send_string(data_0);
    

    _i2c_write(LCD_2);
    _i2c_start();
    //lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Temp = "); 
    lcd_put_cur(0, 8);
    sprintf(data_3, "%02d", adc_data_3);
    lcd_send_string(data_3);
    
    lcd_put_cur(1, 0);
    lcd_send_string("Light = ");
    lcd_put_cur(1, 8);
    sprintf(data_1, "%04d", adc_data_1);
    lcd_send_string(data_1);        
    
}