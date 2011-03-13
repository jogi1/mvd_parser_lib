#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_mvd_parsing.h"
#include <stdbool.h>
#include "mvdparse.h"
#include <zend_API.h>

static function_entry mvd_parse_functions[] = {
    PHP_FE(mvd_parse, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry mvd_parsing_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_MVD_PARSING_EXTNAME,
    mvd_parse_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_MVD_PARSING_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_MVD_PARSING
ZEND_GET_MODULE(mvd_parsing)
#endif

PHP_FUNCTION(mvd_parse)
{
	char *name;
	int name_len;
	zval *players;
	zval *players_arrays[32];
	zval *current_player;
	int i, count;

	struct player *p;

	struct mvd_demo *demo;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
	{
		RETURN_NULL();
	}

	demo = MVD_Load_From_File(name);

	if (demo == NULL)
	{
		php_printf("could not load \"%s\".\n", name);
		RETURN_NULL();
	}

	MVD_Parse(demo);


	array_init(return_value);

	add_assoc_string(return_value, "map", demo->map_name, 1);
	add_assoc_string(return_value, "name", demo->name, 1);
	add_assoc_long(return_value, "frames", (double) demo->frame);
	add_assoc_double(return_value, "time", demo->time);

	ALLOC_INIT_ZVAL(players);
	array_init_size(players, 32);

	add_assoc_zval(return_value, "players", players);


		count = 0;
		for (i=0; i<32; i++)
		{
			if (demo->players[i].name)
			{
				ALLOC_INIT_ZVAL(current_player);
				array_init_size(current_player, 10);

				add_next_index_zval(players, current_player);

				p = &demo->players[i];

				add_assoc_string(current_player, "name", p->name, 1);
				if (p->team)
					add_assoc_string(current_player, "team", p->team, 1);
				add_assoc_long(current_player, "frags", p->frags);
				add_assoc_long(current_player, "ping", p->ping);
				add_assoc_long(current_player, "pl", p->pl);
			}
		}
	MVD_Destroy(demo);
}
