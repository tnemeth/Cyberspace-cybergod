/**
 *  \file    param_cmds.c
 *  \brief   Parameters management.
 *
 *           Project: cybergod (Cyberspace administrator client program).
 *
 *           Parameters management functions.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    Sun, May 16 2010
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <cyberspace.h>

#include "0config.h"
#include "version.h"
#include "main.h"


enum {INT, CHR, STR};


typedef struct {
        char        * name;
        unsigned char key;
        int           type;
        char        * desc;
} param_desc;


static param_desc parameter[] = {
        {"max_clients",  'm', INT, "maximum clients allowed on the server"},
        {"backup_delay", 'b', INT, "delay between backups/state-dumps"},
        {"time_speed",   't', INT, "time acceleration"},
        {"restrict_ip",  'r', STR, "IP address restriction for god/probes"},
        {NULL, 0, 0, NULL}
};


static int show_parameters(void)
{
        int i;

        printf("Parameters list:\n");
        for (i = 0 ; parameter[i].name ; i++)
        {
                printf("\t%s\t%s\n", parameter[i].name, parameter[i].desc);
        }

        return 1;
}


static int search_parameter(char * data, char ** value)
{
        int i;
        int found = -1;
        char * param = strchr(data, ' ') + 1;

        if (value)
        {
                *value = strchr(data, '=') + 1;
                *value[-1] = 0;
        }
        for (i = 0 ; parameter[i].name ; i++)
        {
                if (strcmp(parameter[i].name, param) == 0)
                {
                        found = i;
                        break;
                }
        }

        return found;
}


int get_parameter(char * data)
{
        int           len;
        int           found;
        unsigned char reply[MAX_PACKET_SIZE] = {0};

        if (strchr(data, ' ') == NULL)
        {
                return show_parameters();
        }

        found = search_parameter(data, NULL);
        exit_on(found < 0, 1, "Unknown parameter...\n");

        cyberspace_transmit(cybersock, CMD_GET_PARAM, &parameter[found].key, 1);
        len = packet_read(cybersock, reply, MAX_PACKET_SIZE);
        exit_on(len < 0, 0, "Reception error (%s)\n", get_error_info(len));
        exit_on(len == 0, 0, "Reception error(read error)\n");
        if (reply[2] != PACKET_MSG_ACK)
        {
                printf("Error receiving parameter information.\n");
                return 1;
        }

        printf("%s=", parameter[found].name);
        switch (parameter[found].type)
        {
                case INT: printf("%d\n", reply[3]);  break;
                case CHR: printf("%c\n", reply[3]);  break;
                case STR: printf("%s\n", &reply[3]); break;
        }

        return 1;
}


int set_parameter(char * data)
{
        int           len;
        char        * value;
        int           found;
        unsigned char message[MAX_PACKET_SIZE] = {0};

        if (strchr(data, ' ') == NULL)
        {
                return show_parameters();
        }
        exit_on(strchr(data, '=') == NULL, 1, "no new value given (set param=value)\n");

        found = search_parameter(data, &value);
        exit_on(found < 0, 1, "Unknown parameter...\n");

        printf("Setting %s to %s\n", parameter[found].name, value);
        len = 1;
        message[0] = parameter[found].key;
        switch (parameter[found].type)
        {
                case INT: message[1] = atoi(value);  break;
                case CHR: message[1] = value[0];     break;
                case STR: len = snprintf((char *) &message[1], LEN_IPADDR, "%s", value);
                          break;
        }

        cyberspace_transmit(cybersock, CMD_SET_PARAM, message, len + 1);
        check_reply("Error setting parameter...\n");

        return 1;
}

