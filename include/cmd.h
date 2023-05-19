#ifndef __CMD_H
#define __CMD_H


#include <stdbool.h>

#define CLI_SUCCESS           0
#define CLI_ERROR             1
#define CLI_AUTH_REQUIRED     2
#define CLI_EOL               3
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6

#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SYS_ERROR 11
#define DECIMAL_STRLEN_MIN 1

#define INIT_MATCHVEC_SIZE 10
#define MAX_OPTIONAL_CMD_NUM 256 /* max optional cmd number in {} */
#define DECIMAL_STRLEN_MAX 10
#define CTC_MAX_UINT16_VALUE            0xFFFF      /**< Max unsigned 16bit value */
#define CTC_MAX_UINT8_VALUE             0xFF        /**< Max unsigned 8bit value */
#define CTC_MAX_UINT32_VALUE            0xFFFFFFFF  /**< Max unsigned 32bit value */

#define VECTOR_MIN_SIZE 1
#define CTC_VTI_MAXHIST 50
#define uint32 unsigned int
#define int32 int
#define uint8 unsigned char
#define uint64 unsigned long long int 
#define HOST_NAME 32
#define CMD_ARGC_MAX   256

#define sal_memset      memset
#define sal_memcpy      memcpy
#define sal_isspace isspace
#define sal_strncmp strncmp
#define sal_strlen strlen
#define sal_strcmp strcmp
#define sal_strchr strchr
#define sal_strncpy strncpy
#define sal_tolower tolower
#define sal_memcmp memcmp

#define sal_strtou32(x, y, z) strtoul((char*)x, (char**)y, z)

#define DEBUG(...)

char* mem_strdup (const char *str1);

#define XSTRDUP(str)       mem_strdup(str)

#define CTC_CMD_OPTION(S)   ((S[0]) == '[')
extern int32 ctc_is_cmd_var(char* cmd);
#define CTC_CMD_VARIABLE(S) ctc_is_cmd_var(S)
#define CTC_CMD_VARARG(S)   ((S[0]) == '.')
#define CTC_CMD_RANGE(S)    ((S[0] == '<'))
#define CTC_CMD_NUMBER(S) ((S[0] <= '9') && (S[0] >= '0'))


#define vector_reset(V)   ((V)->max = 0)
#define vector_slot(V, I)  ((V)->index[(I)])
#define vector_max(V)     ((V)->max)




enum ctc_terminal_stats_e
{
    CTC_VTI_NORMAL,
    CTC_VTI_CLOSE,
    CTC_VTI_MORE,
    CTC_VTI_MORELINE,
    CTC_VTI_START,
    CTC_VTI_CONTINUE
};
typedef enum ctc_terminal_stats_e ctc_terminal_stats_t;

/* Is this vti connect to file or not */
enum ctc_terminal_type_e
{
    CTC_VTI_TERM,
    CTC_VTI_FILE,
    CTC_VTI_SHELL,
    CTC_VTI_SHELL_SERV
};
typedef enum ctc_terminal_type_e terminal_type_t;

