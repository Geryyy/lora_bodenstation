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

signed char smp_frameReady(fifo_t* buffer);
void serialRadioTunnel(void);

Serial pc(USBTX, USBRX, 115200);
Thread radioThread;
RFM98W radio(PB_5, PB_4, PB_3, PB_10, PA_8, PA_0, 2, smp_frameReady, NULL, false);

signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
{
#ifdef SMP
    int32_t len = fifo_datasize(buffer);
    // printf("SMP-Frame received!!!\n\t");
    for(int i = 0; i<len; i++){
        uint8_t ch;
        fifo_read_byte(&ch,buffer);
        pc.putc(ch);
    }
#else
    uint8_t data[LORA_PACKET_LENGTH];
    radio.readData(data,LORA_PACKET_LENGTH);
    int32_t len = LORA_PACKET_LENGTH;

    for(int i = 0; i<LORA_PACKET_LENGTH; i++){
        pc.putc(data[i]);
    }
#endif

    return len;
}


void radioTask(){
    while(true){
        radio.serviceRadio();
        wait(0.2);
    }
}



int main(){
    printf("Lora Bodenstation V0.1\nGerald Ebmer 2018\n\n");    
    radioThread.start(radioTask);
    serialRadioTunnel();

    while(true){
        // shoud not reach this point
    }
}


#define PACKET_LENGTH 24

void serialRadioTunnel(){
    static time_t time1;
    time_t TIMEOUT = 3;
    char msg[PACKET_LENGTH];
    static int i = 0;

    time1 = time(NULL);

    while(true){
        if(pc.readable() == true){
            msg[i] = pc.getc();
            if(i==0){
                time1 = time(NULL);
            }
            i++;
        }

        if(i>=PACKET_LENGTH){
#ifdef SMP
            radio.sendPacket(msg,PACKET_LENGTH);
#else
            radio.sendData((uint8_t*)msg,PACKET_LENGTH);
#endif
            i = 0;
            time1 = time(NULL);
            // printf("full packet\n");
        }

        if( ((time(NULL)-time1) > TIMEOUT) && (i > 0) ){
#ifdef SMP
            radio.sendPacket(msg,i);
#else
            radio.sendData((uint8_t*)msg,i);
#endif
            i = 0;
            time1 = time(NULL);
            // printf("Timeout\n");
        }
    }
}