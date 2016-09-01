#ifndef BABBLER_CMD_CORE_H
#define BABBLER_CMD_CORE_H

#include "babbler.h"

/**************************************/
// Универсальные команды, рекомендуются для всех устройств

// Команды

/** Вывести список команд */
extern const babbler_cmd_t CMD_HELP;
extern const babbler_man_t MAN_HELP;
/** Проверить доступность устройства */
extern const babbler_cmd_t CMD_PING;
extern const babbler_man_t MAN_PING;

/**************************************/
// Обработчики команд

/** 
 * Вывести список команд.
 */
int cmd_help(char* reply_buffer, int argc=0, char *argv[]=NULL);

/** 
 * Проверить доступность устройства.
 */
int cmd_ping(char* reply_buffer, int argc=0, char *argv[]=NULL);

#endif // BABBLER_CMD_CORE_H

