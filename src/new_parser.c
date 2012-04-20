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



#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "defaults.h"
#include "common.h"

#include "mvd_parser.h"
#include "mvd_defs.h"
#include "protocol.h"
#include "tools.h"

#include "readablechars.h"

#include "parse_functions.h"

#include "fragfile.h"

char *stat_string[] = {
        "STAT_HEALTH",
        "STAT_FRAGS",
        "STAT_WEAPON",
        "STAT_AMMO",
        "STAT_ARMOR",
        "STAT_WEAPONFRAME",
        "STAT_SHELLS",
        "STAT_NAILS",
        "STAT_ROCKETS",
        "STAT_CELLS",
        "STAT_ACTIVEWEAPON",
        "STAT_TOTALSECRETS",
        "STAT_TOTALMONSTERS",
        "STAT_SECRETS",
        "STAT_MONSTERS",
        "STAT_ITEMS",
        "STAT_VIEWHEIGHT",
        "STAT_TIME",
        "STAT_18",
        "STAT_19",
        "STAT_20",
        "STAT_21",
        "STAT_22",
        "STAT_23",
        "STAT_24",
        "STAT_25",
        "STAT_26",
        "STAT_27",
        "STAT_28",
        "STAT_29",
        "STAT_30",
        "STAT_31",
        "STAT_32"

};

char *svc_strings[] =
{
  "svc_bad",
  "svc_nop",
  "svc_disconnect",
  "svc_updatestat",
  "svc_version",		// [long] server version
  "svc_setview",		// [short] entity number
  "svc_sound",		// <see code>
  "svc_time",			// [float] server time
  "svc_print",		// [string] null terminated string
  "svc_stufftext",	// [string] stuffed into client's console buffer
  // the string should be \n terminated
  "svc_setangle",		// [vec3] set the view angle to this absolute value

  "svc_serverdata",	// [long] version ...
  "svc_lightstyle",	// [byte] [string]
  "svc_updatename",	// [byte] [string]
  "svc_updatefrags",	// [byte] [short]
  "svc_clientdata",	// <shortbits + data>
  "svc_stopsound",	// <see code>
  "svc_updatecolors",	// [byte] [byte]
  "svc_particle",		// [vec3] <variable>
  "svc_damage",		// [byte] impact [byte] blood [vec3] from

  "svc_spawnstatic",
  "OBSOLETE svc_spawnbinary",
  "svc_spawnbaseline",

  "svc_temp_entity",	// <variable>
  "svc_setpause",
  "svc_signonnum",
  "svc_centerprint",
  "svc_killedmonster",
  "svc_foundsecret",
  "svc_spawnstaticsound",
  "svc_intermission",
  "svc_finale",

  "svc_cdtrack",
  "svc_sellscreen",

  "svc_smallkick",
  "svc_bigkick",

  "svc_updateping",
  "svc_updateentertime",

  "svc_updatestatlong",
  "svc_muzzleflash",
  "svc_updateuserinfo",
  "svc_download",
  "svc_playerinfo",
  "svc_nails",
  "svc_choke",
  "svc_modellist",
  "svc_soundlist",
  "svc_packetentities",
  "svc_deltapacketentities",
  "svc_maxspeed",
  "svc_entgravity",

  "svc_setinfo",
  "svc_serverinfo",
  "svc_updatepl",
  "svc_nails2",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL"
};

/*
static struct mvd_frame *MVD_Frame_Add(struct mvd_demo *demo)
{
	struct mvd_frame *frame;

	frame = calloc(1, sizeof(struct mvd_frame));

	if (frame == NULL)
		return NULL;

	if (demo->frame_first == NULL)
	{
		demo->frame_first = frame;
		demo->frame_current = frame;
	}
	else
	{
		demo->frame_current->next = frame;
		frame->prev = demo->frame_current;
		demo->frame_current = frame;
	}

	return frame;
}
*/

static char *ReadFile(char *name, int *length)
{
	FILE *f;
	size_t rlen;
	char *buf;
	int len;

	f = fopen(name, "r");

	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);

	if (len < 0)
	{
		fclose(f);
		return NULL;
	}

	fseek(f, 0, SEEK_SET);

	buf = calloc(len + 1, sizeof(char));

	if (!buf)
	{
		fclose(f);
		return NULL;
	}

	rlen = fread(buf, 1, len, f);

	fclose(f);

	if (len != rlen || len <= 0)
	{
		free(buf);
		return NULL;

	}
	if (length != NULL)
		*length = len;

	return buf;
}

