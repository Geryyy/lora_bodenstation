/* @file: radio.h
 * @author: Gerald Ebmer
 * @date: 13.06.2018
 * @brief: header file for radio class
 */

#include "mbed.h"
#include "libsmp.h"
#include "libfifo.h"
#include "globals.h"

#define TX_MAX (127) 


class Radio{

public:
    Radio(bool debug);
    virtual int serviceRadio() = 0; // rein virtuelle Methode

    uint32_t readData(uint8_t* data, uint32_t max_len);
    uint32_t sendData(uint8_t* data, uint32_t len);

    int readPacket(char* data, int* len);
    int sendPacket(char* data, int len);

    fifo_t sendFifo;
    fifo_t receiveFifo;

    void debugprint(const char* msg);

protected:
    bool _debug;
    Mutex debug_mutex;

private:
    uint8_t sendBuffer[GET_BUFFER_SIZE(LORA_PACKET_LENGTH * 10)];
    uint8_t receiveBuffer[GET_BUFFER_SIZE(LORA_PACKET_LENGTH * 10)];

    virtual int sendBytes(unsigned char *data, int len) = 0; // rein virtuelle Methode

    // SMP
    unsigned char buffer[100];
    fifo_t fifo;
    smp_struct_t smp;
    unsigned int frameLength; //Länge des SMP Frames
    unsigned char transmitBuffer[SMP_SEND_BUFFER_LENGTH(1000)]; 
    unsigned char* messageStart; //Pointer auf den Start der Nachricht im transmitBuffer
    signed char (*smp_frameReady)(fifo_t* buffer); //Frame wurde empfangen

};