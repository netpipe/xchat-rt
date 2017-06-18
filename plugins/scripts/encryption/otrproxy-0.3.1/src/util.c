/*
 *  Off-the-Record Messaging Proxy
 *  Copyright (C) 2004-2005  Nikita Borisov and Ian Goldberg
 *                           <otr@cypherpunks.ca>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *custom_userdir = NULL;

/* Set the userdir to the given string, if non-NULL. */
void util_userdir_set(const char *userdir)
{
    if (userdir) {
	free(custom_userdir);
	custom_userdir = strdup(userdir);
    }
}

static void set_default_userdir(void)
{
    char *filename;

#ifdef WIN32
    const char *dir = "C:\\.otrproxy";
#else
    const char *homedir = getenv("HOME");
    const char *userdir = ".otrproxy";

    if (!homedir) homedir = "";
#endif

#ifdef WIN32
    filename = strdup(dir);
#else
    filename = malloc(strlen(homedir) + 1 + strlen(userdir) + 1);
    if (filename) {
	sprintf(filename, "%s/%s", homedir, userdir);
    }
#endif
    free(custom_userdir);
    custom_userdir = filename;
}

/* Construct a full path for the file with the given name in the user dir.
 * Whoever calls this must free() the result.  Pass NULL to just get the
 * user dir name. */
char *util_userdir_file(const char *basename)
{
    char *filename;
    size_t basenamelen;

    if (custom_userdir == NULL) set_default_userdir();

    if (basename == NULL) {
	return strdup(custom_userdir);
    }

    basenamelen = strlen(basename) + 1;  /* Add 1 for the / */

    filename = malloc(strlen(custom_userdir) + 1 + basenamelen + 1);
    if (filename) {
#ifdef WIN32
	sprintf(filename, "%s\\%s", custom_userdir, basename);
#else
	sprintf(filename, "%s/%s", custom_userdir, basename);
#endif
    }
    return filename;
}
