/**
 *  \file    main.h
 *  \brief   Global definitions and variables.
 *
 *           Project: cybergod (Cyberspace administrator client program).
 *
 *           Thisfile contains the global definitions, structures and
 *           variables used by the program.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    Sun, May 16 2010
 */


#ifndef MAIN_H
#define MAIN_H

#define exit_on(cond, rv, message, args...)                     \
        if (cond)                                               \
        {                                                       \
                printf(message, ##args);                        \
                return rv;                                      \
        }

#define check_reply(message, args...)                                   \
        {                                                               \
                unsigned char reply[MAX_PACKET_SIZE] = {0};             \
                int len = packet_read(cybersock, reply, MAX_PACKET_SIZE);               \
                exit_on(len < 0, 0, "Reception error (%s)\n", get_error_info(len));     \
                exit_on(len == 0, 0, "Reception error(read error)\n");                  \
                if (reply[2] != PACKET_MSG_ACK)                         \
                {                                                       \
                        printf(message, ##args);                        \
                }                                                       \
        }


extern int cybersock;

#endif /* MAIN_H */