void MVD_Destroy(struct mvd_demo *demo)
{
	int i;
	struct player *p;
	struct mvd_frame *f, *cf;
	char **s;
	struct frag_info *fi, *fio;

	fi = demo->frags_start;
	while (fi)
	{
		fio = fi->next;
		free(fi);
		fi = fio;
	}

	Fragfile_Destroy(demo->fragfile);

	for (i=0;i<32;i++)
	{
		p = &demo->players[i];

		free(p->statistics);
		if (p->name == NULL)
			continue;

		free(p->name);
		free(p->name_readable);
		free(p->team);
		free(p->team_readable);
		free(p->userinfo);
	}

	/*
	f = demo->frame_first;

	while (f)
	{
		cf = f;
		f = f->next;
		free(cf);
	}
	*/

	s = demo->sound_list;
	if (s)
	{
		while (*s)
		{
			free(*s);
			s++;
		}
		free(demo->sound_list);
	}

	s = demo->model_list;
	if (s)
	{
		while (*s)
		{
			free(*s);
			s++;
		}
		free(demo->model_list);
	}

	free(demo->hostname);
	free(demo->hostname_readable);

	free(demo->map);
	free(demo->map_name);
	free(demo->name);
	free(demo->buf);
	free(demo->serverinfo);
	free(demo);
}

struct mvd_demo *MVD_Load_From_File(char *filename)
{

	FILE *f;

	struct mvd_demo *demo;

	if (filename == NULL)
	{
		return NULL;
	}

	demo = calloc(1, sizeof(struct mvd_demo));

	if (demo == NULL)
	{
		return NULL;
	}

	demo->name = strdup(filename);

	if (demo->name == NULL)
	{
		free(demo);
		return NULL;
	}

	demo->buf = ReadFile(filename, &demo->size);

	if (demo->buf == NULL)
	{
		free(demo->name);
		free(demo);
		return NULL;
	}

	demo->current_position = demo->buf;

	return demo;
}


static int MVD_MSG_BeginReading(struct mvd_demo *demo)
{
	demo->current_message_read_count = 0;
	demo->current_message_bad_read = false;
	return 0;
}

static int MVD_Read(struct mvd_demo *demo, void *buf, unsigned int size)
{
	if ((demo->current_position - demo->buf) + size > demo->size)
	{
		return 1;
	}

	memcpy(buf, demo->current_position, size);
	demo->current_position += size;

	return 0;
}

static int MVD_ReadFrame(struct mvd_demo *demo)
{
	unsigned char c;

	int current_size;

	int i;

	MVD_MSG_BeginReading(demo);

	//MVD_Frame_Add(demo);

	demo->frame++;

      readnext:

	if (MVD_Read(demo, &c, sizeof(c)))
	{
		return 2;
	}

	demo->time += c * 0.001;

	if (MVD_Read(demo, &c, sizeof(c)))
	{
		return 1;
	}

	switch ((c & 7))
	{
		case dem_cmd:
			demo->current_position += sizeof(usercmd_t);
			demo->current_position += 12;
			goto readnext;

		case dem_read:
		      readit:
			if (MVD_Read(demo, &current_size, 4))
			{
				return 1;
			}

			current_size = LittleLong(current_size);

			demo->current_message_size = current_size;

			demo->current_message_position = demo->current_position;

			demo->current_position += current_size;

			//MVD_Read(demo, demo->current_message, demo->current_message_size);

			switch (demo->last_type)
			{
				case dem_multiple:
					goto readnext;
					break;
				case dem_single:
					break;
			}
			return 0;

		case dem_set:
			if (MVD_Read(demo, &i, sizeof(i)))
			{
				return 1;
			}
			demo->outgoing_sequence = LittleLong(i);
			if (MVD_Read(demo, &i, sizeof(i)))
			{
				return 1;
			}
			demo->incoming_sequence = LittleLong(i);
			goto readnext;

		case dem_multiple:
			if (MVD_Read(demo, &i, sizeof(i)))
			{
				return 1;
			}
			demo->last_to = LittleLong(i);
			demo->last_type = dem_multiple;
			goto readit;

		case dem_single:
			demo->last_to = c >> 3;
			demo->last_type = dem_single;
			goto readit;

		case dem_stats:
			demo->last_to = c >> 3;
			demo->last_type = dem_stats;
			goto readit;

		case dem_all:
			demo->last_to = 0;
			demo->last_type = dem_all;
			goto readit;

		default:
			return 1;
	}
	return 0;
}


