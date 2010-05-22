/**
 *  \file    main.c
 *  \brief   Main program.
 *
 *           Project: cybergod (Cyberspace administrator client program).
 *
 *           This is the main file of the program.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    Sat, May 15 2010
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <cyberspace.h>

#include "0config.h"
#include "version.h"
#include "commands.h"
#include "param_cmds.h"
#include "objects_cmds.h"


int cybersock = -1;


static int load_config(char * data)
{
        return 1;
}


static int save_config(char * data)
{
        return 1;
}


static int dump_state(char * data)
{
        return 1;
}


static int logout(char * data)
{
        return 0;
}


static cmdentry god_cmds[] = {
        {"get",  "get a parameter from cyberspace",  "parameter",       get_parameter},
        {"set",  "set a parameter in cyberspace",    "parameter value", set_parameter},
        {"add",  "add an object to cyberspace",      "object-type",     add_object},
        {"del",  "delete an object from cyberspace", NULL,              del_object},
        {"load", "load the configuration",           "filename",        load_config},
        {"save", "save the configuration",           "filename",        save_config},
        {"dump", "dump the system state",            NULL,              dump_state},
        {"exit", "exit from cyberspace",             NULL,              logout},
        {NULL, NULL}
};


int main(void)
{
        cybersock = cyberspace_connect("127.0.0.1", 2233, client_god, "Neo");

        if (cybersock < 0)
        {
                fprintf(stderr, "Error connecting cyberspace (%s).\n",
                                get_error_info(cybersock));
        }

        cli_mainloop(god_cmds, "cyber", 0);

        message_send(cybersock, CMD_DISCONNECT, 1);

        return 0;
}

