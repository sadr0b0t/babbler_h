#include "babbler_serial.h"

/**
 * Обработать входные данные: разобрать строку, выполнить одну или 
 * несколько команд, записать ответ
 *
 * @param buffer - входные данные, строка
 * @param buffer_size - размер входных данных
 * @param reply_buffer - ответ, строка, оканчивающаяся нулём
 * @return размер ответа в байтах (0, чтобы не отправлять ответ)
 */
 /**
 * Handle input data: parse string, run one or multiple commands, 
 * write reply.
 *
 * @param buffer - input data, zero-terminated string
 * @param buffer_size - number of input bytes
 * @param reply_buffer - reply, zero-terminated string
 * @return size of reply in bytes (0 for no reply)
 */
int handle_input(char* buffer, int buffer_size, char* reply_buffer) {
    // добавим к входным данным завершающий ноль, 
    // чтобы рассматривать их как корректную строку
    // (на тот случай, если принимающий модуль этого не сделал сам)
    // lets add finishing zero to have valid zero-terminated string
    // (in case if communication module did not do this itself)
    buffer[buffer_size] = 0;
    
    // как-нибудь отреагируем на запрос - пусть будет простое эхо
    // react somehow for request - let it be simple echo
    sprintf(reply_buffer, "you say: %s", buffer);
  
    return strlen(reply_buffer);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting babbler-powered device, type something to see echo");
    
    //babbler_serial_setup(handle_input, 9600);
    babbler_serial_setup(handle_input, BABBLER_SERIAL_SKIP_PORT_INIT);
}

void loop() { 
    babbler_serial_tasks();
}

