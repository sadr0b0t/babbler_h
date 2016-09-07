#ifndef BABBLER_JSON_IN_H
#define BABBLER_JSON_IN_H

#include "stddef.h"

/**
 * Найти команду по имени в buffer, выполнить, записать ответ в reply_buffer,
 * вернуть размер ответа.
 *
 * buffer содержит команду и параметры в строке JSON вида
 * {"cmd": "cmd_name", "params": ["param1", "param2"], "id": "cmd_id"}
 *     cmd - имя команды, строка (обязательное поле)
 *     params - параметры, массив json (необязательное поле)
 *     id - клиентский идентификатор команды, отправляется обратно вместе с ответом, 
 *             строка (необязательное поле)
 *
 * Команда ищется по имени cmd_name среди зарегистрированных команд в глобальном
 * массиве BABBLER_COMMANDS, количество команд должно быть определено в глобальной 
 * переменной int BABBLER_COMMANDS_COUNT.
 * Если команда найдена, выполняется вызовом command.exec_cmd
 * Если команда не найдена, в reply_buffer записывается ответ REPLY_DONTUNDERSTAND
 * 
 * @param buffer символьный буфер, содержит имя команды и параметры, разделенные пробелами -
 *    строка, оканчивающаяся нулем
 * @param reply_buffer символьный буфер для записи ответа
 * @param _wrap_reply указатель на функцию, производящую дополнительную обработку ответа,
 *     например оборачивание в пакет JSON или XML. Ничего не менять, если NULL. 
 *     Значение по умолчанию NULL.
 * @return количество символов, записанных командой в reply_buffer
 */
int handle_command_json(char* buffer, char* reply_buffer, 
            int (*_wrap_reply)(char* cmd, char* cmd_id, char* reply_buffer)=NULL);

#endif // BABBLER_JSON_IN_H

