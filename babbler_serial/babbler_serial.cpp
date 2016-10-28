
#include "Arduino.h"

#include "babbler_lib_config.h"

#include "babbler_serial.h"
#include "babbler_io.h"

// Буферы для обмена данными с компьютером через последовательный порт
// +1 байт в конце для завершающего нуля
static char* _read_buffer;
static int _read_buffer_size;
static char* _write_buffer;
static int _write_buffer_size;

static int _write_size;

/** см module:babbler_io.h~packet_filter */
// указатель на функцию - фильтр пакетов
static packet_filter _is_packet;

// указатель на функцию - обработчик входящих пакетов
/** см module:babbler_io.h~input_handler */
static input_handler _handle_input;

/**
 * Настроить фильтр пакетов.
 * @param {module:babbler_io.h~packet_filter} is_packet - указатель на функцию, 
 *       которая определяет, является ли содержимое буфера пакетом.
 *     is_packet:@param input - входные данные
 *     is_packet:@param input_len - размер данных в буфере
 *     is_packet:@return 
 *         true - буфер содержит корректный пакет
 *         false - содержимое буфера не является корректным пакетом
 */
void babbler_serial_set_packet_filter(packet_filter is_packet) {
    _is_packet = is_packet;
}

/**
 * Настроить обработчик пакетов входных данных.
 * @param {module:babbler_io.h~input_handler} handle_input - указатель на функцию - обрабатчик входных данных:
 *       разбирает строку, выполняет одну или несколько команд, записывает ответ.
 *     handle_input:@param input_buffer - входные данные, массив байт (строка или двоичный)
 *     handle_input:@param input_len - размер входных данных
 *     handle_input:@param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 *     handle_input:@param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *         Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *         максимальный размер буфера
 *     handle_input:@return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере)
 */
void babbler_serial_set_input_handler(input_handler handle_input) {
    _handle_input = handle_input;
}

/**
 * Предварительная настройка модуля канала связи - последовательный порт Serial, 
 * выполнить один раз в setup.
 * @param speed скорость передачи данных в бит/c (бод) для последовательно порта, 
 *     см Serial.begin(speed). 
 *     Допольнительное специальное значение speed=BABBLER_SERIAL_SKIP_PORT_INIT (-1) - пропустить
 *     инициализацию порта.
 */
void babbler_serial_setup(
        char* read_buffer, int read_buffer_size,
        char* write_buffer, int write_buffer_size,
        long speed) {
    _read_buffer = read_buffer;
    _read_buffer_size = read_buffer_size;
    _write_buffer = write_buffer;
    _write_buffer_size = write_buffer_size;
    
    if(speed != BABBLER_SERIAL_SKIP_PORT_INIT) {
        Serial.begin(speed);
    }
}

/**
 * Постоянные задачи для канала связи последовательный порт Serial, 
 * выполнять на каждой итерации в бесконечном цикле loop
 * При получении команды вызывает функцию handle_input, указатель
 * на которую передан в babbler_serial_setup.
 */
void babbler_serial_tasks() {
    int readSize = 0;
    int writeSize = 0;
    
    // берем из порта по одному символу до тех пор, пока 
    // - есть данные
    // - или фильтр пакетов не определит пакет,
    // - или пока количество символов не превысит лимит чтения на один
    // раз (размер буфера)
    while((!_is_packet || !_is_packet(_read_buffer, readSize)) && 
            Serial.available() > 0 && readSize < _read_buffer_size) {
        _read_buffer[readSize] = Serial.read();
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
        
        #ifdef DEBUG_SERIAL
            Serial.print("Read: ");
            Serial.print(read_buffer);
            Serial.print(" (size=");
            Serial.print(readSize);
            Serial.println(")");
        #endif // DEBUG_SERIAL
        
        // теперь можно выполнить команду, ответ попадет в write_buffer
        writeSize = _handle_input(_read_buffer, readSize, _write_buffer, _write_buffer_size);
        _write_size = writeSize;
    }
    
    // отправляем ответ
    if(_write_size > 0) {
        #ifdef DEBUG_SERIAL
            Serial.print("Write: ");
            Serial.print(_write_buffer);
            Serial.print(" (size=");
            Serial.print(write_size);
            Serial.println(")");
        #endif // DEBUG_SERIAL
        
        // пишем данные
        Serial.write(_write_buffer, _write_size);
        _write_size = 0;
    }
}

