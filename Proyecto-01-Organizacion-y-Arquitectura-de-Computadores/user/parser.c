// parser.c - Análisis de la línea de comandos ingresada por el usuario
//
// Convierte una línea de texto (ej. "cat archivo.txt | grep error > out.txt")
// en un árbol de estructuras `struct cmd` que commands.c sabe ejecutar.
//
// Gramática soportada (según el alcance funcional del enunciado):
//   linea      -> tuberia
//   tuberia    -> comando ('|' comando)*
//   comando    -> (palabra | redireccion)*
//   redireccion-> '<' palabra | '>' palabra

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/parser.h"

// Caracteres considerados espacio en blanco y símbolos especiales.
static char whitespace[] = " \t\r\n\v";
static char symbols[] = "<|>";

static struct cmd* parsepipe(char **ps, char *es);
static struct cmd* parseexec(char **ps, char *es);
static struct cmd* parseredirs(struct cmd *cmd, char **ps, char *es);
static int gettoken(char **ps, char *es, char **q, char **eq);
static int peek(char **ps, char *es, char *toks);
static char* mkcopy(char *s, char *es);

// Constructores de cada tipo de nodo del árbol de comandos.
struct cmd*
execcmd(void)
{
  struct execcmd *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = CMD_EXEC;
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int mode, int fd)
{
  struct redircmd *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = CMD_REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = CMD_PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parsepipe(&s, es);

  // Si sobró texto sin consumir, hay un error de sintaxis.
  peek(&s, es, "");
  if (s != es) {
    fprintf(2, "sh: sintaxis no reconocida cerca de: %s\n", s);
    exit(1);
  }
  return cmd;
}

// tuberia -> comando ('|' comando)*
static struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if (peek(ps, es, "|")) {
    gettoken(ps, es, 0, 0); // consume '|'
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

// comando -> (palabra | redireccion)*
static struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok;
  struct execcmd *cmd;
  struct cmd *ret;

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  ret = parseredirs(ret, ps, es);
  while (!peek(ps, es, "|")) {
    tok = gettoken(ps, es, &q, &eq);
    if (tok == 0)
      break;
    if (tok != 'a') {
      fprintf(2, "sh: token inesperado en la línea de comandos\n");
      exit(1);
    }
    if (cmd->nargv >= MAXARGS - 1) {
      fprintf(2, "sh: demasiados argumentos\n");
      exit(1);
    }
    cmd->argv[cmd->nargv] = mkcopy(q, eq);
    cmd->nargv++;
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[cmd->nargv] = 0;
  return ret;
}

// redireccion -> ('<' palabra | '>' palabra)*
static struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while (peek(ps, es, "<>")) {
    tok = gettoken(ps, es, 0, 0);
    if (gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(2, "sh: se esperaba un nombre de archivo tras '%c'\n", tok);
      exit(1);
    }
    switch (tok) {
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), O_RDONLY, 0);
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), O_WRONLY | O_CREATE | O_TRUNC, 1);
      break;
    }
  }
  return cmd;
}

// Avanza 'ps' hasta el próximo token y retorna:
//   0            si llegó al final de la línea
//   '<', '>', '|' si encontró ese símbolo
//   'a'          si encontró una palabra (argumento o nombre de archivo)
// 'q' y 'eq' (si no son nulos) marcan el inicio/fin del texto del token.
static int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;

  s = *ps;
  while (s < es && strchr(whitespace, *s))
    s++;
  if (q)
    *q = s;

  ret = *s;
  switch (*s) {
  case 0:
    break;
  case '|':
  case '<':
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if (eq)
    *eq = s;

  while (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

// Como gettoken(), pero sin consumir: solo informa si el próximo
// carácter no-blanco pertenece al conjunto 'toks'.
static int
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while (s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

// Copia el rango [s, es) a un nuevo buffer terminado en '\0'.
static char*
mkcopy(char *s, char *es)
{
  int n = es - s;
  char *c = malloc(n + 1);
  memmove(c, s, n);
  c[n] = 0;
  return c;
}
