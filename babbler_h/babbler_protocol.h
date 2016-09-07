#ifndef BABBLER_PROTOCOL_H
#define BABBLER_PROTOCOL_H

#include "stddef.h"

/**
 * Добавить в конец ответа перевод строки \n
 *
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_newline(char* cmd, char* reply_buffer);

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
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_json(char* cmd, char* reply_buffer);

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
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_with_id_json(char* cmd, char* cmd_id, char* reply_buffer);

/**
 * Обернуть ответ в формат XML вида:
 * <cmd_reply><cmd>cmd_name</cmd><reply>reply_value</reply></cmd_reply>
 * 
 * здесь значение cmd_name - имя команды
 * reply_value - ответ выполненной команды (исходное содержимое reply_buffer)
 *
 * Новое значение перезаписывается в reply_buffer, его размера должно достаточно,
 * чтобы вместить новую строку.
 *
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_xml(char* cmd, char* reply_buffer);

/**
 * Обернуть ответ в формат XML вида:
 * <cmd_reply><cmd>cmd_name</cmd><id>cmd_id</id><reply>reply_value</reply></cmd_reply>
 * 
 * здесь значение cmd_name
 *     cmd_name - имя команды
 *     cmd_id - клиентский идентификатор команды (пришел с командой)
 *     reply_value - ответ выполненной команды (исходное содержимое reply_buffer)
 *
 * Новое значение перезаписывается в reply_buffer, его размера должно достаточно,
 * чтобы вместить новую строку.
 *
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_with_id_xml(char* cmd, char* cmd_id, char* reply_buffer);

/**
 * Вычленить из входного буфера команды, разделенные специальным символом separator 
 * (по умолчанию - точка с запятой ";"); выполнить команды одну за одной, 
 * результаты выполнения сохранить и вернуть в одной строке в порядке выполнения
 * в reply_buffer, также разделенные символом separator (по умолчанию - точка с запятой ";").
 * 
 * Например:
 * Вход: name;ping;model
 * Результат: Anton's Rraptor;ok;Rraptor
 *
 * @param buffer входные данные - команды, разделенные симоволом separator
 * @param buffer_size размер входных данных
 * @param reply_buffer буфер для записи ответов
 * @param _handle_command указатель на функцию, выполняющую выбранную команду
 *     _handle_command:@param buffer символьный буфер, содержит имя команды
 *     _handle_command:@param reply_buffer символьный буфер для записи ответа
 *     _handle_command:@param _wrap_reply указатель на функцию, которая при необходимости
 *         сможет выполнить пост-обработку ответа команды (обернет в JSON, XML или типа того)
 *     _handle_command:@return количество символов, записанных в reply_buffer
 * @param _wrap_reply указатель на функцию, которая при необходимости сможет выполнить 
 *     пост-обработку ответа команды (обернет в JSON, XML или типа того) - передаётся в _handle_command.
 *    Значение по умолчанию: NULL
 *     _wrap_reply:@param cmd имя команды
 *     _wrap_reply:@param reply_buffer символьный буфер для записи ответа
 *    _wrap_reply:@return количество символов, записанных в reply_buffer
 * @param separator разделитель команд во буфере входных данных buffer и ответов в 
 *     буфере ответа reply_buffer
 * @return длина общего ответа: количество символов, записанных в reply_buffer
 */
int handle_input_multicmd(char* buffer, int buffer_size, char* reply_buffer, 
        int (*_handle_command)(char* buffer, char* reply_buffer, int (*_wrap_reply)(char* cmd, char* reply_buffer)), 
        int (*_wrap_reply)(char* cmd, char* reply_buffer)=NULL, const char* separator=";");


#endif // BABBLER_PROTOCOL_H

