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

signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
{
    int32_t len = fifo_datasize(buffer);
    printf("SMP-Frame received!!!\n\t");
    for(int i = 0; i<len; i++){
        uint8_t ch;
        fifo_read_byte(&ch,buffer);
        printf("%c",ch);
    }
    printf("\n\n");
    return len;
}

Thread radioThread;
RFM98W radio(PB_5, PB_4, PB_3, PB_10, PA_8, PA_0, 10, smp_frameReady, NULL, false);

void radioTask(){
    while(true){
        radio.serviceRadio();
        wait(1);
    }
}

Serial pc(USBTX, USBRX, 115200);

int main(){
    
    printf("Lora Bodenstation V0.1\nGerald Ebmer 2018\n\n");    

    char msg[] = "Hello World!\n";
    uint8_t rxmsg[256];
    radioThread.start(radioTask);

    while(true){
        wait(3);
        
        // printf("transmit data\n");
        // radio.sendPacket(msg,sizeof(msg));

        
        // radio.sendData((uint8_t*)msg,sizeof(msg));
        
        // printf("received msg: \t");
        // uint32_t len = radio.readData(rxmsg,256);
        // for(uint32_t i = 0; i<len; i++){
        //     printf("%c",rxmsg[i]);
        // }
        // printf("\n\n");

        // radio.sendPacket(msg,sizeof(msg));
        // printf("Packet send\n");
    }
}