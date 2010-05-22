/**
 *  \file    commands.c
 *  \brief   Command line commands parsing system.
 *
 *           Project: project independant file.
 *
 *           This file contains functions that can be used to integrate a
 *           command line interface to a program.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    mar 14 oct 2008
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>

#include "xmem.h"
#include "commands.h"

static int intern_cli_cmd_help(char * data);
static int intern_cli_cmd_quit(char * data);


/**
 *  \defgroup cmdsvars Internal command line parsing structures and variables
 *  @{
 */

/*! History entry definition. */
typedef struct {
        char command[MAX_CMD_LEN];        /*!< Command to store in history. */
} histentry;


/*! Internal commands definitions. */
static cmdentry internals[] = {
        {"help", "This help", NULL, intern_cli_cmd_help},
        {"quit", "Quit",      NULL, intern_cli_cmd_quit},
        {NULL, NULL, NULL}
};

/*! User provided commands. */
static cmdentry * command = NULL;

/*! History buffer. */
static char    ** history = NULL;

/** @} */


/**
 *  \brief User keypress waiting function.
 *
 *         This function waits for the user to press a key. If after the
 *         specified timeout, nothing has arrived, the function returns
 *         with an error status.
 *
 * @param timeout       timeout in ms
 * @return              the status of the input
 * @retval      1       a key is pending in the input buffer
 * @retval      0       timeout elapsed
 * @retval      -1      error
 */
int cli_key_pending(int timeout)
{
        int status;
        fd_set rfds;
        struct timeval tv = {0, timeout * 1000};

        FD_ZERO(&rfds);
        FD_SET(0, &rfds); /* stdin */

        status = select(1, &rfds, NULL, NULL, &tv);
        if (status > 0)
        {
                return 1;
        }
        else if (status == 0)
        {
                return 0;
        }
        return -1;
}


/**
 *  \brief Keyboard information reading function.
 *
 *         This function reads input from the keyboard until the user
 *         presses ENTER.
 *
 *  @param info         prompt message
 *  @param data         keyboard input data
 *  @return             status of the input
 *  @retval 0           exit requested (CTRL-D)
 *  @retval 1           valid data
 */
int cli_read_keyboard(const char *info, char *data)
{
        printf("%s> ", info);

        if ((! fgets(data, MAX_CMD_LEN, stdin)) && feof(stdin))
        {
                printf("\n");
                return 0;
        }
        data[strlen(data) - 1] = 0;

        return 1;
}


/**
 *  \brief User command function: display online help.
 *
 *         This function displaus the online help for user functions.
 *
 * @param data          command line options (unused)
 * @return              always 1 for the program to continue
 */
static int intern_cli_cmd_help(char * data)
{
        int i;

        printf(" COMMAND\tDESCRIPTION\n");
        for (i = 0 ; command[i].command ; i++)
        {
                printf("   %s\t\t%s\t%s%s%s\n",
                                command[i].command,
                                command[i].help,
                                command[i].options ? "(" : "",
                                command[i].options ? command[i].options : "",
                                command[i].options ? ")" : ""
                                );
        }

        return 1;
}


/**
 *  \brief User command function: quit the program.
 *
 *         This function is used to quit the main loop of the system. This
 *         is usually used to quit the program.
 *
 * @param data          command line options (unused)
 * @return              always 0 for the program to stop
 */
static int intern_cli_cmd_quit(char * data)
{
        return 0;
}


/**
 *  \brief Command line parsing main loop.
 *
 *         This function waits for user inputs and executes them as commands.
 *
 * @param commands              user provided list of commands (table)
 * @param prompt                command prompt to display
 * @param hist_size             size for history backup
 */
void cli_mainloop(cmdentry * commands, const char * prompt, int hist_size)
{
        int cont = 1;
        int cmd = -1;

        command = commands;
        if (hist_size > 0)
        {
                history = xmalloc(sizeof(histentry) * hist_size);
                memset(history, sizeof(histentry) * hist_size, 0);
        }

        while (cont)
        {
                char data[MAX_CMD_LEN] = {0};
                int i = cli_read_keyboard(prompt, data);
                int found = 0;
                cmdentry * entry = internals;

                if (i == 0)
                {
                        break;
                }

start:
                for (i = 0 ; entry[i].command ; i++)
                {
                        int len = strlen(entry[i].command);

                        if (strncmp(data, entry[i].command, len) == 0)
                        {
                                cmd = i;
                                found = 1;
                                if (entry[i].execute(data) == 0)
                                {
                                        cont = 0;
                                }
                                break;
                        }
                }
                if ((found == 0) && (entry == internals))
                {
                        entry = commands;
                        goto start;
                }

                if (found == 0)
                {
                        if ((cmd > -1) && (data[0] == 0))
                        {
                                (void)command[cmd].execute(data);
                        }
                        else
                        {
                                printf("*** Unknown command ***\n");
                        }
                }
        }

        if (history)
        {
                free(history);
        }
        history = NULL;
}

