// commands.c - Ejecución de comandos, redirecciones y tuberías
//
// runcmd() recorre el árbol de `struct cmd` construido por parser.c y
// lo ejecuta usando los mecanismos estándar de xv6 (fork, exec, pipe,
// open, dup, wait).

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/parser.h"
#include "user/commands.h"

static int fork1(void);

void
runcmd(struct cmd *cmd)
{
  struct execcmd *ecmd;
  struct redircmd *rcmd;
  struct pipecmd *pcmd;
  int p[2];

  if (cmd == 0)
    exit(0);

  switch (cmd->type) {

  default:
    fprintf(2, "sh: tipo de comando desconocido\n");
    exit(1);

  case CMD_EXEC:
    // Comando simple: reemplaza este proceso por el programa pedido.
    ecmd = (struct execcmd*)cmd;
    if (ecmd->argv[0] == 0)
      exit(0);
    exec(ecmd->argv[0], ecmd->argv);
    // Si exec() retorna, es porque falló.
    fprintf(2, "sh: comando no encontrado: %s\n", ecmd->argv[0]);
    exit(1);

  case CMD_REDIR:
    // Redirección: cierra el descriptor (0 o 1) y abre el archivo en su
    // lugar, para que quede con el número de descriptor más bajo
    // disponible (0 o 1, justo el que acabamos de cerrar).
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if (open(rcmd->file, rcmd->mode) < 0) {
      fprintf(2, "sh: no se pudo abrir %s\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    break;

  case CMD_PIPE:
    // Tubería: crea un pipe y dos procesos hijos.
    // El izquierdo escribe su salida (fd 1) en el pipe.
    // El derecho lee su entrada (fd 0) desde el pipe.
    // Si 'right' es a su vez otra tubería, esto se resuelve solo por
    // recursión (cmd->right vuelve a ser CMD_PIPE).
    pcmd = (struct pipecmd*)cmd;
    if (pipe(p) < 0) {
      fprintf(2, "sh: error al crear el pipe\n");
      exit(1);
    }
    if (fork1() == 0) {
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if (fork1() == 0) {
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    break;
  }
  exit(0);
}

// fork() con verificación de error.
static int
fork1(void)
{
  int pid;

  pid = fork();
  if (pid == -1) {
    fprintf(2, "sh: error al crear el proceso (fork)\n");
    exit(1);
  }
  return pid;
}
