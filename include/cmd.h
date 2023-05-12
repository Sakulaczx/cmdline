#ifndef __CMD_H
#define __CMD_H



#define GET_CLI_COMMAND(_str)  \
({\
    for (idx = 0; idx < argc; idx++) {\
        if (!strcmp(argv[idx], _str))\
            break;\
    }\
    if (idx == argc) idx = -1;\
    idx;\
})

#define CLEAR_ALL_CMD_FLAG()    \
({                              \
    for (int i = 0; i < sizeof(cmd_optb_arg3)/sizeof(cmd_optb_arg3[0]); i++) {                \
        cmd_optb_arg3[i].flag = 0;                \
    }                                           \
    for (int i = 0; i < sizeof(cmd_opcc_arg4)/sizeof(cmd_opcc_arg4[0]); i++) {                \
        cmd_opcc_arg4[i].flag = 0;                \
    }                                           \
})

#define GET_CLI_COMMAND(_str)  \
({\
    for (idx = 0; idx < argc; idx++) {\
        if (!strcmp(argv[idx], _str))\
            break;\
    }\
    if (idx == argc) idx = -1;\
    idx;\
})


#define CMD_TAB_ALL_TB_ARG3()\
({\
    if (!strcmp(argv[2], cmd_arg2[0])) {\
        for (int i = 0; i < sizeof(cmd_optb_arg3)/sizeof(cmd_optb_arg3[0]); i++) {\
            if (memcmp(cmd, cmd_optb_arg3[i].cmd, len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_optb_arg3[i].cmd;\
        }\
    }\
})

#define CMD_TAB_ALL_TB_ARG4() \
({\
    if (!strcmp(argv[2], cmd_arg2[0])) {\
        for (int i = 0; i < sizeof(cmd_tb_cntmode_arg4)/sizeof(cmd_tb_cntmode_arg4[0]); i++) {\
            if (memcmp(cmd, cmd_tb_cntmode_arg4[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_tb_cntmode_arg4[i];\
        }\
        for (int i = 0; i < sizeof(cmd_tb_loadmode_arg4)/sizeof(cmd_tb_loadmode_arg4[0]); i++) {\
            if (memcmp(cmd, cmd_tb_loadmode_arg4[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_tb_loadmode_arg4[i];\
        }\
        for (int i = 0; i < sizeof(cmd_tb_loadevent_arg4)/sizeof(cmd_tb_loadevent_arg4[0]); i++) {\
            if (memcmp(cmd, cmd_tb_loadevent_arg4[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_tb_loadevent_arg4[i];\
        }\
    }\
})

#define CMD_TAB_ALL_CC_ARG35()\
({\
    if (!strcmp(argv[2], cmd_arg2[1])) {\
        for (int i = 0; i < sizeof(cmd_cc_arg3)/sizeof(cmd_cc_arg3[0]); i++) {\
            if (memcmp(cmd, cmd_cc_arg3[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_cc_arg3[i];\
        }\
        for (int i = 0; i < sizeof(cmd_cc_shadow_arg5)/sizeof(cmd_cc_shadow_arg5[0]); i++) {\
            if (memcmp(cmd, cmd_cc_shadow_arg5[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_cc_shadow_arg5[i];\
        }\
    }\
})

#define CMD_TAB_ALL_CC_ARG4()\
({\
    if (!strcmp(argv[2], cmd_arg2[1])) {\
        for (int i = 0; i < sizeof(cmd_opcc_arg4)/sizeof(cmd_opcc_arg4[0]); i++) {\
            if (memcmp(cmd, cmd_opcc_arg4[i].cmd, len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_opcc_arg4[i].cmd;\
        }\
    }\
})

#define CMD_TAB_ALL_AQ_ARG3()\
({\
    if (!strcmp(argv[2], cmd_arg2[2])) {\
        for (int i = 0; i < sizeof(cmd_aq_arg3)/sizeof(cmd_aq_arg3[0]); i++) {\
            if (memcmp(cmd, cmd_aq_arg3[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_aq_arg3[i];\
        }\
    }\
})

#define CMD_TAB_ALL_AQ_ARG4()\
({\
    if (!strcmp(argv[2], cmd_arg2[2])) {\
        for (int i = 0; i < sizeof(cmd_aq_arg4)/sizeof(cmd_aq_arg4[0]); i++) {\
            if (memcmp(cmd, cmd_aq_arg4[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_aq_arg4[i];\
        }\
    }\
})

#define CMD_TAB_ALL_AQ_ARG5()\
({\
    if (!strcmp(argv[2], cmd_arg2[2])) {\
        for (int i = 0; i < sizeof(cmd_aq_arg5)/sizeof(cmd_aq_arg5[0]); i++) {\
            if (memcmp(cmd, cmd_aq_arg5[i], len) != 0)\
                continue;\
            cmdv[n_found++] = cmd_aq_arg5[i];\
        }\
    }\
})

#define CMD_ARG2_PARSE() \
({\
    if (!strcmp(argv[argc - 1], cmd_arg2[0]) ) {\
        for (int i = 0; i < sizeof(cmd_optb_arg3)/sizeof(cmd_optb_arg3[0]); i++) {\
            cmdv[n_found++] = cmd_optb_arg3[i].cmd;\
        }\
    }\
    if (!strcmp(argv[argc - 1], cmd_arg2[1]) ) {\
        for (int i = 0; i < sizeof(cmd_cc_arg3)/sizeof(cmd_cc_arg3[0]); i++) {\
            cmdv[n_found++] = cmd_cc_arg3[i];\
        }\
    }\
    if (!strcmp(argv[argc - 1], cmd_arg2[2]) ) {\
        for (int i = 0; i < sizeof(cmd_aq_arg3)/sizeof(cmd_aq_arg3[0]); i++) {\
            cmdv[n_found++] = cmd_aq_arg3[i];\
        }\
    }\
})

#define CMD_OPCC_ARG4_PARSE() \
({\
    if (!strcmp(argv[2], cmd_arg2[1])) {\
        if (!strcmp(argv[argc - 1], cmd_opcc_arg4[0].cmd)) {\
            cmd_opcc_arg4[0].flag = 1;\
            for (int i = 0; i < sizeof(cmd_val)/sizeof(cmd_val[0]); i++) {\
                cmdv[n_found++] = cmd_val[i];\
            }\
        }\
        if (!strcmp(argv[argc - 1], cmd_opcc_arg4[1].cmd)) {\
            cmd_opcc_arg4[1].flag = 1;\
            for (int i = 0; i < sizeof(cmd_cc_shadow_arg5)/sizeof(cmd_cc_shadow_arg5[0]); i++) {\
                cmdv[n_found++] = cmd_cc_shadow_arg5[i];\
            }\
        }\
        if (!strcmp(argv[argc - 1], cmd_opcc_arg4[2].cmd)) {\
            cmd_opcc_arg4[2].flag = 1;\
            for (int i = 0; i < sizeof(cmd_val)/sizeof(cmd_val[0]); i++) {\
                cmdv[n_found++] = cmd_en[i];\
            }\
        }\
    }\
})

#define CMD_OPTB_ARG3_PARSE() \
({\
    if (!strcmp(argv[2], cmd_arg2[0]) ) {\
        if (!strcmp(argv[argc - 1], cmd_optb_arg3[0].cmd) ) {\
            cmd_optb_arg3[0].flag = 1;\
            for (int i = 0; i < sizeof(cmd_tb_cntmode_arg4)/sizeof(cmd_tb_cntmode_arg4[0]); i++) {\
                cmdv[n_found++] = cmd_tb_cntmode_arg4[i];\
            }\
        }\
        if (!strcmp(argv[argc - 1], cmd_optb_arg3[1].cmd) ) {\
            cmd_optb_arg3[1].flag = 1;\
            for (int i = 0; i < sizeof(cmd_val)/sizeof(cmd_val[0]); i++) {\
                cmdv[n_found++] = cmd_val[i];\
            }\
        }\
        if (!strcmp(argv[argc - 1], cmd_optb_arg3[2].cmd) ) {\
            cmd_optb_arg3[2].flag = 1;\
            for (int i = 0; i < sizeof(cmd_tb_loadmode_arg4)/sizeof(cmd_tb_loadmode_arg4[0]); i++) {\
                cmdv[n_found++] = cmd_tb_loadmode_arg4[i];\
            }\
        }\
        if (!strcmp(argv[argc - 1], cmd_optb_arg3[3].cmd) ) {\
            cmd_optb_arg3[3].flag = 1;\
            for (int i = 0; i < sizeof(cmd_tb_loadevent_arg4)/sizeof(cmd_tb_loadevent_arg4[0]); i++) {\
                cmdv[n_found++] = cmd_tb_loadevent_arg4[i];\
            }\
        }\
    }\
})


#define CMD_AQ_ARG5_PARSE()\
({\
    if (!strcmp(argv[2], cmd_arg2[2]) ) {\
        for (int i = 0; i < sizeof(cmd_aq_arg5)/sizeof(cmd_aq_arg5[0]); i++) {\
                cmdv[n_found++] = cmd_aq_arg5[i];\
        }\
    }\
})

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

typedef struct 
{
    /* data */
    char *cmd;
    char flag;
} option_cmd_t;

struct cmd_tbl *find_cmd(char *const argv[], int argc);
struct cmd_tbl *find_cmd_tbl(char *const argv[], int argc, struct cmd_tbl *table,
                 int table_len);
int cmd_process(int flag, int argc, char *const argv[],
                   int *repeatable, unsigned long *ticks);
static int cmd_call(struct cmd_tbl *cmdtp, int flag, int argc,
            char *const argv[], int *repeatable);
int complete_subcmdv(struct cmd_tbl *cmdtp, int count, int argc,
             char *const argv[], char last_char,
             int maxv, char *cmdv[]);
int complete_cmdv(int argc, char *const argv[], char last_char,
             int maxv, char *cmdv[]);


#endif