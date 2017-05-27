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


/** Реализация команды pin_mode (установить режим пина: INPUT/OUTPUT) */
/** pin_mode (set pin mode: INPUT/OUTPUT) command implementation */
int cmd_pin_mode(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    bool paramsOk = true;
    bool success = false;
    
    // ожидаемые параметры
    // argv[0] - имя команды "pin_mode" (первый параметр всегда имя команды)
    // argv[1] - номер пина
    // argv[2] - режим пина: OUTPUT/INPUT
    // expected params
    // argv[0] - cmd name: "pin_mode" (1st param is always command name)
    // argv[1] - pin number
    // argv[2] - pin mode: INPUT/OUTPUT
    if(argc == 3) {
        // распознать номер пина из 2го параметра
        // parse pin number from 2nd param
        char* pinStr = argv[1];
        int pin = atoi(pinStr);
        
        if(pin > 0 || (pin == 0 && strcmp("0", pinStr) == 0)) {
            // с номером пина все ок,
            // распознать режим INPUT/OUTPUT из 3го параметра
            // pin number is ok,
            // parse mode INPUT/OUTPUT from the 3rd param 
            char* modeStr = argv[2];
            if(strcmp("OUTPUT", modeStr) == 0) {
                // выполнить команду
                // execute command
                pinMode(pin, OUTPUT);
                success = true;
            } else if(strcmp("INPUT", modeStr) == 0) {
                // выполнить команду
                // execute command
                pinMode(pin, INPUT);
                success = true;
            } else {
                paramsOk = false;
            }
        } else {
            paramsOk = false;
        }
    } else {
        paramsOk = false;
    }
    
    // запишем ответ
    // write reply
    if(success) {
        // с параметрами все в порядке, команда выполнена
        // params ok, command executed
        strcpy(reply_buffer, REPLY_OK);
    } else if(!paramsOk) {
        // команда не выполнена, проблема с параметрами
        // command not executed with bad params error
        strcpy(reply_buffer, REPLY_BAD_PARAMS);
    } else {
        // команда не выполнена, какая-то другая ошибка
        // command not executed with some other error
        strcpy(reply_buffer, REPLY_ERROR);
    }
    
    return strlen(reply_buffer);
}

/** Реализация команды digital_write (записать значение HIGH/LOW в порт) */
/** digital_write (write value HIGH/LOW to port) command implementation */
int cmd_digital_write(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    bool paramsOk = true;
    bool success = false;
    
    // ожидаемые параметры
    // argv[0] - имя команды "digital_write" (первый параметр всегда имя команды)
    // argv[1] - номер пина
    // argv[2] - значение: HIGH/1/LOW/0
    // expected params
    // argv[0] - cmd name: "digital_write" (1st param is always command name)
    // argv[1] - pin number
    // argv[2] - value: HIGH/1/LOW/0
    if(argc == 3) {
        // распознать номер пина из 2го параметра
        // parse pin number from 2nd param
        char* pinStr = argv[1];
        int pin = atoi(pinStr);
        
        if(pin > 0 || (pin == 0 && strcmp("0", pinStr) == 0)) {
            // с номером пина все ок,
            // распознать значение HIGH/1/LOW/0 из 3го параметра
            // pin number is ok,
            // parse value HIGH/1/LOW/0 from the 3rd param
            char* valStr = argv[2];
            if(strcmp("HIGH", valStr) == 0 || strcmp("1", valStr) == 0) {
                // выполнить команду
                // execute command
                digitalWrite(pin, HIGH);
                success = true;
            } else if(strcmp("LOW", valStr) == 0 || strcmp("0", valStr) == 0) {
                // выполнить команду
                // execute command
                digitalWrite(pin, LOW);
                success = true;
            } else {
                paramsOk = false;
            }
        } else {
            paramsOk = false;
        }
    } else {
        paramsOk = false;
    }
    
    // запишем ответ
    // write reply
    if(success) {
        // с параметрами все в порядке, команда выполнена
        // params ok, command executed
        strcpy(reply_buffer, REPLY_OK);
    } else if(!paramsOk) {
        // команда не выполнена, проблема с параметрами
        // command not executed with bad params error
        strcpy(reply_buffer, REPLY_BAD_PARAMS);
    } else {
        // команда не выполнена, какая-то другая ошибка
        // command not executed with some other error
        strcpy(reply_buffer, REPLY_ERROR);
    }
    
    return strlen(reply_buffer);
}

babbler_cmd_t CMD_PIN_MODE = {
    /* имя команды */
    /* command name */
    "pin_mode",
    /* указатель на функцию с реализацией команды */
    /* pointer to function with command implementation*/
    &cmd_pin_mode
};

babbler_man_t MAN_PIN_MODE = {
    /* имя команды */
    /* command name */
    "pin_mode",
    /* краткое описание */
    /* short description */
    "set pin mode: INPUT/OUTPUT",
    /* руководство */ 
    /* manual */
    "SYNOPSIS\n"
    "    pin_mode pin mode\n"
    "DESCRIPTION\n"
    "Set i/o pin mode to INPUT/OUTPUT.\n"
    "OPTIONS\n"
    "    pin - i/o pin number\n"
    "    mode - pin mode: INPUT or OUTPUT"
};

babbler_cmd_t CMD_DIGITAL_WRITE = {
    /* имя команды */
    /* command name */
    "digital_write",
    /* указатель на функцию с реализацией команды */
    /* pointer to function with command implementation*/
    &cmd_digital_write
};

babbler_man_t MAN_DIGITAL_WRITE = {
    /* имя команды */
    /* command name */
    "digital_write",
    /* краткое описание */
    /* short description */
    "write digital value HIGH/LOW (1/0) to output port",
    /* подробное описание */
    /* detailed description */
    "SYNOPSIS\n"
    "    digital_write pin val\n"
    "DESCRIPTION\n"
    "Write digital value HIGH/LOW (1/0) to output port.\n"
    "OPTIONS\n"
    "    pin - pin number\n"
    "    val - value to write: 0/LOW for LOW or 1/HIGH for HIGH"
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
    CMD_PIN_MODE,
    CMD_DIGITAL_WRITE
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
    MAN_PIN_MODE,
    MAN_DIGITAL_WRITE
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
}

void loop() {
    // постоянно следим за последовательным портом, ждем входные данные
    // monitor serial port for input data
    babbler_serial_tasks();
}

