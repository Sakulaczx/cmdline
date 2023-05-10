#include <stdio.h>
#include <conio.h>
#include <string.h>

#define CONFIG_SYS_MAXARGS      30
#define CONFIG_SYS_CBSIZE       256
#define CONFIG_SYS_PROMPT "CMDLINE>"
#define isblank(c)	(c == ' ' || c == '\t')
static const char erase_seq[] = "\b \b";    /* erase sequence */


char *cmdname = "asdfdsa";
char console_buffer[CONFIG_SYS_CBSIZE + 1]; /* console I/O buffer   */
static char tmp_buf[CONFIG_SYS_CBSIZE + 1]; /* copy of console I/O buffer */

char *cmdtest[10][5] = {
    {"auyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},

};
char *cmdtest1[5] = {"abcd", "bbcd", "cbcd", "ebcd"};


typedef struct 
{
    /* data */
    char *cmd;
    char flag;
} option_cmd_t;


char *cmd_arg1[2] = {"epwm", "0"};
char *cmd_arg2[4] = {"timebase", "cntcomp", "actqualifier", "tripzone"};
option_cmd_t cmd_op_arg3[4] = {{"cntmode", 0}, {"period", 0}, {"loadmode", 0}, {"loadevent", 0}};
char *cmd_arg4[9] = {"up", "dowm", "up-dowm", "stop", "shadow", "directly", "zero", "sync", "zero-and-sync"};
char *cmd_arg4_cntmode[4] = {"up", "dowm", "up-dowm", "stop"};
char *cmd_arg4_loadmode[2] = {"shadow", "directly"};
char *cmd_arg4_period[2] = {"VALUE", "<cr>"};
char *cmd_arg4_loadevent[3] = {"zero", "sync", "zero-and-sync"};


#define CLEAR_ALL_CMD_FLAG()    \
({                              \
    for (int i = 0; i < 4; i++) {                \
        cmd_op_arg3[i].flag = 0;                \
    }                                           \
})

#define CMD_OP_ARG3_PARSE() \
({\
    if (!strcmp(argv[argc - 1], cmd_op_arg3[0].cmd) ) {\
        cmd_op_arg3[0].flag = 1;\
        for (int i = 0; i < sizeof(cmd_arg4_cntmode)/sizeof(cmd_arg4_cntmode[0]); i++) {\
            cmdv[n_found++] = cmd_arg4_cntmode[i];\
        }\
    }\
    if (!strcmp(argv[argc - 1], cmd_op_arg3[1].cmd) ) {\
        cmd_op_arg3[1].flag = 1;\
        for (int i = 0; i < sizeof(cmd_arg4_period)/sizeof(cmd_arg4_period[0]); i++) {\
            cmdv[n_found++] = cmd_arg4_period[i];\
        }\
    }\
    if (!strcmp(argv[argc - 1], cmd_op_arg3[2].cmd) ) {\
        cmd_op_arg3[2].flag = 1;\
        for (int i = 0; i < sizeof(cmd_arg4_loadmode)/sizeof(cmd_arg4_loadmode[0]); i++) {\
            cmdv[n_found++] = cmd_arg4_loadmode[i];\
        }\
    }\
    if (!strcmp(argv[argc - 1], cmd_op_arg3[3].cmd) ) {\
        cmd_op_arg3[3].flag = 1;\
        for (int i = 0; i < sizeof(cmd_arg4_loadevent)/sizeof(cmd_arg4_loadevent[0]); i++) {\
            cmdv[n_found++] = cmd_arg4_loadevent[i];\
        }\
    }\
})


static const char   tab_seq[] = "        "; /* used to expand TABs */

struct cmd_tbl {
	char		*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
					/*
					 * Same as ->cmd() except the command
					 * tells us if it can be repeated.
					 * Replaces the old ->repeatable field
					 * which was not able to make
					 * repeatable property different for
					 * the main command and sub-commands.
					 */
	int		(*cmd_rep)(struct cmd_tbl *cmd, int flags, int argc,
				   char *const argv[], int *repeatable);
					/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl *cmd, int flags, int argc,
			       char *const argv[]);
	char		*usage;		/* Usage message	(short)	*/
	/* do auto completion on the arguments */
	int		(*complete)(int argc, char *const argv[],
				    char last_char, int maxv, char *cmdv[]);
};

