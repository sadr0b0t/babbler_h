#include "babbler.h"
#include "babbler_cmd_core.h"
#include "babbler_cmd_devinfo.h"
#include "babbler_protocol.h"

#include "babbler_serial.h"

#include "babbler_config.h"

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

/**
 * Обработать входные данные - разобрать строку, выполнить одну или 
 * несколько команд, записать ответ
 *
 * @param buffer - входные данные, строка
 * @param buffer_size - размер входных данных
 * @param reply_buffer - ответ, строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
int handle_input(char* buffer, int buffer_size, char* reply_buffer) {
    // строка должна оканчиваться нулем
    buffer[buffer_size] = 0;
    return handle_input_multicmd(buffer, buffer_size, reply_buffer, &handle_command);
    //return handle_input_multicmd(buffer, buffer_size, reply_buffer, &handle_command, &wrap_reply_json);
    //return handle_input_multicmd(buffer, buffer_size, reply_buffer, &handle_command, &wrap_reply_xml);
    //return handle_input_multicmd(buffer, buffer_size, reply_buffer, &handle_command, &wrap_reply_newline);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler_h test...");
    
    babbler_serial_setup(handle_input, BABBLER_SERIAL_SKIP_PORT_INIT);
    //babbler_serial_setup(handle_input, 9600);
}

void loop() {
    babbler_serial_tasks();
}

