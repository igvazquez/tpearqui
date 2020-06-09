//syscallDispatcher.c

#include <stdint.h>
#include <timerTick.h>
#include <videoDriver.h>
#include <screenDriver.h>
#include <keyboardDriver.h>
#include <rtcDriver.h>

//Funcion encargada de llamar a la funcion asociada a la systemCall llamada y pasarle
// los parametros correctos.
uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx)
{
	switch (rdi)
	{
	case 0:
		//int ticks_elapsed();
		return ticks_elapsed();

	case 1:
		//void printStringf( char * string, unsigned int font, unsigned int background);
		printStringf((char *)rsi, rdx, rcx);
		break;
	case 2:
		//char getKey();
		return getKey();
	case 3:
		//void setCursorPos(unsigned int x, unsigned int y);
		setCursorPos(rsi, rdx);
		break;
	case 4:
		//int verPixelCount();
		return getScreenHeight();
	case 5:
		//int horPixelCount()
		return getScreenWidth();
	case 6:
		//uint8_t getTime(uint64_t timeDescriptor)
		return getTime(rsi);
	case 7:
		// void setSize(int size);
		setSize(rsi);
		break;
	case 8:
		//int setScreen(unsigned int id);
		setScreen(rsi);
		break;
	case 9:
		//extern char *cpuVendor( char *result);
		return cpuVendor(rsi);
	}
	return 0;
}