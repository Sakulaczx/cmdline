#include "cmd.h"
#include "shell.h"
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>



vector cmdvec = NULL;

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

ctc_cmd_desc_t** matched_desc_ptr  = NULL;



int32
ctc_is_cmd_var(char* cmd)
{
    int32 index = 0;

    if (cmd[0] == '<')
    {
        return 1;
    }

    for (index = 0; index < strlen(cmd); index++)
    {
        if ((cmd[index] >= 'A') && (cmd[index] <= 'Z'))
        {
            return 1;
        }
    }

    return 0;
}



void
ctc_vti_vec_ensure(vector v, uint32 num)
{
    if (v->alloced > num)
    {
        return;
    }

    v->index = realloc(v->index, sizeof(void*) * (v->alloced * 2));
    sal_memset(&v->index[v->alloced], 0, sizeof(void*) * v->alloced);
    v->alloced *= 2;

    if (v->alloced <= num)
    {
        ctc_vti_vec_ensure(v, num);
    }
}



int
ctc_vti_vec_empty_slot(vector v)
{
    uint32 i;

    if (v->max == 0)
    {
        return 0;
    }

    for (i = 0; i < v->max; i++)
    {
        if (v->index[i] == 0)
        {
            return i;
        }
    }

    return i;
}


int
ctc_vti_vec_set(vector v, void* val)
{
    uint32 i;

    i = ctc_vti_vec_empty_slot(v);
    ctc_vti_vec_ensure(v, i);

    v->index[i] = val;

    if (v->max <= i)
    {
        v->max = i + 1;
    }

    return i;
}

/* Check same string element existence.  If it isn't there return
    1. */
int32
ctc_cmd_unique_string(vector v, char* str)
{
    int32 i;
    char* match;

    for (i = 0; i < vector_max(v); i++)
    {
        if ((match = vector_slot(v, i)) != NULL)
        {
            if (sal_strcmp(match, str) == 0)
            {
                return 0;
            }
        }
    }

    return 1;
}


vector
ctc_vti_vec_init(uint32 size)
{
    vector v = malloc(sizeof(struct _vector));

    if (!v)
    {
        return NULL;
    }
    sal_memset(v, 0, sizeof(struct _vector));

    /* allocate at least one slot */
    if (size == 0)
    {
        size = 1;
    }

    v->alloced = size;
    v->max = 0;
    v->index = malloc(sizeof(void*) * size);
    if (!v->index)
    {
        free(v);
        return NULL;
    }

    sal_memset(v->index, 0, sizeof(void*) * size);
    v->direction = 0;
    v->is_desc = 0;
    v->is_multiple = 0;

    return v;
}


uint32
ctc_vti_vec_count(vector v)
{
    uint32 i;
    uint32 count = 0;

    for (i = 0; i < v->max; i++)
    {
        if (v->index[i] != NULL)
        {
            count++;
        }
    }

    return count;
}

char*
cmd_parse_token(char** cp, cmd_token_type* token_type)
{
    char* sp = NULL;
    char* token = NULL;
    int32 len = 0;
    int32 need_while = 1;

    if (**cp == '\0')
    {
        *token_type = cmd_token_unknown;
        return NULL;
    }

    while (**cp != '\0' && need_while)
    {
        switch (**cp)
        {
        case ' ':
            (*cp)++;
            ;
            break;

        case '{':
            (*cp)++;
            *token_type = cmd_token_cbrace_open;
            return NULL;

        case '(':
            (*cp)++;
            *token_type = cmd_token_paren_open;
            return NULL;

        case '|':
            (*cp)++;
            *token_type = cmd_token_separator;
            return NULL;

        case ')':
            (*cp)++;
            *token_type = cmd_token_paren_close;
            return NULL;

        case '}':
            (*cp)++;
            *token_type = cmd_token_cbrace_close;
            return NULL;

        case '\n':
            (*cp)++;
            break;

        case '\r':
            (*cp)++;
            break;

        default:
            need_while = 0;
            break;
        }
    }

    sp = *cp;

    while (!(**cp == ' ' || **cp == '\r' || **cp == '\n' || **cp == ')' || **cp == '(' || **cp == '{' || **cp == '}' || **cp == '|') && **cp != '\0')
    {
        (*cp)++;
    }

    len = *cp - sp;

    if (len)
    {
        token = malloc(len + 1);
        if (token == NULL)
        {
            return NULL;
        }
        sal_memcpy(token, sp, len);
        *(token + len) = '\0';
        if (CTC_CMD_VARIABLE(token))
        {
            *token_type = cmd_token_var;
        }
        else
        {
            *token_type = cmd_token_keyword;
        }

        return token;
    }

    *token_type = cmd_token_unknown;
    return NULL;
}

vector
ctc_vti_vec_copy(vector v)
{
    uint32 size;
    vector new_v = malloc(sizeof(struct _vector));

    if (!new_v)
    {
        return NULL;
    }
    memset(new_v, 0, sizeof(struct _vector));

    new_v->max = v->max;
    new_v->alloced = v->alloced;
    new_v->direction = v->direction;
    new_v->is_desc = v->is_desc;
    new_v->is_multiple = v->is_multiple;

    size = sizeof(void*) * (v->alloced);
    new_v->index = (void**)malloc(size);
    if (!new_v->index)
    {
        free(new_v);
        return NULL;
    }

    sal_memcpy(new_v->index, v->index, size);

    return new_v;
}

vector
cmd_make_cli_tree(ctc_cmd_desc_t* tmp_desc, char** descstr, vector parent, int32* dp_index, int32 depth)
{
    cmd_token_type token_type = 0;

    char* token = NULL;
    vector cur_vec = NULL;
    vector pending_vec = NULL;
    vector sub_parent_vec = NULL;
    ctc_cmd_desc_t* desc = NULL;
    int32 flag = 0;
    vector p = NULL;

    while (*(tmp_desc->str) != '\0')
    {
        token = cmd_parse_token(&tmp_desc->str, &token_type);

        switch (token_type)
        {
        case cmd_token_paren_open:
            cur_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
            cur_vec->is_desc = 0;
            cur_vec->is_multiple = 0;
            cur_vec->direction = 0;
            if (flag)    /* '(' after '|', finish previous building */
            {
                flag++;
                if (flag == 2)  /* flag==2 first keyword or VAR after seperator */
                {
                    pending_vec = cur_vec;
                }
                else if (flag == 3)  /* 2 words are after seperator, current and pending vectors belong to sub_parent_vec */
                {
                    sub_parent_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                    ctc_vti_vec_set(sub_parent_vec, pending_vec);
                    ctc_vti_vec_set(sub_parent_vec, cur_vec);
                    ctc_vti_vec_set(parent, sub_parent_vec);
                }
                else    /* 2 more words are after seperator */
                {
                    ctc_vti_vec_set(sub_parent_vec, cur_vec);     /* all more vectors belong to sub_parent_vec */
                }
            }
            else
            {
                ctc_vti_vec_set(parent, cur_vec);
            }

            cmd_make_cli_tree(tmp_desc, descstr, cur_vec, dp_index, depth + 1);
            cur_vec = NULL;
            break;

        case cmd_token_cbrace_open:
            cur_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
            cur_vec->is_desc = 0;
            cur_vec->is_multiple = 1;     /* this is difference for {} and(), other codes are same */
            cur_vec->direction = 0;
            if (flag)
            {
                flag++;
                if (flag == 2)
                {
                    pending_vec = cur_vec;
                }
                else if (flag == 3)
                {
                    sub_parent_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                    ctc_vti_vec_set(sub_parent_vec, pending_vec);
                    ctc_vti_vec_set(sub_parent_vec, cur_vec);
                    ctc_vti_vec_set(parent, sub_parent_vec);
                }
                else
                {
                    ctc_vti_vec_set(sub_parent_vec, cur_vec);
                }
            }
            else
            {
                ctc_vti_vec_set(parent, cur_vec);
            }

            cmd_make_cli_tree(tmp_desc, descstr, cur_vec, dp_index, depth + 1);
            cur_vec = NULL;
            break;

        case cmd_token_paren_close:
        case cmd_token_cbrace_close:
            if (flag == 1)
            {
                parent->is_option = 1;
            }
            else if (flag == 2)  /* flag==2 first keyword after seperator, only one keyword  */
            {
                ctc_vti_vec_set(parent, pending_vec);
            }

            flag = 0;
            return parent;
            break;

        case cmd_token_separator:
            if (!parent->direction && (ctc_vti_vec_count(parent) > 1)) /* if current parent is tranverse and has more than 2 vector, make it a sub parent*/
            {
                p = ctc_vti_vec_copy(parent);
                sal_memset(parent->index, 0, sizeof(void*) * parent->max);
                vector_reset(parent);
                ctc_vti_vec_set(parent, p);
            }

            parent->direction = 1;
            if (flag == 2)    /* new seperator starts, finish previous */
            {
                ctc_vti_vec_set(parent, pending_vec);
            }

            flag = 1;     /*flag=1, new seperator starts*/
            cur_vec = NULL;
            break;

        case cmd_token_keyword:
            if (!cur_vec)
            {
                cur_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                cur_vec->direction = 0;
                cur_vec->is_multiple = 0;
                cur_vec->is_desc = 1;

                if (flag)
                {
                    flag++;
                    if (flag == 2)  /* flag==2 first keyword or VAR after seperator */
                    {
                        pending_vec = cur_vec;
                    }
                    else if (flag == 3)  /* flag==3 seconds keyword or VAR after seperator */
                    {
                        sub_parent_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                        ctc_vti_vec_set(sub_parent_vec, pending_vec);
                        ctc_vti_vec_set(sub_parent_vec, cur_vec);
                        ctc_vti_vec_set(parent, sub_parent_vec);
                    }
                    else     /* flag>3, more keywords */
                    {
                        ctc_vti_vec_set(sub_parent_vec, cur_vec);
                    }
                }
                else
                {
                    ctc_vti_vec_set(parent, cur_vec);
                }
            }

            desc = malloc(sizeof(ctc_cmd_desc_t));
            if(NULL == desc)
            {
                return NULL;
            }
            sal_memset(desc, 0, sizeof(ctc_cmd_desc_t));
            desc->cmd = token;
            desc->str = descstr[*dp_index];
            if (depth > 0)
            {
                desc->is_arg = 1;
            }
            else
            {
                desc->is_arg = 0;
            }

            (*dp_index)++;

            ctc_vti_vec_set(cur_vec, desc);
            break;

        case cmd_token_var:
            if (!cur_vec)
            {
                cur_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                if(NULL == cur_vec)
                {
                    return NULL;
                }
                cur_vec->direction = 0;
                cur_vec->is_multiple = 0;
                cur_vec->is_desc = 1;

                if (flag)    /* deal with seperator */
                {
                    flag++;
                    if (flag == 2)  /* flag==2 first keyword or VAR after seperator */
                    {
                        pending_vec = cur_vec;
                    }
                    else if (flag == 3)  /* flag==3 seconds keyword or VAR after seperator */
                    {
                        sub_parent_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
                        ctc_vti_vec_set(sub_parent_vec, pending_vec);
                        ctc_vti_vec_set(sub_parent_vec, cur_vec);
                        ctc_vti_vec_set(parent, sub_parent_vec);
                    }
                    else     /* flag>3, more keywords or VAR */
                    {
                        ctc_vti_vec_set(sub_parent_vec, cur_vec);
                    }
                }
                else
                {
                    ctc_vti_vec_set(parent, cur_vec);
                }
            }

            desc = malloc(sizeof(ctc_cmd_desc_t));
            if (desc == NULL)
            {
                break;
            }
            sal_memset(desc, 0, sizeof(ctc_cmd_desc_t));
            desc->cmd = token;
            desc->str = descstr[*dp_index];
            desc->is_arg = 1;
            desc->is_var = 1;
            (*dp_index)++;

            ctc_vti_vec_set(cur_vec, desc);
            break;

        default:
            break;
        }
    }

    return parent;
}


