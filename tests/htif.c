#include "spinlock.h"

#define false 0
#define true 1
#define NULL (void*)0
typedef int bool;

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned long uintptr_t;

#define HTIF_DATA_BITS		48
#define HTIF_DATA_MASK		((1ULL << HTIF_DATA_BITS) - 1)
#define HTIF_DATA_SHIFT		0
#define HTIF_CMD_BITS		8
#define HTIF_CMD_MASK		((1ULL << HTIF_CMD_BITS) - 1)
#define HTIF_CMD_SHIFT		48
#define HTIF_DEV_BITS		8
#define HTIF_DEV_MASK		((1ULL << HTIF_DEV_BITS) - 1)
#define HTIF_DEV_SHIFT		56

#define HTIF_DEV_SYSTEM		0
#define HTIF_DEV_CONSOLE	1

#define HTIF_CONSOLE_CMD_GETC	0
#define HTIF_CONSOLE_CMD_PUTC	1

#if __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#else
# define TOHOST_CMD(dev, cmd, payload) ({ \
  if ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

volatile uint64_t tohost __attribute__((section(".htif")));
volatile uint64_t fromhost __attribute__((section(".htif")));

static uint64_t *htif_fromhost = NULL;
static uint64_t *htif_tohost = NULL;
static bool htif_custom = false;

static int htif_console_buf;

static inline uint64_t __read_tohost(void)
{
	return (htif_custom) ? *htif_tohost : tohost;
}

static inline void __write_tohost(uint64_t val)
{
	if (htif_custom)
		*htif_tohost = val;
	else
		tohost = val;
}

static inline uint64_t __read_fromhost(void)
{
	return (htif_custom) ? *htif_fromhost : fromhost;
}

static inline void __write_fromhost(uint64_t val)
{
	if (htif_custom)
		*htif_fromhost = val;
	else
		fromhost = val;
}

static void __check_fromhost()
{
	uint64_t fh = __read_fromhost();
	if (!fh)
		return;
	__write_fromhost(0);

	/* this should be from the console */
	if (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	switch (FROMHOST_CMD(fh)) {
		case HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			break;
		case HTIF_CONSOLE_CMD_PUTC:
			break;
		default:
			__builtin_trap();
	}
}

static void __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	while (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

#if __riscv_xlen == 32
spinlock_t htif_lock;

static void do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	while (1) {
		uint64_t fh = fromhost;
		if (fh) {
			if (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				fromhost = 0;
				break;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

void htif_putc(char ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	volatile uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uintptr_t)magic_mem);
}
#else
void htif_putc(char ch)
{
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
}
#endif

void _putchar(char ch) {
	htif_putc(ch);
}

void htif_poweroff(int exit_code)
{
	while (__read_tohost())
		__check_fromhost();

	while(1) {
		fromhost = 0;
		tohost = 1 | ((exit_code &= 0xff) << 1);
	}
}
