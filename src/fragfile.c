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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defaults.h"
#include "common.h"
#include "mvd_parser.h"
#include "tokenize_string.h"
#include "fragfile.h"


struct frag_msg_type_string
{
    enum frag_msg_type type;
    char *string;
};

struct frag_msg_type_string frag_msg_type_string[] = {
    {fmt_death, "PLAYER_DEATH"},
    {fmt_suicide, "PLAYER_SUICIDE"},
    {fmt_teamkilled_unknown, "X_TEAMKILLED_UNKNOWN"},
    {fmt_teamkills_unknown, "X_TEAMKILLS_UNKNOWN"},
    {fmt_x_fragged_by_y, "X_FRAGGED_BY_Y"},
    {fmt_x_frags_y, "X_FRAGS_Y"},
    {fmt_x_teamkills_y, "X_TEAMKILLS_Y"},
    {fmt_x_teamkilled_by_y, "X_TEAMKILLED_BY_Y"},
};

static int add_weapon_class(struct fragfile *ff, char *identifier, char *long_name, char *short_name, char *image_name)
{
    struct weapon_class *c, **ncl, **ocl;
    int i;

    if (ff == NULL)
        return 1;

    c = calloc(1, sizeof(struct weapon_class));

    if (c == NULL)
        return 1;

    c->identifier = strdup(identifier);
    if (c->identifier == NULL)
    {
        free(c);
        return 1;
    }

    c->long_name = strdup(long_name);
    if (c->long_name == NULL)
    {
        free(c->identifier);
        free(c);
        return 1;
    }

    
    if (c->short_name)
    {
        c->short_name = strdup(short_name);
        if (c->short_name == NULL)
        {
            free(c->identifier);
            free(c->long_name);
            free(c);
            return 1;
        }
    }

    if (c->image_name)
    {
        c->image_name = strdup(image_name);
        if (c->image_name == NULL)
        {
            free(c->identifier);
            free(c->long_name);
            free(c->short_name);
            free(c);
            return 1;
        }
    }

    ncl = calloc(ff->weapon_class_count + 2, sizeof(struct weapon_class **));

    if (ncl == NULL)
    {
        free(c->identifier);
        free(c->long_name);
        free(c->short_name);
        free(c->image_name);
    }

    ocl = ff->weapon_class;

    for (i=0; i<ff->weapon_class_count; i++)
        ncl[i] = ocl[i];

    ncl[i] = c;

    free(ff->weapon_class);

    ff->weapon_class = ncl;

    ff->weapon_class_count++;

	return 0;
}

static int register_obituary(struct fragfile *ff, enum frag_msg_type type, struct weapon_class *wc, char *msg1, char *msg2)
{
    struct obituary *o, **nol;
    int i;

    if (ff == NULL)
        return 1;

    if (wc == NULL)
        return 1;

    if (msg1 == NULL)
        return 1;

    o = calloc(1, sizeof(struct obituary));

    if (o == NULL)
        return 1;


    o->type = type;
    o->wc = wc;

    o->msg1 = strdup(msg1);

    if (o->msg1 == NULL)
    {
        free(o);
        return 1;
    }

    if (msg2)
    {
        o->msg2 = strdup(msg2);
        if (o->msg2 == NULL)
        {
            free(o->msg1);
            free(o);
            return 1;
        }
    }

    nol = calloc(ff->obituary_count + 2, sizeof(struct obituary *));

    if (nol == NULL)
    {
        free(o->msg2);
        free(o->msg1);
        free(o);
        return 1;
    }

    for (i=0; i<ff->obituary_count; i++)
        nol[i] = ff->obituary[i];

    nol[i] = o;

    free(ff->obituary);

    ff->obituary = nol;

    ff->obituary_count++;

    return 0;
}

static int get_frag_message_type(char *s, enum frag_msg_type *frag_msg_type)
{
    int i;

    if (s == NULL)
        return 1;
    if (frag_msg_type == NULL)
        return 1;

    for (i=0; i<sizeof(frag_msg_type_string)/sizeof(struct frag_msg_type_string);i++)
        if (strcasecmp(s, frag_msg_type_string[i].string) == 0)
        {
            *frag_msg_type = frag_msg_type_string[i].type;
            return 0;
        }

    return 1;
}

static int get_weapon_class(struct fragfile *ff, char *identifier, struct weapon_class **wc)
{
    int i;

    if (ff == NULL)
        return 1;

    if (identifier == NULL)
        return 1;

    if (wc == NULL)
        return 1;

    for (i=0; i<ff->weapon_class_count; i++)
        if (strcmp(ff->weapon_class[i]->identifier, identifier) == 0)
        {
            *wc = ff->weapon_class[i];
            return 0;
        }
    return 1;
}

struct fragfile *Fragfile_Load(char *filename)
{
    FILE *f;
    struct tokenized_string *ts;
    struct fragfile *ff;
    char line[1024];
    int x, y;
    char *rcode;
    int abort = 0;
    enum frag_msg_type fmsg_type;
    struct weapon_class *wc;

