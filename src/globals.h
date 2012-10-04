/**
 * Arquivo com definições globais, acessíveis a todos os arquivos do projeto.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * @def SERIALPORT
 *      Define o nome da porta serial que deve ser utilizada para se conectar.
 */
#ifdef Q_OS_WIN
# define SERIALPORT "COM2"
#else
# define SERIALPORT "/dev/ttyS1"
#endif


#define ERR_BAD_MODE 0x002

#endif // GLOBALS_H