int cmd_never_repeatable(struct cmd_tbl *cmdtp, int flag, int argc,
             char *const argv[], int *repeatable)
{
    *repeatable = 0;

    return cmdtp->cmd(cmdtp, flag, argc, argv);
}

#define GET_CLI_COMMAND(_str)  \
({\
    for (idx = 0; idx < argc; idx++) {\
        if (!strcmp(argv[idx], _str))\
            break;\
    }\
    if (idx == argc) idx = -1;\
    idx;\
})

static int testcmdfunc1( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello1!\r\n");
    int idx = 0;


    idx = GET_CLI_COMMAND("cntmode");
    if (idx != -1) {
        idx = GET_CLI_COMMAND("up");
        if (idx != -1) {
            printf("cntmode up!\r\n");
        }

        idx = GET_CLI_COMMAND("dowm");
        if (idx != -1) {
            printf("cntmode dowm!\r\n");
        }

        idx = GET_CLI_COMMAND("up-dowm");
        if (idx != -1) {
            printf("cntmode up-dowm!\r\n");
        }

        idx = GET_CLI_COMMAND("stop");
        if (idx != -1) {
            printf("cntmode stop!\r\n");
        }
    }

    idx = GET_CLI_COMMAND("loadmode");
    if (idx != -1) {
        idx = GET_CLI_COMMAND("shadow");
        if (idx != -1) {
            printf("loadmode shadow!\r\n");
        }
        idx = GET_CLI_COMMAND("directly");
        if (idx != -1) {
            printf("loadmode directly!\r\n");
        }
    }

    idx = GET_CLI_COMMAND("period");
    if (idx != -1) {
        printf("timebase period: %d!\r\n", atoi(argv[idx + 1]));
    }

    return 0;
}
static int testcmdfunc2( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello2!\r\n");
    return 0;
}
static int testcmdfunc3( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello3!\r\n");
    return 0;
}
static int testcmdfunc4( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello4!\r\n");
    return 0;
}
static int testcmdfunc5( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello5!\r\n");
    return 0;
}
//struct cmd_tbl *testcmd = {"adafasdf", 2, cmd_never_repeatable, testcmdfunc, "asdfs",NULL};
struct cmd_tbl testcmd = {0};
struct cmd_tbl *p_testcmd = NULL;
struct cmd_tbl testcmd2[5] = {
    {"epwm timebase {cntmode {up | dowm | up-dowm | freez}}", 20, cmd_never_repeatable, testcmdfunc1, "asdfs", NULL},
    {"epwm dc", 20, cmd_never_repeatable, testcmdfunc2, "asdfs", NULL},
    {"epwm cc", 20, cmd_never_repeatable, testcmdfunc3, "asdfs", NULL},
    {"epwm aq", 20, cmd_never_repeatable, testcmdfunc4, "asdfs", NULL},
    {"epwm tz", 20, cmd_never_repeatable, testcmdfunc5, "asdfs", NULL},
};

static int find_common_prefix(char *const argv[])
{
    int i, len;
    char *anchor, *s, *t;

    if (*argv == NULL)
        return 0;

    /* begin with max */
    anchor = *argv++;
    len = strlen(anchor);
    while ((t = *argv++) != NULL) 
    {
        s = anchor;
        for (i = 0; i < len; i++, t++, s++) 
        {
            if (*t != *s)
                break;
        }
        len = s - anchor;
    }
    return len;
}

