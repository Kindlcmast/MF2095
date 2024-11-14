// johan kindlundh
#include <stdio.h> //printf(), fgets()
#include <stdlib.h> //atoi()
#include <string.h> //strlen()
#include <ctype.h>  //isdigit()
#include <math.h> //round()

#define LOG_SIZE 10
#define INPUT_SIZE 12 
// unsiged int max val= 4294967295 , med T och \n blir max tecken 12 dock kanske detta skulle vara 11 för att inte få int overflow men fgets läser bara storleken-1  för att få plats med \0

unsigned int min;
unsigned int max; 
unsigned int count = 0;
double mean = 0; //15 to 17 decimal digits of precision. Max double value: 1.797693e+308


void update_min_max_mean(unsigned int temp){
    if (count==1){//När count =1 så är  det första värdet 
        min=temp;
        max=temp;
    }
    if (temp>max){
        max=temp;
    }
    if(temp<min){
        min=temp;
    }
    mean= mean +((temp-mean)/count);
}

void clear_inbuffer(){
    int garbage;
    do {
        garbage = getchar();
    } while (garbage != '\n' && garbage != EOF);
}
int main (int argc, char *argv[]){
    unsigned int temperature_log[LOG_SIZE]; 
    memset(temperature_log, 0, sizeof(temperature_log)); // kanske ha ett annat defult värde, typ en int max eller nått för enklare felsökning
    char input[INPUT_SIZE];
    

while (1)
{
    if (fgets(input, sizeof(input), stdin) == NULL) { // Null ---> EOF,
    // fgets() will read only n - 1 characters to leave space for the null terminator.
    //fgets() will Add the newline character (\n) if Enter was pressed, and there is room for it.
            break;
        }
unsigned int lenght = strlen(input);
    
    if (input[lenght - 1] != '\n'){
        clear_inbuffer();
        goto err;
    }
     if (input[0] != 'T' && lenght > 2 || input[0]=='T' && lenght < 3 ) {
            goto err;
        }
    
    switch (input[0]) {
        case 'T':{            
            for (int i = 1; i < lenght - 1; i++) { // borde ändras om det handlar  om negativa och possetiva värden (alt få input i kelvin)
            //atoi does not provide any error handling. If the string contains invalid characters (like letters) before any numbers, atoi returns 0 and does not indicate an error.
                if (!isdigit(input[i])) { //https://en.cppreference.com/w/c/string/byte/isdigit
                    goto err; break;
                    }}
                if (input[1]=='0' && lenght>3){ // T09 etc  - invalid
                    goto err; break;
                }
            unsigned int temperature = atoi(&input[1]); 
            /*
            Once it encounters the first numeric character, 
            it continues reading characters until it reaches a non-numeric character or the end of the string.
            */
            
            if (count < LOG_SIZE) {
                temperature_log[count] = temperature;
                count++;
                update_min_max_mean(temperature);
                printf("Received Temperature: %d\n", temperature);

            } else {
                printf("Log Full\n");
            }

        break;
        }

        case 'A' :{
            if (count == 0) {
                    printf("Average Temperature: N/A\n");
                } else {
                    double r_mean=round(mean * 100)/100;
                    printf("Average Temperature: %.2f\n", r_mean);
                } 
            break;
            }

        case 'N' :{
           if (count == 0) {
                printf("Minimum Temperature: N/A\n");
            } else {
                printf("Minimum Temperature: %d\n", min);
            }
        break;
        }
    
        case 'X':{
            if (count == 0) {
                        printf("Maximum Temperature: N/A\n");
                    } else {
                        printf("Maximum Temperature: %d\n", max);
                    }
        break;
        }
    
        case 'L':{
            printf("Log: %d entries\n", count);
            if (count>0){
                for (int i = 0; i < count; i++) {
                printf("Temperature: %d\n", temperature_log[i]);
            }}
        break;
        }

        case 'Q':{ 
            printf("Exiting...\n");
            return 0;
        }
        
        err:
        default:{
                printf("Input Error\n");
        break;
        }
    }
}
return 0;
}