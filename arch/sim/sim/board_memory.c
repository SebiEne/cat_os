/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/* The heap memory definition */

static uint8_t g_heap_memory[CONFIG_SIM_HEAP_SIZE];

/****************************************************************************
 * Public Variabless
 ****************************************************************************/

/* Normally these definitions are exported from the linker script but in this
 * target we are not using a linker script.
 */

unsigned long _sbss;
unsigned long _ebss;

unsigned long _sheap = (unsigned long)&g_heap_memory[0];
unsigned long _eheap = (unsigned long)&g_heap_memory[CONFIG_SIM_HEAP_SIZE];
