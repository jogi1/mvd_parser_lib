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
#include "events.h"
#include "tools.h"
#include "mvd_defs.h"
#include <math.h>

char *Get_Userinfo_Value_By_Key(char *key, char *userinfo)
{
	char buf[128];
	char *index = NULL;
	char *index_org = NULL;
	int _size = 0;
	char *r;

	sprintf(buf, "\\%s\\", key);
	index = strstr(userinfo, buf);

	if (index == NULL)
		return NULL;

	index += strlen(buf);
	index_org = index;

	while (*index != '\\' && *index != 0)
	{
		_size++;
		index++;
	}
#if !_WIN32
	return strndup(index_org, _size);
#else
	r = calloc(_size+1, sizeof(char));

	if (r == NULL)
		return NULL;

	memcpy(r, index_org, _size);

	return r;
#endif
}

char *my_strdup(char *s)
{
	char *r;
	int len;
	int align;

	if (s == NULL)
		return NULL;

	len = strlen(s);

	align = 1;
	while ((len + align) % 4 != 0)
		align++;

	r = malloc(len + align);

	if (r == NULL)
		return NULL;

	memcpy(r, s, len);

	r[len] = '\0';

	return r;
}

float VectorDistance (vec3_t a, vec3_t b)
{
	vec3_t c;

	VectorSubtract(a, b, c);

	return sqrt(pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2));
}

int is_player(struct player *p)
{
	if (p->name == NULL)
		return 0;

	if (p->name[0] == '\0')
		return 0;

	if (p->spectator == 1)
		return 0;

	return 1;
}
