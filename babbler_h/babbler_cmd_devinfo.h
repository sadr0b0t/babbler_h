#ifndef BABBLER_CMD_DEVINFO_H
#define BABBLER_CMD_DEVINFO_H

#include "babbler.h"


// Команды для получения информации об устройстве

// Команды
/** Получить собственное имя устройства */
extern const babbler_cmd_t CMD_NAME;
extern const babbler_man_t MAN_NAME;
/** Получить модель устройства */
extern const babbler_cmd_t CMD_MODEL;
extern const babbler_man_t MAN_MODEL;
/** Получить серийный номер устройства */
extern const babbler_cmd_t CMD_SERIAL_NUMBER;
extern const babbler_man_t MAN_SERIAL_NUMBER;
/** Получить словесное описание устройства */
extern const babbler_cmd_t CMD_DESCRIPTION;
extern const babbler_man_t MAN_DESCRIPTION;
/** Получить версию прошивки устройства */
extern const babbler_cmd_t CMD_VERSION;
extern const babbler_man_t MAN_VERSION;
/** Получить производителя устройства */
extern const babbler_cmd_t CMD_MANUFACTURER;
extern const babbler_man_t MAN_MANUFACTURER;
/** Получить ссылку на страницу устройства */
extern const babbler_cmd_t CMD_URI;
extern const babbler_man_t MAN_URI;


/**************************************/
// Обработчики команд

/** 
 * Получить собственное имя устройства.
 */
int cmd_name(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить модель устройства.
 */
int cmd_model(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить серийный номер устройства.
 */
int cmd_serial_number(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить словесное описание устройства. 
 */
int cmd_description(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить версию прошивки устройства.
 */
int cmd_version(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить производителя устройства.
 */
int cmd_manufacturer(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Получить ссылку на страницу устройства.
 */
int cmd_uri(char* reply_buffer, int argc=0, char *argv[]=NULL);

#endif // BABBLER_CMD_DEVINFO_H

