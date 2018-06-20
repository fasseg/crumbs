#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>

#define PROGRAM_NAME "Crumbs"
#define PROGRAM_VERSION "0.0.1"

#define ACTION_INSERT 0
#define ACTION_INSERT_EXEC 1
#define ACTION_LIST 2
#define ACTION_EXEC 3

#define DEFAULT_CONFIG "/etc/crumb.conf"

struct crumb_config {
    char *path;
};

void show_help(char *progr_name);

int insert(const char *name, const char *cmd);

int insert_exec(const char *name, const char *cmd);

int exec(const char *name);

int list();

void read_config(char *path);

void fatal(const char *msg);

void check_data_dir(const char *path);
