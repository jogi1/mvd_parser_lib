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

#include "defaults.h"
#include "common.h"
#include "mvd_parser.h"
#include "protocol.h"
#include "tools.h"
#include "mvd_defs.h"

#include "readablechars.h"
#include "tokenize_string.h"


void PF_Set_Stats(struct mvd_demo *demo, int stat, int value)
{
	extern char *stat_string[];
	struct player *p;

	if (demo == NULL)
		return;

	p = &demo->players[demo->last_to];

	p->stats[stat] = value;

	if (stat == STAT_HEALTH)
		FLAG_SET(p->flags, PFS_HEALTH);

	if (stat == STAT_ARMOR)
		FLAG_SET(p->flags, PFS_ARMOR);

	if (stat == STAT_ITEMS)
		FLAG_SET(p->flags, PFS_ITEMS);
}

void PF_Set_Userinfo(struct mvd_demo *demo, int user, int userid, char *userinfo)
{
	char *s, *d;
	struct player *p;

	if (demo == NULL)
		return;

	if (userinfo == NULL)
		return;

	p = &demo->players[user];

	if (p->userinfo)
		free(p->userinfo);

	p->userid = userid;
	p->userinfo = userinfo;

	if (p->name)
		free(p->name);

	p->name = Get_Userinfo_Value_By_Key("name", userinfo);

	if (p->name == NULL)
	{
		free(p->team);
		p->team = NULL;
		free(p->team_readable);
		p->team_readable = NULL;
		return;
	}

	if (p->name_readable)
	{
		free(p->name_readable);
		p->name_readable = NULL;
	}

	p->name_readable = strdup(p->name);
	if (p->name_readable)
	{
		s = p->name_readable;

		while (*s)
		{
			*s = readablechars[(unsigned char)*s];
			s++;
		}
	}

	if (p->team)
		free(p->team);

	p->team = Get_Userinfo_Value_By_Key("team", userinfo);

	if (p->team)
	{
		if (p->team_readable)
			free(p->team_readable);

		p->team_readable = strdup(p->team);

		s = p->team_readable;

		while (*s)
		{
			*s = readablechars[(unsigned char)*s];
			s++;
		}
	}

	s = Get_Userinfo_Value_By_Key("*spectator", userinfo);

	if (s == NULL)
		p->spectator = 0;
	else
	{
		if (atoi(s) == 0)
			p->spectator = 0;
		else
			p->spectator = 1;

		free(s);
	}
	
}

void PF_Update_Frags(struct mvd_demo *demo, int player, int frags)
{
	if (demo == NULL)
		return;

	demo->players[player].frags = frags;
}

void PF_Update_Ping(struct mvd_demo *demo, int player, int ping)
{
	if (demo == NULL)
		return;

	demo->players[player].ping = ping;
}

void PF_Update_Pl(struct mvd_demo *demo, int player, int pl)
{
	if (demo == NULL)
		return;

	demo->players[player].pl = pl;
}

void PF_Update_Entertime(struct mvd_demo *demo, int player, float entertime)
{
	if (demo == NULL)
		return;

	demo->players[player].entertime = entertime;
}

/*
static void add_string_to_frame(struct mvd_demo *demo, char *string)
{
	struct string **list, *entry;
	int i;

	list = calloc(demo->current_frame->strings_count + 2, sizeof(struct string *));

	if (list == NULL)
		return;

	entry = calloc(1, sizeof(struct string));

	if (entry == NULL)
	{
		free(list);
		return;
	}

	for (i=0; i<demo->current_frame->strings_count; i++)
		list[i] = demo->current_frame->strings[i];

	list[i] = entry;

	free(demo->current_frame->strings);

	demo->current_frame->strings = list;

	entry->string = string;
}
*/

void PF_Print(struct mvd_demo *demo, char *string)
{
	if (demo == NULL)
		return;

	if (string == NULL)
		return;

	//add_string_to_frame(demo, string);
}