static void MVD_MSG_Read(struct mvd_demo *demo, void *buf, unsigned int size)
{

	if (demo->current_message_read_count + size > demo->current_message_size)
	{
		demo->current_message_bad_read = 1;
		return;
	}
	memcpy(buf, demo->current_message_position, size);
	demo->current_message_read_count += size;
	demo->current_message_position += size;
}

static unsigned char MVD_MSG_ReadByte(struct mvd_demo *demo)
{
	unsigned char byte;

	MVD_MSG_Read(demo, &byte, sizeof(unsigned char));
	if (demo->current_message_bad_read)
		byte = -1;
	return byte;
}

static char MVD_MSG_ReadChar(struct mvd_demo *demo)
{
	char c;

	MVD_MSG_Read(demo, &c, sizeof(char));
	return c;
}

static int MVD_MSG_ReadShort(struct mvd_demo *demo)
{
	int i;

	unsigned char s;

	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i = s;
	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i += s << 8;
	return i;
}

static int MVD_MSG_ReadLong(struct mvd_demo *demo)
{
	unsigned char s;

	int i;

	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i = s;
	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i += s << 8;
	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i += s << 16;
	MVD_MSG_Read(demo, &s, sizeof(unsigned char));
	i += s << 24;
	return i;
}

static float MVD_MSG_ReadFloat(struct mvd_demo *demo)
{
	union
	{
		unsigned char b[4];
		float f;
		int l;
	} dat;

	MVD_MSG_Read(demo, &dat.b[0], sizeof(unsigned char));
	MVD_MSG_Read(demo, &dat.b[1], sizeof(unsigned char));
	MVD_MSG_Read(demo, &dat.b[2], sizeof(unsigned char));
	MVD_MSG_Read(demo, &dat.b[3], sizeof(unsigned char));

	dat.l = LittleLong(dat.l);
}

static char *MVD_MSG_ReadString(struct mvd_demo *demo)
{
	unsigned int l;

	int c;

	static char string[2048];

	l = 0;

	do
	{
		c = MVD_MSG_ReadByte(demo);

		if (c == 255)
			continue;
		if (c == -1 || c == 0)
			break;

		string[l] = c;
		l++;
	}
	while (l < sizeof(string) - 1);

	string[l] = 0;

	return string;
}

static char *MVD_MSG_ReadStringLine(struct mvd_demo *demo)
{
	unsigned int l;

	int c;

	static char string[2048];

	l = 0;

	do
	{
		c = MVD_MSG_ReadByte(demo);

		if (c == 255)
			continue;
		if (c == -1 || c == 0 || c == '\n')
			break;

		string[l] = c;
		l++;
	}
	while (l < sizeof(string) - 1);

	string[l] = 0;

	return string;
}

static float MVD_MSG_ReadCoord(struct mvd_demo *demo)
{
	return MVD_MSG_ReadShort(demo) * (1.0 / 8);
}

static float MVD_MSG_ReadAngle(struct mvd_demo *demo)
{
	return MVD_MSG_ReadChar(demo) * (360.0 / 256);
}

static float MVD_MSG_ReadAngle16(struct mvd_demo *demo)
{
	return MVD_MSG_ReadShort(demo) * (360.0 / 65536);
}

static void MVD_MSG_ReadDeltaUsercmd(struct mvd_demo *demo, usercmd_t * from, usercmd_t * move, int protoversion)
{
	int bits;

	memcpy(move, from, sizeof(*move));

	bits = MVD_MSG_ReadByte(demo);

	if (protoversion == 26)
	{
		if (bits & CM_ANGLE1)
			move->angles[0] = MVD_MSG_ReadAngle16(demo);
		move->angles[1] = MVD_MSG_ReadAngle16(demo);
		if (bits & CM_ANGLE3)
			move->angles[2] = MVD_MSG_ReadAngle16(demo);

		if (bits & CM_FORWARD)
			move->forwardmove = MVD_MSG_ReadChar(demo) << 3;
		if (bits & CM_SIDE)
			move->sidemove = MVD_MSG_ReadChar(demo) << 3;
		if (bits & CM_UP)
			move->upmove = MVD_MSG_ReadChar(demo) << 3;
	}
	else
	{
		if (bits & CM_ANGLE1)
			move->angles[0] = MVD_MSG_ReadAngle16(demo);
		if (bits & CM_ANGLE2)
			move->angles[1] = MVD_MSG_ReadAngle16(demo);
		if (bits & CM_ANGLE3)
			move->angles[2] = MVD_MSG_ReadAngle16(demo);

		if (bits & CM_FORWARD)
			move->forwardmove = MVD_MSG_ReadShort(demo);
		if (bits & CM_SIDE)
			move->sidemove = MVD_MSG_ReadShort(demo);
		if (bits & CM_UP)
			move->upmove = MVD_MSG_ReadShort(demo);
	}

	if (bits & CM_BUTTONS)
		move->buttons = MVD_MSG_ReadByte(demo);

	if (bits & CM_IMPULSE)
		move->impulse = MVD_MSG_ReadByte(demo);

	if (protoversion == 26)
	{
		if (bits & CM_MSEC)
			move->msec = MVD_MSG_ReadByte(demo);
	}
	else
	{
		move->msec = MVD_MSG_ReadByte(demo);
	}
}

