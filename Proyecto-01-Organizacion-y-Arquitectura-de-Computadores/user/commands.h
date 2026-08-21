// commands.h - Prototipos de funciones de ejecución y construcción de comandos

#ifndef COMMANDS_H
#define COMMANDS_H

#include "user/parser.h"

// Constructores de nodos de comandos
struct cmd* execcmd(void);
struct cmd* redircmd(struct cmd *subcmd, char *file, int mode, int fd);
struct cmd* pipecmd(struct cmd *left, struct cmd *right);

// Ejecuta el árbol de comandos 'cmd' generado por parsecmd().
// IMPORTANTE: se asume que ya se está en un proceso hijo (creado con
// fork() antes de llamar a runcmd), ya que esta función siempre
// termina llamando a exit() y nunca retorna.
void runcmd(struct cmd *cmd);

#endif // COMMANDS_H
