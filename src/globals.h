#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * @def SERIALPORT
 * Define o nome da porta serial padrão para se conectar.
 *
 * Em sistemas Windows, o padrão é <tt>COM1</tt>. Para sistemas *nix será
 * utilizado o caminho <tt>/dev/ttyS0</tt>.
 *
 * A porta serial padrão é utilizada como valor inicial para o respectivo campo
 * na janela de configurações do jogo GameOptions.
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
