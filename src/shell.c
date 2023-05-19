#include "shell.h"
#include "cmd.h"
#include <string.h>
#include <stdio.h>

#define isblank(c)	(c == ' ' || c == '\t')

static const char erase_seq[] = "\b \b";    /* erase sequence */
char console_buffer[CONFIG_SYS_CBSIZE + 1]; /* console I/O buffer   */
static const char   tab_seq[] = "        "; /* used to expand TABs */
static char tmp_buf[CONFIG_SYS_CBSIZE + 1]; /* copy of console I/O buffer */

extern option_cmd_t cmd_opcc_arg4[3];
extern option_cmd_t cmd_optb_arg3[4];
extern struct cmd_tbl testcmd2[5];

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
                //CLEAR_ALL_CMD_FLAG();
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
        cmd_execute(console_buffer);

        // if (len > 0)
        //     strncpy(lastcommand, console_buffer, CONFIG_SYS_CBSIZE + 1);

        // rc = cli_simple_run_command(lastcommand, flag);
    }
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

char *delete_char (char *buffer, char *p, int *colp, int *np, int plen)
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
                //printf("%s", *s);
                putchar(*s);
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

void cmd_execute(char *buf)
{
    vector vline;
    int ret;

    strcpy(tmp_buf, buf);
    vline = ctc_cmd_make_strvec(tmp_buf);
    if (vline == NULL)
    {
        return CMD_SUCCESS;
    }

    ret = ctc_cmd_execute_command(vline, NULL);

    ctc_cmd_free_strvec(vline);

    return;
}


int cmd_auto_complete(const char *prompt, char *buf, int *np, int *colp)
{
    int n = *np, col = *colp;
    char *argv[CONFIG_SYS_MAXARGS + 1];     /* NULL terminated  */
    char *cmd_argv[CONFIG_SYS_MAXARGS + 1];     /* NULL terminated  */

    char *cmdv[20] = {0};
    //char *cmdv;

    char *s, *t;
    const char *sep;
    int i = 0, j, k, len, seplen, argc, cmd_argc;
    int cnt, ret;
    char last_char;
    const char *ps_prompt = CONFIG_SYS_PROMPT;
    vector vline;
    char** matched = NULL;
    vector describe;
    ctc_cmd_desc_t* desc;

    if (strcmp(prompt, ps_prompt) != 0)
        return 0;   /* not in normal console */

    cnt = strlen(buf);
    if (cnt >= 1)
        last_char = buf[cnt - 1];
    else
        last_char = '\0';

    /* copy to secondary buffer which will be affected */
    strcpy(tmp_buf, buf);

    vline = ctc_cmd_make_strvec(tmp_buf);
    if (sal_isspace((int)tmp_buf[cnt - 1]))
    {
        ctc_vti_vec_set(vline, '\0');
    }

    // if (last_char == ' ') {
    //     describe = ctc_cmd_describe_command(vline, &ret);
    //     for (i = 0; i < vector_max(describe); i++)
    //     {
    //         if ((desc = vector_slot(describe, i)) != NULL)
    //         {
    //             if (desc->cmd[0] == '\0')
    //             {
    //                 continue;
    //             }
    //         }
    //     }

    // } 
    // else 
    if (vline != NULL) 
    {
        matched = ctc_cmd_complete_command(vline, &ret);
        if (ret != CMD_ERR_NO_MATCH && ret != CMD_ERR_NOTHING_TODO) {
            for (i = 0; matched[i] != NULL; i++) {
                cmdv[i] = matched[i];
            }
        }

    }

    ctc_cmd_free_strvec(vline);

    //cmdv = matched[0];
    /* separate into argv */
    argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);
    //cmd_argc = make_argv(testcmd2[0].name, sizeof(cmd_argv)/sizeof(cmd_argv[0]), cmd_argv);

    /* do the completion and return the possible completions */
     //i = complete_cmdv(argc, argv, last_char,
    //     sizeof(cmdv) / sizeof(cmdv[0]), cmdv);

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

int make_argv(char *s, int argvsz, char *argv[])
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

void print_argv(const char *banner, const char *leader, const char *sep,
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




int find_common_prefix(char *const argv[])
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

int cmd_never_repeatable(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[], int *repeatable)
{
    *repeatable = 0;

    return cmdtp->cmd(cmdtp, flag, argc, argv);
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