static int MVD_TranslateFlags(int src)
{
	int dst = 0;

	if (src & DF_EFFECTS)
		dst |= PF_EFFECTS;
	if (src & DF_SKINNUM)
		dst |= PF_SKINNUM;
	if (src & DF_DEAD)
		dst |= PF_DEAD;
	if (src & DF_GIB)
		dst |= PF_GIB;
	if (src & DF_WEAPONFRAME)
		dst |= PF_WEAPONFRAME;
	if (src & DF_MODEL)
		dst |= PF_MODEL;

	return dst;
}

static void MVD_HM_svc_nop(struct mvd_demo *demo)
{
	return;
}

static void MVD_HM_svc_disconnect(struct mvd_demo *demo)
{
	demo->ended = 1;
	return;
}

static void MVD_HM_nq_svc_time(struct mvd_demo *demo)
{
	MVD_MSG_ReadFloat(demo);
	return;
}

static void MVD_HM_svc_print(struct mvd_demo *demo)
{
	char *s, *c;
	int i;

	MVD_MSG_ReadByte(demo);
	s = MVD_MSG_ReadString(demo);

	i = strlen(s);

	demo->print_buffer_position += snprintf(demo->print_buffer + demo->print_buffer_position, sizeof(demo->print_buffer) - demo->print_buffer_position, "%s", s);

	if (s[i-1] == '\n')
	{
		c = demo->print_buffer;

		if (FLAG_CHECK(demo->flags, MPF_GATHER_STATS))
		{
			Event_Add_Print(demo, strdup(demo->print_buffer));
		}

        /*
		while (*c)
		{
			*c = readablechars[(unsigned char) *c];
			c++;
		}
        */

		demo->print_buffer[0] = '\0';

		demo->print_buffer_position = 0;

	}
	else
	{
		demo->print_buffer_position += i;
	}
	return;
}

static void MVD_HM_svc_centerprint(struct mvd_demo *demo)
{
	MVD_MSG_ReadString(demo);
	return;
}

static void MVD_HM_svc_stufftext(struct mvd_demo *demo)
{
	char *s;

	s = MVD_MSG_ReadString(demo);

	if (strncmp("fullserverinfo", s, strlen("fullserverinfo")) == 0)
	{
		if (demo->serverinfo)
			free(demo->serverinfo);
		demo->serverinfo = strdup(s + strlen("fullserverinfo "));
		PF_Update_Serverinfo(demo);
	}
	// mvdsv, ktx? weapon stats
	if (strncmp("//wps", s, strlen("//wps")) == 0)
	{
        PF_Weapon_Stats(demo, s);
	}
	
	return;
}

static void MVD_HM_svc_damage(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);	// armor
	MVD_MSG_ReadByte(demo);	// blood
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	return;
}

static void MVD_HM_svc_serverdata(struct mvd_demo *demo)
{
	int i;

	MVD_MSG_ReadLong(demo);	// protoversion
	MVD_MSG_ReadLong(demo);	// servercount
	MVD_MSG_ReadString(demo);	// gamedir
	MVD_MSG_ReadFloat(demo);	// demotime
	if (demo->map_name)
		free(demo->map_name);
	demo->map_name = strdup(MVD_MSG_ReadString(demo));	// levelname 
	for (i = 0; i < 10; i++)
		MVD_MSG_ReadFloat(demo);	// movevar 
	return;
}

static void MVD_HM_svc_cdtrack(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);
	return;
}

