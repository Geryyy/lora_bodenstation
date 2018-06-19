#include "mbed.h"
#include "RFM98W.h"

/* Pinbelegung */
/*
---SPI3---
SCK     PB_3
MISO    PB_4
MOSI    PB_5
---GPIOS---
CS      PB_10
RESET   PA_8
INT     PA_0
*/


int main(){
    RFM98W radio(PB_5, PB_4, PB_3, PB_10, PA_8, PA_0);
    
    printf("Lora Bodenstation V0.1\nGerald Ebmer 2018\n\n");    

    char msg[] = "Hello World!\n";

    while(true){
        wait(1);
        radio.sendPacket(msg,sizeof(msg));
        printf("Packet send\n");
    }
}