struct ctc_vti_struct_s
{
    /* File descripter of this vty. */
    int32 fd;
    int (*printf)(struct ctc_vti_struct_s* vti, const char *szPtr, const int szPtr_len);
    int (*grep)(struct ctc_vti_struct_s* vti, const char *szPtr, const int szPtr_len);
    int (*quit)(struct ctc_vti_struct_s* vti);    
    bool grep_en;
    bool fprintf;
    bool for_en;
    int32 for_cnt;
    int32 sort_lineth;
    uint64 cmd_usec;
    int (*output)(const char *szPtr, const int szPtr_len, void*);/*used in user define shell mode(ctc_shell_mode = 2)*/
    terminal_type_t type;
    /* Node status of this vty */
    int32 node;
    /* What address is this vty comming from. */
    char* address;
    /* Privilege level of this vty. */
    int32 privilege;
    /* Failure count */
    int32 fail;
    /* Command input buffer */
    char* buf;
    /* Command cursor point */
    int32 cp;
    /* Command length */
    int32 length;
    /* Command max length. */
    int32 max;
    /* Histry of command */
    char* hist[CTC_VTI_MAXHIST];
    /* History lookup current point */
    int32 hp;
    /* History insert end point */
    int32 hindex;
    /* For current referencing point of interface, route-map,
       access-list etc... */
    void* index;
    /* For multiple level index treatment such as key chain and key. */
    void* index_sub;
    /* For escape character. */
    unsigned char escape;
    /* Current vty status. */
    ctc_terminal_stats_t status;
    /* IAC handling */
    unsigned char iac;
    /* IAC SB handling */
    unsigned char iac_sb_in_progress;
    /* Window width/height. */
    int32 width;
    int32 height;
    int32 scroll_one;
    /* Configure lines. */
    int32 lines;
    /* Terminal monitor. */
    int32 monitor;
    /* In configure mode. */
    int32 config;
    /* Timeout seconds and thread. */
    unsigned long v_timeout;
};
typedef struct ctc_vti_struct_s ctc_vti_t;


struct _vector
{
    uint32 max;                 /* max number of used slot */
    uint32 alloced;             /* number of allocated slot */
    void** index;               /* index to data */
    unsigned char direction;    /* 0 for transverse, 1for vertical*/
    unsigned char is_desc;      /* 0 for vector pointer, 1 for desc pointer */
    unsigned char is_multiple;  /* 0 for single selection (), 1 for multiple selection {} */
    unsigned char is_option;     /* 0 for cannot skip, 1 for can skip */
};
typedef struct _vector* vector;

struct ctc_cmd_node_s
{
    /* Node index. */
    char node;

    /* Prompt character at vty interface. */
    char prompt[HOST_NAME];

    /* Is this node's configuration goes to vtysh ? */
    int32 vtysh;

    /* Node's configuration write function */
    int32 (* func)(ctc_vti_t*);

    /* Vector of this node's command list. */
    vector cmd_vector;
};
typedef struct ctc_cmd_node_s ctc_cmd_node_t;




/* Command description structure. */
struct ctc_cmd_desc_s
{
    char* cmd;      /* Command string. */
    char* str;        /* Command's description. */
    uint8 is_arg;
    uint8 is_var;
};
typedef struct ctc_cmd_desc_s ctc_cmd_desc_t;

enum cmd_token_type_s
{
    cmd_token_paren_open,
    cmd_token_paren_close,
    cmd_token_cbrace_open,
    cmd_token_cbrace_close,
    cmd_token_brace_open,
    cmd_token_brace_close,
    cmd_token_separator,
    cmd_token_keyword,
    cmd_token_var,
    cmd_token_unknown
};
typedef enum cmd_token_type_s cmd_token_type;

enum ctc_match_type_e
{
    CTC_CTC_NO_MATCH,
    CTC_EXTEND_MATCH,
    CTC_IPV4_PREFIX_MATCH,
    CTC_IPV4_MATCH,
    CTC_IPV6_PREFIX_MATCH,
    CTC_IPV6_MATCH,
    CTC_RANGE_MATCH,
    CTC_VARARG_MATCH,
    CTC_PARTLY_MATCH,
    CTC_EXACT_MATCH,
    CTC_OPTION_MATCH,
    CTC_INCOMPLETE_CMD
};
typedef enum ctc_match_type_e ctc_match_type_t;

typedef enum best_match_type_s
{
    CTC_CMD_EXACT_MATCH = 0,
    CTC_CMD_PARTLY_MATCH,
    CTC_CMD_EXTEND_MATCH,
    CTC_CMD_IMCOMPLETE_MATCH
} best_match_type_t;