static void MVD_HM_svc_playerinfo(struct mvd_demo *demo)
{
	int flags, iflags, num;

	int i;


	num = MVD_MSG_ReadByte(demo);	// number
	flags = MVD_MSG_ReadShort(demo);	//flags
	MVD_MSG_ReadByte(demo);	// frame

	iflags = MVD_TranslateFlags(flags);

	for (i = 0; i < 3; i++)
		if (flags & (DF_ORIGIN << i))
			demo->players[num].origin[i] = MVD_MSG_ReadCoord(demo);

	for (i = 0; i < 3; i++)
		if (flags & (DF_ANGLES << i))
			MVD_MSG_ReadAngle16(demo);

	if (flags & DF_MODEL)
		MVD_MSG_ReadByte(demo);
	if (flags & DF_SKINNUM)
		MVD_MSG_ReadByte(demo);
	if (flags & DF_EFFECTS)
		MVD_MSG_ReadByte(demo);
	if (flags & DF_WEAPONFRAME)
		MVD_MSG_ReadByte(demo);

	return;
}

static void MVD_HM_svc_modellist(struct mvd_demo *demo)
{
	char *s;

	MVD_MSG_ReadByte(demo);
	while (1)
	{
		s = MVD_MSG_ReadString(demo);
		if (!s[0])
			break;

		if (PF_Modellist_Add(demo, 1))
		{
			demo->current_message_bad_read = true;
			return;
		}

		if(PF_Modellist_Add_Entry(demo, strdup(s)))
		{
			demo->current_message_bad_read = true;
			return;
		}
	}
	MVD_MSG_ReadByte(demo);
}

static void MVD_HM_svc_soundlist(struct mvd_demo *demo)
{
	char *s;
	int c;

	MVD_MSG_ReadByte(demo);

	while (1)
	{
		s = MVD_MSG_ReadString(demo);
		if (!s[0])
			break;

		if (PF_Soundlist_Add(demo, 1))
		{
			demo->current_message_bad_read = true;
			return;
		}

		if(PF_Soundlist_Add_Entry(demo, strdup(s)))
		{
			demo->current_message_bad_read = true;
			return;
		}
	}
	MVD_MSG_ReadByte(demo);
	return;
}

static void MVD_HM_svc_spawnstaticsound(struct mvd_demo *demo)
{
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadByte(demo);	// number
	MVD_MSG_ReadByte(demo);	// volume
	MVD_MSG_ReadByte(demo);	// attenuation
	return;
}

static void MVD_HM_svc_spawnbaseline(struct mvd_demo *demo)
{
	MVD_MSG_ReadShort(demo);	// entity
	MVD_MSG_ReadByte(demo);	// modelindex
	MVD_MSG_ReadByte(demo);	// frame
	MVD_MSG_ReadByte(demo);	// colormap
	MVD_MSG_ReadByte(demo);	// skinnum

	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);

	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);

	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);

	return;
}

static void MVD_HM_svc_updatefrags(struct mvd_demo *demo)
{
	int player, frags;

	player = MVD_MSG_ReadByte(demo);	// num
	frags = MVD_MSG_ReadShort(demo);	// frags

	PF_Update_Frags(demo, player, frags);
	return;
}


static void MVD_HM_svc_updateping(struct mvd_demo *demo)
{
	int player, ping;

	player = MVD_MSG_ReadByte(demo);	// num
	ping = MVD_MSG_ReadShort(demo);	// ping

	PF_Update_Ping(demo, player, ping);
	return;
}

static void MVD_HM_svc_updatepl(struct mvd_demo *demo)
{
	int player, pl;

	player = MVD_MSG_ReadByte(demo);	// num
	pl = MVD_MSG_ReadByte(demo);	// pl

	PF_Update_Pl(demo, player, pl);
	return;
}

static void MVD_HM_svc_updateentertime(struct mvd_demo *demo)
{
	int player;
	float entertime;
	
	player = MVD_MSG_ReadByte(demo);
	entertime = MVD_MSG_ReadFloat(demo);

	PF_Update_Entertime(demo, player, entertime);
	return;
}

static void MVD_HM_svc_updateuserinfo(struct mvd_demo *demo)
{
	int user, userid;
	char *userinfo;

	user = MVD_MSG_ReadByte(demo);
	userid = MVD_MSG_ReadLong(demo);
	userinfo = strdup(MVD_MSG_ReadString(demo));

	PF_Set_Userinfo(demo, user, userid, userinfo);

	return;
}


