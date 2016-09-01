#include "babbler.h"
#include "babbler_cmd_core.h"
#include "babbler_protocol.h"
#include "babbler_serial.h"

#define LED_PIN 13

/** Реализация команды ledon (включить лампочку) */
/** ledon (turn led ON) command implementation */
int cmd_ledon(char* reply_buffer, int argc=0, char *argv[]=NULL) {
    digitalWrite(LED_PIN, HIGH);
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** Реализация команды ledoff (включить лампочку) */
/** ledoff (turn led OFF) command implementation */
int cmd_ledoff(char* reply_buffer, int argc=0, char *argv[]=NULL) {
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
    //return handle_command(buffer, reply_buffer);
    //return handle_command(buffer, reply_buffer, &wrap_reply_json);
    return handle_command(buffer, reply_buffer, &wrap_reply_xml);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler-powered device, type help for list of commands");
    
    pinMode(LED_PIN, OUTPUT);
    
    //babbler_serial_setup(handle_input, 9600);
    babbler_serial_setup(handle_input, BABBLER_SERIAL_SKIP_PORT_INIT);
}

void loop() {
    // постоянно следим за последовательным портом, ждем входные данные
    // monitor serial port for input data
    babbler_serial_tasks();
}

