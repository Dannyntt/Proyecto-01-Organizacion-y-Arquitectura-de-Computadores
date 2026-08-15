// parser.h - Definiciones y prototipos del analizador de línea de comandos

#ifndef PARSER_H
#define PARSER_H

#define MAXARGS 32

// Tipos de comando reconocidos por el parser.
#define CMD_EXEC   1  // comando simple con argumentos
#define CMD_REDIR  2  // redirección de entrada/salida (<, >)
#define CMD_PIPE   3  // tubería (|)

// Nodo base: todo comando parseado comienza con este campo 'type',
// lo que permite hacer "downcast" a la estructura concreta según el caso.
struct cmd {
  int type;
};

// Comando simple: programa + argumentos (ej. "grep error archivo.txt").
struct execcmd {
  int type;
  char *argv[MAXARGS];
  int nargv;
};

// Redirección: envuelve a otro comando y redirige un descriptor de
// archivo (0 = stdin para '<', 1 = stdout para '>') antes de ejecutarlo.
struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  int mode; // flags de open(): O_RDONLY para '<', O_WRONLY|O_CREATE|O_TRUNC para '>'
  int fd;   // descriptor a redirigir: 0 o 1
};

// Tubería: conecta la salida de 'left' con la entrada de 'right'.
// Permite tuberías múltiples porque 'right' puede ser a su vez otro CMD_PIPE.
struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

// Analiza la línea 's' completa y retorna el árbol de comandos resultante.
struct cmd* parsecmd(char *s);

// Constructores de cada tipo de nodo.
struct cmd* execcmd(void);
struct cmd* redircmd(struct cmd *subcmd, char *file, int mode, int fd);
struct cmd* pipecmd(struct cmd *left, struct cmd *right);

#endif // PARSER_H
