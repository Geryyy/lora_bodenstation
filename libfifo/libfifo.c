/*
 * @file libfifo.c
 *
 * Basic functions for circular buffer fifo structures
 *
 *  Created on: 19.04.2016
 *      Author: Peter
 */

#include "libfifo.h"
#ifdef BINARY_FIFO
#include "math.h"
#endif

// ===== DEFINITIONS =====

//======Private Functions======
static uint16_t getFifoPtr(uint16_t current, int32_t offset,const fifo_t* fifo)
{
    int32_t newPtr = current + offset;
    if(newPtr < 0)
    {
        return newPtr + fifo->buffersize;
    }
    else
        return newPtr % fifo->buffersize;
}

static uint32_t min(uint32_t number1, uint32_t number2)
{
    if(number1 <= number2)
        return number1;
    else
        return number2;
}

//======Public Functions========

/**
 * @brief initialize fifo structure
 */
uint8_t fifo_init(fifo_t* fifo, uint8_t* buffer, uint16_t buffersize)
{
    fifo->buffer = buffer;
    fifo->buffersize = buffersize;
    fifo_clear(fifo);
    return 0;
}

/**
* @brief Clear the fifo
* Deletes all data in the fifo
*/
void fifo_clear(fifo_t* fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

/**
 * @brief check if fifo ist empty
 * returns 1 if fifo is empty, 0 otherwise.
 */
uint8_t fifo_empty(const fifo_t* fifo)
{
    if (fifo->head == fifo->tail)
        return 1;
    return 0;
}

/**
 * @brief check if fifo is full
 *
 * Returns 1 if fifo is full, 0 otherwise.
 */
uint8_t fifo_full(const fifo_t* fifo)
{
    if (getFifoPtr(fifo->tail,1,fifo) == fifo->head)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief get bytes in the fifo
 *
 * Returns number of bytes in the fifo.
 */
uint32_t fifo_datasize(const fifo_t* fifo)
{
    int64_t size;
    size = fifo->tail - fifo->head;
    if (size < 0)
    {
        size += fifo->buffersize;
    }
    return (uint32_t) size;
}

/**
* @brief Returns the number of saved objects in the fifo
*/
uint32_t fifo_saved_objects(const fifo_t* fifo, uint32_t objectSize)
{
    return fifo_datasize(fifo)/objectSize;
}

/**
 * @brief get free space of fifo
 *
 * Returns number of free bytes.
 */
uint32_t fifo_free_space(const fifo_t* fifo)
{
    return fifo->buffersize - fifo_datasize(fifo) - 1;
}

/**
 * @brief get free space of fifo
 *
 * Returns number of free objects (n * objectsize) in the fifo.
 */
uint32_t fifo_free_objects(const fifo_t* fifo, uint32_t objectSize)
{
    return fifo_free_space(fifo)/objectSize;
}

/**
 * @brief deletes first object in the fifo
 *
 * Returns 1 if object has been removed, 0 if no object has been removed
 */
uint8_t fifo_delete_object(fifo_t* fifo, uint32_t objectSize)
{
    if (!fifo_empty(fifo))
    {
        fifo->head = getFifoPtr(fifo->head,objectSize,fifo);
        return 1;
    }
    return 0;
}

/**
 * @brief deletes n objects in the fifo
 *
 * Returns the number of removed objects
 */
uint8_t fifo_delete_n_Objects(uint32_t number, fifo_t* fifo, uint32_t objectSize)
{
    uint32_t i;
    for (i = 0; i < number; i++)
    {
        if (!fifo_delete_object(fifo,objectSize))
        {
            break;
        }
    }

    return i;
}

void fifo_delete_bytes(uint32_t number, fifo_t* fifo)
{
	if(number == 0)
		return;

	number = min(number,fifo_datasize(fifo));

	fifo->head = getFifoPtr(fifo->head,number,fifo);

	if (fifo->tail == fifo->head)
		fifo->tail = fifo->head = 0;
}

/**
* @brief reads bytes from the fifo but doesn't delete them
* Reads number bytes from the fifo and returns the number of bytes that had been read.
* This function just reads the data and doesn't delete it, so fifo pointers are not changed.
*/
uint32_t fifo_peak_bytes(uint8_t *buffer,const fifo_t* fifo, uint32_t number)
{
	number = min(number, fifo_datasize(fifo));

    unsigned int l;

    /*
     * first get the data from fifo->head until the end of the buffer
     */
    l = min(number, fifo->buffersize - getFifoPtr(fifo->head,0,fifo));
    MEMCPYFUNCTION(buffer, fifo->buffer + getFifoPtr(fifo->head,0,fifo), l);

    /*
     * then get the rest (if any) from the beginning of the buffer
     */
    MEMCPYFUNCTION(buffer + l, fifo->buffer, number - l);

	return number;
}

/**
 * @brief read n objects from fifo with offset, but do net delete them
 *
 * Read arbitrary number of objects from fifo with a specified offset.
 * Returns the number of objects successfully read.
 * If offset + number is greater than number of bytes in the fifo this function returns 0.
 * This function does not delete the objects from the fifo, so pointers are not changed
 */
uint32_t fifo_peak_bytes_offset(uint8_t *buffer, uint32_t offset, const fifo_t* fifo, uint32_t number)
{
    fifo_t tmpFifo;
    uint32_t datasize = fifo_datasize(fifo);
    if(datasize < (offset + number))
        return 0;
    tmpFifo = *fifo;
    tmpFifo.head = getFifoPtr(tmpFifo.head,offset,&tmpFifo);
    return fifo_peak_bytes(buffer,&tmpFifo,number);
}

/**
 * @brief read n objects from fifo with offset, but do net delete them
 *
 * Read arbitrary number of objects from fifo with a specified offset.
 * Returns the number of objects successfully read.
 * This function does not delete the objects from the fifo, so pointers are not changed
 */
uint32_t fifo_peak_offset(void *buffer, uint32_t offset, const fifo_t* fifo, uint32_t number, uint32_t objectSize)
{
    return fifo_peak_bytes_offset(buffer,offset * objectSize,fifo,number * objectSize);
}

/**
* @brief reads bytes from the fifo
* Reads number bytes from the fifo and returns the number of bytes that had been read
*/
uint32_t fifo_read_bytes(uint8_t* buffer, fifo_t* fifo, uint32_t number)
{
	number = fifo_peak_bytes(buffer,fifo,number);
    fifo_delete_bytes(number,fifo);

    return number;
}

/************************************************************************/
/* @brief read one byte to the fifo                                     */
/* This function reads one data byte from the fifo and returns 1 if the */
/* operation was successful, 0 otherwise                                */
/* @return unint8_t														*/
/************************************************************************/
uint8_t fifo_read_byte(uint8_t* var, fifo_t* fifo)
{
	if(fifo_empty(fifo))
		return 0;

	*var = fifo->buffer[fifo->head];
	fifo->head = getFifoPtr(fifo->head,1,fifo);
	return 1;
}

/**
 * @brief read n objects from fifo, but do net delete them
 *
 * Read arbitrary number of objects from fifo.
 * Returns the number of objects successfully read.
 * This function does not delete the objects from the fifo, so pointers are not changed
 */
uint32_t fifo_peak(void *buffer, uint32_t number,const fifo_t* fifo, uint32_t objectSize)
{
    uint32_t readedBytes = fifo_peak_bytes(buffer,fifo,objectSize*number);
    return readedBytes/objectSize;
}

/**
 * @brief read n objects from fifo
 *
 * Read arbitrary number of objects from fifo.
 * Returns the number of objects successfully read.
 */
uint32_t fifo_read(void *buffer, uint32_t number, fifo_t* fifo, uint32_t objectSize)
{
    uint32_t readedBytes = fifo_read_bytes(buffer,fifo,objectSize*number);
    return readedBytes/objectSize;
}


/**
* @brief write bytes to the fifo
* Writes number bytes to the fifo and returns the number of bytes that had been written
*/
uint32_t fifo_write_bytes(const uint8_t* buffer, fifo_t* fifo, uint32_t number)
{
    unsigned int l;

	if(fifo_full(fifo))
		return 0;

    number = min(number, fifo_free_space(fifo));

    /* first put the data starting from fifo->in to buffer end */
    l = min(number, fifo->buffersize - getFifoPtr(fifo->tail,0,fifo));
    MEMCPYFUNCTION(fifo->buffer + getFifoPtr(fifo->tail,0,fifo), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    MEMCPYFUNCTION(fifo->buffer, buffer + l, number - l);

     fifo->tail = getFifoPtr(fifo->tail,number,fifo);

    return number;
}

/************************************************************************/
/* @brief write one byte to the fifo                                    */
/* This function writes one data byte into the fifo and returns 1 if the*/
/* operation was successful, 0 otherwise                                */
/* @return unint8_t														*/
/************************************************************************/
uint8_t fifo_write_byte(uint8_t var, fifo_t* fifo)
{
	if(fifo_full(fifo))
		return 0;

	fifo->buffer[fifo->tail] = var;
	fifo->tail = getFifoPtr(fifo->tail,1,fifo);
	return 1;
}

/**
 * @brief write n objects in fifo
 *
 * Write arbitrary number of objects into fifo.
 *
 * Returns the number of objects successfully written.
 */
uint32_t fifo_write(const void* buffer, uint32_t number, fifo_t* fifo, uint32_t objectSize)
{
    uint32_t writtenBytes = fifo_write_bytes(buffer,fifo,number * objectSize);
    return writtenBytes/objectSize;
}
