#include "babbler_serial.h"

// Размеры буферов для чтения команд и записи ответов
// Read and write buffer size for communication modules
#define SERIAL_READ_BUFFER_SIZE 128
#define SERIAL_WRITE_BUFFER_SIZE 512

// Буферы для обмена данными с компьютером через последовательный порт.
// +1 байт в конце для завершающего нуля
// Data exchange buffers to communicate with computer via serial port.
// +1 extra byte at the end for terminating zero
char serial_read_buffer[SERIAL_READ_BUFFER_SIZE+1];
char serial_write_buffer[SERIAL_WRITE_BUFFER_SIZE];

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
 */
int handle_input(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size) {
    // добавим к входным данным завершающий ноль, 
    // чтобы рассматривать их как корректную строку
    // lets add finishing zero to have valid zero-terminated string
    input_buffer[input_len] = 0;
    
    // как-нибудь отреагируем на запрос - пусть будет простое эхо
    // react somehow for request - let it be simple echo
    if(reply_buf_size > input_len + 10)
        sprintf(reply_buffer, "you say: %s\n", input_buffer);
    else
        sprintf(reply_buffer, "you are too verbose, dear\n");
    
    return strlen(reply_buffer);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler-powered device, type something to have a talk");
    
    babbler_serial_set_input_handler(handle_input);
    //babbler_serial_setup(
    //    serial_read_buffer, SERIAL_READ_BUFFER_SIZE,
    //    serial_write_buffer, SERIAL_WRITE_BUFFER_SIZE,
    //    9600);
    babbler_serial_setup(
        serial_read_buffer, SERIAL_READ_BUFFER_SIZE,
        serial_write_buffer, SERIAL_WRITE_BUFFER_SIZE,
        BABBLER_SERIAL_SKIP_PORT_INIT);
}

void loop() {
    // постоянно следим за последовательным портом, ждем входные данные
    // monitor serial port for input data
    babbler_serial_tasks();
}

