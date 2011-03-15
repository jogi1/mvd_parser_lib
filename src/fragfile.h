enum frag_msg_type
{
    fmt_death,
    fmt_suicide,
    fmt_teamkilled_unknown,
    fmt_teamkills_unknown,
    fmt_x_fragged_by_y,
    fmt_x_frags_y,
    fmt_x_teamkills_y,
    fmt_x_teamkilled_by_y

};

struct found_players
{
    struct player *player;
    char *start, *end;
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
    enum frag_msg_type type;
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

    struct weapon_class **weapon_class;
    int weapon_class_count;

    struct obituary **obituary;
    int obituary_count;

};

struct fragfile *Fragfile_Load(char *filename);
int Fragfile_Parse_Message(struct mvd_demo *demo, char *message, struct frag_info *fi);
int Fragfile_Parse_Message_Store(struct mvd_demo *demo, char *message);
void Fragfile_Destroy(struct fragfile *ff);
