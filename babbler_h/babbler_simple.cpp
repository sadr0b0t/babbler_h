#include "babbler_simple.h"

#include "babbler.h"
#include "babbler_io.h"

#include "string.h"
#include "stdio.h"

// максимальное количество параметров (блоков, разделенных проблелом) в команде
// (некрасивый хак, уж лучше использовать динамическую память, наверное)
#ifndef CMD_MAX_TOKENS
#define CMD_MAX_TOKENS 20
#endif

/**
 * Найти команду по имени, выполнить, записать ответ в reply_buffer,
 * вернуть размер ответа.
 *
 * Буфер input_buffer содержит имя команды и (при необходимости) параметры, 
 * разделенные пробелами; всё вместе - строка, оканчивающая нулем:
 *     cmd_name [params]
 * 
 * Команда cmd_name выполняется при помощи вызова {module:babbler.h~handle_command}
 *
 * @param input_buffer - символьный буфер, содержит имя команды и параметры, разделенные пробелами;
 *    строка, оканчивающаяся нулем
 * @param reply_buffer - буфер для записи ответа, массив байт (строка или двоичный)
 * @param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа.
 *     Реализация функции должна следить за тем, чтобы длина ответа не превышала
 *     максимальный размер буфера
 * @param wrap_reply - указатель на функцию, производящую дополнительную обработку ответа,
 *   например оборачивание в пакет JSON или XML. Ничего не трогать, если NULL. 
 *   Значение по умолчанию NULL.
 *     wrap_reply:@param cmd - имя команды
 *     wrap_reply:@param reply_buffer - буфер, содержит текущее значение ответа (строка, оканчивающаяся нулем), 
 *         в него же должен быть записан ответ
 *     wrap_reply:@param reply_buf_size - размер буфера reply_buffer - максимальная длина ответа
 *     wrap_reply:@return размер нового ответа в байтах или код ошибки
 *         >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *         0: не отправлять ответ
 *        -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 * @return длина ответа в байтах или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int handle_command_simple(char* input_buffer, char* reply_buffer, int reply_buf_size, 
        int (*wrap_reply)(char* cmd, int argc, char* argv[], char* reply_buffer, int reply_buf_size)) {
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
    token = strtok_r(input_buffer, " ", &last);
    while(token != NULL && tokensNum < CMD_MAX_TOKENS) {
        tokens[tokensNum] = token;
        tokensNum++;
        
        token = strtok_r(NULL, " ", &last);
    }
    
    // выполнить команду
    int reply_len = handle_command(tokens[0], tokensNum, tokens, reply_buffer, reply_buf_size);
    
    // дополнительно обернуть ответ
    if(wrap_reply != NULL) {
        reply_len = wrap_reply(tokens[0], tokensNum, tokens, reply_buffer, reply_buf_size);
    }
    
    return reply_len;
}

/**
 * Фильтр пакетов, отделяющихся переносом строки.
 * Определить, является ли содержимое буфера пакетом.
 * См {module:babbler_io.h~packet_filter}
 * @param input - входные данные
 * @param input_len - длина данных в буфере
 * @return 
 *     true - буфер содержит корректный пакет 
 *         (последний символ буфера - перенос строки '\n')
 *     false - содержимое буфера не является корректным пакетом
 *         (последний символ буфера - не перенос строки)
 */
bool packet_filter_newline(char* input, int input_len) {
    if(input_len > 0 && input[input_len-1] == '\n') {
        return true;
    } else {
        return false;
    }
}

/**
 * "Распаковать" пакет в буфере в строку: добавить завершающий ноль, 
 * убрать символ окончания строки (если требуется).
 * Реального размера буфера должно хватить, чтобы вместить еще один символ
 * (достоточно input_len+1).
 * @param input - буфер с входными данными
 * @param input_len - длина пакета в буфере
 * @param drop_newline - удалять ли завершающий перенос строки:
 *     true - удалять (записать на его место завершающий ноль)
 *     false - не удалять (ноль будет записан после переноса)
 * @return новая длина строки с пакетом (input_len+1, если срезали завершающий перенос
 *     строки; input_len, т.е. без изменений, если не срезали)
 */
int unpack_input_as_str(char* input, int input_len, bool drop_newline) {
    if(drop_newline && input_len > 0 && input[input_len-1] == '\n') {
        input[input_len-1] = 0;
    } else {
        input[input_len] = 0;
    }
    return strlen(input);
}

/**
 * "Упаковать" ответ в пакет для отправки через модуль ввода-вывода: 
 * в данном случае добавить перенос строки последним символом и (при необходимости)
 * завершающий ноль.
 * Реального размера буфера reply_buf_size должно хватить, чтобы вместить все дополнительные 
 * символы (reply_len+2, если добавляем завершающий ноль, reply_len+1, если не добавляем).
 * @param reply_buffer - буфер с ответом
 * @param reply_buf_size - размер буфера с ответом
 * @param reply_len - длина ответа в буфере
 * @param add_termzero - добавить завершающий ноль после добавления символа перевода строки:
 *     true - добавлять, false - не добавлять.
 * @return новая длина ответа в буфере или код ошибки
 *     >0, <=reply_buf_size: количество байт, записанных в reply_buffer
 *     0: не отправлять ответ
 *    -1: ошибка при формировании ответа (не хватило места в буфере, ошибка выделения памяти и т.п.)
 */
int pack_reply_newline(char* reply_buffer, int reply_len, int reply_buf_size, bool add_termzero) {
    if(reply_len > 0) {
        // добавим в конец перевод строки, если в буфере есть место
        if(reply_len <= reply_buf_size) {
            reply_buffer[reply_len] = '\n';
            reply_len++;
            
            // запишем в конец ноль, если требуется
            if(add_termzero) {
                // места всё еще должно хватать
                if(reply_len <= reply_buf_size) {
                    reply_buffer[reply_len] = 0;
                } else {
                    // В буфере не достаточно места, чтобы сформировать полностью корректный ответ
                    reply_len = REPLY_BUF_ERROR;
                }
            }
        } else {
            // В буфере не достаточно места, чтобы сформировать полностью корректный ответ
            reply_len = REPLY_BUF_ERROR;
        }
    }
    return reply_len;
}

/**
 * Записать в буфер сообщение об ошибке, соответствующее коду в error_code.
 * Код ошибки должен быть меньше нуля, иначе никаких действий не происходит и
 * функция возвращает исходное значение error_code.
 * @param reply_buffer - буфер для записи сообщения об ошибке
 * @param error_code - код ошибки < 0
 * @param reply_buf_size
 * return длина сообщения в буфере reply_buffer, если error_code меньше нуля и 
 *    распознан; исходное значение error_code, если error_code больше или 
 *    равен нулю.
 */
int write_reply_error(char* reply_buffer, int error_code, int reply_buf_size) {
    if(error_code < 0) {
        if(error_code == REPLY_BUF_ERROR) {
            strcpy(reply_buffer, REPLY_REPLY_BUF_ERROR);
            error_code = strlen(reply_buffer);
        } else {
            sprintf(reply_buffer, "error: %d", error_code);
            error_code = strlen(reply_buffer);
        }
    }
    return error_code;
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
int handle_input_simple(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size) {
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
    int reply_len = handle_command_simple(input_buffer, reply_buffer, reply_buf_size-2);
    
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

