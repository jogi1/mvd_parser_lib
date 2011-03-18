#define FT_VICTIM_ONLY (1<<0)
#define FT_KILLER_ONLY (1<<1)
#define FT_KILLER_VICTIM (1<<2)
#define FT_VICTIM_KILLER (1<<3)

struct found_players
{
    struct player *player;
    char *start, *end;
};

struct frag_type
{
    char *identifier;
    char *name;
    int flags;
};

struct weapon_class
{
    char *identifier;
    char *long_name;
    char *short_name;
    char *image_name;
};

struct obituary
{
    struct frag_type *ft;
    struct weapon_class *wc;
    char *msg1;
    char *msg2;
};

struct frag_info
{
    struct frag_info *next;
    double time;
    struct player *killer, *victim;
    struct weapon_class *wc;
};

struct fragfile
{
    // meta info
    char *title;
    char *description;
    char *author;
    char *email;
    char *webpage;

    struct frag_type **frag_type;
    int frag_type_count;

    struct weapon_class **weapon_class;
    int weapon_class_count;

    struct obituary **obituary;
    int obituary_count;
};

struct fragfile *Fragfile_Load(char *filename);
int Fragfile_Parse_Message(struct mvd_demo *demo, char *message, struct frag_info *fi);
int Fragfile_Parse_Message_Store(struct mvd_demo *demo, char *message);
void Fragfile_Destroy(struct fragfile *ff);
