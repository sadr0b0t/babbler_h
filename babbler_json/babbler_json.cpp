#include "babbler_json.h"

#include "babbler.h"
#include "babbler_simple.h"
#include "utility/json.h"
#include "stdio.h"

// максимальное количество параметров (блоков, разделенных проблелом) в команде
// (некрасивый хак, уж лучше использовать динамическую память, наверное)
#ifndef CMD_MAX_TOKENS
#define CMD_MAX_TOKENS 20
#endif

#define SERIAL_READ_BUFFER_SIZE 128
#define SERIAL_WRITE_BUFFER_SIZE 512

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
int wrap_reply_json(char* cmd, int argc, char* argv[], char* reply_buffer, int reply_buf_size) {
    return wrap_reply_with_id_json(cmd, NULL, argc, argv, reply_buffer, reply_buf_size);
}

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
int wrap_reply_with_id_json(char* cmd, char* cmd_id, int argc, char* argv[], char* reply_buffer, int reply_buf_size) {
    // TODO: чтобы поэкономить память, можно не создавать новый временный буфер,
    // а сдвинуть содержимое старого reply_buffer на нужно количество символов вправо
    // и добавить нужные данные слева и справа
    char cmd_reply_buffer[SERIAL_WRITE_BUFFER_SIZE];
    
    if(cmd_id != NULL) {
        if(strlen(reply_buffer) > 0 && reply_buffer[0] == '{') {
            // ответ начинается с открывающейся фигурной скобки { - 
            // будем считать, что там объект JSON и не будем брать его
            // в кавычки
            sprintf(cmd_reply_buffer, 
                "{\"cmd\": \"%s\", \"id\": \"%s\", \"reply\": \"%s\"}", 
                cmd, cmd_id, reply_buffer);
        } else {
            sprintf(cmd_reply_buffer, 
                "{\"cmd\": \"%s\", \"id\": \"%s\", \"reply\": \"%s\"}", 
                cmd, cmd_id, reply_buffer);
        }
    } else {
        // нет cmd_id - нет поля
        if(strlen(reply_buffer) > 0 && reply_buffer[0] == '{') {
            // ответ начинается с открывающейся фигурной скобки { - 
            // будем считать, что там объект JSON и не будем брать его
            // в кавычки
            sprintf(cmd_reply_buffer, 
                "{\"cmd\": \"%s\", \"reply\": \"%s\"}", 
                cmd, reply_buffer);
        } else {
            sprintf(cmd_reply_buffer, 
                "{\"cmd\": \"%s\", \"reply\": \"%s\"}", 
                cmd, reply_buffer);
        }
    }
        
    strcpy(reply_buffer, cmd_reply_buffer);
    return strlen(reply_buffer);
}


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
 */
int handle_command_json(char* buffer, char* reply_buffer, int reply_buf_size, 
            int (*wrap_reply)(char* cmd, char* cmd_id, int argc, char* argv[], char* reply_buffer, int reply_buf_size)) {
    // по умолчанию обнулим ответ
    reply_buffer[0] = 0;
    bool foundCmd = false;
    bool foundParams = false;
    bool foundId = false;
    
    // длина строки с присланной командой точно больше
    // суммарной длины строк со значениями параметров,
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
                    // нашли поле с идентификатором команды
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
    
    int reply_len = 0;
    
    if(foundCmd) {
        // выполнить команду
        reply_len = handle_command(argv[0], argc, argv, reply_buffer, reply_buf_size);
    } else  {
        // скорее всего некорректный JSON или нет нужного поля cmd,
        // отвечаем ошибкой
        strcpy(reply_buffer, REPLY_DONTUNDERSTAND);
        reply_len = strlen(reply_buffer);
    }
    
    if(wrap_reply != NULL) {
        if(foundCmd) {
            reply_len = wrap_reply(argv[0], cmd_id, argc, argv, reply_buffer, reply_buf_size);
        } else {
            // TODO: здесь следует или экранировать входную строку
            // так, чтобы она не поломала отправляемый обратно JSON,
            // или отправлять поле cmd пустым
            // возможные решения для экранирования 
            // http://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
            // пока просто оставляем пустым
            //_wrap_reply(buffer, cmd_id, reply_buffer);
            reply_len = wrap_reply("", cmd_id, argc, argv, reply_buffer, reply_buf_size);
        }
    }
    
    // почистим ресурсы
    free(argv_mem);
    json_value_free(value);
    
    return reply_len;
}

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
 *    -1: ошибка при формировании ответа (не хватило места в буфере)
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
 *    -1: error while constructing reply (not enought space in reply_buffer)
 */
int handle_input_json(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size) {
    // "распакуем" пакет: добавим завершающий ноль, срежем перевод строки (если есть)
    // (места в буфере для дополнительного нуля точно хватит, т.к. даже если длина входных данных
    // input_len равна размеру буфера SERIAL_READ_BUFFER_SIZE, у нас все равно в буфере есть 
    // один лишний байт)
    // "unpack" package: add terminating zero, cut newline at the end (if present)
    // (we have enough space for terminating zero in buffer, because even if input data length
    // input_len is equal to input buffer size SERIAL_READ_BUFFER_SIZE, we still have
    // one extra byte in the buffer)
    unpack_input_as_str(input_buffer, input_len, true);
    
    // выполняем команду (reply_buf_size-2 - место для переноса строки и завершающего нуля)
    // execute command (reply_buf_size-2 - place for newline and terminating zero)
    int reply_len = handle_command_json(input_buffer, reply_buffer, reply_buf_size-2, wrap_reply_with_id_json);
    
    // проверить на ошибку
    // check for error
    if(reply_len < 0) {
        reply_len = write_reply_error(reply_buffer, reply_len, reply_buf_size-2);
    }
    
    // "упаковать" пакет для отправки - добавить перенос строки
    // "pack" reply to send - add newline at the end
    reply_len = pack_reply_newline(reply_buffer, reply_len, reply_buf_size);
    
    return reply_len;
}

