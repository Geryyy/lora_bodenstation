/* @file: radio.h
 * @author: Gerald Ebmer
 * @date: 13.06.2018
 * @brief: header file for radio class
 */

#include "libsmp.h"
#include "libfifo.h"

#define TX_MAX (127) 


class Radio{

public:
    Radio();
    virtual int serviceRadio() = 0; // rein virtuelle Methode

    int readPacket(char* data, int* len);
    int sendPacket(char* data, int len);

private:
    bool debug;

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