#include <stdio.h>
#include "memory.h"
#include "cpu_state.h"

int main() {
    Memory mem;
    CPU_State cpu;

    printf("Initializing Simulator...\n");

    if (!memory_init(&mem)) {
        return 1;
    }
    printf("Memory initialized: Flash=%dKB, SRAM=%dKB\n", 
           FLASH_SIZE/1024, SRAM_SIZE/1024);

    cpu_reset(&cpu);
    printf("CPU State reset.\n");

    // Тест записи в SRAM
    memory_write_byte(&mem, 0x20000000, 0xAB);
    uint8_t val = memory_read_byte(&mem, 0x20000000);
    printf("Test SRAM access: Wrote 0xAB, Read 0x%02X\n", val);

    memory_free(&mem);
    printf("Done.\n");
    return 0;
}