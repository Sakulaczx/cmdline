#include <stdio.h>
#include <conio.h>

#define CONFIG_SYS_MAXARGS      30

#define CONFIG_SYS_CBSIZE       256
#define CONFIG_SYS_PROMPT "CMDLINE>"

#define ll_entry_declare(_type, _name, _list)               \
    _type _cmd_list_2_##_list##_2_##_name __attribute__((aligned (4)))  \
            __attribute__((used))               \
 //           __section(".cmd_list")

int _CMDLINE_START = 0;

#define ll_entry_start(_type, _list)                    \
({                                  \
    extern void *_CMDLINE_START; \
    (_type *)&_CMDLINE_START;                           \
})

#define ll_entry_end(_type, _list)                  \
({                                  \
    extern void *_CMDLINE_END; \
    (_type *)&_CMDLINE_END;                         \
})

#define ll_entry_count(_type, _list)                    \
    ({                              \
        _type *start = ll_entry_start(_type, _list);        \
        _type *end = ll_entry_end(_type, _list);        \
        unsigned int _ll_result = end - start;          \
        _ll_result;                     \
    })

char *cmdname = "asdfdsa";
char console_buffer[CONFIG_SYS_CBSIZE + 1]; /* console I/O buffer   */
static char tmp_buf[CONFIG_SYS_CBSIZE + 1]; /* copy of console I/O buffer */

char *cmdtest[20][5] = {
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},
    {"quyqwva", "aonf", "cadds", "kasdf", "dsac"},
    {"abcd", "bbcd", "cbcd", "ebcd", "dsac"},

};
char *cmdtest1[5] = {"abcd", "bbcd", "cbcd", "ebcd"};

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

static int testcmdfunc( struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
    printf("Hello!\r\n");
    return 0;
}

//struct cmd_tbl *testcmd = {"adafasdf", 2, cmd_never_repeatable, testcmdfunc, "asdfs",NULL};
struct cmd_tbl testcmd = {0};
struct cmd_tbl *p_testcmd = NULL;
struct cmd_tbl testcmd2 = {"adafasdf", 2, cmd_never_repeatable, testcmdfunc, "asdfs", NULL};

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

struct cmd_tbl *find_cmd_tbl(const char *cmd, struct cmd_tbl *table,
                 int table_len)
{
    struct cmd_tbl *cmdtp;
    struct cmd_tbl *cmdtp_temp = table; /* Init value */
    const char *p;
    int len;
    int n_found = 0;

    if (!cmd)
        return NULL;
    /*
     * Some commands allow length modifiers (like "cp.b");
     * compare command name only until first dot.
     */
    len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

    for (cmdtp = table; cmdtp != table + table_len; cmdtp++) 
    {
        if (strncmp(cmd, cmdtp->name, len) == 0) 
        {
            if (len == strlen(cmdtp->name))
                return cmdtp;   /* full match */

            cmdtp_temp = cmdtp; /* abbreviated command ? */
            n_found++;
        }
    }
    if (n_found == 1) 
    {           /* exactly one match */
        return cmdtp_temp;
    }

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

    if (argc == 0 || last_char == ' ')
    {
        for (int i = 0; i < 5; i++) {
            if (n_found >= maxv - 2)
            {
                cmdv[n_found++] = "...";
                break;
            }
            // if (argc == 0) cmdv[n_found++] = cmdtest[i];
            // if (argc == 1) cmdv[n_found++] = cmdtest1[i];
            // else 
                cmdv[n_found++] = cmdtest[argc][i];
        }
        /* output full list of commands */
        // for (; cmdtp != cmdend; cmdtp++) 
        // {
        //     if (n_found >= maxv - 2)
        //     {
        //         cmdv[n_found++] = "...";
        //         break;
        //     }
        //     cmdv[n_found++] = cmdtp->name;
        // }
        cmdv[n_found] = NULL;
        return n_found;
    }

    /* more than one arg or one but the start of the next */
    // if (argc > 1 || last_char == '\0' || isblank(last_char))
    // {
    //         cmdtp = find_cmd_tbl(argv[0], cmdtp, count);
    //     if (cmdtp == NULL || cmdtp->complete == NULL)
    //     {
    //         cmdv[0] = NULL;
    //         return 0;
    //     }
    //     return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
    // }
    
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
    for (int i = 0; i < sizeof(cmdtest[argc - 1])/sizeof(cmdtest[argc - 1][0]); i++) {
        clen = strlen(cmdtest[argc - 1][i]);
        if (memcmp(cmd, cmdtest[argc - 1][i], len) != 0)
             continue;
        cmdv[n_found++] = cmdtest[argc - 1][i];
    }

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
    testcmd.cmd = testcmdfunc;

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
        //puts(prompt);
        //putchar('>');

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
                return p - p_buf;

            default:
                if (c == '\t') {
                    //printf("a");    

                    *p = '\0';
                    if (cmd_auto_complete(prompt,
                                  console_buffer,
                                  &n, &col)) 
                    {
                        p = p_buf + n;  /* reset */
                        continue;
                    }

                    printf("%s", tab_seq + (col & 07));
                    col += 8 - (col & 07);

                }
                else 
                {
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
    }

}

int main(int argc, char const *argv[])
{
    // printf("Hello World!\n");
    // printf("Hello World!\n");
    // puts("asdfasd");
    cli_simple_loop();


    system("pause");
    return 0;
}
