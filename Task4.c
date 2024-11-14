

#include <avr/io.h>
#include <avr/interrupt.h>
#define ICR_VALUE 1023 // topp value for timer, ie ~ the frequency
//PWM frq = 16M/(Prescaler * (ICR +1)) --> PWM frq = 1953.125 hz 
volatile uint8_t timerowerflow=0;

// tics/s= (16M/prescaler)
// overflows/s= (tics/s)/2^8 = 61,03....
void init_pwm() {
    
    //timer for blink at high AD Voltage
    TIMSK0 =  0b00000001;    // Enable Timer0 overflow interrupt
    TCCR0B =  0b00000101;    //  Timer0 prescaler to 1024: 0b 0 0 0 0 0 CS02 0 CS00



    DDRB = 0b00000010; // OC1A output PB1-> pin 9 on Arduino  0b 0 0 0 0 0 0 PB1 0
    
    //Fast PWM mode  non-inverted output on OC1A (PB1) 
    TCCR1A = 0b10000010;  // 0b COM1A1 0 0 0 0 0 WGM11 0 
    // COM1A1 Non-inverted PWM on OC1A, pin will be high when counter is less than OCR1A
    // WGM11 Part of Fast PWM mode configuration
    
    TCCR1B = 0b00011010;  // 0b 0 0 0 WGM13 WGM12 0 CS11 0, prescaler = 8
    // WGM13, WGM12 enable Fast PWM with ICR1 as top)
    // CS11 Prescaler 8 for Timer1
    
    ICR1 = ICR_VALUE;  //topp value for timer, the maximum ticks for one loop before timer is set to zero
    
    
    OCR1A = 102; //Start at ~10%
}

void read_AD() {
    // ADC configuration for free-running mode
    ADMUX =0b01000000; // REFS0 for AVcc reference voltage 5V, MUX bits =0000 to select ADC0
    ADCSRA = 0b11101111; // 0b ADEN ADSC ADATE 0 ADIE ADPS2 ADPS1 ADPS0 
    ADCSRB = 0b00000000; // free running mode, ADTS2=ADTS1=ADTS0=0, ACME: AIN1 = Analog Comparator Negative Input ingen aning vad detta gör när jag ändå bara har possetitiva värden, eller om man flyttar referensjord på nått sätt
    //  ADPS2 ADPS1 ADPS0 Prescaler = 128 ger ett mer stabilt värde, än de som har lägre prescaler, hur länge en kondnsator laddas upp innan värdet utvärderas. funkar int emed noll av någon anledning
    //  ADEN Enable the ADC, turn it on
    //  ADATE Enable ADC Free-Running mode
    //  ADSC Start the ADC conversion, In Free Running mode, write this bit to one to start the first conversion.
    //  ADIE Enable ADC interrupt, 1 =ADC Conversion Complete Interrupt is activated
}

ISR(ADC_vect) {
    if (timerowerflow < 61){ 
    OCR1A = (uint16_t)ADC;  // 10-bit -> 16-bit
    //since ICR1 = ICR_VALUE=(uint16_t)ADC max value
    //@ max AD value the duty will be 100% 
    //@ min AD value (0) the duty will be 0%
} else{
    OCR1A=102;
}
}

ISR(TIMER0_OVF_vect) {
    // ADC value is greater than 1000 (~4.88V)
    if (ADC > 1000) {
        timerowerflow++;  
        if (timerowerflow >= 122) {
            timerowerflow = 0; 
        }
    } else {
        timerowerflow = 0;  
    }
}

int main(void) {
    init_pwm();
    
    read_AD(); 
    
    sei();

    while (1) {}
}