void PF_Update_Serverinfo(struct mvd_demo *demo)
{
	unsigned char *s;

	if (demo == NULL)
		return;

	if (demo->serverinfo == NULL)
		return;

	// map
	if (demo->map)
		free(demo->map);
	demo->map = Get_Userinfo_Value_By_Key("map", demo->serverinfo);

	// hostname, hostname_readable
	if (demo->hostname)
		free(demo->hostname);
	demo->hostname = Get_Userinfo_Value_By_Key("hostname", demo->serverinfo);
	
	demo->hostname_readable = strdup(demo->hostname);

	if (demo->hostname_readable)
	{
		s = demo->hostname_readable;
		while (*s)
		{
			*s = readablechars[*s];
			s++;
		}
	}
}

void PF_Weapon_Stats(struct mvd_demo *demo, char *string)
{
	int player, weapon, attacks, hits;
	struct tokenized_string *t;

	if (demo == NULL)
		return;

	if (string == NULL)
		return;

	if (!FLAG_CHECK(demo->flags, MPF_STATS_INITIALIZED))
		return;

	t = Tokenize_String(string);

	if (t == NULL)
		return;

	if (t->count != 5)
	{
		Tokenize_String_Delete(t);		
		return;
	}

	player = atoi(t->tokens[1]);
	attacks = atoi(t->tokens[3]);
	hits = atoi(t->tokens[4]);

	// yay
	if (strcmp(t->tokens[2], "sg") == 0)
	{
		demo->players[player].statistics->sg_attacks = attacks;
		demo->players[player].statistics->sg_hits = hits;
	}
	else if (strcmp(t->tokens[2], "ssg") == 1)
	{
		demo->players[player].statistics->ssg_attacks = attacks;
		demo->players[player].statistics->ssg_hits = hits;
	}
	else if (strcmp(t->tokens[2], "ng") == 1)
	{
		demo->players[player].statistics->ng_attacks = attacks;
		demo->players[player].statistics->ng_hits = hits;
	}
	else if (strcmp(t->tokens[2], "sng") == 1)
	{
		demo->players[player].statistics->sng_attacks = attacks;
		demo->players[player].statistics->sng_hits = hits;
	}
	else if (strcmp(t->tokens[2], "gl") == 1)
	{
		demo->players[player].statistics->gl_attacks = attacks;
		demo->players[player].statistics->gl_hits = hits;
	}
	else if (strcmp(t->tokens[2], "rl") == 1)
	{
		demo->players[player].statistics->rl_attacks = attacks;
		demo->players[player].statistics->rl_hits = hits;
	}
	else if (strcmp(t->tokens[2], "lg") == 1)
	{
		demo->players[player].statistics->lg_attacks = attacks;
		demo->players[player].statistics->lg_hits = hits;
	}

	Tokenize_String_Delete(t);		
}

int PF_Soundlist_Add(struct mvd_demo *demo, int entries)
{
	char **old_list, **list;
	int old_count, count, i;

	if (demo == NULL)
		return 1;

	if (entries == 0)
		return 1;

	old_list = demo->sound_list;

	old_count = count = demo->sound_list_size;

	demo->sound_list_size += entries;

	count += entries + 1;

	list = calloc(count, sizeof(char *));

	if (list == NULL)
		return 1;

	for (i=0; i<old_count; i++)
		list[i] = old_list[i];

	if (old_list)
		free(old_list);

	demo->sound_list = list;

	return 0;
}

int PF_Soundlist_Add_Entry(struct mvd_demo *demo, char *entry)
{
	if (demo == NULL)
		return 1;

	if (entry == NULL)
		return 1;

	demo->sound_list[demo->sound_list_count] = entry;
	demo->sound_list_count++;

	return 0;
}

int PF_Modellist_Add(struct mvd_demo *demo, int entries)
{
	char **old_list, **list;
	int old_count, count, i;

	if (demo == NULL)
		return 1;

	if (entries == 0)
		return 1;

	old_list = demo->model_list;

	old_count = count = demo->model_list_size;

	demo->model_list_size += entries;

	count += entries + 1;

	list = calloc(count, sizeof(char *));

	if (list == NULL)
		return 1;

	for (i=0; i<old_count; i++)
		list[i] = old_list[i];

	if (old_list)
		free(old_list);

	demo->model_list = list;

	return 0;
}

int PF_Modellist_Add_Entry(struct mvd_demo *demo, char *entry)
{
	if (demo == NULL)
		return 1;

	if (entry == NULL)
		return 1;

	demo->model_list[demo->model_list_count] = entry;
	demo->model_list_count++;

	return 0;
}

