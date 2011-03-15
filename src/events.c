#include "defaults.h"
#include "common.h"
#include "mvd_parser.h"

static struct event *add_event(struct mvd_demo *demo)
{
	struct event *e, **list, **old_list;
	int i;

	if (demo == NULL)
		return NULL;

	e = calloc(1, sizeof(struct event));

	if (e == NULL)
		return NULL;

	list = calloc(demo->event_count + 2, sizeof(struct event **));

	if (list == NULL)
	{
		free(e);
		return NULL;
	}

	old_list = demo->event;

	for (i=0; i<demo->event_count; i++)
		list[i] = old_list[i];

	list[i] = e;

	free(old_list);

	demo->event = list;

	demo->event_count++;

	return e;
}

int Event_Add_Sound(struct mvd_demo *demo, vec3_t origin, int sound)
{
	struct event *e;
	struct sound_event *s;

	if (demo == NULL)
		return 1;

	e = add_event(demo);

	if (e == NULL)
		return 1;

	s = calloc(1, sizeof(struct sound_event));

	if (s == NULL)
		return 1;

	e->type = EVENT_SOUND;
	
	e->data = (void *)s;

	s->origin[0] = origin[0];
	s->origin[1] = origin[1];
	s->origin[2] = origin[2];

	s->sound = sound;

	return 0;
}

int Event_Add_Print(struct mvd_demo *demo, char *string)
{
	struct event *e;

	if (demo == NULL)
		return 1;

	if (string == NULL)
		return 1;

	e = add_event(demo);

	if (e == NULL)
		return 1;

    e->type = EVENT_PRINT;
	e->data = (void *) string;

	return 0;
}


void Event_Cleanup(struct mvd_demo *demo)
{
	struct event **l;
	struct sound_event *s;

	if (demo == NULL)
		return;

	l = demo->event;

	if (l == NULL)
		return;

	while (*l)
	{

		if ((*l)->type == EVENT_SOUND)
		{
			s = (struct sound_event *) (*l)->data;
			free(s);
		}
		else
			free((*l)->data);
		free(*l);
		l++;
	}

	free(demo->event);

	demo->event = NULL;
	demo->event_count = 0;
}
