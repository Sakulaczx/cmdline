#ifndef __SHELL_H
#define __SHELL_H

#include "cmd.h"

#define CONFIG_SYS_MAXARGS      30
#define CONFIG_SYS_CBSIZE       256
#define CONFIG_SYS_PROMPT "CMDLINE>"

int cmd_never_repeatable(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[], int *repeatable);
int cli_readline_into_buffer(const char *const prompt, char *buffer,
                 int timeout);
int cli_readline(const char *const prompt);
void cli_simple_loop(void);
int cli_simple_parse_line(char *line, char *argv[]);
static char *delete_char (char *buffer, char *p, int *colp, int *np, int plen);
int cmd_auto_complete(const char *prompt, char *buf, int *np, int *colp);
static int make_argv(char *s, int argvsz, char *argv[]);
static void print_argv(const char *banner, const char *leader, const char *sep,
               int linemax, char *const argv[]);
static int find_common_prefix(char *const argv[]);
int cli_simple_run_command(const char *cmd, int flag);




#endif