#include "hal.h"

static volatile uint32_t jiffy = 0;
static int fps = 0;
static int nr_draw = 0;
#define NR_INTR_PER_MS 10
//100 hz = 周期为0.01秒  1 ms = 0.001s 每0.01s中断到来一次, 1 ms = 10 jiffy
void incr_nr_draw(void)
{
	nr_draw++;
}

int get_fps()
{
	return fps;
}

void timer_event(void)
{
	jiffy++;
	if (jiffy % (HZ / 2) == 0)
	{
		fps = nr_draw * 2 + 1;
		nr_draw = 0;
	}
}

uint32_t SDL_GetTicks()
{
	/* TODO: Return the time in millisecond. */
	//assert(0);
	return NR_INTR_PER_MS * jiffy;
}

void SDL_Delay(uint32_t ms)
{
	/* TODO: Return from this function after waiting for `ms' milliseconds. */
	uint32_t startJ = jiffy;
	while( jiffy * NR_INTR_PER_MS < startJ * NR_INTR_PER_MS +  ms);
	//assert(0);
}
