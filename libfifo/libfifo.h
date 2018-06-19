/*
 * @file libfifo.h
 *
 * Includes basic structures to use with circular buffer fifo structures
 *
 *  Created on: 19.04.2016
 *  Author: Peter
 */

#ifndef LIBFIFO_H_
#define LIBFIFO_H_

 #ifdef __cplusplus
extern "C"
{
#endif

// ===== INCLUDES =====
#include <stdint.h>
#include <string.h>

//#define BINARY_FIFO
#define GET_BUFFER_SIZE(realsize) (realsize + 1)

#define MEMCPYFUNCTION(dest,src,length) memcpy(dest,src,length)

// ===== STRUCTS =====

typedef struct stc_fifo_s
{
	uint8_t *buffer;
	uint32_t head;
	uint32_t tail;
	uint32_t buffersize;
} fifo_t;

// ===== DECLARATIONS =====

uint8_t fifo_init(fifo_t* fifo, uint8_t* buffer, uint16_t buffersize);
void fifo_clear(fifo_t* fifo);
uint8_t fifo_empty(const fifo_t* fifo);
uint8_t fifo_full(const fifo_t* fifo);
uint32_t fifo_datasize(const fifo_t* fifo);
uint32_t fifo_saved_objects(const fifo_t* fifo, uint32_t objectSize);
uint32_t fifo_free_space(const fifo_t* fifo);
uint32_t fifo_free_objects(const fifo_t* fifo, uint32_t objectSize);
void* fifo_get_nth_Object(uint32_t number,const fifo_t* fifo, uint32_t objectSize);
uint8_t fifo_delete_object(fifo_t* fifo, uint32_t objectSize);
uint8_t fifo_delete_n_Objects(uint32_t number, fifo_t* fifo, uint32_t objectSize);
void fifo_delete_bytes(uint32_t number, fifo_t* fifo);
uint32_t fifo_peak_bytes(uint8_t *buffer,const fifo_t* fifo, uint32_t number);
uint32_t fifo_read_bytes(uint8_t* buffer, fifo_t* fifo, uint32_t number);
uint8_t fifo_read_byte(uint8_t* var, fifo_t* fifo);
uint32_t fifo_peak(void *buffer, uint32_t number,const fifo_t* fifo, uint32_t objectSize);
uint32_t fifo_read(void *buffer, uint32_t number, fifo_t* fifo, uint32_t objectSize);
uint32_t fifo_write_bytes(const uint8_t* buffer, fifo_t* fifo, uint32_t number);
uint8_t fifo_write_byte(uint8_t var, fifo_t* fifo);
uint32_t fifo_write(const void* buffer, uint32_t number, fifo_t* fifo, uint32_t objectSize);

 #ifdef __cplusplus
}
#endif

#endif
