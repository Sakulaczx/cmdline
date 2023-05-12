#include "cmd.h"
#include "shell.h"
#include <stddef.h>
#include <ctype.h>

char *cmdname = "asdfdsa";

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

struct cmd_tbl testcmd = {0};
struct cmd_tbl *p_testcmd = NULL;


char *cmd_arg1[3] = {"epwm", "VALUE", "<cr>"};
char *cmd_arg2[4] = {"timebase", "cntcomp", "actqualifier", "tripzone"};

// tb cmd
option_cmd_t cmd_optb_arg3[4] = {{"cntmode", 0}, {"period", 0}, {"loadmode", 0}, {"loadevent", 0}};
char *cmd_tb_cntmode_arg4[4] = {"up", "dowm", "up-dowm", "stop"};
char *cmd_tb_loadmode_arg4[2] = {"shadow", "directly"};
char *cmd_tb_loadevent_arg4[3] = {"zero", "sync", "zero-and-sync"};


// cc cmd
char *cmd_cc_arg3[4] = {"cmpa", "cmpb", "cmpc", "cmpd"};
option_cmd_t cmd_opcc_arg4[3] = {{"counter", 0}, {"shadow", 0}, {"gloable", 0}};
char *cmd_cc_shadow_arg5[7] = {"zero", "period", "zero-prd", "freeze", "sync-zero", "sync-zero-prd", "sync"};

// aq cmd
char *cmd_aq_arg3[2] = {"epwma", "epwmb"};
char *cmd_aq_arg4[10] = {"zero", "period", "up-cmpa", "down-cmpa", "up-cmpb", "down-cmpb", "up-t1", "down-t1", "up-t2", "down-t2"};
char *cmd_aq_arg5[4] = {"no", "low", "high", "toggle"};

// tz cmd
char *cmd_tz_arg3[6] = {"tza", "tzb", "dcaevt1", "dcaevt2", "dcbevt1", "dcbevt2"};
char *cmd_tz_arg4[4] = {"high-z", "high", "low", "disable"};


//common cmd
char *cmd_en[2] = {"disable", "enable"};
char *cmd_val[2] = {"VALUE", "<cr>"};

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

struct cmd_tbl testcmd2[5] = {
    {"epwm EPWM timebase {cntmode {up | dowm | up-dowm | freez}}", 20, cmd_never_repeatable, testcmdfunc1, "asdfs", NULL},
    {"epwm init", 20, cmd_never_repeatable, testcmdfunc4, "asdfs", NULL},
    {"epwm tb", 20, cmd_never_repeatable, testcmdfunc5, "asdfs", NULL},
    {"epwm EPWM cntcomp {cmpa | cmpb | cmpc | cmpd} {counter COUNTER | global {enable | disable} | shadow {zero | period | zero-period | freeze | sync-zero | sync-period | sync-period-zero | sync}}", 20, cmd_never_repeatable, testcmdfunc2, "asdfs", NULL},
    {"epwm EPWM actqualifier {epwma | epwmb} ", 20, cmd_never_repeatable, testcmdfunc3, "asdfs", NULL},
};

struct cmd_tbl *find_cmd(char *const argv[], int argc)
{
    struct cmd_tbl *cmd_start = &testcmd2[0];
    const int cmd_len = 5;
    //printf("lens is %d\r\n", len);
    return find_cmd_tbl(argv, argc, cmd_start, cmd_len);
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
        	if (isdigit(argv[i][0])) {
                continue;
            }
            if (strstr(cmdtp->name, argv[i]) == NULL) {
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

static int cmd_call(struct cmd_tbl *cmdtp, int flag, int argc,
            char *const argv[], int *repeatable)
{
    int result;

    result = cmdtp->cmd_rep(cmdtp, flag, argc, argv, repeatable);

    return result;
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


    if (argc == 1) {
        if (last_char == ' ') {
//            cmdv[n_found++] = cmd_arg1[1];
//            cmdv[n_found++] = cmd_arg1[2];
            for (int i = 1; i < 3; i++) {
                if (n_found >= maxv - 2)
                {
                    cmdv[n_found++] = "...";
                    break;
                }
                cmdv[n_found++] = cmd_arg1[i];
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

    if (argc == 3)
    {
        if (last_char == ' ') {
            CMD_ARG2_PARSE();
                       
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            for (int i = 0; i < sizeof(cmd_arg2)/sizeof(cmd_arg2[0]); i++) {
                if (memcmp(cmd, cmd_arg2[i], len) != 0)
                    continue;
                cmdv[n_found++] = cmd_arg2[i];
            }
        }
    }

    if (argc == 4 || argc == 6 || argc == 8 || argc == 10) {
        if (last_char == ' ') {
            
            if (!strcmp(argv[2], cmd_arg2[1]) ) {
                for (int i = 0; i < sizeof(cmd_opcc_arg4)/sizeof(cmd_opcc_arg4[0]); i++) {
                    if (cmd_opcc_arg4[i].flag != 1)
                        cmdv[n_found++] = cmd_opcc_arg4[i].cmd;
                }               
            }
            CMD_OPTB_ARG3_PARSE();
            if (!strcmp(argv[2], cmd_arg2[2]) ) {
                for (int i = 0; i < sizeof(cmd_aq_arg4)/sizeof(cmd_aq_arg4[0]); i++) {
                    cmdv[n_found++] = cmd_aq_arg4[i];
                }               
            }
                       
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            CMD_TAB_ALL_TB_ARG3();
            CMD_TAB_ALL_CC_ARG35();
            CMD_TAB_ALL_AQ_ARG3();
            CMD_TAB_ALL_AQ_ARG5();
    
            for (int i = 0; i < sizeof(cmd_en)/sizeof(cmd_en[0]); i++) {
                if (memcmp(cmd, cmd_en[i], len) != 0)
                    continue;
                cmdv[n_found++] = cmd_en[i];
            }
        }
    }

    if (argc == 5 || argc == 7 || argc == 9 || argc == 11) {
        if (last_char == ' ') {
            if (!strcmp(argv[2], cmd_arg2[0])) {
                for (int i = 0; i < 4; i++) {
                    if (cmd_optb_arg3[i].flag != 1)
                        cmdv[n_found++] = cmd_optb_arg3[i].cmd;
                }               
            }
            CMD_OPCC_ARG4_PARSE();
            CMD_AQ_ARG5_PARSE();
            
            cmdv[n_found] = NULL;
            return n_found;
        } else {
            CMD_TAB_ALL_TB_ARG4();
            CMD_TAB_ALL_CC_ARG4();
            CMD_TAB_ALL_AQ_ARG4();         
        }
    }

    cmdv[n_found] = NULL;
    return n_found;
}

 int complete_cmdv(int argc, char *const argv[], char last_char,
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



