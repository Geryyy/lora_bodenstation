#include "mbed.h"
#include "RFM98W.h"
#include "Radio.h"
#include "logprintf.h"

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
signed char smp_rogueframeReady(fifo_t* buffer);
void serialRadioTunnel(void);
void debugTask(void);

// Serial debugpc(PC_10,PC_5,115200);
// Serial pc(USBTX, USBRX, 115200);

/* debug: change serials */
Serial pc(PC_10,PC_5,115200);
Serial debugpc(USBTX, USBRX, 115200);


// Thread LEDdriverThread(osPriorityNormal, OS_STACK_SIZE,NULL,"LEDdriverThread");
Thread radioThread(osPriorityNormal, OS_STACK_SIZE,NULL,"Radio Thread");
Thread transmitThread(osPriorityNormal, OS_STACK_SIZE,NULL,"Transmit Thread");
Thread debugthread(osPriorityNormal, OS_STACK_SIZE,NULL,"Debug Thread");;

RFM98W radiophy(PB_5, PB_4, PB_3, PB_10, PA_8, PA_0, 2, false);


// RFM98W radiophy(PB_15, PB_14, PB_13, PB_12, PC_6, PC_7, 0, false);
Radio radio(smp_frameReady,smp_rogueframeReady,&radiophy, Radio::host, true);

signed char smp_rogueframeReady(fifo_t* buffer){
     xprintf("\n--> rogue SMP-Frame received!!!\n");
}

signed char smp_frameReady(fifo_t* buffer) //Frame wurde empfangen
{
#ifdef SMP
    int32_t len = fifo_datasize(buffer);
    xprintf("\n--> SMP-Frame received!!!\n");
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


CircularBuffer<char,64> txfifo;

void radioTask(){
    static float radioTZyklus = 0.5;
    while(true){
        // radio.serviceRadio();
        radio.run(radioTZyklus);
        wait(radioTZyklus);
    }
}

int main(){
    
    printf("Lora Bodenstation V0.1\nGerald Ebmer 2018\n\n");    
    radioThread.start(radioTask);
    transmitThread.start(serialRadioTunnel);
    debugthread.start(debugTask);
    // serialRadioTunnel();
    uint8_t c;
    while(true){
        if(pc.readable() == true){
            // txfifo.push(pc.getc());
            c = (uint8_t)pc.getc();
            // xprintf("input: %d\n",c);
            radio.sendData(&c,1);
        }
        // wait_ms(10);

        /* for debugging */
        // wait(0.01);
        // printOnTerminal(&debugpc); // display log output (xprint)
    }
}

void debugTask(){

    while(true){
        printOnTerminal(&debugpc);
        wait_ms(10);
    }
}


#define PACKET_LENGTH 24

void radiotransmit(char* msg, int len){
#ifdef SMP
        radio.sendPacket(msg,len);
#else
        radio.sendData((uint8_t*)msg,len);
#endif
}

void serialRadioTunnel(){
   static char msg[PACKET_LENGTH];
   static Timer t;

   while(true){
       if(!txfifo.empty()){
           int i = 0;
           while(!txfifo.empty() && i<PACKET_LENGTH){
               txfifo.pop(msg[i]);
               i++;
           }
           radiotransmit(msg,i);
       }
       wait(0.1);
   }
}