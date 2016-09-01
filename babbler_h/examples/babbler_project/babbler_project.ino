#include "babbler.h"
#include "babbler_cmd_core.h"

// включить отладку через последовательный порт
// enable serial port debug messages
//#define DEBUG_SERIAL

// Включить модуль коммуникации через последовательный порт Serial
#define BABBLER_SERIAL

// для последовательного порта Serial
#ifdef BABBLER_SERIAL
#include "babbler_serial.h"
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
 * Обработать входные данные: разобрать строку, выполнить одну или 
 * несколько команд, записать ответ
 *
 * @param buffer - входные данные, строка
 * @param buffer_size - размер входных данных
 * @param reply_buffer - ответ, строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
 /**
 * Handle input data: parse string, run one or multiple commands, 
 * write reply.
 *
 * @param buffer - input data, zero-terminated string
 * @param buffer_size - number of input bytes
 * @param reply_buffer - reply, zero-terminated string
 * @return size of reply in bytes (0 for no reply)
 */
int handle_input(char* buffer, int buffer_size, char* reply_buffer) {
    return handle_command(buffer, reply_buffer);
}

/**
 * Предварительная настройка модулей каналов связи, 
 * выполнить один раз в setup
 */
void babbler_setup() {
    // Модули связи
    #ifdef BABBLER_SERIAL
        #ifdef DEBUG_SERIAL
            Serial.println("Enable Babbler Serial communication module");
            babbler_serial_setup(handle_input, BABBLER_SERIAL_SKIP_PORT_INIT);
        #else
            babbler_serial_setup(handle_input, 9600);
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

