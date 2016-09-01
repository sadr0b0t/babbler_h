
#include <babbler_config.h>
#include "babbler.h"

#include "string.h"

// максимальное количество параметров (блоков, разделенных проблелом) в команде
// (некрасивый хак, уж лучше использовать динамическую память, наверное)
#ifndef CMD_MAX_TOKENS
#define CMD_MAX_TOKENS 20
#endif

/**************************************/
// Стандартные ответы на команды
extern const char* REPLY_OK = "ok";
extern const char* REPLY_DONTUNDERSTAND = "dontunderstand";
extern const char* REPLY_BAD_PARAMS = "badparams";
extern const char* REPLY_ERROR = "error";
extern const char* REPLY_BUSY = "busy";

/**
 * Найти команду по имени в buffer, выполнить, записать ответ в reply_buffer,
 * вернуть размер ответа.
 *
 * buffer содержит имя команды и (при необходимости) параметры, разделенные пробелами
 *     cmd_name [params]
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
int handle_command(char* buffer, char* reply_buffer, int (*_wrap_reply)(char* cmd, char* reply_buffer)) {
    // по умолчанию обнулим ответ
    reply_buffer[0] = 0;
    
    bool success = false;
    
    char* tokens[CMD_MAX_TOKENS];
    int tokensNum = 0;
    
    // Разобьем команду на куски по пробелам
    char* token;
    // указатель на строку для внутренних нужд strtok_r, позволяет
    // одновременно обрабатывать несколько строк (strtok может работать 
    // только с одной строкой за раз)
    char* last;
    token = strtok_r(buffer, " ", &last);
    while(token != NULL && tokensNum < CMD_MAX_TOKENS) {
        tokens[tokensNum] = token;
        tokensNum++;
        
        token = strtok_r(NULL, " ", &last);
    }
    
    // Определим, с какой командой имеем дело
    for(int i=0; i < BABBLER_COMMANDS_COUNT && !success; i++) {
        if(strcmp(tokens[0], BABBLER_COMMANDS[i].name) == 0) {
            // Нашли команду с запрошенным именем
            success = true;
            
            // Выполнить команду
            BABBLER_COMMANDS[i].exec_cmd(reply_buffer, tokensNum, tokens);
        }
    }
    
    if(!success) {
        // Подготовить ответ
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
    }
    if(_wrap_reply != NULL) {
        _wrap_reply(tokens[0], reply_buffer);
    }
    
    return strlen(reply_buffer);
}

