
#include "Radio/Radio.h"
#include "mbed.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstddef>


Radio::Radio(bool debug){
    _debug = debug;
    debugprint("Radio()");
    // SMP
    fifo_init(&fifo,buffer,sizeof(buffer));
    smp.buffer = &fifo;
    smp.frameReadyCallback = NULL;
    smp.rogueFrameCallback = 0;
    SMP_Init(&smp);

    // initialize Data Fifos
    fifo_init(&sendFifo, sendBuffer, sizeof(sendBuffer));
	fifo_init(&receiveFifo, receiveBuffer, sizeof(receiveBuffer));

}


uint32_t Radio::readData(uint8_t* data, uint32_t max_len){
    debugprint("readData()");
    return fifo_read_bytes(data, &receiveFifo, max_len);
}

/* writes data to sendfifo and returns number of written bytes */
uint32_t Radio::sendData(uint8_t* data, uint32_t len){
    debugprint("readData()");
    return fifo_write_bytes(data, &sendFifo, len);
}

int Radio::readPacket(char* data, int* len){
    return 0;
}

int Radio::sendPacket(char* data, int len){
    uint32_t txlen = SMP_Send((unsigned char*)data,len,transmitBuffer,sizeof(transmitBuffer), &messageStart);

     /* in debug mode: print input data pointer and data length in terminal */
    if(_debug){
        printf("LoraRadio::write(%p, %d)\n",data,len);
        printf("\tinput data: ");
        for(int i = 0;i<len;i++){
            printf("%c",data[i]);
        }
        printf("\n\tSMP frame:  ");
        for(uint32_t i = 0;i<txlen;i++){
            printf("%.2x",messageStart[i]);
        }
        printf("\n\n");
    }

    while(txlen>0){
        if(txlen >= TX_MAX){
            sendBytes(messageStart,TX_MAX);
            txlen -= TX_MAX;
            messageStart += TX_MAX;
        }
        else{
            // send remaining data
            sendBytes(messageStart,txlen);
            txlen -= txlen; // break;
        }
    }
    return SUCCESS;
}

void Radio::debugprint(const char* msg){
    if(_debug){
        debug_mutex.lock();
        printf("DEBUG:\t%s\n",msg);
        debug_mutex.unlock();
    }
}