static void MVD_HM_svc_lightstyle(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadString(demo);
	return;
}

static void MVD_HM_svc_bad(struct mvd_demo *demo)
{
	return;
}

static void MVD_HM_svc_serverinfo(struct mvd_demo *demo)
{
	char *key, *value, *s;
	int delete_value = 1;

	key = strdup(MVD_MSG_ReadString(demo));
	value = strdup(MVD_MSG_ReadString(demo));

	if (strcmp(key, "map") == 0)
	{
		if (demo->map)
			free(demo->map);
		demo->map = value;
		delete_value = 0;
	}

	if (strcmp(key, "hostname") == 0)
	{
		if (demo->hostname)
			free(demo->hostname);
		demo->hostname = value;

		if (demo->hostname_readable)
			free(demo->hostname_readable);

		demo->hostname_readable = strdup(value);

		if (demo->hostname_readable)
		{
			s = demo->hostname_readable;
			while (*s)
			{
				*s = readablechars[(unsigned char) *s];
				s++;
			}
		}

		delete_value = 0;
	}

	if (demo->game_started == 0)
	{
		if (strcmp(key, "status"))
		{
			if (strcmp(value, "Countdown") != 0)
			{
				demo->game_started = 1;
			}
		}
	}

	free(key);

	if (delete_value)
		free(value);

	return;
}

static void MVD_HM_svc_packetentities(struct mvd_demo *demo)
{
	int word;
	int bits;
	int i;

	while (1)
	{
		word = MVD_MSG_ReadShort(demo);

		if (demo->current_message_bad_read)
		{
			demo->current_message_bad_read = false;
			return;
		}

		if (!word)
			return;

		word &= ~511;
		bits = word;

		if (bits & U_MOREBITS)
		{
			i = MVD_MSG_ReadByte(demo);
			bits |= i;
		}

		if (bits & U_MODEL)
			MVD_MSG_ReadByte(demo);
		if (bits & U_FRAME)
			MVD_MSG_ReadByte(demo);
		if (bits & U_COLORMAP)
			MVD_MSG_ReadByte(demo);
		if (bits & U_SKIN)
			MVD_MSG_ReadByte(demo);
		if (bits & U_EFFECTS)
			MVD_MSG_ReadByte(demo);
		if (bits & U_ORIGIN1)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ORIGIN2)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ORIGIN3)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ANGLE1)
			MVD_MSG_ReadAngle(demo);
		if (bits & U_ANGLE2)
			MVD_MSG_ReadAngle(demo);
		if (bits & U_ANGLE3)
			MVD_MSG_ReadAngle(demo);
	}

	return;

}

static void MVD_HM_svc_updatestatlong(struct mvd_demo *demo)
{
	int stat, value;

	stat = MVD_MSG_ReadByte(demo);
	value = MVD_MSG_ReadLong(demo);
	PF_Set_Stats(demo, stat, value);
	return;
}

static void MVD_HM_svc_updatestat(struct mvd_demo *demo)
{
	int stat, value;

	stat = MVD_MSG_ReadByte(demo);
	value = MVD_MSG_ReadByte(demo);
	PF_Set_Stats(demo, stat, value);
	return;
}

static void MVD_HM_svc_deltapacketentities(struct mvd_demo *demo)
{

	int word, bits, i;
	byte from;

	from = MVD_MSG_ReadByte(demo);

	if (demo->outgoing_sequence - demo->incoming_sequence >= UPDATE_BACKUP - 1)
		return;
	if ((demo->outgoing_sequence - demo->incoming_sequence - 1) >= UPDATE_BACKUP - 1)
		return;

	while (1)
	{
		word = MVD_MSG_ReadShort(demo);

		if (demo->current_message_bad_read)
		{
			demo->current_message_bad_read = false;
			return;
		}

		if (!word)
			return;

		word &= ~511;
		bits = word;

		if (bits & U_MOREBITS)
		{
			i = MVD_MSG_ReadByte(demo);
			bits |= i;
		}

		if (bits & U_MODEL)
			MVD_MSG_ReadByte(demo);
		if (bits & U_FRAME)
			MVD_MSG_ReadByte(demo);
		if (bits & U_COLORMAP)
			MVD_MSG_ReadByte(demo);
		if (bits & U_SKIN)
			MVD_MSG_ReadByte(demo);
		if (bits & U_EFFECTS)
			MVD_MSG_ReadByte(demo);
		if (bits & U_ORIGIN1)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ORIGIN2)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ORIGIN3)
			MVD_MSG_ReadCoord(demo);
		if (bits & U_ANGLE1)
			MVD_MSG_ReadAngle(demo);
		if (bits & U_ANGLE2)
			MVD_MSG_ReadAngle(demo);
		if (bits & U_ANGLE3)
			MVD_MSG_ReadAngle(demo);
	}

	return;
}