struct cmd_tbl *find_cmd_tbl(char *const argv[], int argc, struct cmd_tbl *table,
                 int table_len)
{
    struct cmd_tbl *cmdtp;
    struct cmd_tbl *cmdtp_temp = table; /* Init value */
    const char *p;
    int len;
    int n_found = 0;
    // epwm tb {a A | b B | c C}
    if (!argv)
        return NULL;

    for (cmdtp = table; cmdtp != table + table_len; cmdtp++) 
    {
        n_found = 0;
        for (int i = 0; i < argc; i++) {
            if (strstr(cmdtp->name, argv[n_found]) == NULL) {
                break;
            }
            cmdtp_temp = cmdtp; /* abbreviated command ? */
            n_found++;

            if (n_found == 2)
                return cmdtp;
        }
        // if (strncmp(argv[n_found], cmdtp->name, len) == 0) 
        // {
        //     if (len == strlen(cmdtp->name))
        //         return cmdtp;   /* full match */

        //     cmdtp_temp = cmdtp; /* abbreviated command ? */
        //     n_found++;
        // }
    }
    // if (n_found == 1) 
    // {           /* exactly one match */
    //     return cmdtp_temp;
    // }

    return NULL;    /* not found or ambiguous command */
}

int complete_subcmdv(struct cmd_tbl *cmdtp, int count, int argc,
             char *const argv[], char last_char,
             int maxv, char *cmdv[])
{
    const struct cmd_tbl *cmdend = cmdtp + count;
    const char *p;
    int len, clen;
    int n_found = 0;
    const char *cmd;

    /* sanity? */
    if (maxv < 2)
        return -2;

    cmdv[0] = NULL;

    if (argc == 0)
    {
        for (int i = 0; i < 1; i++) {
            if (n_found >= maxv - 2)
            {
                cmdv[n_found++] = "...";
                break;
            }
            cmdv[n_found++] = cmd_arg1[i];
        }
        cmdv[n_found] = NULL;
        return n_found;
    }

    cmd = argv[argc - 1];
    /*
     * Some commands allow length modifiers (like "cp.b");
     * compare command name only until first dot.
     */
    p = strchr(cmd, '.');
    if (p == NULL)
        len = strlen(cmd);
    else
        len = p - cmd;


    if (argc == 1)
    {
        if (last_char == ' ') {
            for (int i = 0; i < 4; i++) {
                if (n_found >= maxv - 2)
                {
                    cmdv[n_found++] = "...";
                    break;
                }
                cmdv[n_found++] = cmd_arg2[i];
            }
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            for (int i = 0; i < 1; i++) {
                if (memcmp(cmd, cmd_arg1[i], len) != 0)
                    continue;

                cmdv[n_found++] = cmd_arg1[i];
            }
        }
    } 

    if (argc == 2)
    {
        if (last_char == ' ') {
            for (int i = 0; i < 4; i++) {
                if (n_found >= maxv - 2)
                {
                    cmdv[n_found++] = "...";
                    break;
                }
                cmdv[n_found++] = cmd_op_arg3[i].cmd;
            }
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            for (int i = 0; i < 4; i++) {
                if (memcmp(cmd, cmd_arg2[i], len) != 0)
                    continue;
                cmdv[n_found++] = cmd_arg2[i];
            }
        }
    }

    if (argc == 3 || argc == 5 || argc == 7 || argc == 9) {
        if (last_char == ' ') {
            CMD_OP_ARG3_PARSE();
            
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            for (int i = 0; i < 4; i++) {
                if (memcmp(cmd, cmd_op_arg3[i].cmd, len) != 0)
                    continue;
                cmdv[n_found++] = cmd_op_arg3[i].cmd;
            }
        }
    }

    if (argc == 4 || argc == 6 || argc == 8 || argc == 10) {
        if (last_char == ' ') {
            for (int i = 0; i < 4; i++) {
                if (cmd_op_arg3[i].flag != 1)
                    cmdv[n_found++] = cmd_op_arg3[i].cmd;
            }

            cmdv[n_found] = NULL;
            return n_found;
        } else {
            for (int i = 0; i < sizeof(cmd_arg4)/sizeof(cmd_arg4[0]); i++) {
                if (memcmp(cmd, cmd_arg4[i], len) != 0)
                    continue;
                cmdv[n_found++] = cmd_arg4[i];
            }
        }
    }



    /* more than one arg or one but the start of the next */


    /* return the partial matches */
    // for (; cmdtp != cmdend; cmdtp++) 
    // {

    //     clen = strlen(cmdtp->name);
    //     if (clen < len)
    //         continue;

    //     if (memcmp(cmd, cmdtp->name, len) != 0)
    //         continue;

    //     /* too many! */
    //     if (n_found >= maxv - 2) 
    //     {
    //         cmdv[n_found++] = "...";
    //         break;
    //     }

    //     cmdv[n_found++] = cmdtp->name;
    // }

    // for (int i = 0; i < 1; i++) {
    //     clen = strlen(cmdtest[argc - 1][i]);
    //     if (memcmp(cmd, cmdtest[argc - 1][i], len) != 0)
    //          continue;
    //     cmdv[n_found++] = cmd_arg1[i];
    // }

    cmdv[n_found] = NULL;
    return n_found;
}

