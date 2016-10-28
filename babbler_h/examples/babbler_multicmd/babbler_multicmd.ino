#include "babbler.h"
#include "babbler_cmd_core.h"
#include "babbler_cmd_devinfo.h"
#include "babbler_simple.h"
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



extern const char* DEVICE_NAME = "Robot Car";
extern const char* DEVICE_MODEL = "model2";
extern const char* DEVICE_SERIAL_NUBMER = "00000003";
extern const char* DEVICE_DESCRIPTION = "Programmable arduino-based wheeled robot";
extern const char* DEVICE_VERSION = "2.3-devel";
extern const char* DEVICE_MANUFACTURER = "Sad robot";
extern const char* DEVICE_URI = "sadrobot.su";

/** Зарегистрированные команды */
extern const babbler_cmd_t BABBLER_COMMANDS[] = {
    CMD_HELP,
    CMD_PING,
    CMD_NAME,
    CMD_MODEL,
    CMD_DESCRIPTION,
    CMD_VERSION,
    CMD_MANUFACTURER,
    CMD_URI
};

/** Количество зарегистрированных команд */
extern const int BABBLER_COMMANDS_COUNT = sizeof(BABBLER_COMMANDS)/sizeof(babbler_cmd_t);

/** Руководства для зарегистрированных команд */
/** Manuals for registered commands */
extern const babbler_man_t BABBLER_MANUALS[] = {
    MAN_HELP,
    MAN_PING,
    MAN_NAME,
    MAN_MODEL,
    MAN_DESCRIPTION,
    MAN_VERSION,
    MAN_MANUFACTURER,
    MAN_URI
};

/** Количество руководств для зарегистрированных команд */
/** Number of manuals for registered commands */
extern const int BABBLER_MANUALS_COUNT = sizeof(BABBLER_MANUALS)/sizeof(babbler_man_t);

/**
 * Вычленить из входного буфера команды, разделенные специальным символом separator 
 * (по умолчанию - точка с запятой ";"); выполнить команды одну за одной, 
 * результаты выполнения сохранить и вернуть в одной строке в порядке выполнения
 * в reply_buffer, также разделенные символом separator (по умолчанию - точка с запятой ";").
 * 
 * Например:
 * Вход: name;ping;model
 * Результат: Anton's Rraptor;ok;Rraptor
 * @param input_buffer - команды, разделенные симоволом separator, массив байт (строка или двоичный)
 * @param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере)
 */
int handle_multicmd(char* input_buffer, char* reply_buffer, int reply_buf_size,
        const char* separator=";") {
    // обнулим ответ
    reply_buffer[0] = 0;
    
    char cmd_buffer[SERIAL_READ_BUFFER_SIZE];
    char cmd_reply_buffer[SERIAL_WRITE_BUFFER_SIZE];
        
    // Разобьем входящую строку на куски по разделителю команд ';'
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    // первая команда
    token = strtok_r(input_buffer, separator, &last);
    bool firstToken = true;
    while(token != NULL) {
        strcpy(cmd_buffer, token);
        handle_command_simple(cmd_buffer, cmd_reply_buffer, reply_buf_size);
        
        // добавлять к ответу предваряющий разделитель ';' для всех команд,
        // кроме первой
        if(!firstToken) {
            strcat(reply_buffer, separator);
        } else {
            firstToken = false;
        }
        
        // добавить ответ от команды в общий список
        // TODO: сделать что-нибудь в случае, если все ответы не умещаются
        // в буфер (это касается и разных других мест, но здесь особенно,
        // т.к. размер ответа от нескольких команд - динамический)
        strcat(reply_buffer, cmd_reply_buffer);
        
        // следующая команда
        token = strtok_r(NULL, ";", &last);
    }
  
    return strlen(reply_buffer);
}

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
 *    -1: ошибка при формировании ответа (не хватило места в буфере)
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
 *    -1: error while constructing reply (not enought space in reply_buffer)
 */
int handle_input(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size) {
    // "распакуем" пакет: добавим завершающий ноль, срежем перевод строки (если есть)
    // (места в буфере для дополнительного нуля точно хватит, т.к. даже если длина входных данных
    // input_len равна размеру буфера SERIAL_READ_BUFFER_SIZE, у нас все равно в буфере есть 
    // один лишний байт)
    // "unpack" package: add terminating zero, cut newline at the end (if present)
    // (we have enough space for terminating zero in buffer, because even if input data length
    // input_len is equal to input buffer size SERIAL_READ_BUFFER_SIZE, we still have
    // one extra byte in the buffer)
    unpack_input_as_str(input_buffer, input_len, true);
    
    // выполняем команду (reply_buf_size-2 - место для переноса строки и завершающего нуля)
    // execute command (reply_buf_size-2 - place for newline and terminating zero)
    int reply_len = handle_multicmd(input_buffer, reply_buffer, reply_buf_size);
    
    // проверить на ошибку
    // check for error
    if(reply_len < 0) {
        reply_len = write_reply_error(reply_buffer, reply_len, reply_buf_size-2);
    }
    
    // "упаковать" пакет для отправки - добавить перенос строки
    // "pack" reply to send - add newline at the end
    reply_len = pack_reply_newline(reply_buffer, reply_len, reply_buf_size);
    
    return reply_len;
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler-powered device, type help for list of commands");
    
    babbler_serial_set_packet_filter(packet_filter_newline);
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

