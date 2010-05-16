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


int main(void)
{
        int sock = cyberspace_connect("127.0.0.1", 2233, client_god, "Thomas");

        if (sock < 0)
        {
                fprintf(stderr, "Error connecting cyberspace (%s).\n",
                                get_error_info(sock));
        }

        while (1);

        message_send(sock, CMD_DISCONNECT, 1);

        return 0;
}