static void MVD_HM_svc_sound(struct mvd_demo *demo)
{
	int channel;
	int sound;
	vec3_t origin;

	channel = MVD_MSG_ReadShort(demo);

	if (channel & SND_VOLUME)
		MVD_MSG_ReadByte(demo);
	if (channel & SND_ATTENUATION)
		MVD_MSG_ReadByte(demo);

	sound = MVD_MSG_ReadByte(demo);	// sound number

	origin[0] = MVD_MSG_ReadCoord(demo);
	origin[1] = MVD_MSG_ReadCoord(demo);
	origin[2] = MVD_MSG_ReadCoord(demo);

	if (FLAG_CHECK(demo->flags, MPF_GATHER_STATS))
		Event_Add_Sound(demo, origin, sound);
	return;
}

static void MVD_HM_svc_temp_entity(struct mvd_demo *demo)
{
	int type;

	type = MVD_MSG_ReadByte(demo);

	if (type == TE_GUNSHOT || type == TE_BLOOD)
		MVD_MSG_ReadByte(demo);

	if (type == TE_LIGHTNING1 || type == TE_LIGHTNING2 || type == TE_LIGHTNING3)
	{
		MVD_MSG_ReadShort(demo);
		MVD_MSG_ReadCoord(demo);
		MVD_MSG_ReadCoord(demo);
		MVD_MSG_ReadCoord(demo);
	}

	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	return;
}

static void MVD_HM_svc_setangle(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadAngle(demo);
	return;
}

static void MVD_HM_svc_setinfo(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);	// num
	MVD_MSG_ReadString(demo);	// key
	MVD_MSG_ReadString(demo);	// value
	return;
}

static void MVD_HM_svc_muzzleflash(struct mvd_demo *demo)
{
	MVD_MSG_ReadShort(demo);
	return;
}

static void MVD_HM_svc_smallkick(struct mvd_demo *demo)
{
	return;
}

static void MVD_HM_svc_bigkick(struct mvd_demo *demo)
{
	return;
}

static void MVD_HM_svc_intermission(struct mvd_demo *demo)
{
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadAngle(demo);
	return;
}

static void MVD_HM_svc_chokecount(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);
	return;
}

static void MVD_HM_svc_spawnstatic(struct mvd_demo *demo)
{
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadByte(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);
	MVD_MSG_ReadCoord(demo);
	MVD_MSG_ReadAngle(demo);
	return;
}

static void MVD_HM_svc_foundsecret(struct mvd_demo *demo)
{
	return;
}

static void MVD_HM_svc_maxspeed(struct mvd_demo *demo)
{
	MVD_MSG_ReadFloat(demo);
	return;
}

static void MVD_HM_svc_nails2(struct mvd_demo *demo)
{
	int i, x;

	i = MVD_MSG_ReadByte(demo);

	for (x = 0; x < i; x++)
	{
		MVD_MSG_ReadByte(demo);

		MVD_MSG_ReadByte(demo);
		MVD_MSG_ReadByte(demo);
		MVD_MSG_ReadByte(demo);
		MVD_MSG_ReadByte(demo);
		MVD_MSG_ReadByte(demo);
		MVD_MSG_ReadByte(demo);
	}
}

#define MSG_CASE(x) case x: MVD_HM_ ## x(demo);