vector
ctc_cmd_make_descvec(char* string, char** descstr)
{
    vector all_vec = NULL;
    int32 dp_index = 0;
    ctc_cmd_desc_t tmp_desc;

    tmp_desc.str = string;

    if (string == NULL)
    {
        return NULL;
    }

    all_vec = ctc_vti_vec_init(VECTOR_MIN_SIZE);

    if (all_vec == NULL)
    {
        return NULL;
    }

    all_vec->is_desc = 0;
    all_vec->direction = 0;
    all_vec->is_multiple = 0;

    return cmd_make_cli_tree(&tmp_desc, descstr, all_vec, &dp_index, 0);
}

void
cmd_dump_vector_tree(vector all_vec, int32 depth)
{
    vector cur_vec = NULL;
    int32 i = 0;
    int32 j = 0;
    int32 space = 0;
    ctc_cmd_desc_t* desc;

    for (i = 0; i < vector_max(all_vec); i++)
    {
        cur_vec = vector_slot(all_vec, i);

        for (space = 0; space < depth; space++)
        {
            printf("  ");
        }

        printf("%d:", i);
        if (cur_vec->direction)
        {
            printf("V:");
        }
        else
        {
            printf("T:");
        }

        if (cur_vec->is_desc)
        {
            printf("s:");
        }
        else
        {
            printf("v:");
        }

        printf("\n\r");

        if (cur_vec->is_desc)
        {
            for (space = 0; space < depth; space++)
            {
                printf("  ");
            }

            for (j = 0; j < vector_max(cur_vec); j++)
            {
                desc = vector_slot(cur_vec, j);
                printf("  %s ", desc->cmd);
            }

            printf("\n\r");
        }
        else
        {
            cmd_dump_vector_tree(cur_vec, depth + 1);
        }
    }
}

int32
ctc_cmd_cmdsize(vector strvec)
{
    int32 i;
    int32 size = 0;

    for (i = 0; i < vector_max(strvec); i++)
    {
        /*
        vector descvec;
        descvec = vector_slot(strvec, i);
        */
    }

    return size;
}

void
ctc_install_element(char ntype, ctc_cmd_element_t* cmd)
{
    ctc_cmd_node_t* cnode;
    if(NULL == cmd)
    {
        printf("cmd null ptr\n");
        return;
    }

    cnode = vector_slot(cmdvec, ntype);

    if ((cnode == NULL) || (cmd == NULL))
    {
        printf("Command node %d doesn't exist, please check it\n", ntype);
        return;
    }

    ctc_vti_vec_set(cnode->cmd_vector, cmd);

    cmd->strvec = ctc_cmd_make_descvec(cmd->string, cmd->doc);
    cmd->cmdsize = ctc_cmd_cmdsize(cmd->strvec);

    printf("cmdsize=%d for cmd: %s\n\r", cmd->cmdsize, cmd->string);
    if (cmd->strvec->direction)
    {
        printf("Parent V\n\r");
    }
    else
    {
        printf("Parent T\n\r");
    }

    // ctc_cmd_desc_t* desc = NULL;
    // ctc_cmd_element_t* cmd_ele = NULL;
    // vector cur_vec = NULL;

    // cmd_ele = vector_slot(cnode->cmd_vector, 0);
    // cur_vec = vector_slot(cmd_ele->strvec, 0);
    // desc = vector_slot(cur_vec, 0);

    cmd_dump_vector_tree(cmd->strvec, 0);
}


// 内存分配函数
void* util_malloc(int size)
{
    void* ptr = malloc(size);

    if (ptr == NULL) // 如果分配失败，则终止程序
    {
        printf("Memory allocation error!\n");
        //exit(EXIT_FAILURE);
    }

    // 分配成功，则返回
    return ptr;
}

// 生成多叉树节点
NODE* create_node()
{
    NODE* q;

    q = (NODE*)util_malloc(sizeof (NODE));
    q->n_children = 0;
    q->level      = 0;
    q->children   = NULL;

    return q;
}

//tb cmd
// NODE arg53le = {"zero-sync", 0, 3};
// NODE arg52le = {"sync", 0, 1};
// NODE arg51le = {"zero", 0, 3};

// NODE arg52lm = {"directly", 0, 1};
// NODE arg51lm = {"shadow", 0, 4};

// NODE arg52p = {"<cr>", 0, 1};
// NODE arg51p = {"VAULE", 0, 3};

// NODE arg54c = {"stop", 0, 3};
// NODE arg53c = {"up-down", 0, 3};
// NODE arg52c = {"down", 0, 3};
// NODE arg51c = {"up", 0, 3};

// NODE arg44t = {"loadevent", 3, 3,{&arg51le,&arg52le,&arg53le}};
// NODE arg43t = {"loadmode", 2, 3, {&arg51lm,&arg52lm}};
// NODE arg42t = {"period", 2, 3, {&arg51p,&arg52p}};
// NODE arg41t = {"cntmode", 4, 3, {&arg51c,&arg52c,&arg53c,&arg54c}};

// //cc cmd
// NODE arg67s = {"sync", 0, 5};
// NODE arg66s = {"sync-zero-prd", 0, 5};
// NODE arg65s = {"sync-zero", 0, 3};
// NODE arg64s = {"freeze", 0, 3};
// NODE arg63s = {"zero-prd", 0, 3};
// NODE arg62s = {"period", 0, 3};
// NODE arg61s = {"zero", 0, 3};

// NODE arg62g = {"disable", 0, 5};
// NODE arg61g = {"enable", 0, 5};

// NODE arg53cc = {"gloable", 2, 4, {&arg61g,&arg62g}};
// NODE arg52cc = {"shadow-mode", 7, 4, {&arg62s,&arg63s,&arg64s,&arg65s,&arg66s,&arg67s,&arg61s}};
// NODE arg51cc = {"counter", 2, 4, {&arg51p,&arg52p}};


// NODE arg44c = {"cmpd", 3, 3, {&arg51cc,&arg52cc,&arg53cc}};
// NODE arg43c = {"cmpc", 3, 3, {&arg51cc,&arg52cc,&arg53cc}};
// NODE arg42c = {"cmpb", 3, 3, {&arg51cc,&arg52cc,&arg53cc}};
// NODE arg41c = {"cmpa", 3, 3, {&arg51cc,&arg52cc,&arg53cc}};

// //  arg3
// NODE arg31 = {"timebase", 4, 2, {&arg41t,&arg42t,&arg43t,&arg44t}};
// NODE arg32 = {"cntcomp", 4, 2, {&arg41c,&arg42c,&arg43c,&arg44c}};
// NODE arg33 = {"actqualifier", 0, 2, {NULL,NULL,NULL,NULL}};
// NODE arg34 = {"tripzone", 0, 2, {NULL,NULL,NULL,NULL}};

// // arg2
// NODE arg22 = {"<cr>", 0, 1};
// NODE arg21 = {"VALUE", 4, 1, {&arg31, &arg32, &arg33, &arg34}};

// // arg1
 //NODE arg1 = {"PWM", 2, 0, {&arg21, &arg22}};
 //NODE arg1 = {"PWM", 2, 0, };

NODE* search_node_r(char *name, NODE* head)
{
    NODE* temp = NULL;
    int i = 0;

    if (head != NULL)
    {
        //if (strcmp(name, head->name) == 0 && lev == head->level) // 如果名字匹配
        if (strcmp(name, head->name) == 0) // 如果名字匹配
        {
            temp = head;
        }
        else // 如果不匹配，则查找其子节点
        {
            //lev += 1;
            for (i = 0; i < head->n_children && temp == NULL/*如果temp不为空，则结束查找*/; ++i)
            {
                //temp = search_node_r(lev, name, head->children[i]); // 递归查找子节点
                temp = search_node_r(name, head->children[i]); // 递归查找子节点
            }
        }
    }

    return temp; // 将查找到的节点指针返回，也有可能没有找到，此时temp为NULL
}