static int complete_cmdv(int argc, char *const argv[], char last_char,
             int maxv, char *cmdv[])
{
    testcmd.name = cmdname;
    testcmd.complete = NULL;
    testcmd.cmd_rep = cmd_never_repeatable;
    testcmd.maxargs = 1;
    testcmd.usage = cmdname;
    testcmd.cmd = testcmdfunc1;

    p_testcmd = &testcmd;
    return complete_subcmdv(p_testcmd,
                1,
                argc, argv,
                last_char, maxv, cmdv);
}

static void print_argv(const char *banner, const char *leader, const char *sep,
               int linemax, char *const argv[])
{
    int ll = leader != NULL ? strlen(leader) : 0;
    int sl = sep != NULL ? strlen(sep) : 0;
    int len, i;

    if (banner)
    {
        puts("\r\n");
        puts(banner);
    }

    i = linemax;    /* force leader and newline */
    while (*argv != NULL) 
    {
        len = strlen(*argv) + sl;
        if (i + len >= linemax) 
        {
            printf("\r\n");
            if (leader)
                printf("%s",leader);
            i = ll - sl;
        } 
        else if (sep)
            printf("%s",sep);
            
        //puts(*argv++);
        printf("%s", *argv++);
        i += len;
    }
    printf("\r\n");
}

static int make_argv(char *s, int argvsz, char *argv[])
{
    int argc = 0;

    /* split into argv */
    while (argc < argvsz - 1) 
    {

        /* skip any white space */
        while (isblank(*s))
            ++s;

        if (*s == '\0') /* end of s, no more args   */
            break;

        argv[argc++] = s;   /* begin of argument string */

        /* find end of string */
        while (*s && !isblank(*s))
            ++s;

        if (*s == '\0')     /* end of s, no more args   */
            break;

        *s++ = '\0';        /* terminate current arg     */
    }
    argv[argc] = NULL;

    return argc;
}

int cmd_auto_complete(const char *prompt, char *buf, int *np, int *colp)
{
    int n = *np, col = *colp;
    char *argv[CONFIG_SYS_MAXARGS + 1];     /* NULL terminated  */
    char *cmdv[20];
    char *s, *t;
    const char *sep;
    int i, j, k, len, seplen, argc;
    int cnt;
    char last_char;
    const char *ps_prompt = CONFIG_SYS_PROMPT;

    if (strcmp(prompt, ps_prompt) != 0)
        return 0;   /* not in normal console */

    cnt = strlen(buf);
    if (cnt >= 1)
        last_char = buf[cnt - 1];
    else
        last_char = '\0';

    /* copy to secondary buffer which will be affected */
    strcpy(tmp_buf, buf);

    /* separate into argv */
    argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);

    /* do the completion and return the possible completions */
    i = complete_cmdv(argc, argv, last_char,
        sizeof(cmdv) / sizeof(cmdv[0]), cmdv);

    /* no match; bell and out */
    if (i == 0) 
    {
        if (argc > 1)   /* allow tab for non command */
            return 0;
        return 1;
    }

    s = NULL;
    len = 0;
    sep = NULL;
    seplen = 0;
    if (i == 1) 
    { /* one match; perfect */
        if (last_char != '\0' && !isblank(last_char))
            k = strlen(argv[argc - 1]);
        else
            k = 0;

        s = cmdv[0] + k;
        len = strlen(s);
        sep = " ";
        seplen = 1;
    } 
    else if (i > 1 && (j = find_common_prefix(cmdv)) != 0) 
    { /* more */
        if (last_char != '\0' && !isblank(last_char))
            k = strlen(argv[argc - 1]);
        else
            k = 0;

        j -= k;
        if (j > 0) {
            s = cmdv[0] + k;
            len = j;
        }
    }

    if (s != NULL) 
    {
        k = len + seplen;
        /* make sure it fits */
        if (n + k >= CONFIG_SYS_CBSIZE - 2) 
        {
            return 1;
        }

        t = buf + cnt;
        for (i = 0; i < len; i++)
            *t++ = *s++;
        if (sep != NULL)
            for (i = 0; i < seplen; i++)
                *t++ = sep[i];
        *t = '\0';
        n += k;
        col += k;
        //puts(t - k);
        printf("%s", t -k);
        //if (sep == NULL)
        *np = n;
        *colp = col;
    } 
    else 
    {
        print_argv(NULL, "  ", " ", 78, cmdv);
        printf("%s", prompt);
        //putchar('>');
        printf("%s", buf);
    }
    return 1;
}

