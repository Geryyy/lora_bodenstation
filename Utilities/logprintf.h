#ifndef LOGPRINTF_H
#define LOGPRINTF_H

#define LOG(x, ...) \
	{ xprintf("\x1b[34m%12.12s: \x1b[39m"x"\x1b[39;49m\r\n", \
	MODULE_NAME, ##__VA_ARGS__); }
#define WARN(x, ...) \
	{ xprintf("\x1b[34m%12.12s: \x1b[33m"x"\x1b[39;49m\r\n", \
	MODULE_NAME, ##__VA_ARGS__); }

void xprintf(const char *format, ...);
void printOnTerminal();

#endif //LOGPRINTF_H