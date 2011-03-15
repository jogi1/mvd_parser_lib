/*
Copyright (C) 2010 Jürgen Legler

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include <stdbool.h>
#include <stdlib.h>
#include "mvdparse.h"

float calc_acc(int attacks, int hits)
{
	return attacks ? 100.0f * hits / attacks : 0;
}

int main(int argc, char *argv[])
{
	struct mvd_demo *demo;
	struct mvd_frame *frame;
	struct player *player;
    struct frag_info *fi;
	char **s;
	int i, x;

	if (argc < 2)
		return;

	//printf("Parsing: %s\n", argv[1]);

	demo = MVD_Load_From_File(argv[1]);

    printf("%i\n", MVD_Load_Fragfile(demo, "fragfile.dat"));

	i = MVD_Get_Stats(demo);

	if (i == 1)
	{
		printf("something went wrong\n");
        MVD_Destroy(demo);
		return;
	}
	else if (i == 2)
		printf("end of demo reached\n");

	/*
	s = demo->sound_list;

	printf("soundlist\n");
	while (*s)
	{
		printf("%s\n", *s);
		s++;
	}

	printf("modellist\n");
	s = demo->model_list;
	while (*s)
	{
		printf("%s\n", *s);
		s++;
	}

	*/

	for (i=0; i<32; i++)
	{
		if (demo->players[i].name == NULL)
			continue;

		if (demo->players[i].name[0] == '\0')
			continue;

		if (demo->players[i].spectator == 1)
			continue;

		player = &demo->players[i];

		printf("name: %s\n", player->name_readable);
		printf(" deaths: %i\n", player->statistics->deaths);
		printf(" jumps: %i\n", player->statistics->jumps);
		printf(" mh pickups: %i\n", player->statistics->mh);
		printf(" ga pickups: %i\n", player->statistics->ga);
		printf(" ya pickups: %i\n", player->statistics->ya);
		printf(" ra pickups: %i\n", player->statistics->ra);
		printf(" ssg pickups: %i\n", player->statistics->ssg);
		printf(" ng pickups: %i\n", player->statistics->ng);
		printf(" sng pickups: %i\n", player->statistics->sng);
		printf(" gl pickups: %i\n", player->statistics->gl);
		printf(" rl pickups: %i\n", player->statistics->rl);
		printf(" lg pickups: %i\n", player->statistics->lg);
	}

    // print listed frags
    fi = demo->frags_start;

    while (fi)
    {
        printf("%f\n", fi->time);
        if (fi->killer)
            printf("killer: %s\n", fi->killer->name_readable);
        if (fi->victim)
            printf("victim: %s\n", fi->victim->name_readable);
        printf("weapon: %s\n", fi->wc->identifier);
        printf("----\n");
        fi = fi->next;
    }


	MVD_Destroy(demo);
	return;
}

