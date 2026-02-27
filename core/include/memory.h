#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include "memory_map.h"

typedef struct {
    uint8_t *flash;
    uint8_t *sram;
} Memory;

// Инициализация памяти (выделение массивов)
bool memory_init(Memory *mem);

// Освобождение памяти
void memory_free(Memory *mem);

// Чтение байта (для будущего fetch)
uint8_t memory_read_byte(Memory *mem, uint32_t addr);

// Запись байта (для будущего выполнения инструкций)
void memory_write_byte(Memory *mem, uint32_t addr, uint8_t value);

uint16_t memory_read_halfword(Memory *mem, uint32_t addr);

// Чтение 32-битного слова (для периферии)
uint32_t memory_read_word(Memory *mem, uint32_t addr);

#endif // MEMORY_H