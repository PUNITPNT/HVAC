#include <avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>
volatile int flag=1;
volatile int flag1=1;
volatile int y=0;
volatile int z=0;
int cabintemp=0;
int usertemp=0;
void init();
void interrupt0();
void interrupt1();
void acon();
void acoff();
void heateron();
void heateroff();
void bloweron();
void bloweroff();
void manualset(uint16_t ,uint16_t );
int power_led_on();
int power_led_off();
void init()
{
DDRD&=~(1<<PD2); //POWER switch pd2 input interrupt 0
DDRD|=(1<<PD0); //led pd0 power led output
DDRD&=~(1<<PD3); //mode switch pd3 interrupt 1 input
DDRD|=(1<<PD5);//led pd5 ac
DDRD|=(1<<PD6);//led pd6 heater
DDRD|=(1<<PD7);//led pd7 blower
DDRC&=~(1<<PC0);//adc0 PC0
DDRC&=~(1<<PC1);//adc1 PC1
}
void interrupt0()
{
DDRD&=~(1<<2);//interrupt switch power pd2
PORTD|=(1<<2);
EICRA|=(1<<ISC00);
EIMSK|=(1<<INT0);
sei();
}
ISR(INT0_vect)
{
    if(!(PIND&(1<<PD2)))
    flag=1;
    else
    flag=0;
}

void interrupt1()
{
DDRD&=~(1<<3);//interrupt switch mode pd3
PORTD|=(1<<3);
EICRA|=(1<<ISC10);
EIMSK|=(1<<INT1);
sei();
}

ISR(INT1_vect)
{
    if(!(PIND&(1<<PD3)))
    flag1=1;
    else
    flag1=0;
}
uint16_t adc_read0()
{
    ADCSRA|=(1<<ADEN);//ENABLE ADC
    ADMUX|=(1<<REFS0);//ADC CHANNEL 0 WITH AVCC AS REFERENCE
    ADMUX&=~(1<<MUX0);//SELECTING CHANNEL 0
    ADCSRA|=(1<<ADSC);
    while((ADCSRA)&(1<<ADSC));//START CONVERSION
    return(ADC);
}
uint16_t adc_read1()
{
  ADCSRA|=(1<<ADEN);//EN-ABLE ADC
  ADMUX|=(1<<REFS0);//ADC CHANNEL 0 WITH AVCC AS REFERENCE
  ADMUX|=(1<<MUX0);//SELECTING CHANNEL 1
  ADCSRA|=(1<<ADSC);
  while((ADCSRA)&(1<<ADSC));//START CONVERSION
  return(ADC);
}

void acon()
{
  PORTD|=(1<<PD5);
}
void acoff()
{
PORTD&=~(1<<PD5);
}
void heateron()
{
  PORTD|=(1<<PD6);
}
void heateroff()
{
   PORTD&=~(1<<PD6);
}
void bloweron()
{
  PORTD|=(1<<PD7);
}

void bloweroff()
{
   PORTD&=~(1<<PD7);
}

void manualset(uint16_t cabintemp,uint16_t usertemp)
{
if(cabintemp>usertemp)// 0=Cabin 1=user
  {
          acon();
          bloweron();
          _delay_ms(100);
          acoff();
          bloweroff();
          heateroff();

  }
else
  if(cabintemp<usertemp)
  {       acoff();
          heateron();
          bloweron();
         _delay_ms(100);
          heateroff();
          bloweroff();
  }
  else
     {  acoff();
       heateroff();
       bloweroff();

    }
}

int power_led_on()
{

  PORTD|=(1<<PD0);

}

int power_led_off()
{

    PORTD&=~(1<<PD0);
}
long temp_map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main(void)
{
init();
interrupt0();
interrupt1();
while(1)
{
     y=adc_read0();
     cabintemp=temp_map(y,1,255,0,40);
     _delay_ms(100);
     z=adc_read1();
     usertemp=temp_map(z,1,255,0,40);
     if(flag==1)
     {
       power_led_on();
       if(flag1==1)// manual set of temperature is on
       { 
         manualset(cabintemp,usertemp);


       }
       else//manual set temp is off
       {
         usertemp=22;
         manualset(cabintemp,usertemp);


       }


     }
     else
      {
       power_led_off();
       acoff();
       heateroff();
       bloweroff();

      }
}

return 0;
}
