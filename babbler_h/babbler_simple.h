#ifndef BABBLER_SIMPLE_H
#define BABBLER_SIMPLE_H

#include "stddef.h"

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
        int (*wrap_reply)(char* cmd, int argc, char* argv[], char* reply_buffer, int reply_buf_size)=NULL);

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
bool packet_filter_newline(char* input, int input_len);

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
int unpack_input_as_str(char* input, int input_len, bool drop_newline=true);

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
int pack_reply_newline(char* reply_buffer, int reply_len, int reply_buf_size, bool add_termzero=true);

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
int write_reply_error(char* reply_buffer, int error_code, int reply_buf_size);

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
 */
int handle_input_simple(char* input_buffer, int input_len, char* reply_buffer, int reply_buf_size);

#endif // BABBLER_SIMPLE_H

