#include "defaults.h"
#include "common.h"
#include "mvd_parser.h"
#include "events.h"
#include "tools.h"
#include "fragfile.h"

void Stats_Gather(struct mvd_demo *demo)
{
	struct event **l;
	struct sound_event *s;
	struct player *p, *lp, *ljp, *jp;
	float distance;
	float sd = 999;
	char *string;
	struct frag_info fi;

	int i;

	//check events
	l = demo->event;

	if (l)
	{
		while (*l)
		{
			if ((*l)->type == EVENT_SOUND)
			{
				s = (*l)->data;
				if (strstr(demo->sound_list[s->sound], "plyrjmp") != NULL)
				{
					jp = NULL;
					ljp = NULL;
					for (i=0; i<32; i++)
					{
						p = &demo->players[i];

						if (!is_player(p))
							continue;

						distance = VectorDistance(p->origin, s->origin);
						if (distance < sd)
						{
							sd = distance;
							jp = p;
							ljp = &demo->players_last_frame[i];
						}

					}

					if (jp)
					{
						jp->statistics->jumps++;
						/*
						printf("distance: %f %i\n", sd, demo->frame);
						printf("current position  : %f %f %f\n", jp->origin[0], jp->origin[1], jp->origin[2]);
						if (ljp)
						printf("lastframe position: %f %f %f\n", ljp->origin[0], ljp->origin[1], ljp->origin[2]);
						printf("sound position    :   %f %f %f\n", s->origin[0], s->origin[1], s->origin[2]);
						*/
					}
				}
            }
            else if ((*l)->type == EVENT_PRINT)
            {
                // use this to do frag stats via a fragfile at some point
                if (demo->fragfile)
                {
                    string = (char *)(*l)->data;
                    Fragfile_Parse_Message_Store(demo, string);
                    /*
                    if (Fragfile_Parse_Message(demo, string, &fi) == 0)
                    {
                        if (fi.killer)
                            printf("Killer: %s\n", fi.killer->name_readable);
                        if (fi.victim)
                            printf("Victim: %s\n", fi.victim->name_readable);
                        printf("%s\n", fi.wc->identifier);
                    }
                    */
				}
			}
			l++;
		}
	}
	Event_Cleanup(demo);

	// check other stuff

	for (i=0; i<32; i++)
	{
		if (!is_player(&demo->players[i]))
			continue;

		if (demo->players[i].flags == 0)
			continue;

		p = &demo->players[i];
		lp = &demo->players_last_frame[i];

		// megahealth
		if (FLAG_CHECK(p->flags, PFS_HEALTH))
		{

			if (p->stats[STAT_HEALTH] <= 0 && lp->stats[STAT_HEALTH] > 0)
				p->statistics->deaths++;


			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_SUPERHEALTH))
			{
				if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_SUPERHEALTH))
					p->statistics->mh++;
			}
			else
			{
				if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_SUPERHEALTH))
					if (p->stats[STAT_HEALTH] > lp->stats[STAT_HEALTH])
						p->statistics->mh++;
			}
		}

		// armors
		if (FLAG_CHECK(p->flags, PFS_ARMOR))
		{
			if (p->stats[STAT_ARMOR] > lp->stats[STAT_ARMOR])
			{
				if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR1) && FLAG_CHECK(lp->stats[STAT_ITEMS], IT_ARMOR1))
					p->statistics->ga++;
				else if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR2) && FLAG_CHECK(lp->stats[STAT_ITEMS], IT_ARMOR2))
					p->statistics->ya++;
				else if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR3) && FLAG_CHECK(lp->stats[STAT_ITEMS], IT_ARMOR3))
					p->statistics->ra++;
				else
				{
					if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR1))
						p->statistics->ga++;
					if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR2))
						p->statistics->ya++;
					if (FLAG_CHECK(p->stats[STAT_ITEMS], IT_ARMOR3))
						p->statistics->ra++;
				}
			}
		}

		if (FLAG_CHECK(p->flags, PFS_ITEMS))
		{
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_QUAD) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_QUAD))
				p->statistics->quad++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_INVULNERABILITY) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_INVULNERABILITY))
				p->statistics->pent++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_INVISIBILITY) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_INVISIBILITY))
				p->statistics->ring++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_SUPER_SHOTGUN) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_SUPER_SHOTGUN)) 
				p->statistics->ssg++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_NAILGUN) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_NAILGUN)) 
				p->statistics->ng++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_SUPER_NAILGUN) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_SUPER_NAILGUN)) 
				p->statistics->sng++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_GRENADE_LAUNCHER) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_GRENADE_LAUNCHER)) 
				p->statistics->gl++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_ROCKET_LAUNCHER) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_ROCKET_LAUNCHER)) 
				p->statistics->rl++;
			if (!FLAG_CHECK(lp->stats[STAT_ITEMS], IT_LIGHTNING) && FLAG_CHECK(p->stats[STAT_ITEMS], IT_LIGHTNING)) 
				p->statistics->lg++;

		}
	}
}

int Stats_Init(struct mvd_demo *demo)
{
	int i;

	if (demo == NULL)
		return 1;

	for (i=0; i<32; i++)
	{
		demo->players[i].statistics = calloc(1, sizeof(struct stats));
		if (demo->players[i].statistics == NULL)
			return 1;
	}

	return 0;
}
