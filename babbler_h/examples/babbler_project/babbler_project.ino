#include "babbler.h"
#include "babbler_simple.h"
#include "babbler_cmd_core.h"

// включить отладку через последовательный порт
// enable serial port debug messages
//#define DEBUG_SERIAL

// Включить модуль коммуникации через последовательный порт Serial
#define BABBLER_SERIAL

// для последовательного порта Serial
#ifdef BABBLER_SERIAL
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

#endif // BABBLER_SERIAL


/** Зарегистрированные команды */
/** Registered commands */
extern const babbler_cmd_t BABBLER_COMMANDS[] = {
    // команды из babbler_cmd_core.h
    // commands from babbler_cmd.core.h
    CMD_HELP,
    CMD_PING
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
    MAN_PING
};

/** Количество руководств для зарегистрированных команд */
/** Number of manuals for registered commands */
extern const int BABBLER_MANUALS_COUNT = sizeof(BABBLER_MANUALS)/sizeof(babbler_man_t);


/**
 * Предварительная настройка модулей каналов связи, 
 * выполнить один раз в setup
 */
void babbler_setup() {
    // Модули связи
    #ifdef BABBLER_SERIAL
        babbler_serial_set_packet_filter(packet_filter_newline);
        babbler_serial_set_input_handler(handle_input_simple);
    
        #ifdef DEBUG_SERIAL
            Serial.println("Enable Babbler Serial communication module");
            babbler_serial_setup(
                serial_read_buffer, SERIAL_READ_BUFFER_SIZE,
                serial_write_buffer, SERIAL_WRITE_BUFFER_SIZE,
                BABBLER_SERIAL_SKIP_PORT_INIT);
        #else
            babbler_serial_setup(
                serial_read_buffer, SERIAL_READ_BUFFER_SIZE,
                serial_write_buffer, SERIAL_WRITE_BUFFER_SIZE,
                9600);
        #endif // DEBUG_SERIAL
    #endif // BABBLER_SERIAL
}

/**
 * Постоянные задачи для каналов связи, выполнять на каждой итерации 
 * в бесконечном цикле loop
 */
void babbler_tasks(){
    #ifdef BABBLER_SERIAL
        // постоянно следим за последовательным портом, ждем входные данные
        // monitor serial port for input data
        babbler_serial_tasks();
    #endif // BABBLER_SERIAL
}

void setup() {
    #ifdef DEBUG_SERIAL
        Serial.begin(9600);
        Serial.println("Starting babbler-powered device, type help for list of commands");
    #endif
    
    babbler_setup();
}

void loop() {
    babbler_tasks();
}