    if (filename == NULL)
        return NULL;

    f = fopen(filename, "r");

    if (f == NULL)
        return NULL;

    ff = calloc(1, sizeof(*ff));

    if (ff == NULL)
    {
        fclose(f);
        return NULL;
    }

    // register the standard weapon classes
    add_weapon_class(ff, "NONE", "none", NULL, NULL);
    add_weapon_class(ff, "NULL", "null", NULL, NULL);
    add_weapon_class(ff, "NOWEAPON", "noweapon", NULL, NULL);
    
    while ((rcode = fgets(line, sizeof(line), f)) != NULL)
    {
        x = strlen(line);
        if (x <= 4)
            continue;

        // comment
        if (strncmp(line, "//", 2) == 0)
            continue;

        // remove trailing newline
        line[x-1] = '\0';
        x--;

        y = 0;
        // remove  "//" comments after usefull stuff and trailing spaces
        while (y < x-2)
        {
            if (line[y] == '/' && line[y+1] == '/')
            {
                line[y] = '\0';
                y--;
                if (line[y] == ' ')
                {
                    while(line[y] == ' ')
                        y--;
                    line[y+1] = '\0';
                }
                break;
            }
            else if (line[y] == '\t')
                line[y] = ' ';
            y++;
        }

        // do usefulle stuff
        ts = Tokenize_String(line);

        // malformed
        if (ts->count <= 1)
        {
            Tokenize_String_Delete(ts);
            continue;
        }

        // definition
        if (strncmp(ts->tokens[0], "#DEFINE", 7) == 0)
        {
            // weapon class
            if (strncmp(ts->tokens[1], "WEAPON_CLASS", 12) == 0)
            {
                if (ts->count -2 == 2)
                    add_weapon_class(ff, ts->tokens[2], ts->tokens[3], NULL, NULL);
                else if (ts->count -2 == 3)
                    add_weapon_class(ff, ts->tokens[2], ts->tokens[3], ts->tokens[4], NULL);
                else if (ts->count -2 == 4)
                    add_weapon_class(ff, ts->tokens[2], ts->tokens[3], ts->tokens[4], ts->tokens[5]);
                else
                {
                    abort = 1;
                    break;
                }
            }
            // obituary
            if (strncmp(ts->tokens[1], "OBITUARY", 8) == 0)
            {
                if (ts->count >= 4)
                {
                    if (get_frag_message_type(ts->tokens[2], &fmsg_type))
                    {
                        abort = 1;
                    }

                    if (get_weapon_class(ff, ts->tokens[3], &wc))
                    {
                        abort = 1;
                    }

                    if (ts->count == 5)
                        register_obituary(ff, fmsg_type, wc, ts->tokens[4], NULL);
                    else if (ts->count == 6)
                        register_obituary(ff, fmsg_type, wc, ts->tokens[4], ts->tokens[5]);
                }
            }
        }
        Tokenize_String_Delete(ts);
    }

    if (abort)
    {
        Fragfile_Destroy(ff);
        return NULL;
    }

    fclose(f);
    return ff;
}

static int find_players(char *message, struct player *players, struct found_players *fplayers)
{
    int i;
    int count;
    char *s;

    fplayers[0].player = NULL;
    fplayers[1].player = NULL;

    for (i=0, count=0; i<32 && count<2; i++)
    {
        if (players[i].name == NULL)
            continue;
        if (players[i].name[0] == '\0')
            continue;
        if (players[i].spectator == 1)
            continue;

        if ((s = strstr(message, players[i].name)) != NULL)
        {
            fplayers[count].start = s;
            fplayers[count].end = s + strlen(players[i].name);
            fplayers[count].player = &players[i];
            count++;
        }
    }
    return count;
}

static void sort_players(struct found_players *players)
{
    struct found_players temp;

    if (players[1].start < players[0].start)
    { 
        temp.player = players[0].player;
        temp.start = players[0].start;
        temp.end = players[0].end;

        players[0].player = players[1].player;
        players[0].start = players[1].start;
        players[0].end = players[1].end;

        players[1].player = temp.player;
        players[1].start = temp.start;
        players[1].end = temp.end;
    }
}

static void setup_frag_info(struct frag_info *fi, enum frag_msg_type type, struct found_players *found_players, struct weapon_class *wc)
{
    if (fi == NULL)
        return;
    if (found_players == NULL)
        return;
    if (wc == NULL)
        return;

    fi->wc = wc;

    if (type == fmt_x_teamkilled_by_y || type == fmt_x_fragged_by_y)
    {
        fi->killer = found_players[1].player;
        fi->victim= found_players[0].player;
    }
    else if (type == fmt_x_frags_y || type == fmt_x_teamkills_y)
    {
        fi->killer = found_players[0].player;
        fi->victim = found_players[1].player;
    }
    else if (type == fmt_death)
    {
        fi->killer = NULL;
        fi->victim = found_players[0].player;
    }
    else if (type == fmt_suicide)
    {
        fi->killer = found_players[0].player;
        fi->victim = found_players[0].player;
    }
    else if (type == fmt_teamkills_unknown)
    {
        fi->killer = found_players[0].player;
        fi->victim = NULL;
    }
    else if (type == fmt_teamkilled_unknown)
    {
        fi->victim = found_players[0].player;
        fi->killer = NULL;
    }
}

