
#include "Arduino.h"

#include "babbler_config.h"

#include "babbler_serial.h"

// Размеры буферов для чтения команд и записи ответов 
#ifndef CMD_READ_BUFFER_SIZE
#define CMD_READ_BUFFER_SIZE 128
#endif

#ifndef CMD_WRITE_BUFFER_SIZE
#define CMD_WRITE_BUFFER_SIZE 512
#endif


// Буферы для обмена данными с компьютером через последовательный порт
static char read_buffer[CMD_READ_BUFFER_SIZE];
static char write_buffer[CMD_WRITE_BUFFER_SIZE];
static int write_size;

/**
 * Обработать входные данные - разобрать строку, выполнить одну или 
 * несколько команд, записать ответ
 *
 * @param buffer - входные данные, строка
 * @param buffer_size - размер входных данных
 * @param reply_buffer - ответ, строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
static int (*handle_input)(char* buffer, int buffer_size, char* reply_buffer);

/**
 * Предварительная настройка модуля канала связи последовательный порт Serial, 
 * выполнить один раз в setup.
 * 
 * @param _handle_input указатель на функцию, обрабатывающую входные данные:
 *     разбирает строку, выполняет одну или несколько команд, записывает ответ
 *     _handle_input:@param buffer - входные данные, строка
 *     _handle_input:@param buffer_size - размер входных данных
 *     _handle_input:@param reply_buffer - ответ, строка, оканчивающаяся нулём
 *     _handle_input:@return размер ответа в байтах (0, чтобы не отправлять ответ)
 * @param speed скорость передачи данных в бит/c (бод) для последовательно порта, 
 *     см Serial.begin(speed). 
 *     Допольнительное специальное значение speed=BABBLER_SERIAL_SKIP_PORT_INIT (-1) - пропустить
 *     инициализацию порта.
 */
void babbler_serial_setup(int (*_handle_input)(char* buffer, int buffer_size, char* reply_buffer),
        long speed) {
    handle_input = _handle_input;
    
    if(speed != BABBLER_SERIAL_SKIP_PORT_INIT) {
        Serial.begin(speed);
    }
}

/**
 * Постоянные задачи для канала связи последовательный порт Serial, 
 * выполнять на каждой итерации в бесконечном цикле loop
 * При получении команды, вызывает handleInput.
 */
void babbler_serial_tasks() {    
    int readSize = 0;
    int writeSize = 0;
    
    // берем из порта по одному символу до тех пор, пока есть данные
    // или пока количество символов не превысит лимит чтения на один
    // раз: размер буфера минус 1 символ (последний символ в буфере -
    // резерв на завершающий строку ноль)
    while (Serial.available() > 0 && readSize < CMD_READ_BUFFER_SIZE) {
        read_buffer[readSize] = Serial.read();
        readSize++;
        
        // хак: без этой задержки при вводе команд в окно mpide Tools/Serial monitor
        // первый символ введенной строки отделяется от остальных и воспринимается
        // этим кодом как отдельная строка
        if(readSize == 1) {
            delay(100);
        }
    }
    if(readSize > 0) {
        // Считали порцию данных
        
        // строка должна оканчиваться нулем, если специально не указано обратное
        #ifndef DONT_ADD_TERMINATING_ZERO
            read_buffer[readSize] = 0;
        #endif
            
        #ifdef DEBUG_SERIAL
            Serial.print("Read: ");
            Serial.print(read_buffer);
            Serial.print(" (size=");
            Serial.print(readSize);
            Serial.println(")");
        #endif // DEBUG_SERIAL
        
        // теперь можно выполнить команду, ответ попадет в write_buffer
        writeSize = handle_input(read_buffer, readSize, write_buffer);
        write_size = writeSize;
    }
    
    // отправляем ответ
    if(write_size > 0) {
        #ifdef DEBUG_SERIAL
            Serial.print("Write: ");
            Serial.print(write_buffer);
            Serial.print(" (size=");
            Serial.print(write_size);
            Serial.println(")");
        #endif // DEBUG_SERIAL
        
        // добавляем в конце перенос строки, если не указано обратное
        #ifndef DONT_ADD_TERMINATING_NEWLINE
            Serial.println(write_buffer);
        #else
            Serial.print(write_buffer);
        #endif
        write_size = 0;
    }
}

