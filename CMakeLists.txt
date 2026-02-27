cmake_minimum_required(VERSION 3.10)
project(STM32_Simulator C)

set(CMAKE_C_STANDARD 11)

# Включаем директории с заголовками
include_directories(core/include)

# Добавляем исходные файлы ядра
file(GLOB CORE_SOURCES "core/src/*.c")

# Создаем библиотеку ядра (чтобы потом линковать с Go или тестами)
add_library(core_static STATIC ${CORE_SOURCES})

# Создаем исполняемый файл для отладки ядра (standalone)
add_executable(stm32_sim core/main.c)
target_link_libraries(stm32_sim core_static)

# qwen code к vs code подключить 
# router ai.ru 
# qwen coder next
