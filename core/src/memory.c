#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

bool memory_init(Memory *mem) {
    mem->flash = (uint8_t *)calloc(FLASH_SIZE, sizeof(uint8_t));
    if (!mem->flash) {
        perror("Failed to allocate Flash memory");
        return false;
    }

    mem->sram = (uint8_t *)calloc(SRAM_SIZE, sizeof(uint8_t));
    if (!mem->sram) {
        perror("Failed to allocate SRAM memory");
        free(mem->flash);
        return false;
    }

    return true;
}

void memory_free(Memory *mem) {
    if (mem->flash) free(mem->flash);
    if (mem->sram) free(mem->sram);
    mem->flash = NULL;
    mem->sram = NULL;
}

uint8_t memory_read_byte(Memory *mem, uint32_t addr) {
    if (addr >= FLASH_BASE_ADDR && addr < (FLASH_BASE_ADDR + FLASH_SIZE)) {
        return mem->flash[addr - FLASH_BASE_ADDR];
    }
    if (addr >= SRAM_BASE_ADDR && addr < (SRAM_BASE_ADDR + SRAM_SIZE)) {
        return mem->sram[addr - SRAM_BASE_ADDR];
    }
    // В будущем здесь будет периферия
    return 0xFF; // Ошибка доступа
}

void memory_write_byte(Memory *mem, uint32_t addr, uint8_t value) {
    if (addr >= SRAM_BASE_ADDR && addr < (SRAM_BASE_ADDR + SRAM_SIZE)) {
        mem->sram[addr - SRAM_BASE_ADDR] = value;
        return;
    }
    // Flash запись обычно игнорируется при выполнении (нужна отдельная логика прошивки)
}