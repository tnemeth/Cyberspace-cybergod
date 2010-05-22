/**
 *  \file    commands.h
 *  \brief   Command line commands parsing system.
 *
 *           Project: project independant file.
 *
 *           This file contains the exported functions from commands.c.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    mar 14 oct 2008
 */


#ifndef COMMANDS_H
#define COMMANDS_H

/**
 *  \defgroup cmdstr Command line structure information
 *  @{
 */


/*! Maximum length for a command. */
#define MAX_CMD_LEN     128


/*! Structure used to define a user command.
 *
 * - the command is a string that the user has to type in order to execute the
 *   function
 * - the help field describe the command for online help
 * - the option field is for options description
 * - execute is the function to execute - it must return 0 (quit) or 1
 *   (continue) and the data parameter is the full command line.
 */
typedef struct {
        char * command;                 /*!< Command to use for the function. */
        char * help;                    /*!< Online displayed help.       */
        char * options;                 /*!< Help on possible command line options. */
        int (* execute)(char * data);   /*!< Function to execute.         */
} cmdentry;

/** @} */


/** @cond DUPLICATE_DOCUMENTATION */
int cli_key_pending(int timeout);
int cli_read_keyboard(const char *info, char *data);
void cli_mainloop(cmdentry * commands, const char * prompt, int hist_size);
/** @endcond */

#endif /* COMMANDS_H */

