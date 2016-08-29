#ifndef BABBLER_H
#define BABBLER_H

#include "stddef.h"

/**************************************/
// Стандартные ответы на команды (значения см в babbler.cpp)
extern const char* REPLY_OK;
extern const char* REPLY_DONTUNDERSTAND;
extern const char* REPLY_BAD_PARAMS;
extern const char* REPLY_ERROR;
extern const char* REPLY_BUSY;

/**
 * Вся необходимая информация о команде: имя, синтаксис,
 * описание, ссылка на функцию, выполняющую команду.
 */
typedef struct {
    /** Имя команды */
    const char* name;
    
    /** 
     * Указатель на функцию, выполняющую команду.
     * 
     * @param reply_buffer символьный буфер для записи ответа - 
     *     результата выполнения команды
     * @param argc количество аргументов (по умолчанию может быть 0)
     * @param argv значения аргументов (по умолчанию может быть NULL),
     *     если задано, 1й аргумент - имя команды
     * @return длина ответа (количество байт, записанных в reply_buffer)
     */
    int (*exec_cmd)(char* reply_buffer, int argc, char *argv[]);
        
    /** Краткое описание команды */
    const char* short_descr;
    
    /** Руководство по использованию команды: синтаксис, описание, параметры */
    const char* manual;
} babbler_cmd_t;


/** 
 * Зарегистрированные команды 
 * (значения требуется определить в основной программе) 
 */
extern const babbler_cmd_t BABBLER_COMMANDS[];

/** 
 * Количество зарегистрированных команд 
 * (значение требуется определить в основной программе) 
 */
extern const int BABBLER_COMMANDS_COUNT;

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
int handle_command(char* buffer, char* reply_buffer, int (*_wrap_reply)(char* cmd, char* reply_buffer)=NULL);

#endif // BABBLER_H

