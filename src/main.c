#include <stdio.h>
#include "shell.h"
#include "cmd.h"

extern vector cmdvec;
extern ctc_cmd_desc_t** matched_desc_ptr;


CTC_CLI(ctc_cli_common_gateway,
        ctc_cli_common_gateway_cmd,
        "toutes sabc (add|del) gateway",
        "Route",
        "Add",
        "Delete",
        "Default gateway",
        "IP address")
{
    char ret = 0;

    // if (0 == sal_strcmp(argv[0], "add"))
    // {
    //     is_add = TRUE;
    // }

    // ret = sal_inet_pton(AF_INET, argv[1], &gw);
    // if (ret == 0)
    // {
    //     ctc_cli_out("Invalid IP address: %s\n");
    //     return -1;
    // }

    // ret = ctc_cli_set_gw(argv[1], is_add);
    // if (ret < 0)
    // {
    //     ctc_cli_out("%% Set gateway fail\n");
    //     return CLI_ERROR;
    // }

    return CLI_SUCCESS;
}

CTC_CLI(ctc_cli_common_tftp_debug,
        ctc_cli_common_tftp_debug_cmd,
        "tftp server-ip VALUE filename FILENAME",
        "tftp transmit",
        "Server ip address",
        "Input server ip address",
        "File name",
        "Input file name")
{
    //int32 ret = CTC_E_NONE;
    char idx;
    uint32 tftp, serverip;
    // idx = GET_ARGC_INDEX("tftp");
    // if (idx != 0xFF) {

    // }
    //CTC_CLI_GET_UINT32("tftp", tftp, argv[0]);
    CTC_CLI_GET_UINT32("server-ip", serverip, argv[1]);


    printf("tftp: %d server-ip:%d!!\r\n", tftp, serverip );

    printf("hahahhah!!\r\n");
    return CLI_SUCCESS;
}



ctc_cmd_node_t exec_node =
{
    0,
    "\rCTC_CLI(ctc-sdk)# ",
};


int main(int argc, char const *argv[])
{
    // printf("Hello World!\n");
    // puts("asdfasd");

    cmdvec = ctc_vti_vec_init(VECTOR_MIN_SIZE);
    matched_desc_ptr = (ctc_cmd_desc_t**)malloc(sizeof(ctc_cmd_desc_t*) * CMD_ARGC_MAX);
    if (!cmdvec || !matched_desc_ptr)
    {
        printf("\nError: no memory!!");
    }
    sal_memset(matched_desc_ptr, 0 , sizeof(ctc_cmd_desc_t*) * CMD_ARGC_MAX);

    ctc_install_node(&exec_node, NULL);
    ctc_install_element(0, &ctc_cli_common_gateway_cmd);
    ctc_install_element(0, &ctc_cli_common_tftp_debug_cmd);


    cli_simple_loop();

    system("pause");
    return 0;
}