static int MVD_ParseData(struct mvd_demo *demo)
{
	int cmd;

	while (1)
	{
		if (demo->current_message_bad_read)
		{
			return 1;
		}

		cmd = MVD_MSG_ReadByte(demo);

		if (demo->current_message_bad_read)
		{
			break;
		}


		switch (cmd)
		{
			default:
				return 1;

				MSG_CASE(svc_nop);
				break;

				MSG_CASE(svc_disconnect);
				break;

				MSG_CASE(nq_svc_time);
				break;

				MSG_CASE(svc_print);
				break;

				MSG_CASE(svc_centerprint);
				break;

				MSG_CASE(svc_stufftext);
				break;

				MSG_CASE(svc_damage);
				break;

				MSG_CASE(svc_serverdata);
				break;

				MSG_CASE(svc_cdtrack);
				break;

				MSG_CASE(svc_playerinfo);
				break;

				MSG_CASE(svc_modellist);
				break;

				MSG_CASE(svc_soundlist);
				break;

				MSG_CASE(svc_spawnstaticsound);
				break;

				MSG_CASE(svc_spawnbaseline);
				break;

				MSG_CASE(svc_updatefrags);
				break;

				MSG_CASE(svc_updateping);
				break;

				MSG_CASE(svc_updatepl);
				break;

				MSG_CASE(svc_updateentertime);
				break;

				MSG_CASE(svc_updateuserinfo);
				break;

				MSG_CASE(svc_lightstyle);
				break;

				MSG_CASE(svc_bad);
				break;

				MSG_CASE(svc_serverinfo);
				break;

				MSG_CASE(svc_packetentities);
				break;

				MSG_CASE(svc_updatestatlong);
				break;

				MSG_CASE(svc_updatestat);
				break;

				MSG_CASE(svc_deltapacketentities);
				break;

				MSG_CASE(svc_sound);
				break;

				MSG_CASE(svc_temp_entity);
				break;

				MSG_CASE(svc_setangle);
				break;

				MSG_CASE(svc_setinfo);
				break;

				MSG_CASE(svc_muzzleflash);
				break;

				MSG_CASE(svc_smallkick);
				break;

				MSG_CASE(svc_bigkick);
				break;

				MSG_CASE(svc_intermission);
				break;

				MSG_CASE(svc_chokecount);
				break;

				MSG_CASE(svc_spawnstatic);
				break;

				MSG_CASE(svc_foundsecret);
				break;

				MSG_CASE(svc_maxspeed);
				break;

				MSG_CASE(svc_nails2);
				break;

		}
	}

	return 0;
}

// parses the demo till the end (only usefull if you want to get a scoreboard info)
int MVD_Parse(struct mvd_demo *demo)
{
	int ret_val = 0;

	if (demo == NULL)
		return 1;

	while (ret_val == 0 && demo->current_position - demo->buf < demo->size)
	{
		ret_val = MVD_ReadFrame(demo);

		if (ret_val == 0)
		{
			if (MVD_ParseData(demo))
			{
				if (demo->ended)
					return 2;
				return 1;
			}
		}
		else if (ret_val == 1)
			return 1;
		else
			return 2;
	}

	return 0;
}

// steps through a single demo frame each call
int MVD_Step(struct mvd_demo *demo)
{
	int ret_val = 0;
	int i;
    struct frag_info *fi, *fio;
	
	if (demo == NULL)
		return 1;

	for (i=0; i<32; i++)
	{
		memcpy(&demo->players_last_frame[i], &demo->players[i], sizeof(struct player));
		demo->players[i].flags = 0;
	}

    if (FLAG_CHECK(demo->flags, MPF_CLEAN_FRAGS_AFTER_FRAME))
    {
        fi = demo->frags_start;
        while (fi)
        {
            fio = fi->next;
            free(fi);
            fi = fio;
        }

        demo->frags_start = demo->frags_end = NULL;
    }

	ret_val = MVD_ReadFrame(demo);

	if (demo->current_position - demo->buf > demo->size)
		return 2;

	if (ret_val == 0)
	{
		if (MVD_ParseData(demo))
		{
			if (demo->ended)
				return 2;

			return 1;
		}
        if (FLAG_CHECK(demo->flags, MPF_GATHER_STATS))
            Stats_Gather(demo);
	}
	else
	{
		return ret_val; 
	}

	return 0;
}

int MVD_Init(struct mvd_demo *demo, int flags)
{
    if (demo == NULL)
        return 1;

    if (FLAG_CHECK(demo->flags, MPF_INITIATED))
        return 1;

    demo->flags = flags;

    if (FLAG_CHECK(demo->flags, MPF_GATHER_STATS))
    {
        if (Stats_Init(demo))
        {
            return 1;
        }
        FLAG_SET(demo->flags, MPF_STATS_INITIALIZED);
    }


    FLAG_SET(demo->flags, MPF_INITIATED);

    return 0;
}

int MVD_Load_Fragfile(struct mvd_demo *demo, char *filename)
{
    if (demo == NULL)
        return 1;

    if (filename == NULL)
        return 1;

    demo->fragfile = Fragfile_Load(filename);

    if (demo->fragfile == NULL)
        return 1;

    return 0;
}


