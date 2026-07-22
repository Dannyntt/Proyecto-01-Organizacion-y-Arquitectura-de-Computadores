// sh.c - Shell principal para xv6
// Proyecto de Sistemas Operativos 2026-2
//
// Estado: esqueleto inicial para la Entrega 1 (validación del flujo de
// entrega). La implementación completa de ejecución de comandos,
// redirecciones y tuberías se desarrollará para la Entrega 2.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/parser.h"
#include "user/commands.h"
#include "user/utils.h"

#define MAXLINE 128

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

  // Bucle principal del shell.
  while (getcmd(buf, sizeof(buf))) {
    // Elimina el salto de línea final.
    int len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
      buf[len - 1] = 0;

    // TODO (Entrega 2): reemplazar por el parser real (parser.c)
    // que construya la estructura de comando y detecte
    // redirecciones (<, >) y tuberías (|).
    if (strcmp(buf, "exit") == 0) {
      exit(0);
    }

    if (buf[0] == 0)
      continue;

    // TODO (Entrega 2): delegar a commands.c la creación del
    // proceso (fork/exec) y el manejo de descriptores de archivo.
    int pid = fork();
    if (pid < 0) {
      fprintf(2, "sh: error al crear el proceso\n");
      continue;
    }
    if (pid == 0) {
      // Proceso hijo: por ahora solo soporta comandos sin
      // argumentos ni operadores especiales. Se completará en
      // la Entrega 2 usando parser.c y commands.c.
      char *argv[2];
      argv[0] = buf;
      argv[1] = 0;
      exec(buf, argv);
      fprintf(2, "sh: comando no encontrado: %s\n", buf);
      exit(1);
    } else {
      wait(0);
    }
  }

  exit(0);
}