NODE* creat_cmd_tree()
{
    static NODE* temp_n = NULL;
    static NODE* head = NULL;
    char name[20];
    int num;
    char child[20];
    char *t_cmd[]= {// pwm VAULE { timebase {cntmode |period|loadmode|loadevent}| cntcomp | actqualifier | tripzone}
                     {"pwm 2 VALUE <cr>"}, 
                     {"VALUE 4 timebase cntcomp actqualifier tripzone"}, 
                     // tb cmd
                     {"timebase 4 cntmode period loadmode loadevent"}, 
                     {"period 2 VALUE <cr>"}, 
                     {"loadmode 3 zero sync zero-and-sync"}, 
                     {"cntmode 4 up down up-down stop"}, 
                     {"loadevent 2 shadow directly"}, 
                     // cc cmd
                     {"cntcomp 4 cmpa cmpb cmpc cmpd"}, 
                     {"cmpa 3 counter shadow-mode gloable"},
                     {"cmpb 3 counter shadow-mode gloable"},
                     {"cmpb 3 counter shadow-mode gloable"},
                     {"cmpd 3 counter shadow-mode gloable"},
                     {"gloable 2 enable disable"},
                     {"counter 2 VALUE <cr>"},
                     {"shadow-mode 7 zero period zero-prd freeze sync-zero sync-zero-prd sync"},
                     // aq cmd
                     {"counter 2 VALUE <cr>"},

                     //{"cmpb 1 cmpa"} 
                     //{"cc 3 aaaab bbbbb ccccc"},
                    };
    char level;

    if (head != NULL) 
        return head;


    for (int j =0; j < sizeof(t_cmd)/sizeof(t_cmd[0]); j++) {
        sscanf(t_cmd[j], "%s %d", name, &num);
        t_cmd[j] += strlen(name) + 3;
        if (temp_n == NULL) // 若为空
        {
            temp_n = create_node();   // 生成一个新节点
            temp_n->name = strdup(name); // 赋名
            head = temp_n;
        }
        else
        {
            temp_n = search_node_r(name, head); // 根据name找到节点
            level = temp_n->level;
            // 这里默认数据文件是正确的，一定可以找到与name匹配的节点
            // 如果不匹配，那么应该忽略本行数据
        }
        temp_n->n_children = num;
        temp_n->children   = (NODE**)malloc(num * sizeof (NODE*));
        if (temp_n->children == NULL) // 分配内存失败
        {
            printf("Dynamic allocation error!\n");
            //exit(EXIT_FAILURE);
        }
        for (int i = 0; i < num; ++i)
        {
            sscanf(t_cmd[j], "%s", child); // 读取子节点
            t_cmd[j] += strlen(child) + 1;
            temp_n->children[i] = create_node(); // 生成子节点
            temp_n->children[i]->name = strdup(child);
            temp_n->children[i]->level = level+1;
        }

    }


    return head;
}

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


uint32
ctc_cmd_str2uint(char* str, int32* ret)
{
    uint32 i;
    uint32 len;
    uint32 digit;
    uint32 limit, remain;
    uint32 max = 0xFFFFFFFF;
    uint32 total = 0;

    /* Sanify check. */
    if (str == NULL || ret == NULL)
    {
        return 0xFFFFFFFF;
    }

    /* First set return value as error. */
    *ret = -1;

    len = sal_strlen(str);

    /*add for suport parser hex format*/
    if (len >= 2 && !sal_memcmp(str, "0x", 2))
    {
        if (len == 2)
        {
            *ret = -1;
            return 0xFFFFFFFF;
        }
        else if (len > 10)
        {
            *ret = -1;
            return 0xFFFFFFFF;
        }

        for (i = 2; i < len; i++)
        {
            if ((*(str + i) <= '9' && *(str + i) >= '0')
                || (*(str + i) <= 'f' && *(str + i) >= 'a')
                || (*(str + i) <= 'F' && *(str + i) >= 'A'))
            {
                /*do nothing*/
            }
            else
            {
                *ret = -1;
                return 0xFFFFFFFF;
            }
        }

        total = sal_strtou32(str, NULL, 16);
    }
    else
    {

        limit = max / 10;
        remain = max % 10;

        if (len < DECIMAL_STRLEN_MIN || len > DECIMAL_STRLEN_MAX)
        {
            *ret = -1;
            return 0xFFFFFFFF;
        }

        for (i = 0; i < len; i++)
        {
            if (*str < '0' || *str > '9')
            {
                *ret = -1;
                return 0xFFFFFFFF;
            }

            digit = *str++ - '0';

            if (total > limit || (total == limit && digit > remain))
            {
                *ret = -1;
                return 0xFFFFFFFF;
            }

            total = total * 10 + digit;
        }
    }

    *ret = 0;
    return total;
}

uint32
ctc_cmd_get_value(int32* ret, char* name, char* str, uint32 min, uint32 max, uint8 type)
{
    int32 retv = 0;
    uint32 tmp = 0;
    uint8 j = 0;
    char string[100] = {0};

    sal_memcpy(string, name, sal_strlen(name));
    tmp = ctc_cmd_str2uint((str), &retv);

    if(0 == type)   /*get uint8*/
    {
        if (retv < 0 || tmp > CTC_MAX_UINT8_VALUE || tmp < min || tmp > max)
        {
            printf("%% Invalid %s value\n", name);
            *ret = CLI_ERROR;
            return 0;
        }
    }
    else if(1 == type)  /*get uint16*/
    {
        if (retv < 0 || tmp > CTC_MAX_UINT16_VALUE || tmp < min || tmp > max)
        {
            printf("%% Invalid %s value\n", name);
            *ret = CLI_ERROR;
            return 0;
        }
    }
    else    /*get uint32*/
    {
        if (retv < 0 || tmp > CTC_MAX_UINT32_VALUE || tmp < min || tmp > max)
        {
            printf("%% Invalid %s value\n", name);
            *ret = CLI_ERROR;
            return 0;
        }
    }

    for (j = 0; j < sal_strlen(string); j++)
    {
        (string[j]) = sal_tolower((string[j]));
    }

    *ret = retv;
    return tmp;
}



