#ifndef PHP_MVD_PARSING_H
#define PHP_MVD_PARSING_H 1

#define PHP_MVD_PARSING_VERSION "0.1"
#define PHP_MVD_PARSING_EXTNAME "mvd_parsing"



PHP_MINIT_FUNCTION(mvd_parsing);
PHP_FUNCTION(mvd_parse);
PHP_FUNCTION(mvd_load);
PHP_FUNCTION(mvd_step);
PHP_FUNCTION(mvd_get_player_info);

extern zend_module_entry mvd_parsing_module_entry;
#define phpext_mvd_parsing_ptr &mvd_parsing_module_entry

#endif

