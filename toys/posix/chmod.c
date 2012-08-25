/* vi: set sw=4 ts=4:
 *
 * chmod.c - Change file mode bits
 *
 * Copyright 2012 Rob Landley <rob@landley.net>
 *
 * See http://pubs.opengroup.org/onlinepubs/009695399/utilities/chmod.html
 *

USE_CHMOD(NEWTOY(chmod, "<2?vR", TOYFLAG_BIN))

config CHMOD
    bool "chmod"
    default y
    help
        usage: chmod [-R] MODE FILE...

        Change mode of listed file[s] (recursively with -R).

        MODE can be (comma-separated) stanzas: [ugoa][+-=][rwxstXugo]

        Stanzas are applied in order: For each category (u = user,
        g = group, o = other, a = all three, if none specified default is a),
        set (+), clear (-), or copy (=), r = read, w = write, x = execute.
        s = u+s = suid, g+s = sgid, o+s = sticky. (+t is an alias for o+s).
        suid/sgid: execute as the user/group who owns the file.
        sticky: can't delete files you don't own out of this directory
        X = x for directories or if any category already has x set.

        Or MODE can be an octal value up to 7777	ug uuugggooo	top +
        bit 1 = o+x, bit 1<<8 = u+w, 1<<11 = g+1	sstrwxrwxrwx	bottom

        Examples:
        chmod u+w file - allow owner of "file" to write to it.
        chmod 744 file - user can read/write/execute, everyone else read only
*/

#include "toys.h"

DEFINE_GLOBALS(
    char *mode;
)

#define TT this.chmod

#define FLAG_R 1
#define FLAG_v 2

int do_chmod(struct dirtree *try)
{
    mode_t mode;

    if (!dirtree_notdotdot(try)) return 0;

    mode = string_to_mode(TT.mode, try->st.st_mode);
    if (toys.optflags & FLAG_v) {
        char *s = dirtree_path(try, 0);
        printf("chmod '%s' to %04o\n", s, mode);
        free(s);
    }
    wfchmodat(dirtree_parentfd(try), try->name, mode);

    return (toys.optflags & FLAG_R) ? DIRTREE_RECURSE : 0;
}

void chmod_main(void)
{
    TT.mode = *toys.optargs;
    char **file;

    for (file = toys.optargs+1; *file; file++) dirtree_read(*file, do_chmod);
}