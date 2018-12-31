#include "mbed.h"
#include "stdio.h"

#define BUFFER_SIZE 512

static CircularBuffer<char, BUFFER_SIZE> ringbuffer;

void xprintf(const char *format, ...){
    char buffer[BUFFER_SIZE];

    va_list args;
	va_start(args, format);
    int written = vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    if (written < 0) {
		/* do some error handling */
		return;
	}

    if (written > BUFFER_SIZE) {
		/* There are no easy solutions to tackle this. It
		* may be easiest to enlarge
		* your BUFFER_SIZE to avoid this. */
		return; /* this is a short-cut; you may want to do something else.*/
	}

    // fill circularbuffer
    for(int i=0; i<written; i++){
        ringbuffer.push(buffer[i]);
    }

}

void printOnTerminal(){
    while(!ringbuffer.empty()){
        char c;
        ringbuffer.pop(c);
        putc(c,stdout);
    }
    fflush(stdout);
}

void printOnTerminal(Serial *pc){
    while(!ringbuffer.empty()){
        char c;
        ringbuffer.pop(c);
        pc->putc(c);
    }
    // pc->sync();
}