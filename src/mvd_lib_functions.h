
int MVD_Parse(struct mvd_demo *demo);
int MVD_Step(struct mvd_demo *demo);
struct mvd_demo *MVD_Load_From_File(char *filename);
void MVD_Destroy(struct mvd_demo *demo);
int MVD_Get_Stats(struct mvd_demo *demo);
int MVD_Load_Fragfile(struct mvd_demo *demo, char *filename);