struct ctc_cmd_element_s
{
    char* string;       /* Command specification by string. */
    int32 (* func) (struct ctc_cmd_element_s*, int, char**);
    char** doc;         /* Documentation of this command. */
    int32 daemon;         /* Daemon to which this command belong. */
    vector strvec;      /* Pointing out each description vector. */
    int32 cmdsize;        /* Command index count. */
    char* config;       /* Configuration string */
    vector subconfig;   /* Sub configuration string */
};
typedef struct ctc_cmd_element_s ctc_cmd_element_t;



#define DEFUN(funcname, cmdname, cmdstr, helpstr) \
    int32 funcname(ctc_cmd_element_t*, int32, char**); \
    ctc_cmd_element_t cmdname = \
    { \
        cmdstr, \
        funcname, \
        helpstr \
    }; \
    int32 funcname \
        (ctc_cmd_element_t * self, int32 argc, char** argv)


#define CTC_CLI(func_name, cli_name, cmd_str, ARGS...) \
    char* cli_name ## _help[] = {ARGS, NULL}; \
    DEFUN(func_name, cli_name, cmd_str, cli_name ## _help)




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

// 定义多叉树的节点结构体
typedef struct node_t
{
    char* name;               // 节点名
    int   n_children;         // 子节点个数
    int   level;              // 记录该节点在多叉树中的层数
    struct node_t** children; // 指向其自身的子节点，children一个数组，该数组中的元素时node_t*指针
} NODE; // 对结构体重命名

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
void* util_malloc(int size);
NODE* create_node();
NODE* search_node_r(char *name, NODE* head);
vector ctc_cmd_make_strvec(char* string);
char** ctc_cmd_complete_command(vector vline, int32* status);
void ctc_install_node(ctc_cmd_node_t* node, int32 (* func)(ctc_vti_t*));
vector ctc_vti_vec_init(uint32 size);
int32 ctc_desc_unique_string(vector v, char* str);
int32 ctc_cmd_unique_string(vector v, char* str);
void ctc_install_element(char ntype, ctc_cmd_element_t* cmd);
void
ctc_cmd_free_strvec(vector v);

vector
ctc_cmd_describe_command(vector vline, int32* status);

ctc_match_type_t
ctc_cmd_is_cmd_incomplete(vector str_vec, vector vline, ctc_cmd_desc_t** matched_desc_ptr, int32* if_CTC_EXACT_MATCH);

unsigned char
ctc_cli_get_prefix_item(char** argv, unsigned char argc, char* prefix, unsigned char prefix_len);
uint32
ctc_cmd_get_value(int32* ret, char* name, char* str, uint32 min, uint32 max, uint8 type);
uint32
ctc_cmd_str2uint(char* str, int32* ret);
int
ctc_vti_vec_set(vector v, void* val);


#define GET_ARGC_INDEX(str) ctc_cli_get_prefix_item(&argv[0], argc, str, 0)

#define CTC_CLI_CHK_ARGV(argx) \
{\
         if((&argx - &argv[0]) >= argc)\
         {\
            printf("%% Invalid value\n"); \
            return CLI_ERROR;\
         }\
}

#define CTC_CLI_GET_UINT32(NAME, V, STR)                                      \
{                                                                               \
    int32 ret;                                                                  \
    CTC_CLI_CHK_ARGV(STR);                          \
    (V) =  ctc_cmd_get_value(&ret, (NAME), (STR), 0, CTC_MAX_UINT32_VALUE, 2);  \
    if(0 != ret)   \
    {                                                                               \
        return CLI_ERROR;                                                           \
    }                                                                               \
}


#define VECTOR_SET \
    if (if_describe) \
    { \
        if (!ctc_desc_unique_string(matchvec, string)) \
        { \
            ctc_vti_vec_set(matchvec, desc); \
        } \
    } \
    else \
    { \
        if (ctc_cmd_unique_string(matchvec, string)) \
        { \
            ctc_vti_vec_set(matchvec, XSTRDUP(string)); \
        } \
    }


#endif