#ifndef BABBLER_SERIAL_H
#define BABBLER_SERIAL_H

#define BABBLER_SERIAL_SKIP_PORT_INIT -1

/**
 * Предварительная настройка модуля канала связи последовательный порт Serial, 
 * выполнить один раз в setup.
 * 
 * @param _handle_input указатель на функцию, обрабатывающую входные данные:
 *     разбирает строку, выполняет одну или несколько команд, записывает ответ
 *     _handle_input:@param buffer - входные данные, строка
 *     _handle_input:@param buffer_size - размер входных данных
 *     _handle_input:@param reply_buffer - ответ, строка, оканчивающаяся нулём
 *     _handle_input:@return размер ответа в байтах (0, чтобы не отправлять ответ)
 * @param speed скорость передачи данных в бит/c (бод) для последовательно порта, 
 *     см Serial.begin(speed). 
 *     Допольнительное специальное значение speed=BABBLER_SERIAL_SKIP_PORT_INIT (-1) - пропустить
 *     инициализацию порта.
 */
void babbler_serial_setup(int (*_handle_input)(char* buffer, int buffer_size, char* reply_buffer),
        long speed);

/**
 * Постоянные задачи для канала связи последовательный порт Serial, 
 * выполнять на каждой итерации в бесконечном цикле loop
 * При получении команды, вызывает handleInput.
 */
void babbler_serial_tasks();

#endif // BABBLER_SERIAL_H

