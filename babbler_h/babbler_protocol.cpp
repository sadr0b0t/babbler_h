
#include "babbler_config.h"

#include "stdio.h"
#include "string.h"

/**
 * Добавить в конец ответа перевод строки \n
 *
 * @param cmd имя команды
 * @param reply_buffer исходное значение ответа выполненной команды,
 *     перезаписывает новым значение строки с обернутым ответом
 * @param длина новой строки
 */
int wrap_reply_newline(char* cmd, char* reply_buffer) {
    char cmd_reply_buffer[CMD_WRITE_BUFFER_SIZE];
    
    strcpy(reply_buffer+strlen(reply_buffer), "\n");
    return strlen(reply_buffer);
}

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
int wrap_reply_json(char* cmd, char* reply_buffer) {
    // TODO: чтобы поэкономить память, можно не создавать новый временный буфер,
    // а сдвинуть содержимое старого reply_buffer на нужно количество символов вправо
    // и добавить нужные данные слева и справа
    char cmd_reply_buffer[CMD_WRITE_BUFFER_SIZE];
    
    sprintf(cmd_reply_buffer, "{\"cmd\": \"%s\", \"reply\": \"%s\"}", cmd, reply_buffer);
    strcpy(reply_buffer, cmd_reply_buffer);
    return strlen(reply_buffer);
}

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
int wrap_reply_xml(char* cmd, char* reply_buffer) {
    // TODO: чтобы поэкономить память, можно не создавать новый временный буфер,
    // а сдвинуть содержимое старого reply_buffer на нужно количество символов вправо
    // и добавить нужные данные слева и справа
    char cmd_reply_buffer[CMD_WRITE_BUFFER_SIZE];
   
    sprintf(cmd_reply_buffer, "<cmd_reply><cmd>%s</cmd><reply>%s</reply></cmd_reply>", cmd, reply_buffer);
    strcpy(reply_buffer, cmd_reply_buffer);
    return strlen(reply_buffer);
}

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
        int (*_wrap_reply)(char* cmd, char* reply_buffer)=NULL, const char* separator=";") {
    // добавим к входным данным завершающий ноль, 
    // чтобы рассматривать их как корректную строку
    buffer[buffer_size] = 0;
    
    // обнулим ответ
    reply_buffer[0] = 0;
    
    char cmd_buffer[CMD_READ_BUFFER_SIZE];
    char cmd_reply_buffer[CMD_WRITE_BUFFER_SIZE];
        
    // Разобьем входящую строку на куски по разделителю команд ';'
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    // первая команда
    token = strtok_r(buffer, separator, &last);
    bool firstToken = true;
    while(token != NULL) {
        strcpy(cmd_buffer, token);
        _handle_command(cmd_buffer, cmd_reply_buffer, _wrap_reply);
        
        // добавлять к ответу предваряющий разделитель ';' для всех команд,
        // кроме первой
        if(!firstToken) {
            strcat(reply_buffer, separator);
        } else {
            firstToken = false;
        }
        
        // добавить ответ от команды в общий список
        // TODO: сделать что-нибудь в случае, если все ответы не умещаются
        // в буфер (это касается и разных других мест, но здесь особенно,
        // т.к. размер ответа от нескольких команд - динамический)
        strcat(reply_buffer, cmd_reply_buffer);
        
        // следующая команда
        token = strtok_r(NULL, ";", &last);
    }
  
    return strlen(reply_buffer);
}

