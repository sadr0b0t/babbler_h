
#include <babbler_config.h>
#include "babbler.h"

#include "utility/json.h"

// максимальное количество параметров (блоков, разделенных проблелом) в команде
// (некрасивый хак, уж лучше использовать динамическую память, наверное)
#ifndef CMD_MAX_TOKENS
#define CMD_MAX_TOKENS 20
#endif

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
            int (*_wrap_reply)(char* cmd, char* cmd_id, char* reply_buffer)) {
    // по умолчанию обнулим ответ
    reply_buffer[0] = 0;
    bool success = false;
    bool foundCmd = false;
    bool foundParams = false;
    bool foundId = false;
    
    // длина строки с присланной командой точно больше
    // суммарной длины строк со значениеми параметров,
    // даже с учетом завершающих нулей (на каждую строку
    // в JSON будет минимум 2 лишних символа - открывающая 
    // и закрывающая кавычки)
    char* argv_mem = (char*)malloc(strlen(buffer));
    if(argv_mem == NULL) {
        // совсем плохо - не хватило динамической памяти
        strcpy(reply_buffer, REPLY_ERROR);
        return strlen(reply_buffer);
    }
    char* next_param_ptr;
    next_param_ptr = argv_mem;
    
    // количество параметров (1 полюбому - имя команды)
    int argc = 1;
    char* argv[CMD_MAX_TOKENS];
    
    char* cmd_id = NULL;
    
    // на входе строка json вида:
    // {"cmd": "cmd_name", "params": ["param1", "param2"], "id": "cmd_id"}
    // cmd - имя команды, строка (обязательное поле)
    // params - параметры, массив json (необязательное поле)
    // id - клиентский идентификатор команды, отправляется обратно вместе с ответом, 
    //         строка (необязательное поле)
    
    // распарсим json по кусочкам
    json_value* value = json_parse((json_char*)buffer, strlen(buffer));

    // и сформируем список параметров вида:
    // tokes[0]=cmd_name
    // tokens[1]=param1
    // tokens[2]=param2
    if (value != NULL) {
        // базовый формат JSON корректный, проверим, 
        // на месте ли необходимые нам элементы и 
        // попробуем их вычленить
        
        // первый элемент должен быть Object
        if (value->type == json_object) {
            int length;
            length = value->u.object.length;
            for (int i = 0; i < length; i++) {
                if(strcmp("cmd", value->u.object.values[i].name) == 0) {
                    // нашли поле с именем команды
                    json_value* cmdValue = value->u.object.values[i].value;
                    // значение должно быть строка
                    if(cmdValue->type == json_string) {
                        char* cmdStr = cmdValue->u.string.ptr;
                        
                        // пишем в параметры
                        strcpy(next_param_ptr, cmdStr);
                        argv[0] = next_param_ptr;
                        next_param_ptr += strlen(next_param_ptr) + 1;
                        
                        foundCmd = true;
                    }
                } else if(strcmp("params", value->u.object.values[i].name) == 0) {
                    // нашли поле с параметрами
                    json_value* paramsValue = value->u.object.values[i].value;
                    // значение должно быть массив строк
                    if(paramsValue->type == json_array) {
                        // пройдемся по каждому параметру
                        for (int p = 0; p < paramsValue->u.array.length; p++) {
                            json_value* paramValue = paramsValue->u.array.values[p];
                            if(paramValue->type == json_string) {
                                char* paramStr = paramValue->u.string.ptr;
                                
                                // пишем в параметры
                                strcpy(next_param_ptr, paramStr);
                                argv[argc] = next_param_ptr;
                                argc++;
                                
                                // поехали дальше
                                next_param_ptr += strlen(next_param_ptr) + 1;
                            }
                        }
                        
                        foundParams = true;
                    }
                } else if(strcmp("id", value->u.object.values[i].name) == 0) {
                    // нашли поле с идентефикатором команды
                    json_value* idValue = value->u.object.values[i].value;
                    // значение должно быть строка
                    if(idValue->type == json_string) {
                        char* idStr = idValue->u.string.ptr;
                        
                        // пишем в память, которую выдели для параметров,
                        // но счетчик увеличивать не будем
                        strcpy(next_param_ptr, idStr);
                        cmd_id = next_param_ptr;
                        
                        // поехали дальше
                        next_param_ptr += strlen(next_param_ptr) + 1;
                        
                        foundId = true;
                    }
                }
            }
        }
    }
    
    if(foundCmd) {
        // Определим, с какой командой имеем дело
        for(int i=0; i < BABBLER_COMMANDS_COUNT && !success; i++) {
            if(strcmp(argv[0], BABBLER_COMMANDS[i].name) == 0) {
                // Нашли команду с запрошенным именем
                success = true;
                
                // Выполнить команду
                BABBLER_COMMANDS[i].exec_cmd(reply_buffer, argc, argv);
            }
        }
    }
    
    if(!success) {
        // Подготовить ответ
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
    }
    
    if(_wrap_reply != NULL) {
        if(foundCmd) {
            _wrap_reply(argv[0], cmd_id, reply_buffer);
        } else {
            // TODO: здесь следует или экранировать входную строку
            // так, чтобы она не поломала отправляемый обратно JSON,
            // или отправлять поле cmd пустым
            // возможные решения для экранирования 
            // http://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
            // пока просто оставляем пустым
            //_wrap_reply(buffer, cmd_id, reply_buffer);
            _wrap_reply("", cmd_id, reply_buffer);
        }
    }
    
    // почистим ресурсы
    free(argv_mem);
    json_value_free(value);
    
    return strlen(reply_buffer);
}