static int find_obituary(struct fragfile *ff, int player_count, int msg_count, char *msg1, char *msg2, struct frag_info *ft, struct found_players *found_players)
{
    struct obituary *o;
    int i;

    if (ff == NULL)
        return 1;

    if (player_count < 1)
        return 1;

    for (i=0; i<ff->obituary_count; i++)
    {
        o = ff->obituary[i];

        if (player_count == 1)
        {
            if (    o->type == fmt_x_teamkilled_by_y || 
                    o->type == fmt_x_teamkills_y ||
                    o->type == fmt_x_fragged_by_y ||
                    o->type == fmt_x_frags_y)
                continue;
        }

        if (player_count == 2)
        {
            if (    o->type == fmt_death|| 
                    o->type == fmt_suicide||
                    o->type == fmt_teamkilled_unknown||
                    o->type == fmt_teamkills_unknown)
                continue;
        }

        if (msg_count == 1)
        {
            if (strcmp(o->msg1, msg1) == 0)
            {
                setup_frag_info(ft, o->type, found_players, o->wc);
                return 0;
            }
        }
        else if (msg_count == 2)
        {
            if (o->msg2 == NULL)
                continue;

            if ((strcmp(o->msg1, msg1) == 0 ) && (strcmp(o->msg2, msg2) == 0))
            {
                setup_frag_info(ft, o->type, found_players, o->wc);
                return 0;
            }
        }
    }
    return 1;
}

int Fragfile_Parse_Message(struct mvd_demo *demo, char *message, struct frag_info *fi)
{
    char buf1[256], buf2[256];
    struct found_players found_players[2];
    int player_count;
    int msg_count = 1;
    int i;

    if (demo->fragfile == NULL)
        return 1;

    player_count = find_players(message, demo->players, found_players);

    if (player_count == 0)
        return 1;

    if (player_count > 2)
        return 1;

    buf1[0] = '\0';
    buf2[0] = '\0';

    i = strlen(message);
    if (message[i-1] = '\n')
        message[i-1] = '\0';

    if (player_count == 2)
        sort_players(found_players);

    if (found_players[0].start > message)
    {
        snprintf(buf1, found_players[0].start - message + 1, "%s", message);
    }
    else if (found_players[0].start == message)
    {
        if (player_count == 2)
            snprintf(buf1, found_players[1].start - found_players[0].end + 1, "%s", found_players[0].end);
        else
            snprintf(buf1, sizeof(buf1), "%s", found_players[0].end);
    }

    if (player_count == 2)
        snprintf(buf2, sizeof(buf2), "%s", found_players[1].end);

    if (buf2[0] != '\0')
        msg_count = 2;

    if (find_obituary(demo->fragfile, player_count, msg_count, buf1, buf2, fi, found_players))
        return 1;

    return 0;
}

int Fragfile_Parse_Message_Store(struct mvd_demo *demo, char *message)
{
    struct frag_info *fi;

    if (demo == NULL)
        return 1;

    if (message == NULL)
        return 1;

    fi = calloc(1, sizeof(struct frag_info));
    if (fi == NULL)
        return 1;

    if (Fragfile_Parse_Message(demo, message, fi))
    {
        free(fi);
        return 1;
    }

    fi->time = demo->time;

    if (demo->frags_start == NULL)
    {
        demo->frags_start = demo->frags_end = fi;
    }
    else
    {
        demo->frags_end->next = fi;
        demo->frags_end = fi;
    }
    return 0;
}

void Fragfile_Destroy(struct fragfile *ff)
{
    int i;

    if (ff == NULL)
        return;

    free(ff->title);
    free(ff->description);
    free(ff->author);
    free(ff->email);
    free(ff->webpage);

    for (i=0; i<ff->weapon_class_count; i++)
    {
        free(ff->weapon_class[i]->identifier);
        free(ff->weapon_class[i]->long_name);
        free(ff->weapon_class[i]->short_name);
        free(ff->weapon_class[i]->image_name);

        free(ff->weapon_class[i]);
    }

    free(ff->weapon_class);

    for (i=0; i<ff->obituary_count; i++)
    {
        free(ff->obituary[i]->msg1);
        free(ff->obituary[i]->msg2);
        free(ff->obituary[i]);
    }

    free(ff->obituary);

    free(ff);

    return 0;
}

