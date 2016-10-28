
#include "babbler.h"

#include "string.h"

/**************************************/
// Стандартные ответы на команды
extern const char* REPLY_OK = "ok";
extern const char* REPLY_DONTUNDERSTAND = "dontunderstand";
extern const char* REPLY_BAD_PARAMS = "badparams";
extern const char* REPLY_ERROR = "error";
extern const char* REPLY_REPLY_BUF_ERROR = "replybuferror";
extern const char* REPLY_BUSY = "busy";

/**
 * Найти команду по имени, выполнить, записать ответ в reply_buffer,
 * вернуть размер ответа.
 *
 * Команда ищется по имени cmd среди зарегистрированных команд в глобальном
 * массиве BABBLER_COMMANDS, количество команд должно быть определено в глобальной 
 * переменной int BABBLER_COMMANDS_COUNT.
 * Если команда найдена, выполняется вызовом command.exec_cmd
 * Если команда не найдена, в reply_buffer записывается ответ REPLY_DONTUNDERSTAND
 * 
 * @param cmd - символьный буфер, содержит имя команды 
 * @param argc - количество параметров команды
 * @param argv - массив с параметрами команды
 * @param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int handle_command(char* cmd, int argc, char *argv[], char* reply_buffer, int reply_buf_size) {
    // по умолчанию обнулим ответ
    reply_buffer[0] = 0;
    int reply_len = 0;
    
    bool success = false;
    
    // Определим, с какой командой имеем дело
    for(int i=0; i < BABBLER_COMMANDS_COUNT && !success; i++) {
        if(strcmp(cmd, BABBLER_COMMANDS[i].name) == 0) {
            // Нашли команду с запрошенным именем
            success = true;
            
            // Выполнить команду
            reply_len = BABBLER_COMMANDS[i].exec_cmd(reply_buffer, reply_buf_size, argc, argv);
        }
    }
    
    if(!success) {
        // Подготовить ответ - команда не найдена
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
        reply_len = strlen(reply_buffer);
    }
    
    return reply_len;
}

