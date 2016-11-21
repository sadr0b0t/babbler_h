#ifndef BABBLER_SERIAL_H
#define BABBLER_SERIAL_H

#define BABBLER_SERIAL_SKIP_PORT_INIT -1

#include "babbler_io.h"

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
void babbler_serial_set_packet_filter(packet_filter is_packet);

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
 */
void babbler_serial_set_input_handler(input_handler handle_input);

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
        long speed);

/**
 * Постоянные задачи для канала связи последовательный порт Serial, 
 * выполнять на каждой итерации в бесконечном цикле loop
 * При получении команды вызывает функцию handle_input, указатель
 * на которую передан в babbler_serial_setup.
 */
void babbler_serial_tasks();

#endif // BABBLER_SERIAL_H

