#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * @def SERIALPORT
 * Define o nome da porta serial padrão para se conectar.
 * Em sistemas Windows, o padrão é COM1
 */
#ifdef Q_OS_WIN
# define SERIALPORT "COM1"
#else
# define SERIALPORT "/dev/ttyS0"
#endif

#define ERR_NO_ERROR      0x0000
#define ERR_SERIAL_ERROR  0x0001
#define ERR_BAD_GAME_MODE 0x0002

#endif // GLOBALS_H
