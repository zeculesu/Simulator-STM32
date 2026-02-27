#include "execute.h"
#include <stdio.h>
#include <stdint.h>

// Вспомогательная функция: извлечь биты из числа
static inline uint32_t get_bits(uint32_t value, int start, int end) {
    uint32_t mask = (1U << (end - start + 1)) - 1;
    return (value >> start) & mask;
}

// Вспомогательная функция: проверка знака и расширение
static inline int32_t sign_extend(uint32_t value, int bits) {
    uint32_t sign_bit = 1U << (bits - 1);
    if (value & sign_bit) {
        return value | (~0U << bits); // Заполняем единицами старшие биты
    }
    return value;
}

void simulator_step(Simulator *sim) {
    CPU_State *cpu = &sim->cpu;
    Memory *mem = &sim->mem;

    // 1. FETCH: Считываем 16-битную инструкцию по текущему PC
    uint16_t instr = memory_read_halfword(mem, cpu->pc);
    
    // Увеличиваем PC (в Thumb режиме PC увеличивается на 2 после чтения)
    // Важно: В реальном ARM PC при чтении может быть смещен, но для простоты считаем текущий PC точным.
    uint32_t current_pc = cpu->pc;
    cpu->pc += 2;

    // 2. DECODE & EXECUTE
    // Определяем тип инструкции по старшим битам (грубая классификация)
    
    // NOP: 0x46C0 (MOV R8, R8 часто кодируется как NOP в Thumb, или специальный 0xBF00)
    // Для простоты сейчас реализуем только:
    // MOV Rd, #imm8    : 001 00 Rd(3) imm8(8)   -> Op = 00100
    // ADD Rd, Rn, Rm   : 000 1100 Rm(3) Rn(3) Rd(3)
    // SUBS Rd, Rn, Rm  : 000 1101 Rm(3) Rn(3) Rd(3)
    // B   offset       : 111 00 offset(11)

    uint16_t opcode = get_bits(instr, 11, 15);

    switch (opcode) {
        case 0b00100: { // MOV Rd, #imm8 (синтаксис: MOVS Rd, #imm8)
            uint32_t rd = get_bits(instr, 8, 10);
            uint32_t imm8 = get_bits(instr, 0, 7);
            cpu->regs[rd] = imm8;
            printf("[EXEC] MOV R%u, #0x%02X -> R%u = 0x%08X\n", rd, imm8, rd, cpu->regs[rd]);
            break;
        }

        case 0b00011: { // Group: ADD/SUB register
            uint16_t sub_op = get_bits(instr, 9, 10);
            uint32_t rm = get_bits(instr, 6, 8);
            uint32_t rn = get_bits(instr, 3, 5);
            uint32_t rd = get_bits(instr, 0, 2);
            
            if (sub_op == 0b00) { // ADD Rd, Rn, Rm
                cpu->regs[rd] = cpu->regs[rn] + cpu->regs[rm];
                printf("[EXEC] ADD R%u, R%u, R%u -> R%u = 0x%08X\n", rd, rn, rm, rd, cpu->regs[rd]);
            } else if (sub_op == 0b01) { // SUB Rd, Rn, Rm
                cpu->regs[rd] = cpu->regs[rn] - cpu->regs[rm];
                printf("[EXEC] SUB R%u, R%u, R%u -> R%u = 0x%08X\n", rd, rn, rm, rd, cpu->regs[rd]);
            } else {
                printf("[EXEC] Unknown ADD/SUB variant: 0x%04X\n", instr);
            }
            break;
        }

        case 0b11100: { // Unconditional Branch (B)
            int32_t offset = sign_extend(get_bits(instr, 0, 10), 11);
            // Смещение умножается на 2 (выравнивание по полуслову)
            // PC уже указывает на PC+2 (инструкция + 2 байта)
            // Целевой адрес = PC + 4 + (offset * 2)
            // Но в архитектуре ARM PC при исполнении обычно PC = текущ + 4
            // В Thumb: Branch offset adds to (PC + 4) aligned to word.
            // Упрощенная логика для симулятора:
            // Мы уже увеличили PC на 2. В спецификации сказано: Branch target = Align(PC, 4) + 4 + (imm11 << 1).
            // Давайте сделаем проще: откатываемся на текущий адрес (PC-2), добавляем 4 и смещение.
            
            uint32_t target = (current_pc + 4) + (offset << 1);
            // Выравнивание по границе слова (маскируем бит 1)
            target &= ~0x2U; 

            printf("[EXEC] B 0x%08X (Offset: %d)\n", target, offset);
            cpu->pc = target;
            break;
        }

        default:
            printf("[EXEC] Unknown Instruction: 0x%04X at PC 0x%08X\n", instr, current_pc);
            // Для безопасности остановим симуляцию при встрече неизвестной инструкции
            // В реальном коде здесь будет исключение UsageFault
            cpu->pc = 0xFFFFFFFF; // Невероятный адрес для останова
            break;
    }
}

void simulator_run(Simulator *sim) {
    while (sim->cpu.pc < FLASH_BASE_ADDR + FLASH_SIZE) {
        // Простая защита от зацикливания
        static int max_steps = 20; 
        if (max_steps-- <= 0) {
            printf("[SIM] Step limit reached. Stopping.\n");
            break;
        }
        simulator_step(sim);
    }
}