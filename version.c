/**
 *  \file    version.c
 *  \brief   Version information.
 *
 *           Project independant file.
 *
 *           This file contains the functions necessary to retreive and
 *           display version information. This file is compiled each time
 *           a new compilation is requested in order to keep up to date
 *           its build information.
 *
 *  \author  Thomas Nemeth
 *
 *  \version 1.0.0
 *  \date    jeu 20 déc 2007
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "0config.h"


/**
 *  \defgroup versinf Version information management
 *  @{
 */

/*! Full version information string. */
#define INFO PROGRAM" version "VERSION", "AUTHORS" 2007-2008, build "BUILD".\n"

/** @} */


/**
 *  \brief Version information function.
 *
 *         This function returns the version and build revision information to
 *         the calling function.
 *
 * @return      the version information string
 */
char * version_get_info(void)
{
        return INFO;
}

