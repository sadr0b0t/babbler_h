#include "babbler.h"
#include "babbler_simple.h"
#include "babbler_cmd_core.h"
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

#define LED_PIN 13

/** Реализация команды ledon (включить лампочку) */
/** ledon (turn led ON) command implementation */
int cmd_ledon(char* reply_buffer, int reply_buf_size, int argc=0, char *argv[]=NULL) {
    digitalWrite(LED_PIN, HIGH);
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** Реализация команды ledoff (включить лампочку) */
/** ledoff (turn led OFF) command implementation */
int cmd_ledoff(char* reply_buffer, int reply_buf_size, int argc=0, char *argv[]=NULL) {
    digitalWrite(LED_PIN, LOW);
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

babbler_cmd_t CMD_LEDON = {
    /* имя команды */ 
    /* command name */
    "ledon",
    /* указатель на функцию с реализацией команды */ 
    /* pointer to function with command implementation*/ 
    &cmd_ledon
};

babbler_man_t MAN_LEDON = {
    /* имя команды */ 
    /* command name */
    "ledon",
    /* краткое описание */ 
    /* short description */
    "turn led ON",
    /* руководство */ 
    /* manual */
    "SYNOPSIS\n"
    "    ledon\n"
    "DESCRIPTION\n"
    "Turn led ON."
};

babbler_cmd_t CMD_LEDOFF = {
    /* имя команды */ 
    /* command name */
    "ledoff",
    /* указатель на функцию с реализацией команды */ 
    /* pointer to function with command implementation*/ 
    &cmd_ledoff
};

babbler_man_t MAN_LEDOFF = {
    /* имя команды */ 
    /* command name */
    "ledoff",
    /* краткое описание */ 
    /* short description */
    "turn led OFF",
    /* руководство */ 
    /* manual */
    "SYNOPSIS\n"
    "    ledoff\n"
    "DESCRIPTION\n"
    "Turn led OFF."
};

/** Зарегистрированные команды */
/** Registered commands */
extern const babbler_cmd_t BABBLER_COMMANDS[] = {
    // команды из babbler_cmd_core.h
    // commands from babbler_cmd.core.h
    CMD_HELP,
    CMD_PING,
    
    // пользовательские команды
    // custom commands
    CMD_LEDON,
    CMD_LEDOFF
};

/** Количество зарегистрированных команд */
/** Number of registered commands*/
extern const int BABBLER_COMMANDS_COUNT = sizeof(BABBLER_COMMANDS)/sizeof(babbler_cmd_t);


/** Руководства для зарегистрированных команд */
/** Manuals for registered commands */
extern const babbler_man_t BABBLER_MANUALS[] = {
    // команды из babbler_cmd_core.h
    // commands from babbler_cmd.core.h
    MAN_HELP,
    MAN_PING,
    
    // пользовательские команды
    // custom commands
    MAN_LEDON,
    MAN_LEDOFF
};

/** Количество руководств для зарегистрированных команд */
/** Number of manuals for registered commands */
extern const int BABBLER_MANUALS_COUNT = sizeof(BABBLER_MANUALS)/sizeof(babbler_man_t);




/**
 * Обернуть ответ в формат XML вида:
 * <cmd_reply><cmd>cmd_name</cmd><reply>reply_value</reply></cmd_reply>
 * 
 * здесь значения
 *     cmd_name - имя команды
 *     reply_value - ответ выполненной команды (исходное содержимое reply_buffer)
 *
 * Новое значение перезаписывается в reply_buffer, его размера должно достаточно,
 * чтобы вместить новую строку.
 *
 * @param cmd - имя команды
 * @param argc - количество параметров команды
 * @param argv - массив с параметрами команды
 * @param reply_buffer символьный буфер для записи ответа - 
 *     результата выполнения команды
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @param argc количество аргументов (по умолчанию может быть 0)
 * @param argv значения аргументов (по умолчанию может быть NULL),
 *     если задано, 1й аргумент - имя команды
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
/**
 * Wrap reply to XML like:
 * <cmd_reply><cmd>cmd_name</cmd><reply>reply_value</reply></cmd_reply>
 * 
 * values here:
 *     cmd_name - command name
 *     reply_value - command reply (initial value of reply_buffer)
 *
 * New value is written to reply_buffer, its size should be enough for new value.
 *
 * @param cmd - command name
 * @param argc - number of command parameters
 * @param argv - array with command parameters
 * @param reply_buffer - reply buffer, byte array (string or binary)
 * @param reply_buf_size - size of reply_buffer buffer - maximum length of reply.
 *     Function implementation should take care of reply length not exceeding
 *     maximum reply buffer size.
 * @return length of reply in bytes or error code
 *     >0, <=reply_buf_size: number of bytes, written to reply_buffer
 *     0: don't send reply
 *    -1: error while constructing reply (not enought space in reply_buffer, dynamic memory error etc)
 */
int wrap_reply_xml(char* cmd, int argc, char* argv[], char* reply_buffer, int reply_buf_size) {
    // TODO: наверное, чтобы поэкономить память, можно не создавать новый временный буфер,
    // а сдвинуть содержимое старого reply_buffer на нужное количество символов вправо
    // и добавить нужные данные слева и справа
    char cmd_reply_buffer[SERIAL_WRITE_BUFFER_SIZE];
    
    sprintf(cmd_reply_buffer, 
        "<cmd_reply><cmd>%s</cmd><reply>%s</reply></cmd_reply>", 
        cmd, reply_buffer);
    
    strcpy(reply_buffer, cmd_reply_buffer);
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
    int reply_len = handle_command_simple(input_buffer, reply_buffer, reply_buf_size-2, wrap_reply_xml);
    
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
    
    
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // постоянно следим за последовательным портом, ждем входные данные
    // monitor serial port for input data
    babbler_serial_tasks();
}

