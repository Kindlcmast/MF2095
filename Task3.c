

#include <avr/io.h>
#include <avr/interrupt.h>
#define ICR_VALUE 1023 // topp value for timer, ie ~ the frequency
//PWM frq = 16M/(Prescaler * (ICR +1)) --> PWM frq = 1953.125 hz 
volatile uint8_t timerowerflow=0;

// tics/s= (16M/prescaler)
// overflow time= (tics/s)/2^8 = 0.016
void init_pwm() {
    TIMSK0 =  0b00000001;    // Enable Timer1 overflow interrupt
    TCCR0B =  0b00000101;    //  Timer1 prescaler to 1024
    DDRB = 0b00000010; // OC1A output PB1-> pin 9 on Arduino 
    //Fast PWM mode, with non-inverted output on OC1A (PB1) 
    TCCR1A = 0b10000010;  // 0b COM1A1 0 0 0 0 0 WGM11 0 
    TCCR1B = 0b00011010;  // 0b 0 0 0 WGM13 WGM12 0 CS11 0, prescaler = 8
    
    ICR1 = ICR_VALUE;  //topp value for timer, the maximum ticks for one loop
    OCR1A = 0;
}

void read_AD() {
    ADMUX =0b01000000; // REFS0 for AVcc reference voltage 5V, MUX bits =0000 to select ADC0
    ADCSRA = 0b11101111; // 0b ADEN ADSC ADATE 0 ADIE ADPS2 ADPS1 ADPS0 
    ADCSRB = 0b00000000; // free running mode, ADTS2=ADTS1=ADTS0=0, ACME: AIN1 = Analog Comparator Negative Input ingen aning vad detta gör när jag ändå bara har possetitiva värden, eller om man flyttar referensjord på nått sätt
    //  ADPS2 ADPS1 ADPS0 Prescaler = 128 ger ett mer stabilt värde, än de som har lägre prescaler, hur länge en kondnsator laddas upp innan värdet utvärderas. funkar int emed noll av någon anledning
    //  ADEN Enable the ADC, turn it on
    //  ADATE Enable ADC Free-Running mode, tveksam ksk måste kolla mot innehållet i ADCSRB
    //  ADSC Start the ADC conversion, In Free Running mode, write this bit to one to start the first conversion.
    //  ADIE Enable ADC interrupt, 1 =ADC Conversion Complete Interrupt is activated
}

ISR(ADC_vect) {
    if (timerowerflow < 62){ 
    OCR1A = (uint16_t)ADC;  // 10-bit -> 16-bit
    //since ICR1 = ICR_VALUE=(uint16_t)ADC max value
    //@ max AD value the duty will be 100% 
    //@ min AD value (0) the duty will be 0%
} else{
    OCR1A=102;
}
}

ISR(TIMER0_OVF_vect){
    if (ADC>1000){ //4,8875855327468230694037145650049 volts or more
        timerowerflow++;
        if (timerowerflow==122){timerowerflow=0;}
    }
}

int main(void) {
    init_pwm();
    
    read_AD(); 
    
    sei();

    while (1) {}
}
