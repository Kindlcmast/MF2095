#include <stdio.h>
#include <stdlib.h> // atoi(), sscanf()
#include <string.h> // memset()
#include <ctype.h>  // isdigit()
#include <math.h>   // round()

#define LOG_SIZE 10

unsigned int min;
unsigned int max; 
unsigned int count = 0;
double mean = 0;

void update_min_max_mean(unsigned int temp) {
    if (count == 1) {
        min = temp;
        max = temp;
    }
    if (temp > max) {
        max = temp;
    }
    if (temp < min) {
        min = temp;
    }
    mean = mean + ((temp - mean) / count);
}

void clear_inbuffer() {
    int garbage;
    do {
        garbage = getchar();
    } while (garbage != '\n' && garbage != EOF);
}

int main(int argc, char *argv[]) {
    unsigned int temperature_log[LOG_SIZE];
    memset(temperature_log, 0, sizeof(temperature_log));
    unsigned int value;
    char hex[11]; 

    while (1) {
        scanf("%10s", hex); // 10 hex digits
        if (sscanf(hex, "%x", &value) != 1) { //The input operation failed. // reaching EOF, or if no input was given
            printf("Input Error\n");
            clear_inbuffer();
            continue;
        
        }
        int carr;

        if (carr=getchar() != '\n') {clear_inbuffer(); goto err;}

        if (value <= 0x03FFFFFF) {  // Valid temperature 0x00000000 to 0x03FFFFFF
            unsigned int temperature = value;
                if (count < LOG_SIZE) {
                    temperature_log[count] = temperature;
                    count++;
                    update_min_max_mean(temperature);
                    printf("Received Temperature: %d\n", temperature);
                    continue;
                }else {
                printf("Log Full\n");
                continue;}
        } else {
        

        switch (value) {
            case 0x08000000: {  
                if (count == 0) {
                    printf("Average Temperature: N/A\n");
                } else {
                    double r_mean = round(mean * 100) / 100;
                    printf("Average Temperature: %.2f\n", r_mean);
                }
                break;
            }

            case 0x0C000000: { 
                if (count == 0) {
                    printf("Minimum Temperature: N/A\n");
                } else {
                    printf("Minimum Temperature: %d\n", min);
                }
                break;
            }

            case 0x10000000: {  
                if (count == 0) {
                    printf("Maximum Temperature: N/A\n");
                } else {
                    printf("Maximum Temperature: %d\n", max);
                }
                break;
            }

            case 0x14000000: {  
                printf("Log: %d entries\n", count);
                if (count > 0) {
                    for (int i = 0; i < count; i++) {
                        printf("Temperature: %d\n", temperature_log[i]);
                    }
                }
                break;
            }

            case 0x18000000: { 
                printf("Exiting...\n");
                return 0;
            }
            err:
            default: {
                 printf("Input Error\n");
                break;
            }
        }
    }}

    return 0;
}
