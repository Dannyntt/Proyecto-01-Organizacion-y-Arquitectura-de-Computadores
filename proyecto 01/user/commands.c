// commands.c - Ejecución de comandos, redirecciones y tuberías
// Estado: esqueleto para la Entrega 1. La lógica de fork/exec por
// cada etapa de la tubería y el manejo de descriptores de archivo
// para las redirecciones se implementará en la Entrega 2.

#include "kernel/types.h"
#include "user/user.h"
#include "user/commands.h"

// TODO (Entrega 2): implementar runcmd(struct cmd *cmd)
// - CMD_EXEC:  fork + exec del comando con sus argumentos.
// - CMD_REDIR: abrir el archivo indicado y duplicar el descriptor
//              correspondiente (0 para '<', 1 para '>') antes del exec.
// - CMD_PIPE:  crear el pipe, hacer fork para cada lado y conectar
//              los descriptores de lectura/escritura.
