#include "babbler_cmd_devinfo.h"
#include "babbler.h"

#include"string.h"

// Значения свойств устройства (следует определить в главной программе)
extern const char* DEVICE_NAME;
extern const char* DEVICE_MODEL;
extern const char* DEVICE_SERIAL_NUMBER;
extern const char* DEVICE_DESCRIPTION;
extern const char* DEVICE_VERSION;
extern const char* DEVICE_MANUFACTURER;
extern const char* DEVICE_URI;


extern const babbler_cmd_t CMD_NAME = {
    "name",
    &cmd_name
};
extern const babbler_man_t MAN_NAME = {
    "name",
    "get device name", 
    "SYNOPSIS" 
    "    name" 
    "DESCRIPTION" 
    "Get device name."
};

extern const babbler_cmd_t CMD_MODEL = {
    "model",
    &cmd_model
};
extern const babbler_man_t MAN_MODEL = {
    "model",
    "get device model",
    "SYNOPSIS" 
    "    model" 
    "DESCRIPTION" 
    "Get device model."
};

extern const babbler_cmd_t CMD_SERIAL_NUMBER = {
    "serial_number",
    &cmd_serial_number
};
extern const babbler_man_t MAN_SERIAL_NUMBER = {
    "serial_number",
    "get device serial number",
    "SYNOPSIS" 
    "    serial_number" 
    "DESCRIPTION" 
    "Get device serial number."
};

extern const babbler_cmd_t CMD_DESCRIPTION = {
    "description",
    &cmd_description
};
extern const babbler_man_t MAN_DESCRIPTION = {
    "description", 
    "get device description",
    "SYNOPSIS" 
    "    description" 
    "DESCRIPTION" 
    "Get device description"
};

extern const babbler_cmd_t CMD_VERSION = {
    "version",
    &cmd_version
};
extern const babbler_man_t MAN_VERSION = {
    "version",
    "get device version",
    "SYNOPSIS" 
    "    version" 
    "DESCRIPTION" 
    "Get device version."
};

extern const babbler_cmd_t CMD_MANUFACTURER = {
    "manufacturer",
    &cmd_manufacturer
};
extern const babbler_man_t MAN_MANUFACTURER = {
    "manufacturer",
    "get device manufacturer",
    "SYNOPSIS" 
    "    manufacturer" 
    "DESCRIPTION" 
    "Get device manufacturer."
};

extern const babbler_cmd_t CMD_URI = {
    "uri",
    &cmd_uri
};
extern const babbler_man_t MAN_URI = {
    "uri",
    "get device uri",
    "SYNOPSIS" 
    "    uri" 
    "DESCRIPTION" 
    "Get device uri."
};

/** 
 * Получить собственное имя устройства.
 */
int cmd_name(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_NAME);
    return strlen(reply_buffer);
}

/** 
 * Получить модель устройства.
 */
int cmd_model(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_MODEL);
    return strlen(reply_buffer);
}

/** 
 * Получить серийный номер устройства.
 */
int cmd_serial_number(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_SERIAL_NUMBER);
    return strlen(reply_buffer);
}

/** 
 * Получить словесное описание устройства. 
 */
int cmd_description(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_DESCRIPTION);
    return strlen(reply_buffer);
}

/** 
 * Получить версию прошивки устройства.
 */
int cmd_version(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_VERSION);
    return strlen(reply_buffer);
}

/** 
 * Получить производителя устройства.
 */
int cmd_manufacturer(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_MANUFACTURER);
    return strlen(reply_buffer);
}

/** 
 * Получить ссылку на страницу устройства.
 */
int cmd_uri(char* reply_buffer, int reply_buf_size, int argc, char *argv[]) {
    strcpy(reply_buffer, DEVICE_URI);
    return strlen(reply_buffer);
}

