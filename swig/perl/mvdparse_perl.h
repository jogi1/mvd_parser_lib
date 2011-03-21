#include "mvdparse.h"
struct mvd_demo *load (char *file);
int parse (struct mvd_demo *demo);
struct player *get_player(struct mvd_demo *demo, int player);
