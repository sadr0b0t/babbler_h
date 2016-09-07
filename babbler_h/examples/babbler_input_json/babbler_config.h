// Настройки модулей коммуникации и другие настройки
// Communication module config and other config options

// включить отладку через последовательный порт
// enable serial port debug messages
//#define DEBUG_SERIAL

// Размеры буферов для чтения команд и записи ответов
// Read and write buffer size for communication modules
#ifndef CMD_READ_BUFFER_SIZE
#define CMD_READ_BUFFER_SIZE 128
#endif

#ifndef CMD_WRITE_BUFFER_SIZE
#define CMD_WRITE_BUFFER_SIZE 256
#endif

// выключить добавление завершающего нуля к принимаемым данным
// (по умолчанию добавление нуля включено, не следует выключать при обмене строками)
// disable adding terminating zero when read input data
// (adding zero is enabled by default, don't disable when receive strings)
//#define DONT_ADD_TERMINATING_ZERO

// выключить добавление переноса строки к отправляемым данным
// (по умолчанию добавление переноса строки включено)
// disable adding newline when send data
// (adding newline to sent data is enabled by default)
//#define DONT_ADD_TERMINATING_NEWLINE

