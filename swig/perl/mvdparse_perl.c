#include <string.h>
#include "mvdparse_perl.h"


struct mvd_demo *load(char *file)
{
    return MVD_Load_From_File(file);
}

int parse (struct mvd_demo *demo)
{
    return MVD_Parse(demo);
}

struct player *get_player(struct mvd_demo *demo, int player)
{
    if (demo == NULL)
        return NULL;

    if (player < 0 || player > 32)
        return NULL;

    return &demo->players[player];
}