static char *delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0)
        return p;

    if (*(--p) == '\t') 
    {       /* will retype the whole line */
        while (*colp > plen) 
        {
            printf("%s", erase_seq);
            (*colp)--;
        }
        for (s = buffer; s < p; ++s)
        {
            if (*s == '\t') 
            {
                printf("%s", tab_seq + ((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } else 
            {
                ++(*colp);
                printf("%s", *s);
            }
        }
    } 
    else 
    {
        printf("%s", erase_seq);
        (*colp)--;
    }
    (*np)--;

    return p;
}

int cli_readline_into_buffer(const char *const prompt, char *buffer,
                 int timeout)
{
    char *p = buffer;
    char *p_buf = p;
    int n = 0;              /* buffer index     */
    int plen = 0;           /* prompt length    */
    int col;                /* output column cnt    */
    char    c;

    /* print prompt */
    if (prompt) {
        plen = strlen(prompt);
        printf("%s", prompt);
    }
    col = plen;

    for (;;) {
        c = getch();
        //printf("0x%x\r\n", c);
        switch(c) {
            case '\r':          /* Enter        */
            case '\n':
                *p = '\0';
                //printf("\r\n");
                CLEAR_ALL_CMD_FLAG();
                return p - p_buf;

            case 0x08:          /* ^H  - backspace  */
            case 0x7F:          /* DEL - backspace  */
                p = delete_char(p_buf, p, &col, &n, plen);
                continue;

            default:
                if (c == '\t') {
                    //printf("a");    
                    *p = '\0';
                    if (cmd_auto_complete(prompt,
                                  console_buffer,
                                  &n, &col)) {
                        p = p_buf + n;  /* reset */
                        continue;
                    }

                    printf("%s", tab_seq + (col & 07));
                    col += 8 - (col & 07);
                } else {
                    char buf[2];
                    /*
                     * Echo input using puts() to force an
                     * LCD flush if we are using an LCD
                     */
                    ++col;
                    buf[0] = c;
                    buf[1] = '\0';
                    printf("%s", buf);
                }
                *p++ = c;
                ++n;
        }
    }
}

int cli_readline(const char *const prompt)
{
    /*
     * If console_buffer isn't 0-length the user will be prompted to modify
     * it instead of entering it from scratch as desired.
     */
    console_buffer[0] = '\0';

    return cli_readline_into_buffer(prompt, console_buffer, 0);
}

static int cmd_call(struct cmd_tbl *cmdtp, int flag, int argc,
            char *const argv[], int *repeatable)
{
    int result;

    result = cmdtp->cmd_rep(cmdtp, flag, argc, argv, repeatable);

    return result;
}

struct cmd_tbl *find_cmd(char *const argv[], int argc)
{
    struct cmd_tbl *cmd_start = &testcmd2[0];
    const int cmd_len = 5;
    //printf("lens is %d\r\n", len);
    return find_cmd_tbl(argv, argc, cmd_start, cmd_len);
}

int cmd_process(int flag, int argc, char *const argv[],
                   int *repeatable, unsigned long *ticks)
{
    int rc = 0;
    struct cmd_tbl *cmdtp;

    /* Look up command in command table */
    cmdtp = find_cmd(argv, argc);
    if (cmdtp == NULL)
    {
        puts("Unsupported cmd\r\n");
        return 1;
    }

    /* found - check max args */
    if (argc > cmdtp->maxargs)
        rc = -1;

    /* If OK so far, then do the command */
    if (!rc) 
    {
        int newrep;
        rc = cmd_call(cmdtp, flag, argc, argv, &newrep);
        *repeatable &= newrep;
    }

    return rc;
}

int cli_simple_parse_line(char *line, char *argv[])
{
    int nargs = 0;

    while (nargs < CONFIG_SYS_MAXARGS)
    {
        /* skip any white space */
        while (isblank(*line))
            ++line;

        if (*line == '\0') 
        {   /* end of line, no more args    */
            argv[nargs] = NULL;
            return nargs;
        }

        argv[nargs++] = line;   /* begin of argument string */

        /* find end of string */
        while (*line && !isblank(*line))
            ++line;

        if (*line == '\0') 
        {   /* end of line, no more args    */
            argv[nargs] = NULL;
            return nargs;
        }

        *line++ = '\0';     /* terminate current arg     */
    }

    return nargs;
}


int cli_simple_run_command(const char *cmd, int flag)
{
    char cmdbuf[CONFIG_SYS_CBSIZE]; /* working copy of cmd      */
    char *token;            /* start of token in cmdbuf */
    char *sep;          /* end of token (separator) in cmdbuf */
    //char finaltoken[CONFIG_SYS_CBSIZE];
    char *str = cmdbuf;
    char *argv[CONFIG_SYS_MAXARGS + 1]; /* NULL terminated  */
    int argc, inquotes;
    int repeatable = 1;
    int rc = 0;


    if (!cmd || !*cmd)
        return -1;  /* empty command */

    if (strlen(cmd) >= CONFIG_SYS_CBSIZE)
    {
        puts("## Command too long!\r\n");
        return -1;
    }

    strcpy(cmdbuf, cmd);

    /* Process separators and check for invalid
     * repeatable commands
     */

    while (*str) 
    {
        /*
         * Find separator, or string end
         * Allow simple escape of ';' by writing "\;"
         */
        for (inquotes = 0, sep = str; *sep; sep++) 
        {
            if ((*sep == '\'') &&
                (*(sep - 1) != '\\'))
                inquotes = !inquotes;

            if (!inquotes &&
                (*sep == ';') &&    /* separator        */
                (sep != str) && /* past string start    */
                (*(sep - 1) != '\\'))   /* and NOT escaped */
                break;
        }

        /*
         * Limit the token to data between separators
         */
        token = str;
        if (*sep)
        {
            str = sep + 1;  /* start of command for next pass */
            *sep = '\0';
        } 
        else 
        {
            str = sep;  /* no more commands for next pass */
        }

        /* find macros in this token and replace them */
        /* Extract arguments */
        argc = cli_simple_parse_line(token, argv);
        if (argc == 0) 
        {
            rc = -1;    /* no command at all */
            continue;
        }

        if (cmd_process(flag, argc, argv, &repeatable, NULL))
            rc = -1;

        /* Did the user stop this? */
    }

    return rc ? rc : repeatable;
}


void cli_simple_loop(void)
{
    static char lastcommand[CONFIG_SYS_CBSIZE + 1] = { 0, };
    int len;
    int flag;
    int rc = 1;

    for (;;) {
        len = cli_readline(CONFIG_SYS_PROMPT);
       // printf("%d\r\n", len);
        printf("\n");
        if (len > 0)
            strncpy(lastcommand, console_buffer, CONFIG_SYS_CBSIZE + 1);

        rc = cli_simple_run_command(lastcommand, flag);

    }
}

int main(int argc, char const *argv[])
{
    // printf("Hello World!\n");
    // puts("asdfasd");
    cli_simple_loop();

    system("pause");
    return 0;
}
