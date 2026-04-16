#ifndef BABBLER_IO_H
#define BABBLER_IO_H

// Определения общих интерфейсов для модулей ввода-вывода Babbler.


#define REPLY_BUF_ERROR -1

/**
 * Фильтр пакетов. Определяет, является ли содержимое буфера пакетом.
 * @param input - входные данные
 * @param input_len - размер данных в буфере
 * @return 
 *     true - буфер содержит корректный пакет
 *     false - содержимое буфера не является корректным пакетом
 */
typedef bool (*packet_filter)(char* input, int input_len);

/**
 * Обработать входные данные: разобрать строку, выполнить одну или 
 * несколько команд, записать ответ.
 * @param input_buffer - входные данные, массив байт (строка или двоичный)
 * @param input_len - размер входных данных
 * @param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
/**
 * Handle input data: parse string, run one or multiple commands, 
 * write reply.
 * @param input_buffer - input data, byte array (string or binary)
 * @param input_len - input data length
 * @param reply_buffer - reply buffer, byte array (string or binary)
 * @param reply_buf_size - size of reply_buffer buffer - maximum length of reply.
 *     Function implementation should take care of reply length not exceeding
 *     maximum reply buffer size.
 * @return length of reply in bytes or error code
 *     >0, <=reply_buf_size: number of bytes, written to reply_buffer
 *     0: don't send reply
 *    -1: error while constructing reply (not enought space in reply_buffer, dynamic memory error etc)
 */
typedef int (*input_handler)(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size);

#endif // BABBLER_IO_H