unsigned char
ctc_cli_get_prefix_item(char** argv, unsigned char argc, char* prefix, unsigned char prefix_len)
{
    unsigned char index = 0;

    while (index < argc)
    {
        if ((prefix_len && !sal_strncmp(argv[index], prefix, prefix_len)) ||
            (!sal_strncmp(argv[index], prefix, sal_strlen(argv[index])) &&
             (sal_strlen(argv[index]) == sal_strlen(prefix))) )
        {
            return index;
        }

        index++;
    }

    return 0xFF;
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



int
ctc_vti_vec_set_index(vector v, uint32 i, void* val)
{
    ctc_vti_vec_ensure(v, i);

    v->index[i] = val;

    if (v->max <= i)
    {
        v->max = i + 1;
    }

    return i;
}


void
ctc_install_node(ctc_cmd_node_t* node, int32 (* func)(ctc_vti_t*))
{
    ctc_vti_vec_set_index(cmdvec, node->node, node);
    node->func = func;
    node->cmd_vector = ctc_vti_vec_init(VECTOR_MIN_SIZE);
    if (!node->cmd_vector)
    {
        printf("System error: no memory for install node!\n\r");
    }
}

int complete_subcmdv(struct cmd_tbl *cmdtp, int count, int argc,
             char *const argv[], char last_char,
             int maxv, char *cmdv[])
{
    //const struct cmd_tbl *cmdend = cmdtp + count;
    const char *p;
    int len;
    int n_found = 0;
    const char *cmd;
    NODE* temp = NULL;

    /* sanity? */
    if (maxv < 2)
        return -2;

    cmdv[0] = NULL;






    temp = creat_cmd_tree();

    if (argc == 0)
    {
        for (int i = 0; i < 1; i++) {
            //cmdv[n_found++] = arg1.name;
            cmdv[n_found++] = temp->name;

        }
        // for (int i = 0; i < 1; i++) {
        //     if (n_found >= maxv - 2)
        //     {
        //         cmdv[n_found++] = "...";
        //         break;
        //     }
        //     cmdv[n_found++] = cmd_arg1[i];
        // }
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



    if (last_char == ' ') {
        if (isdigit(argv[argc - 1][0])) {
            temp = search_node_r("VALUE", temp);            
        } else {
           // temp = search_node_r(argc-1, argv[argc - 1], &arg1);
            temp = search_node_r(argv[argc - 1], temp);

        }

        if (temp != NULL) {
            for (int i = 0; i < temp->n_children; i++) {
                cmdv[n_found++] = temp->children[i]->name;
            }
        }
        cmdv[n_found] = NULL;
           
        return n_found;
    } else {
        if (isdigit(argv[argc - 2][0])) {
            temp = search_node_r("VALUE", temp);            
        } else {
            //temp = search_node_r(argc-2, argv[argc - 2], &arg1);
            temp = search_node_r(argv[argc - 2], temp);
        }

        for (int i = 0; i < temp->n_children; i++) {
            if (memcmp(cmd, temp->children[i]->name, len) != 0)
                continue;

            cmdv[n_found++] = temp->children[i]->name;
        }        
    }
    // if (argc == 1) {
    //     if (last_char == ' ') {

    //         temp = search_node_r(argv[argc - 1], &arg1);

    //         if (temp != NULL) {
    //             for (int i = 0; i < temp->n_children; i++) {
    //                 cmdv[n_found++] = temp->children[i]->name;
    //             }
                
    //         }
            

    //         // for (int i = 1; i < 3; i++) {
    //         //     if (n_found >= maxv - 2)
    //         //     {
    //         //         cmdv[n_found++] = "...";
    //         //         break;
    //         //     }
    //         //     cmdv[n_found++] = cmd_arg1[i];
    //         // }
            
    //         cmdv[n_found] = NULL;
           
    //         return n_found;
    //     } else {
    //         for (int i = 0; i < 1; i++) {
    //             if (memcmp(cmd, cmd_arg1[i], len) != 0)
    //                 continue;

    //             cmdv[n_found++] = cmd_arg1[i];
    //         }        
    //     }
    // }
    
    
    // if (argc == 2)
    // {
    //     if (last_char == ' ') {
    //         if (isdigit(argv[argc - 1][0])) {
    //             temp = search_node_r("VALUE", &arg1);
    //             if (temp != NULL) {
    //                 for (int i = 0; i < temp->n_children; i++) {
    //                     cmdv[n_found++] = temp->children[i]->name;
    //                 }

    //             }
    //         }
    //         // for (int i = 0; i < 4; i++) {
    //         //     if (n_found >= maxv - 2)
    //         //     {
    //         //         cmdv[n_found++] = "...";
    //         //         break;
    //         //     }
    //         //     cmdv[n_found++] = cmd_arg2[i];
    //         // }
    //         cmdv[n_found] = NULL;
    //         return n_found;
    //     } else {
    //         for (int i = 0; i < 1; i++) {
    //             if (memcmp(cmd, cmd_arg1[i], len) != 0)
    //                 continue;

    //             cmdv[n_found++] = cmd_arg1[i];
    //         }
    //     }
    // } 

    // if (argc == 3)
    // {
    //     if (last_char == ' ') {
    //         CMD_ARG2_PARSE();
                       
    //         cmdv[n_found] = NULL;
    //         return n_found;
    //     } else {
    //         for (int i = 0; i < sizeof(cmd_arg2)/sizeof(cmd_arg2[0]); i++) {
    //             if (memcmp(cmd, cmd_arg2[i], len) != 0)
    //                 continue;
    //             cmdv[n_found++] = cmd_arg2[i];
    //         }
    //     }
    // }

    // if (argc == 4 || argc == 6 || argc == 8 || argc == 10) {
    //     if (last_char == ' ') {
            
    //         if (!strcmp(argv[2], cmd_arg2[1]) ) {
    //             for (int i = 0; i < sizeof(cmd_opcc_arg4)/sizeof(cmd_opcc_arg4[0]); i++) {
    //                 if (cmd_opcc_arg4[i].flag != 1)
    //                     cmdv[n_found++] = cmd_opcc_arg4[i].cmd;
    //             }               
    //         }
    //         CMD_OPTB_ARG3_PARSE();
    //         if (!strcmp(argv[2], cmd_arg2[2]) ) {
    //             for (int i = 0; i < sizeof(cmd_aq_arg4)/sizeof(cmd_aq_arg4[0]); i++) {
    //                 cmdv[n_found++] = cmd_aq_arg4[i];
    //             }               
    //         }
                       
    //         cmdv[n_found] = NULL;
    //         return n_found;
    //     } else {
    //         CMD_TAB_ALL_TB_ARG3();
    //         CMD_TAB_ALL_CC_ARG35();
    //         CMD_TAB_ALL_AQ_ARG3();
    //         CMD_TAB_ALL_AQ_ARG5();
    
    //         for (int i = 0; i < sizeof(cmd_en)/sizeof(cmd_en[0]); i++) {
    //             if (memcmp(cmd, cmd_en[i], len) != 0)
    //                 continue;
    //             cmdv[n_found++] = cmd_en[i];
    //         }
    //     }
    // }

    // if (argc == 5 || argc == 7 || argc == 9 || argc == 11) {
    //     if (last_char == ' ') {
    //         if (!strcmp(argv[2], cmd_arg2[0])) {
    //             for (int i = 0; i < 4; i++) {
    //                 if (cmd_optb_arg3[i].flag != 1)
    //                     cmdv[n_found++] = cmd_optb_arg3[i].cmd;
    //             }               
    //         }
    //         CMD_OPCC_ARG4_PARSE();
    //         CMD_AQ_ARG5_PARSE();
            
    //         cmdv[n_found] = NULL;
    //         return n_found;
    //     } else {
    //         CMD_TAB_ALL_TB_ARG4();
    //         CMD_TAB_ALL_CC_ARG4();
    //         CMD_TAB_ALL_AQ_ARG4();         
    //     }
    // }

    cmdv[n_found] = NULL;
    return n_found;
}

vector
ctc_cmd_node_vector(vector v, int ntype)
{
    ctc_cmd_node_t* cnode = vector_slot(v, ntype);

    return cnode->cmd_vector;
}


int32
ctc_cmd_CTC_RANGE_MATCH(char* range, char* str)
{
    char* p;
    char buf[DECIMAL_STRLEN_MAX + 1];
    char* endptr = NULL;
    uint32 min, max, val;

    if (str == NULL)
    {
        return 1;
    }

    val = sal_strtou32(str, &endptr, 10);
    if (*endptr != '\0')
    {
        return 0;
    }

    range++;
    p = sal_strchr(range, '-');
    if (p == NULL)
    {
        return 0;
    }

    if (p - range > DECIMAL_STRLEN_MAX)
    {
        return 0;
    }

    sal_strncpy(buf, range, p - range);
    buf[p - range] = '\0';
    min = sal_strtou32(buf, &endptr, 10);
    if (*endptr != '\0')
    {
        return 0;
    }

    range = p + 1;
    p = sal_strchr(range, '>');
    if (p == NULL)
    {
        return 0;
    }

    if (p - range > DECIMAL_STRLEN_MAX)
    {
        return 0;
    }

    sal_strncpy(buf, range, p - range);
    buf[p - range] = '\0';
    max = sal_strtou32(buf, &endptr, 10);
    if (*endptr != '\0')
    {
        return 0;
    }

    if (val < min || val > max)
    {
        return 0;
    }

    return 1;
}


ctc_match_type_t
ctc_cmd_string_match(char* str, char* command)
{
    ctc_match_type_t match_type = CTC_CTC_NO_MATCH;

    if (CTC_CMD_VARARG(str))
    {
        match_type = CTC_VARARG_MATCH;
    }
    else if (CTC_CMD_RANGE(str))
    {
        if (ctc_cmd_CTC_RANGE_MATCH(str, command))
        {
            match_type = CTC_RANGE_MATCH;
        }
    }
    else if (CTC_CMD_OPTION(str) || CTC_CMD_VARIABLE(str))
    {
        match_type = CTC_EXTEND_MATCH;
    }
    else if (sal_strncmp(command, str, sal_strlen(command)) == 0)
    {
        if (sal_strcmp(command, str) == 0)
        {
            match_type = CTC_EXACT_MATCH;
        }
        else
        {
            match_type = CTC_PARTLY_MATCH;
        }
    }

    return match_type;
}



ctc_match_type_t
ctc_cmd_filter_command_tree(vector str_vec, vector vline, int32* index, ctc_cmd_desc_t** matched_desc_ptr, int32 depth, int32* if_CTC_EXACT_MATCH)
{
    int32 j = 0;
    char* str = NULL;
    ctc_match_type_t match_type = CTC_CTC_NO_MATCH;
    vector cur_vec = NULL;
    ctc_cmd_desc_t* desc = NULL;
    char* command = NULL;
    int32 old_index = 0;
    int32 k = 0;
    int32 no_option = 0;

    while (*index < vector_max(vline))
    {
        command = vector_slot(vline, *index);
        if (!command)
        {
            return CTC_OPTION_MATCH;
        }

        if (str_vec->is_desc)
        {
            if (str_vec->direction == 0) /* Tranverse */
            {
                for (j = 0; j < vector_max(str_vec); j++)
                {
                    desc = vector_slot(str_vec, j);
                    str = desc->cmd;

                    if ((match_type = ctc_cmd_string_match(str, command)) == CTC_CTC_NO_MATCH)
                    {
                        return CTC_CTC_NO_MATCH;
                    }
                    else /* matched */
                    {
                        matched_desc_ptr[*index] = desc;
                        (*index)++;
                        if (*index < vector_max(vline))
                        {
                            command = vector_slot(vline, *index);
                            if (!command) /* next is null */
                            {
                                return CTC_OPTION_MATCH;
                            }
                        }
                        else
                        {
                            j++;
                            break;
                        }
                    }
                }

                if (j < vector_max(str_vec))
                {
                    return CTC_INCOMPLETE_CMD;
                }

                return match_type;
            }
            else /* vertical */
            {
                for (j = 0; j < vector_max(str_vec); j++)
                {
                    desc = vector_slot(str_vec, j);
                    str = desc->cmd;
                    if ((match_type = ctc_cmd_string_match(str, command)) == CTC_CTC_NO_MATCH)
                    {
                        continue;
                    }
                    else
                    {
                        matched_desc_ptr[*index] = desc;
                        (*index)++;
                        break;
                    }
                }

                if (match_type == CTC_CTC_NO_MATCH)
                {
                    if (!str_vec->is_option)
                    {
                        return CTC_CTC_NO_MATCH;
                    }
                    else /* if vetical vector and only has one element, it is optional */
                    {
                        return CTC_OPTION_MATCH;
                    }
                }
                else
                {
                    return match_type;
                }
            }
        }
        else /* shall go to next level's vector */
        {
            if (str_vec->direction == 0) /* Tranverse */
            {
                for (j = 0; j < vector_max(str_vec); j++)
                {
                    cur_vec = vector_slot(str_vec, j);
                    if (cur_vec->direction && vector_max(cur_vec) == 1) /* optinal vector */
                    {
                        while (!cur_vec->is_desc)
                        {
                            cur_vec = vector_slot(cur_vec, 0);
                        }

                        desc = vector_slot(cur_vec, 0);
                        command = vector_slot(vline, *index);
                        if (command && CTC_CMD_VARIABLE(desc->cmd) && !CTC_CMD_NUMBER(command) && !CTC_CMD_VARIABLE(command)) /* skip if input is keyword but desc is VAR */
                        {
                            printf("\n\rLine: %d, index=%d,  skip if input is keyword but desc is VAR", __LINE__, *index);

                            continue;
                        }
                    }

                    cur_vec = vector_slot(str_vec, j); /* retry to get the current vector */
                    if ((match_type = ctc_cmd_filter_command_tree(cur_vec, vline, index, matched_desc_ptr, depth + 1, if_CTC_EXACT_MATCH)) == CTC_CTC_NO_MATCH)
                    {
                        return CTC_CTC_NO_MATCH;
                    } /* else, matched, index will be increased and go on next match */

                    /* else, matched, index will be increased and go on next match */
                    if (*index >= vector_max(vline))
                    {
                        j++;
                        DEBUG("\n\rLine: %d, index=%d, j=%d: reach the end of input word", __LINE__, *index, j);

                        break;
                    }
                }

                no_option = 0;

                for (k = j; k < vector_max(str_vec); k++) /* check if all the left cmds in the tranverse list can be skipped */
                {
                    cur_vec = vector_slot(str_vec, k);
                    #if 0
                    if (!cur_vec->direction || vector_max(cur_vec) > 1) /* optional vector shall be vertical and has one cmd*/
                    {
                        no_option = 1;
                        break;
                    }
                    #endif
                    if (!cur_vec->is_option)
                    {
                        no_option = 1;
                        break;
                    }
                }

                if ((j < vector_max(str_vec)) && no_option)
                {
                    return CTC_INCOMPLETE_CMD;
                }

                /* too many input words */
                if (depth == 0 && *index != vector_max(vline) && (command = vector_slot(vline, *index)))
                {
                    printf("\n\rLine: %d, index=%d,  too more cmds", __LINE__, *index);

                    return CTC_CTC_NO_MATCH;
                }

                return match_type;
            }
            else /* Vertical */
            {
                int32 cbrace_matched = 0;
                int32 cbrace_try_result = 0;
                if (str_vec->is_multiple)
                {
                    char match_j[MAX_OPTIONAL_CMD_NUM] = {0};
                    DEBUG("\r\nLine %d: *index: %d, entering cbrace checking", __LINE__, *index);

                    do
                    {
                        cbrace_try_result = 0;

                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            if (j >= MAX_OPTIONAL_CMD_NUM)
                            {
                                DEBUG("\n\rLine: %d, index=%d,  too many optional cmds", __LINE__, *index);
                                break;
                            }

                            if (!match_j[j])
                            {
                                cur_vec = vector_slot(str_vec, j);
                                match_type = ctc_cmd_filter_command_tree(cur_vec, vline, index, matched_desc_ptr, depth + 1, if_CTC_EXACT_MATCH);
                                if (match_type == CTC_CTC_NO_MATCH)
                                {
                                    continue;
                                }
                                else if (match_type == CTC_INCOMPLETE_CMD)
                                {
                                    return CTC_INCOMPLETE_CMD;
                                }
                                else
                                {
                                    match_j[j] = 1;
                                    cbrace_matched++;
                                    cbrace_try_result++;
                                    break;
                                }
                            }
                        }
                    }
                    while (cbrace_try_result); /* if match none, shall exit loop */

                    if (cbrace_matched || str_vec->is_option)
                    {
                        DEBUG("\r\ncbrace_matched: Line %d: *index: %d, command: %s, j: %d", __LINE__, *index, command, j);

                        return CTC_OPTION_MATCH;
                    }
                    else
                    {
                        DEBUG("\r\nNone cbrace matched in Line %d: *index: %d, command: %s, j: %d", __LINE__, *index, command, j);
                    }
                }
                else /* paren:(a1 |a2 ) */
                {
                    int32 matched_j = -1;
                    ctc_match_type_t previous_match_type = CTC_CTC_NO_MATCH;
                    old_index = *index;

                    for (j = 0; j < vector_max(str_vec); j++) /* try to get best match in the paren list */
                    {
                        cur_vec = vector_slot(str_vec, j);
                        *index = old_index;
                        if (!cur_vec->is_desc)
                        {
                            match_type = ctc_cmd_filter_command_tree(cur_vec, vline, index, matched_desc_ptr, depth + 1, if_CTC_EXACT_MATCH);
                        }
                        else
                        {
                            desc = vector_slot(cur_vec, 0);
                            str = desc->cmd;
                            command = vector_slot(vline, *index);
                            match_type = ctc_cmd_string_match(str, command);
                        }

                        if (match_type > previous_match_type)
                        {
                            previous_match_type = match_type;
                            matched_j = j;
                        }
                    }

                    if (previous_match_type != CTC_CTC_NO_MATCH) /* found best match */
                    {
                        cur_vec = vector_slot(str_vec, matched_j);
                        *index = old_index;
                        match_type = ctc_cmd_filter_command_tree(cur_vec, vline, index, matched_desc_ptr, depth + 1, if_CTC_EXACT_MATCH);
                        DEBUG("\r\nLine %d: *index: %d, Found best match %d, returned type: %d",  __LINE__, *index, matched_j, match_type);

                        return match_type;
                    }
                    else /* no match */
                    {
                       if (!str_vec->is_option)
                        {
                            return CTC_CTC_NO_MATCH;
                        }
                        else  /* if vertical vector only has one element, it is optional */
                        {
                            return CTC_OPTION_MATCH;
                        }
                    }
                }

                return match_type;
            }
        }
    } /* while */

    return match_type;
}



ctc_match_type_t
ctc_cmd_filter_by_completion(vector strvec, vector vline, ctc_cmd_desc_t** matched_desc_ptr, int32* if_CTC_EXACT_MATCH)
{
    int32 index = 0;

    return ctc_cmd_filter_command_tree(strvec, vline, &index, matched_desc_ptr, 0, if_CTC_EXACT_MATCH);
}

best_match_type_t
ctc_cmd_best_match_check(vector vline, ctc_cmd_desc_t** matched_desc_ptr, int32 if_describe)
{
    int32 i = 0;
    char* command = NULL;
    char* str = NULL;
    ctc_cmd_desc_t* matched_desc = NULL;
    int32 max_index = vector_max(vline);

    if (if_describe)
    {
        max_index--;
    }

    for (i = 0; i < max_index; i++)
    {
        command = vector_slot(vline, i);
        matched_desc = matched_desc_ptr[i];
        if (command && command[0] >= 'a' && command[0] <= 'z') /* keyword format*/
        {
            str = matched_desc->cmd;
            if (CTC_CMD_VARIABLE(str))
            {
                return CTC_CMD_EXTEND_MATCH; /* extend match */
            }

            if (sal_strncmp(command, str, sal_strlen(command)) == 0)
            {
                if (sal_strcmp(command, str) == 0) /* exact match */
                {
                    continue;
                }
                else
                {
                    return CTC_CMD_PARTLY_MATCH; /* partly match */
                }
            }
        }
    }

    return CTC_CMD_EXACT_MATCH; /* exact match */
}

void
ctc_vti_vec_free(vector v)
{
    free(v->index);
    free(v);
}

int32
ctc_desc_unique_string(vector v, char* str)
{
    int32 i;
    ctc_cmd_desc_t* desc;

    for (i = 0; i < vector_max(v); i++)
    {
        if ((desc = vector_slot(v, i)) != NULL)
        {
            if (sal_strcmp(desc->cmd, str) == 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

/* If src matches dst return dst string, otherwise return NULL */
/* This version will return the dst string always if it is
   CTC_CMD_VARIABLE for '?' key processing */
char*
ctc_cmd_entry_function_desc(char* src, char* dst)
{
    if (CTC_CMD_VARARG(dst))
    {
        return dst;
    }

    if (CTC_CMD_RANGE(dst))
    {
        if (ctc_cmd_CTC_RANGE_MATCH(dst, src))
        {
            return dst;
        }
        else
        {
            return NULL;
        }
    }

    /* Optional or variable commands always match on '?' */
    if (CTC_CMD_OPTION(dst) || CTC_CMD_VARIABLE(dst))
    {
        return dst;
    }

    /* In case of 'command \t', given src is NULL string. */
    if (src == NULL)
    {
        return dst;
    }

    if (sal_strncmp(src, dst, sal_strlen(src)) == 0)
    {
        return dst;
    }
    else
    {
        return NULL;
    }
}


static ctc_cmd_desc_t desc_cr = { "<cr>", "" };

int32
ctc_cmd_describe_cmd_tree(vector vline, int32* index, vector str_vec, vector matchvec, int32 if_describe, int32 depth)
{
    int32 j = 0;
    int32 ret = 0;
    char* str = NULL;
    ctc_match_type_t match_type = CTC_CTC_NO_MATCH;
    vector cur_vec = NULL;
    ctc_cmd_desc_t* desc = NULL;
    char* command = NULL;
    char* string = NULL;
    int32 old_index  = 0;

    while (*index < vector_max(vline))
    {
        command = vector_slot(vline, *index);
            
        if (str_vec->is_desc)
        {
            if (str_vec->direction == 0) /* Tranverse */
            {
                for (j = 0; j < vector_max(str_vec); j++)
                {
                    command = vector_slot(vline, *index);
                    desc = vector_slot(str_vec, j);
                    str = desc->cmd;

                    if (command) /* not null command */
                    {
                        if ((match_type = ctc_cmd_string_match(str, command)) == CTC_CTC_NO_MATCH)
                        {
                            return 0;
                        }
                        else /* matched */
                        {
                            if (*index == (vector_max(vline) - 1))  /* command is last string*/
                            {
                                if(desc->is_var)
                                {
                                    return 0;
                                }

                                string = ctc_cmd_entry_function_desc(command, desc->cmd);
                                if (string)
                                {
                                    VECTOR_SET;
                                }

                                DEBUG("\r\nLine %d:depth: %d, *index: %d, string: %s, j: %d", __LINE__, depth, *index, string, j);

                                return 0; /* not null, last word match */
                            }
                            else /* not null, not last word */
                            {
                                (*index)++;
                                command = vector_slot(vline, *index);
                                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);
                            }
                        }
                    }
                    else /* command is null, always the last word */
                    {
                        string = ctc_cmd_entry_function_desc(command, desc->cmd);
                        if (string)
                        {
                            VECTOR_SET;
                        }

                        DEBUG("\r\nLine %d:depth: %d, *index: %d, string: %s, j: %d", __LINE__, depth, *index, string, j);

                        return 2;
                    }
                }

                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                return 1;
            }
            else /* vertical */
            {
                command = vector_slot(vline, *index);
                if (command) /* not null */
                {
                    if (*index == (vector_max(vline) - 1)) /* command is last string */
                    {
                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            desc = vector_slot(str_vec, j);
                            str = desc->cmd;
                            if ((match_type = ctc_cmd_string_match(str, command)) != CTC_CTC_NO_MATCH)
                            {
                                string = ctc_cmd_entry_function_desc(command, desc->cmd);
                                if (string)
                                {
                                    VECTOR_SET;
                                }

                                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                                return 2; /* shall match only one */
                            }
                        } /* for j */

                    }
                    else /* command not last word */
                    {
                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            desc = vector_slot(str_vec, j);
                            str = desc->cmd;
                            if ((match_type = ctc_cmd_string_match(str, command)) != CTC_CTC_NO_MATCH)
                            {
                                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                                return 1; /* shall match only one */
                            }
                        } /* for j */

                    }
                }
                else /*  last string, null command */
                {
                    for (j = 0; j < vector_max(str_vec); j++)
                    {
                        desc = vector_slot(str_vec, j);
                        str = desc->cmd;
                        string = ctc_cmd_entry_function_desc(command, desc->cmd);
                        if (string)
                        {
                            VECTOR_SET;
                        }
                    } /* for j */

                    DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                    return 2;
                }

                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                return 0;
            }
        }
        else /* shall go to next level's vector */
        {
            if (str_vec->direction == 0) /* Tranverse */
            {
                for (j = 0; j < vector_max(str_vec); j++)
                {
                    cur_vec = vector_slot(str_vec, j);
                    if (cur_vec->direction && vector_max(cur_vec) == 1) /* optinal vector */
                    {
                        while (!cur_vec->is_desc)
                        {
                            cur_vec = vector_slot(cur_vec, 0);
                        }

                        desc = vector_slot(cur_vec, 0);
                        command = vector_slot(vline, *index);
                        if (command && CTC_CMD_VARIABLE(desc->cmd) && !CTC_CMD_NUMBER(command) && !CTC_CMD_VARIABLE(command)) /* skip if input is keyword but desc is VAR */
                        {
                            continue;
                        }
                    }

                    cur_vec = vector_slot(str_vec, j); /* retry to get the current vector */
                    old_index = *index;
                    ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                    if (ret == 2)
                    {
                        if (cur_vec->direction && cur_vec->is_option && (old_index == *index)) /* optional vector */
                        { /*(old_index == *index) means index is not increased in the optional vector */
                            continue;
                        }

                        DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                        return 2;
                    }

                    if (ret == 3)
                    {
                        return 3;
                    }

                    if (ret == 0)
                    {
                        DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d ", __LINE__, depth, *index, command, j);

                        return 0;
                    }
                }

                if (!depth && (j == vector_max(str_vec)) && ((command = vector_slot(vline, *index)) == NULL)) /* all tranverse vector has been searched */
                {
                    DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d ", __LINE__, depth, *index, command, j);

                    string = "<cr>";

                    if (if_describe)
                    {
                        if (!ctc_desc_unique_string(matchvec, string))
                        {
                            ctc_vti_vec_set(matchvec, &desc_cr);
                        }
                    }
                    else
                    {
                        if (ctc_cmd_unique_string(matchvec, string))
                        {
                            ctc_vti_vec_set(matchvec, XSTRDUP(desc_cr.cmd));
                        }
                    }
                }

                DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d", __LINE__, depth, *index, command, j);

                return 1;
            }
            else /* Vertical */
            {
                if (str_vec->is_multiple) /* {a1|a2} */
                {
                    char match_j[100] = {0};
                    int32  cbrace_try_result = 0;
                    int32 cbrace_matched = 0;

                    do
                    {
                        cbrace_try_result = 0;

                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            cur_vec = vector_slot(str_vec, j);
                            command = vector_slot(vline, *index);
                            if (!command) /* it's time to match NULL */
                            {
                                break;
                            }

                            if (!match_j[j]) /* find those not searched */
                            {
                                if (*index == (vector_max(vline) - 1)) /* last word */
                                {
                                    ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                                    if (ret)
                                    {
                                        DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d, ret: %d", __LINE__, depth, *index, command, j, ret);
                                        return ret;
                                    }
                                }
                                else /* not last word */
                                {
                                    old_index = *index;
                                    ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                                    if (ret)
                                    {
                                        match_j[j] = 1; /* matched */
                                        cbrace_matched++;
                                        cbrace_try_result++;
                                        command = vector_slot(vline, *index);
                                        if ((!command || vector_max(cur_vec) > 1) && (ret == 2)) /* "a1 A1" format in one of the list */
                                        {
                                            return 3;
                                        }
                                    }
                                    else
                                    {
                                        if (*index > old_index) /* inner "a1 A1" format but no match */
                                        {
                                            return 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    while (cbrace_try_result);  /* if match none, shall exit loop */

                    if (!command)
                    {
                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            cur_vec = vector_slot(str_vec, j);
                            if (!match_j[j])
                            {
                                ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                            }
                        }

#if 1
                        if(j==vector_max(str_vec) && str_vec->is_option)
                        {
                            string = "<cr>";
                            if (if_describe)
                            {
                                if (!ctc_desc_unique_string(matchvec, string))
                                {
                                    ctc_vti_vec_set(matchvec, &desc_cr);
                                }
                            }
                            else
                            {
                                if (ctc_cmd_unique_string(matchvec, string))
                                {
                                    ctc_vti_vec_set(matchvec, XSTRDUP(desc_cr.cmd));
                                }
                            }
                        }
#endif
                    }

                    DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d, ret: %d", __LINE__, depth, *index, command, j, ret);

                    if (cbrace_matched)
                    {
                        return 1;
                    }
                    else
                    {
                    if (ret == 0 && str_vec->is_option) /* optional vector, can skill matching NULL command */
                    {
                        return 1;
                    }
                        return ret;
                    }
                } /* end of {} */
                else /*(a1|a2) */
                {
                    if (*index != (vector_max(vline) - 1)) /* not last word */
                    {
                        int32 matched_j = -1;
                        ctc_match_type_t previous_match_type = CTC_CTC_NO_MATCH;
                        old_index = *index;

                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            *index = old_index;
                            cur_vec = vector_slot(str_vec, j);
                            if (!cur_vec->is_desc)
                            {
                                match_type = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                            }
                            else
                            {
                                desc = vector_slot(cur_vec, 0);
                                str = desc->cmd;
                                command = vector_slot(vline, *index);
                                match_type = ctc_cmd_string_match(str, command);
                            }

                            if (match_type > previous_match_type)
                            {
                                previous_match_type = match_type;
                                matched_j = j;
                            }
                        }

                        if (previous_match_type != CTC_CTC_NO_MATCH) /* found best match*/
                        {
                            cur_vec = vector_slot(str_vec, matched_j);
                            *index = old_index;
                            ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                        }
                        else /* all list not matched*/
                        {
                            ret = 0;
                        }
                    }
                    else /*last word, can be null */
                    {
                        int32 if_matched = 0;

                        for (j = 0; j < vector_max(str_vec); j++)
                        {
                            cur_vec = vector_slot(str_vec, j);
                            ret = ctc_cmd_describe_cmd_tree(vline, index, cur_vec, matchvec, if_describe, depth + 1);
                            if (ret)
                            {
                                if_matched = ret;
                            }
                        }

                        ret = if_matched;
                    }

                    if (ret == 0 && str_vec->is_option) /* optional vector, can skill matching NULL command */
                    {
                        return 1;
                    }

                    DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d, ret: %d", __LINE__, depth, *index, command, j, ret);

                    return ret;
                }
            }
        }
    } /* while */

    DEBUG("\r\nLine %d:depth: %d, *index: %d, command: %s, j: %d \n\r", __LINE__, depth, *index, command, j);

    return ret;
}



vector
ctc_cmd_describe_complete_cmd(vector vline, vector cmd_vector, vector matchvec, int32 if_describe)
{
    int32 i = 0;
    int32 index = 0;
    ctc_cmd_element_t* cmd_element = NULL;


    //     ctc_cmd_desc_t* desc = NULL;
    // ctc_cmd_element_t* cmd_ele = NULL;
    // vector cur_vec = NULL;

    // cmd_ele = vector_slot(cmd_vector, 0);
    // cur_vec = vector_slot(cmd_ele->strvec, 0);
    // desc = vector_slot(cur_vec, 0);



    for (i = 0; i < vector_max(cmd_vector); i++)
    {
        index = 0;
        if ((cmd_element = vector_slot(cmd_vector, i)) != NULL)
        {
            ctc_cmd_describe_cmd_tree(vline, &index, cmd_element->strvec, matchvec, if_describe, 0);
        }
    }

    return matchvec;
}

/* Check LCD of matched command. */
int32
ctc_cmd_lcd(char** matched)
{
    int32 i;
    int32 j;
    int32 lcd = -1;
    char* s1, * s2;
    char c1, c2;

    if (matched[0] == NULL || matched[1] == NULL)
    {
        return 0;
    }

    for (i = 1; matched[i] != NULL; i++)
    {
        s1 = matched[i - 1];
        s2 = matched[i];

        for (j = 0; (c1 = s1[j]) && (c2 = s2[j]); j++)
        {
            if (c1 != c2)
            {
                break;
            }
        }

        if (lcd < 0)
        {
            lcd = j;
        }
        else
        {
            if (lcd > j)
            {
                lcd = j;
            }
        }
    }

    return lcd;
}


vector
ctc_cmd_describe_command(vector vline, int32* status)
{
    int32 i;
    int32 if_CTC_EXACT_MATCH = 0;

    vector cmd_vector;
    vector matchvec;
    ctc_match_type_t match;
    ctc_cmd_element_t* cmd_element = NULL;

    int32 best_match_type = 0;
    unsigned short matched_count[3] = {0};
    char* CTC_PARTLY_MATCH_element = NULL;
    char* CTC_EXTEND_MATCH_element = NULL;

    /* Make copy vector of current node's command vector. */
    cmd_vector = ctc_vti_vec_copy(ctc_cmd_node_vector(cmdvec, 0));
    if (!cmd_vector)
    {
        *status = CMD_SYS_ERROR;
        return NULL;
    }

    /* Prepare match vector */
    matchvec = ctc_vti_vec_init(INIT_MATCHVEC_SIZE);

    CTC_PARTLY_MATCH_element = (char*)malloc(sizeof(char) * vector_max(cmd_vector));
    if (!CTC_PARTLY_MATCH_element)
    {
        DEBUG("Error: no memory!!\n\r");
        if (matchvec)
        {
            ctc_vti_vec_free(matchvec);
        }
        ctc_vti_vec_free(cmd_vector);
        return NULL;
    }

    CTC_EXTEND_MATCH_element = (char*)malloc(sizeof(char) * vector_max(cmd_vector));
    if (!CTC_EXTEND_MATCH_element)
    {
        DEBUG("Error: no memory!!\n\r");
        if (matchvec)
        {
            ctc_vti_vec_free(matchvec);
        }
        ctc_vti_vec_free(cmd_vector);
        free(CTC_PARTLY_MATCH_element);
        return NULL;
    }


    sal_memset(CTC_PARTLY_MATCH_element, 0, sizeof(char) * vector_max(cmd_vector));
    sal_memset(CTC_EXTEND_MATCH_element, 0, sizeof(char) * vector_max(cmd_vector));

    /* filter command elements */
    if (vector_slot(vline, 0) != NULL)
    {
        for (i = 0; i < vector_max(cmd_vector); i++)
        {
            match = 0;
            if_CTC_EXACT_MATCH = 1;
            cmd_element = vector_slot(cmd_vector, i);
            if (cmd_element)
            {
                match = ctc_cmd_filter_by_completion(cmd_element->strvec, vline, matched_desc_ptr, &if_CTC_EXACT_MATCH);
                if (!match)
                {
                    vector_slot(cmd_vector, i) = NULL;
                }
                else /* matched, save the exact match element*/
                {
                    best_match_type = ctc_cmd_best_match_check(vline, matched_desc_ptr, 1);
                    matched_count[best_match_type]++;
                    if (best_match_type == CTC_CMD_PARTLY_MATCH)
                    {
                        CTC_PARTLY_MATCH_element[i] = 1;
                        CTC_EXTEND_MATCH_element[i] = 0;
                    }
                    else if (best_match_type == CTC_CMD_EXTEND_MATCH)
                    {
                        CTC_EXTEND_MATCH_element[i] = 1;
                        CTC_PARTLY_MATCH_element[i] = 0;
                    }
                    else
                    {
                        CTC_EXTEND_MATCH_element[i] = 0;
                        CTC_PARTLY_MATCH_element[i] = 0;
                    }

                    DEBUG("cmd element %d best matched %d: %s \n\r", i, best_match_type, cmd_element->string);
                }
            }
        } /* for cmd filtering */

    }

    if (matched_count[CTC_CMD_EXACT_MATCH]) /* found exact match, filter all partly and extend match elements */
    {
        for (i = 0; i < vector_max(cmd_vector); i++)
        {
            if (CTC_EXTEND_MATCH_element[i] || CTC_PARTLY_MATCH_element[i]) /* filter all other elements */
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd element %d filterd for not exact match \n\r", i);
            }
        }
    }
    else if (matched_count[CTC_CMD_PARTLY_MATCH]) /* found partly match, filter all extend match elements */
    {
        for (i = 0; i < vector_max(cmd_vector); i++)
        {
            if (CTC_EXTEND_MATCH_element[i]) /* filter all other elements */
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd element %d filterd for not exact match \n\r", i);
            }
        }
    }

    free(CTC_PARTLY_MATCH_element);
    free(CTC_EXTEND_MATCH_element);

    /* make desc vector */
    matchvec = ctc_cmd_describe_complete_cmd(vline, cmd_vector, matchvec, 1);

    ctc_vti_vec_free(cmd_vector);

    if (vector_slot(matchvec, 0) == NULL)
    {
        ctc_vti_vec_free(matchvec);
        *status = CMD_ERR_NO_MATCH;
    }
    else
    {
        *status = CMD_SUCCESS;
    }

    return matchvec;
}


ctc_match_type_t
ctc_cmd_is_cmd_incomplete(vector str_vec, vector vline, ctc_cmd_desc_t** matched_desc_ptr, int32* if_CTC_EXACT_MATCH)
{
    int32 index = 0;
    ctc_match_type_t match = 0;

    match = ctc_cmd_filter_command_tree(str_vec, vline, &index, matched_desc_ptr, 0, if_CTC_EXACT_MATCH);

    return match;
}



char* ctc_cmd_execute_command_argv[CMD_ARGC_MAX];
int32 ctc_cmd_execute_command_argc;

int32
ctc_cmd_execute_command(vector vline, ctc_cmd_element_t** cmd)
{
    int32 i = 0;
    int32 if_CTC_EXACT_MATCH = 0;
    int32 best_match_type = 0;
    vector cmd_vector = NULL;
    ctc_cmd_element_t* cmd_element = NULL;
    ctc_cmd_element_t* matched_element = NULL;
    unsigned short matched_count[4] = {0};
    int32 matched_index[4] = {0};
    int32 ret = 0;
    ctc_match_type_t match = 0;

    /* Make copy of command elements. */
    cmd_vector = ctc_vti_vec_copy(ctc_cmd_node_vector(cmdvec, 0));
    if (!cmd_vector)
    {
        return CMD_SYS_ERROR;
    }

    /* filter command elements */
    for (i = 0; i < vector_max(cmd_vector); i++)
    {
        match = 0;
        cmd_element = vector_slot(cmd_vector, i);
        if (cmd_element)
        {
            match = ctc_cmd_filter_by_completion(cmd_element->strvec, vline, matched_desc_ptr, &if_CTC_EXACT_MATCH);
            if (!match)
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd: %d: filtered \n\r", i);
            }
            else
            {
                DEBUG("cmd: %d matched type: %d: %s \n\r", i, match, cmd_element->string);

                if (CTC_INCOMPLETE_CMD == match)
                {
                    matched_count[CTC_CMD_IMCOMPLETE_MATCH]++;
                }
                else
                {
                    best_match_type = ctc_cmd_best_match_check(vline, matched_desc_ptr, 0);
                    matched_index[best_match_type] = i;
                    matched_count[best_match_type]++;
                }
            }
        }
    }

    if (!matched_count[CTC_CMD_EXACT_MATCH] && !matched_count[CTC_CMD_PARTLY_MATCH]
        && !matched_count[CTC_CMD_EXTEND_MATCH] && !matched_count[CTC_CMD_IMCOMPLETE_MATCH])
    {
        ctc_vti_vec_free(cmd_vector);
        return CMD_ERR_NO_MATCH;
    }

    if (matched_count[CTC_CMD_IMCOMPLETE_MATCH] && !matched_count[CTC_CMD_EXACT_MATCH] && !matched_count[CTC_CMD_PARTLY_MATCH]
        && !matched_count[CTC_CMD_EXTEND_MATCH])
    {
        ctc_vti_vec_free(cmd_vector);
        return CMD_ERR_INCOMPLETE;
    }

    if ((matched_count[CTC_CMD_EXACT_MATCH] > 1) ||
        (!matched_count[CTC_CMD_EXACT_MATCH]  && (matched_count[CTC_CMD_PARTLY_MATCH] > 1 || matched_count[CTC_CMD_EXTEND_MATCH] > 1) )) /* exact match found, can be 1 or more */
    {
        ctc_vti_vec_free(cmd_vector);
        return CMD_ERR_AMBIGUOUS;
    }


    if (matched_count[CTC_CMD_EXACT_MATCH]) /* single match */
    {
        matched_element = vector_slot(cmd_vector, matched_index[CTC_CMD_EXACT_MATCH]);
    }
    else if (matched_count[CTC_CMD_PARTLY_MATCH])
    {
        matched_element = vector_slot(cmd_vector, matched_index[CTC_CMD_PARTLY_MATCH]);
    }
    else
    {
        matched_element = vector_slot(cmd_vector, matched_index[CTC_CMD_EXTEND_MATCH]);
    }

    ctc_vti_vec_free(cmd_vector);

    /*retry to get new desc */
    ctc_cmd_is_cmd_incomplete(matched_element->strvec, vline, matched_desc_ptr, &if_CTC_EXACT_MATCH);

    /* Argument treatment */
    ctc_cmd_execute_command_argc = 0;

    for (i = 0; i < vector_max(vline); i++)
    {
        ctc_cmd_desc_t* desc = matched_desc_ptr [i];
        if (desc->is_arg)
        {
            if (!CTC_CMD_VARIABLE(desc->cmd)) /* keywords, use origina, user input can be partiall */
            {
                char* cp = vector_slot(vline, i);
                if (cp)
                {
                    cp = realloc(cp, sal_strlen(desc->cmd) + 1);
                    if (NULL == cp)
                    {
                        return CMD_ERR_AMBIGUOUS;
                    }
                    vector_slot(vline, i) = cp; /* cp changed,  must be freed*/
                    sal_memcpy(cp, desc->cmd, sal_strlen(desc->cmd));
                    cp[sal_strlen(desc->cmd)] = '\0';
                }
            }

            ctc_cmd_execute_command_argv[ctc_cmd_execute_command_argc++] = vector_slot(vline, i);
        }

        if (ctc_cmd_execute_command_argc >= CMD_ARGC_MAX)
        {
            return CMD_ERR_EXEED_ARGC_MAX;
        }
    }

    ret = (*matched_element->func)(matched_element, ctc_cmd_execute_command_argc, ctc_cmd_execute_command_argv);


    return ret;
}


char**
ctc_cmd_complete_command(vector vline, int32* status)
{
    int32 i = 0;
    int32 if_CTC_EXACT_MATCH = 0;
    int32 index = vector_max(vline) - 1;
    int32 lcd = 0;
    vector cmd_vector = NULL;
    vector matchvec = NULL;
    ctc_cmd_element_t* cmd_element = NULL;
    ctc_match_type_t match = 0;
    char** match_str = NULL;

    int32 best_match_type = 0;
    unsigned short matched_count[3] = {0};
    char* CTC_PARTLY_MATCH_element = NULL;
    char* CTC_EXTEND_MATCH_element = NULL;

    if (vector_slot(vline, 0) == NULL)
    {
        *status = CMD_ERR_NOTHING_TODO;
        return match_str;
    }

    /* Make copy of command elements. */
    cmd_vector = ctc_vti_vec_copy(ctc_cmd_node_vector(cmdvec, 0));

    // ctc_cmd_desc_t* desc = NULL;
    // ctc_cmd_element_t* cmd_ele = NULL;
    // vector cur_vec = NULL;

    // cmd_ele = vector_slot(cmd_vector, 0);
    // cur_vec = vector_slot(cmd_ele->strvec, 0);
    // desc = vector_slot(cur_vec, 0);

    if (!cmd_vector)
    {
        DEBUG("Error: no memory!!\n\r");
        return NULL;
    }

    CTC_PARTLY_MATCH_element = (char*)malloc(sizeof(char) * vector_max(cmd_vector));
    CTC_EXTEND_MATCH_element = (char*)malloc(sizeof(char) * vector_max(cmd_vector));
    if (!CTC_PARTLY_MATCH_element || !CTC_EXTEND_MATCH_element)
    {
        DEBUG("Error: no memory!!\n\r");
        if (CTC_PARTLY_MATCH_element)
        {
            free(CTC_PARTLY_MATCH_element);
        }

        if (CTC_EXTEND_MATCH_element)
        {
            free(CTC_EXTEND_MATCH_element);
        }
        ctc_vti_vec_free(cmd_vector);
        return NULL;
    }
    sal_memset(CTC_PARTLY_MATCH_element, 0, sizeof(char) * vector_max(cmd_vector));
    sal_memset(CTC_EXTEND_MATCH_element, 0, sizeof(char) * vector_max(cmd_vector));

    /* filter command elements */

    for (i = 0; i < vector_max(cmd_vector); i++)
    {
        match = 0;
        cmd_element = vector_slot(cmd_vector, i);
        if_CTC_EXACT_MATCH = 1;
        if (cmd_element)
        {
            match = ctc_cmd_filter_by_completion(cmd_element->strvec, vline, matched_desc_ptr, &if_CTC_EXACT_MATCH);
            if (!match)
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd element %d filtered \n\r", i);
            }
            else
            {
                best_match_type = ctc_cmd_best_match_check(vline, matched_desc_ptr, 1);
                matched_count[best_match_type]++;
                if (best_match_type == CTC_CMD_PARTLY_MATCH)
                {
                    CTC_PARTLY_MATCH_element[i] = 1;
                    CTC_EXTEND_MATCH_element[i] = 0;
                }
                else if (best_match_type == CTC_CMD_EXTEND_MATCH)
                {
                    CTC_EXTEND_MATCH_element[i] = 1;
                    CTC_PARTLY_MATCH_element[i] = 0;
                }
                else
                {
                    CTC_EXTEND_MATCH_element[i] = 0;
                    CTC_PARTLY_MATCH_element[i] = 0;
                }
                DEBUG("cmd element %d best match %d: %s \n\r", i, best_match_type, cmd_element->string);
            }
        }
    } /* for cmd filtering */

    if (matched_count[CTC_CMD_EXACT_MATCH]) /* found exact match, filter all partly and extend match elements */
    {
        for (i = 0; i < vector_max(cmd_vector); i++)
        {
            if (CTC_EXTEND_MATCH_element[i] || CTC_PARTLY_MATCH_element[i]) /* filter all other elements */
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd element %d filterd for not exact match \n\r", i);
            }
        }
    }
    else if (matched_count[CTC_CMD_PARTLY_MATCH]) /* found partly match, filter all extend match elements */
    {
        for (i = 0; i < vector_max(cmd_vector); i++)
        {
            if (CTC_EXTEND_MATCH_element[i]) /* filter all other elements */
            {
                vector_slot(cmd_vector, i) = NULL;
                DEBUG("cmd element %d filterd for not exact match \n\r", i);
            }
        }
    }

    free(CTC_PARTLY_MATCH_element);
    free(CTC_EXTEND_MATCH_element);

    /* Prepare match vector. */
    matchvec = ctc_vti_vec_init(INIT_MATCHVEC_SIZE);
    if (!matchvec)
    {
        *status = CMD_WARNING;
        ctc_vti_vec_free(cmd_vector);
        return NULL;
    }

    
    //     ctc_cmd_desc_t* desc = NULL;
    // ctc_cmd_element_t* cmd_ele = NULL;
    // vector cur_vec = NULL;

    // cmd_ele = vector_slot(cmd_vector, 0);
    // cur_vec = vector_slot(cmd_ele->strvec, 0);
    // desc = vector_slot(cur_vec, 0);

    
    

    matchvec = ctc_cmd_describe_complete_cmd(vline, cmd_vector, matchvec, 0);

    /* We don't need cmd_vector any more. */
    ctc_vti_vec_free(cmd_vector);

    /* No matched command */
    if (vector_slot(matchvec, 0) == NULL)
    {
        ctc_vti_vec_free(matchvec);

        /* In case of 'command \t' pattern.  Do you need '?' command at
         the end of the line. */
        if (vector_slot(vline, index) == NULL)
        {
            *status = CMD_ERR_NOTHING_TODO;
        }
        else
        {
            *status = CMD_ERR_NO_MATCH;
        }

        return NULL;
    }

    /* Only one matched */
    if (vector_slot(matchvec, 1) == NULL)
    {
        match_str = (char**)matchvec->index;
        free(matchvec);
        if ((sal_strcmp(match_str[0], "<cr>") == 0) || CTC_CMD_VARIABLE(match_str[0])) /* if only cr or VAR matched, dont show it*/
        {
            free(match_str);
            *status = CMD_ERR_NOTHING_TODO;
            return NULL;
        }

        *status = CMD_COMPLETE_FULL_MATCH;
        return match_str;
    }

    /* Make it sure last element is NULL. */
    ctc_vti_vec_set(matchvec, NULL);

    /* Check LCD of matched strings. */
    if (vector_slot(vline, index) != NULL)
    {
        lcd = ctc_cmd_lcd((char**)matchvec->index);

        if (lcd)
        {
            int32 len = sal_strlen(vector_slot(vline, index));

            if (len < lcd)
            {
                char* lcdstr;

                lcdstr = malloc(lcd + 1);
                if (NULL == lcdstr)
                {
                    return NULL;
                }
                sal_memcpy(lcdstr, matchvec->index[0], lcd);
                lcdstr[lcd] = '\0';

                /* match_str =(char **) &lcdstr; */

                /* Free matchvec. */
                for (i = 0; i < vector_max(matchvec); i++)
                {
                    if (vector_slot(matchvec, i))
                    {
                        free(vector_slot(matchvec, i));
                    }
                }

                ctc_vti_vec_free(matchvec);

                /* Make new matchvec. */
                matchvec = ctc_vti_vec_init(INIT_MATCHVEC_SIZE);
                ctc_vti_vec_set(matchvec, lcdstr);
                match_str = (char**)matchvec->index;
                free(matchvec);

                *status = CMD_COMPLETE_MATCH;
                return match_str;
            }
        }
    }

    match_str = (char**)matchvec->index;
    free(matchvec);
    *status = CMD_COMPLETE_LIST_MATCH;
    return match_str;

}

