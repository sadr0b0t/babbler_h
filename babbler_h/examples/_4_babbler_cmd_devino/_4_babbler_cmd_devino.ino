#include "babbler.h"
#include "babbler_simple.h"
#include "babbler_cmd_core.h"
#include "babbler_cmd_devinfo.h"
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


extern const char* DEVICE_NAME = "Babbler demo";
extern const char* DEVICE_MODEL = "prototype1";
extern const char* DEVICE_SERIAL_NUBMER = "00000003";
extern const char* DEVICE_DESCRIPTION = "Communicative Arduino-based device powered by Babbler communication library";
extern const char* DEVICE_VERSION = "0.1-devel";
extern const char* DEVICE_MANUFACTURER = "Sad robot";
extern const char* DEVICE_URI = "sadrobot.su";

#define LED_PIN 13
bool ledison = false;

/** Реализация команды ledon (включить лампочку) */
/** ledon (turn led ON) command implementation */
int cmd_ledon(char* reply_buffer, int reply_buf_size, int argc=0, char *argv[]=NULL) {
    digitalWrite(LED_PIN, HIGH);
    ledison = true;
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** Реализация команды ledoff (включить лампочку) */
/** ledoff (turn led OFF) command implementation */
int cmd_ledoff(char* reply_buffer, int reply_buf_size, int argc=0, char *argv[]=NULL) {
    digitalWrite(LED_PIN, LOW);
    ledison = false;
    
    // команда выполнена
    strcpy(reply_buffer, REPLY_OK);
    return strlen(reply_buffer);
}

/** Реализация команды ledstatus (cтатус лампочки) */
/** ledstatus (get led status) command implementation */
int cmd_ledstatus(char* reply_buffer, int reply_buf_size, int argc=0, char *argv[]=NULL) {
    if(ledison) {
        strcpy(reply_buffer, "on");
    } else {
        strcpy(reply_buffer, "off");
    }
    
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

babbler_cmd_t CMD_LEDSTATUS = {
    /* имя команды */
    /* command name */
    "ledstatus",
    /* указатель на функцию с реализацией команды */
    /* pointer to function with command implementation*/
    &cmd_ledstatus
};

babbler_man_t MAN_LEDSTATUS = {
    /* имя команды */
    /* command name */
    "ledstatus",
    /* краткое описание */
    /* short description */
    "get led status: on/off",
    /* руководство */
    /* manual */
    "SYNOPSIS\n"
    "    ledstatus\n"
    "DESCRIPTION\n"
    "Get led status: on/off."
};

/** Зарегистрированные команды */
/** Registered commands */
extern const babbler_cmd_t BABBLER_COMMANDS[] = {
    // команды из babbler_cmd_core.h
    // commands from babbler_cmd.core.h
    CMD_HELP,
    CMD_PING,
    
    // команды из babbler_cmd_devinfo.h
    // commands from babbler_cmd.devinfo.h
    CMD_NAME,
    CMD_MODEL,
    CMD_DESCRIPTION,
    CMD_VERSION,
    CMD_MANUFACTURER,
    CMD_URI,
    
    // пользовательские команды
    // custom commands
    CMD_LEDON,
    CMD_LEDOFF,
    CMD_LEDSTATUS
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
    
    // команды из babbler_cmd_devinfo.h
    // commands from babbler_cmd.devinfo.h
    MAN_NAME,
    MAN_MODEL,
    MAN_DESCRIPTION,
    MAN_VERSION,
    MAN_MANUFACTURER,
    MAN_URI,
    
    // пользовательские команды
    // custom commands
    MAN_LEDON,
    MAN_LEDOFF,
    MAN_LEDSTATUS
};

/** Количество руководств для зарегистрированных команд */
/** Number of manuals for registered commands */
extern const int BABBLER_MANUALS_COUNT = sizeof(BABBLER_MANUALS)/sizeof(babbler_man_t);


void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler-powered device, type help for list of commands");
    
    babbler_serial_set_packet_filter(packet_filter_newline);
    babbler_serial_set_input_handler(handle_input_simple);
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

