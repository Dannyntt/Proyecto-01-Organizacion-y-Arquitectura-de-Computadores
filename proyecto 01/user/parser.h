// parser.h - Definiciones y prototipos del analizador de línea de comandos
// Se implementará completamente en la Entrega 2.

#ifndef PARSER_H
#define PARSER_H

#define MAXARGS 32

// Tipos de comando que el parser deberá reconocer.
#define CMD_EXEC   1  // comando simple con argumentos
#define CMD_REDIR  2  // redirección de entrada/salida (<, >)
#define CMD_PIPE   3  // tubería (|)

// Estructura base de un comando parseado (a completar en Entrega 2).
struct cmd {
  int type;
};

// TODO (Entrega 2): implementar en parser.c
// struct cmd* parsecmd(char *buf);

#endif // PARSER_H
