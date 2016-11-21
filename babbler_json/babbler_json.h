#ifndef BABBLER_JSON_H
#define BABBLER_JSON_H

#include "stddef.h"

/**
 * Обернуть ответ в формат JSON вида:
 * {"cmd": "cmd_name", "reply": "reply_value"}
 * 
 * здесь значение cmd_name - имя команды
 * reply_value - ответ выполненной команды (исходное содержимое reply_buffer)
 *
 * Новое значение перезаписывается в reply_buffer, его размера должно достаточно,
 * чтобы вместить новую строку.
 *
 * @param cmd - имя команды
 * @param argc - количество параметров команды
 * @param argv - массив с параметрами команды
 * @param reply_buffer - исходное значение ответа выполненной команды,
 *     перезаписывается новым значением строки с обернутым ответом
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int wrap_reply_json(char* cmd, int argc, char* argv[], char* reply_buffer, int reply_buf_size);

/**
 * Обернуть ответ в формат JSON вида:
 * {"cmd": "cmd_name", "id": "cmd_id", "reply": "reply_value"}
 * 
 * здесь значение 
 *     cmd_name - имя команды
 *     cmd_id - клиентский идентификатор команды (пришел с командой)
 *     reply_value - ответ выполненной команды (исходное содержимое reply_buffer)
 *
 * Новое значение перезаписывается в reply_buffer, его размера должно достаточно,
 * чтобы вместить новую строку.
 *
 * @param cmd - имя команды
 * @param cmd_id - клиентский идентификатор команды (пришел с командой)
 * @param argc - количество параметров команды
 * @param argv - массив с параметрами команды
 * @param reply_buffer - исходное значение ответа выполненной команды,
 *     перезаписывается новым значением строки с обернутым ответом
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int wrap_reply_with_id_json(char* cmd, char* cmd_id, int argc, char* argv[], char* reply_buffer, int reply_buf_size);

/**
 * Найти команду по имени в input_buffer, выполнить, записать ответ в reply_buffer,
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
 * Если команда найдена, выполняется вызовом command.exec_cmd.
 * Если команда не найдена, в reply_buffer записывается ответ REPLY_DONTUNDERSTAND.
 * 
 * @param input_buffer - символьный буфер, содержит имя команды и параметры, разделенные пробелами -
 *    строка, оканчивающаяся нулем
 * @param reply_buffer - символьный буфер для записи ответа
 * @param wrap_reply - указатель на функцию, производящую дополнительную обработку ответа,
 *     например оборачивание в пакет JSON или XML. Ничего не менять, если NULL. 
 *     Значение по умолчанию NULL.
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int handle_command_json(char* input_buffer, char* reply_buffer, int reply_buf_size, 
            int (_wrap_reply)(char* cmd, char* cmd_id, int argc, char* argv[], char* reply_buffer, int reply_buf_size)=NULL);
            
/**
 * Обработать входные данные: разобрать строку, выполнить одну или 
 * несколько команд, записать ответ.
 * @param input_buffer - входные данные, массив байт (строка или двоичный)
 * @param input_len - размер входных данных
 * @param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 */
/**
 * Handle input data: parse string, run one or multiple commands, 
 * write reply.
 * @param input_buffer - input data, byte array (string or binary)
 * @param input_len - input data length
 * @param reply_buffer - reply buffer, byte array (string or binary)
 * @param reply_buf_size - size of reply_buffer buffer - maximum length of reply.
 *     Function implementation should take care of reply length not exceeding
 *     maximum reply buffer size.
 * @return length of reply in bytes or error code
 *     >0, <=reply_buf_size: number of bytes, written to reply_buffer
 *     0: don't send reply
 */
int handle_input_json(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size);

#endif // BABBLER_JSON_H

