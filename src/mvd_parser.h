#define CM_MSEC (1<<7)

#define MPF_GATHER_STATS (1<<0)
#define MPF_STATS_INITIALIZED (1<<1)

#define PFS_HEALTH (1<<0)
#define PFS_ARMOR (1<<1)
#define PFS_ITEMS (1<<2)

#define EVENT_INVALID 0
#define EVENT_SOUND 1
#define EVENT_PRINT 2

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef unsigned char byte;

struct string
{
	double time;
	char *string;
};

struct stats
{
	// ktx wp stats
	int sg_attacks, sg_hits;
	int ssg_attacks, ssg_hits;
	int ng_attacks, ng_hits;
	int sng_attacks, sng_hits;
	int gl_attacks, gl_hits;
	int rl_attacks, rl_hits;
	int lg_attacks, lg_hits;

	int jumps;

	int ga, ya, ra;

	int mh;

	int quad, pent, ring;

	int ssg, ng, sng, gl, rl, lg;

	int deaths;


};

struct player
{
	int userid;

	char *name;
	char *name_readable;
	char *team;
	char *team_readable;

	float entertime;
	int ping;
	int pl;

	int frags;

	int flags;

	char *userinfo;

	int spectator;

	unsigned char topcolor;
	unsigned char bottomcolor;

	int stats[MAX_CL_STATS];

	vec3_t angles;
	vec3_t origin;

	struct stats *statistics;
};

struct event
{
	int type;
	void *data;
};

struct sound_event
{
	vec3_t origin;
	int sound;
};

struct mvd_demo
{
	char *name;

	char *buf;		// demo in memory
	unsigned int size;	// size

	char *current_position;	// current position parsing through the demo

	unsigned int frame;

	unsigned int flags;

	unsigned int current_message_size;
	unsigned char *current_message_position;
	unsigned int current_message_read_count;
	int current_message_bad_read;

	int last_to;
	int last_type;

	int outgoing_sequence, incoming_sequence;

	double time;

	int game_started;

	int msg_readcount;

	struct mvd_frame *current_frame, *last_frame;

	char *map_name;
	char *map;

	char *hostname;
	char *hostname_readable;

	char *serverinfo;

	int error;

	int ended;

	char **sound_list;
	int sound_list_count;
	int sound_list_size;

	char **model_list;
	int model_list_count;
	int model_list_size;

	struct player players[32];
	struct player players_last_frame[32];

	struct event **event;
	int event_count;

	char print_buffer[1024];
	int print_buffer_position;

    struct fragfile *fragfile;

    struct frag_info *frags_start;
    struct frag_info *frags_end;


};

