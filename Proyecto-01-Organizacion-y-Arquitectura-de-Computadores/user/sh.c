// sh.c - Shell principal para xv6
// Proyecto de Sistemas Operativos 2026-2
//
// Bucle principal: muestra el prompt, lee una línea, la parsea con
// parsecmd() y ejecuta el árbol de comandos resultante con runcmd()
// en un proceso hijo. El comando interno `exit` se maneja aparte,
// sin crear un proceso nuevo.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/parser.h"
#include "user/commands.h"
#include "user/utils.h"

// Lee una línea desde la entrada estándar.
// Retorna 0 si se llegó a EOF, 1 en caso contrario.
int
getcmd(char *buf, int nbuf)
{
  fprintf(2, "$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if (buf[0] == 0) // EOF
    return 0;
  return 1;
}

int
main(void)
{
  static char buf[MAXLINE];
  int pid;

  // Bucle principal del shell.
  while (getcmd(buf, sizeof(buf))) {
    // Elimina el salto de línea final que deja gets().
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
      buf[len - 1] = 0;

    if (buf[0] == 0)
      continue; // línea vacía, vuelve a mostrar el prompt

    // Comando interno: exit. Se maneja en el propio proceso del
    // shell, sin necesidad de crear un hijo.
    if (strcmp(buf, "exit") == 0)
      exit(0);

    // Cualquier otro comando: se parsea y ejecuta en un proceso hijo,
    // para que el shell (el padre) siga vivo y pueda seguir leyendo
    // comandos después de que termine.
    if ((pid = fork()) < 0) {
      fprintf(2, "sh: error al crear el proceso\n");
      continue;
    }
    if (pid == 0) {
      runcmd(parsecmd(buf));
      // runcmd() nunca retorna (siempre termina en exit()).
    }
    wait(0);
  }

  exit(0);
}