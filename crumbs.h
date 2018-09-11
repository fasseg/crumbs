#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <stdbool.h>

#define PROGRAM_NAME "Crumbs"
#define PROGRAM_VERSION "0.0.1"

#define ACTION_INSERT 0
#define ACTION_INSERT_EXEC 1
#define ACTION_LIST 2
#define ACTION_EXEC 3

#define CRUMBEXISTS -1
#define EDIREXISTS -2

#define DEFAULT_CONFIG SYSCONFDIR "/crumbs.conf"

struct crumb_config {
    char *path;
};

void show_help(char *progr_name);

int insert(const char *name, const char *cmd);

int insert_exec(const char *name, const char *cmd);

int exec(const char *name);

int list(const char *path);

void read_config(char *path);

void init_data_dir(const char *path);

void create_dirs(const int len, const char **dirs);

void create_dir(const char *prefix, const char *name);

int crumb_exists(const char *name);

char * read_command(const char *name);

void view(const char *name);

void delete_crumb(const char *name);

int delete_file(char **path_segments, char *file);

void check_name(const char *name);

void edit(const char *name);

char *path_for_name(const char *name);