void
ctc_cmd_free_strvec(vector v)
{
    int32 i;
    char* cp;

    if (!v)
    {
        return;
    }

    for (i = 0; i < vector_max(v); i++)
    {
        if ((cp = vector_slot(v, i)) != NULL)
        {
            free(cp);
        }
    }

    ctc_vti_vec_free(v);
}


vector
ctc_cmd_make_strvec(char* string)
{
    char* cp, * start, * token;
    int32 strlen;
    vector strvec;

    if (string == NULL)
    {
        return NULL;
    }

    cp = string;

    /* Skip white spaces. */
    while (sal_isspace((int32) * cp) && *cp != '\0')
    {
        cp++;
    }

    /* Return if there is only white spaces */
    if (*cp == '\0')
    {
        return NULL;
    }

    if (*cp == '!' || *cp == '#')
    {
        return NULL;
    }

    /* Prepare return vector. */
    strvec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
    if (!strvec)
    {
        return NULL;
    }

    /* Copy each command piece and set into vector. */
    while (1)
    {
        start = cp;

        while (!(sal_isspace((int32) * cp) || *cp == '\r' || *cp == '\n') && *cp != '\0')
        {
            cp++;
        }

        strlen = cp - start;
        token = malloc(strlen + 1);
        if (NULL == token)
        {
            free(strvec);
            return NULL;
        }
        sal_memcpy(token, start, strlen);
        *(token + strlen) = '\0';
        ctc_vti_vec_set(strvec, token);

        while ((sal_isspace((int32) * cp) || *cp == '\n' || *cp == '\r') && *cp != '\0')
        {
            cp++;
        }

        if (*cp == '\0')
        {
            return strvec;
        }
    }
}

char *
mem_strdup (const char *str1)
{
    char *str2;
    int  len;

    len = strlen (str1);

    str2 = malloc(len+1);

    if (str2 == NULL)
      return NULL;

    strcpy (str2, str1);

    return str2;
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


