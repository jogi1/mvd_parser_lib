char *Get_Userinfo_Value_By_Key(char *key, char *userinfo);
#define FLAG_SET(X, Y) (X |= Y)
#define FLAG_UNSET(X, Y) (X &= ~Y)
#define FLAG_CHECK(X, Y) (X & Y)
float VectorDistance (vec3_t a, vec3_t b);
int is_player(struct player *p);
