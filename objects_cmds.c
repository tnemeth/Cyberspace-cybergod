/**
 *  \file    objects_cmds.c
 *  \brief   Objects management.
 *
 *           Project: cybergod (Cyberspace administrator client program).
 *
 *           File description here...
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
#include "commands.h"


typedef struct {
        char  * name;
        char    key;
        int  (* get_parameters)(void);
} object_desc;


#define get_data(prompt, cont)                                          \
        if (cli_read_keyboard(prompt, cont) == 0)                       \
        {                                                               \
                return 0;                                               \
        }


#define send_configuration(fmt, args...)                                \
        {                                                               \
                unsigned char data[MAX_DATA_SIZE] = {0};                \
                snprintf((char *)data, MAX_DATA_SIZE, fmt, ##args);     \
                cyberspace_transmit(cybersock, CMD_ADD_OBJECT,          \
                                data, strlen((char *) data));           \
        }


static int star_parameters(void)
{
        char name[MAX_CMD_LEN] = {0};
        char mass[MAX_CMD_LEN] = {0};
        char radius[MAX_CMD_LEN] = {0};

        get_data("star/name",   name);
        get_data("star/mass",   mass);
        get_data("star/radius", radius);

        send_configuration("S:%s/%s/%s", name, mass, radius);

        return 1;
}


static int planet_parameters(void)
{
        char name[MAX_CMD_LEN] = {0};
        char dist[MAX_CMD_LEN] = {0};
        char mass[MAX_CMD_LEN] = {0};
        char radius[MAX_CMD_LEN] = {0};
        char period[MAX_CMD_LEN] = {0};

        get_data("planet/name",      name);
        get_data("planet/star-dist", dist);
        get_data("planet/mass",      mass);
        get_data("planet/radius",    radius);
        get_data("planet/period",    period);

        send_configuration("p:%s/%s/%s/%s/%s", name, dist, mass, radius, period);

        return 1;
}


static int station_parameters(void)
{
        char name[MAX_CMD_LEN] = {0};
        char dist[MAX_CMD_LEN] = {0};
        char radius[MAX_CMD_LEN] = {0};
        char period[MAX_CMD_LEN] = {0};

        get_data("station/name",      name);
        get_data("station/star-dist", dist);
        get_data("station/radius",    radius);
        get_data("station/period",    period);

        send_configuration("p:%s/%s/%s/%s", name, dist, radius, period);

        return 1;
}


static int gate_parameters(void)
{
        char name[MAX_CMD_LEN] = {0};
        char host[MAX_CMD_LEN] = {0};
        char xpos[MAX_CMD_LEN] = {0};
        char ypos[MAX_CMD_LEN] = {0};
        char zpos[MAX_CMD_LEN] = {0};

        get_data("gate/name",     name);
        get_data("gate/host",     host);
        get_data("gate/coords/x", xpos);
        get_data("gate/coords/y", ypos);
        get_data("gate/coords/z", zpos);

        send_configuration("g:%s/%s/%s/%s/%s", name, host, xpos, ypos, zpos);

        return 1;
}


static int asteroid_parameters(void)
{
        char pos[MAX_CMD_LEN] = {0};
        char count[MAX_CMD_LEN] = {0};
        char dist[MAX_CMD_LEN] = {0};

        get_data("asteroids/pos[0=belt/1=sphere]", pos);
        get_data("asteroids/count", count);
        get_data("asteroids/distance", dist);

        send_configuration("a:%s/%s/%s", pos, count, dist);

        return 1;
}


static object_desc object[] = {
        {"star",     'S', star_parameters},
        {"planet",   'p', planet_parameters},
        {"station",  's', station_parameters},
        {"gate",     'g', gate_parameters},
        {"asteroid", 'a', asteroid_parameters},
        {NULL, 0, NULL}
};


static int show_objects(void)
{
        int i;

        printf("Objects:\n");
        for (i = 0 ; object[i].name ; i++)
        {
                printf("\t%s\n", object[i].name);
        }

        return 1;
}


static int search_object(char * data, char ** name)
{
        int i;
        int found = -1;
        char * obj = strchr(data, ' ') + 1;

        if (name)
        {
                *name = strchr(data, '=') + 1;
                *name[-1] = 0;
        }
        for (i = 0 ; object[i].name ; i++)
        {
                if (strcmp(object[i].name, obj) == 0)
                {
                        found = i;
                        break;
                }
        }

        return found;
}


int add_object(char * data)
{
        int found;

        if (strchr(data, ' ') == NULL)
        {
                return show_objects();
        }

        found = search_object(data, NULL);
        exit_on(found < 0, 1, "Object not found!\n");

        printf("Information for: %s\n", object[found].name);
        exit_on(object[found].get_parameters() == 0, 1, "Operation cancelled...\n");

        check_reply("Error adding %s...\n", object[found].name);

        return 1;
}


int del_object(char * data)
{
        int           found;
        char        * which;
        unsigned char message[MAX_PACKET_SIZE] = {0};

        if (strchr(data, ' ') == NULL)
        {
                return show_objects();
        }

        found = search_object(data, &which);
        exit_on(found < 0, 1, "Object not found!\n");

        snprintf((char *)message, MAX_DATA_SIZE, "%c:%s", object[found].key, which);
        cyberspace_transmit(cybersock, CMD_DEL_OBJECT,
                        message, strlen((char *)message));
        check_reply("Could not delete %s %s.\n", object[found].name, which);

        return 1;
}

