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
    PHP_FE(mvd_step, NULL)
    PHP_FE(mvd_load, NULL)
    PHP_FE(mvd_get_player_info, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry mvd_parsing_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_MVD_PARSING_EXTNAME,
    mvd_parse_functions,
	PHP_MINIT(mvd_parsing),
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

#define demo_resource_name "mvd demo"
static int demo_resource_handler_id;

void demo_destruct_handler(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct mvd_demo *demo;
	demo = (struct mvd_demo *)rsrc->ptr;
	if (demo)
		MVD_Destroy(demo);
}

PHP_MINIT_FUNCTION(mvd_parsing)
{
	demo_resource_handler_id = zend_register_list_destructors_ex(demo_destruct_handler, NULL, demo_resource_name, module_number);

	return SUCCESS;
}

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

	if (demo->map)
		add_assoc_string(return_value, "map", demo->map, 1);
	if (demo->map_name)
		add_assoc_string(return_value, "map_name", demo->map, 1);
	add_assoc_string(return_value, "name", demo->name, 1);
	add_assoc_long(return_value, "frames", (double) demo->frame);
	add_assoc_double(return_value, "time", demo->time);

	ALLOC_INIT_ZVAL(players);
	array_init_size(players, 32);

	add_assoc_zval(return_value, "players", players);


	count = 0;
	for (i=0; i<32; i++)
	{
		if (demo->players[i].name == NULL)
			continue;

		if (demo->players[i].name[0] == '\0')
			continue;

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
	MVD_Destroy(demo);
}


PHP_FUNCTION(mvd_load)
{
	char *name;
	int name_len;
	struct mvd_demo *demo;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE)
	{
		RETURN_NULL();
	}

	demo = MVD_Load_From_File(name);

	ZEND_REGISTER_RESOURCE(return_value, demo, demo_resource_handler_id);
}

PHP_FUNCTION(mvd_step)
{
	struct mvd_demo *demo;
	zval *zdemo;
	int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdemo) == FAILURE)
	{
		RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE(demo, struct mvd_demo *, &zdemo, -1, demo_resource_name, demo_resource_handler_id);


	retval = MVD_Step(demo);
	RETURN_LONG(retval);
}

PHP_FUNCTION(mvd_get_player_info)
{
	struct mvd_demo *demo;
	zval *zdemo;
	int retval;
	zval *players;
	zval *players_arrays[32];
	zval *current_player;
	int i, count;
	struct player *p;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zdemo) == FAILURE)
	{
		RETURN_NULL();
	}

	ZEND_FETCH_RESOURCE(demo, struct mvd_demo *, &zdemo, -1, demo_resource_name, demo_resource_handler_id);

	array_init(return_value);

	if (demo->map)
		add_assoc_string(return_value, "map", demo->map, 1);
	if (demo->map_name)
		add_assoc_string(return_value, "map_name", demo->map, 1);
	add_assoc_string(return_value, "name", demo->name, 1);
	add_assoc_long(return_value, "frames", (double) demo->frame);
	add_assoc_double(return_value, "time", demo->time);

	ALLOC_INIT_ZVAL(players);
	array_init_size(players, 32);

	add_assoc_zval(return_value, "players", players);


	count = 0;
	for (i=0; i<32; i++)
	{
		if (demo->players[i].name == NULL)
			continue;

		if (demo->players[i].name[0] == '\0')
			continue;

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
	return;
}


