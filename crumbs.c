#include "crumbs.h"

struct crumb_config config;

int main(int argc, char **argv) {
    int opt;
    int opt_idx = 0;

    char *config_path = DEFAULT_CONFIG;

    static struct option opts[] = {
        {"help", no_argument, 0, 'h'},
        {"config", required_argument, 0, 'c'}
    };

    while((opt = getopt_long(argc, argv, "hc:", opts, &opt_idx)) > 0) {
        switch(opt) {
            case 'h':
                show_help(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'c':
                config_path = optarg;
                break;
        }
    }

    read_config(config_path);
    check_data_dir(config.path);

    if (strncmp(argv[optind], "insert", 5) == 0) {
        if (argc != optind + 3) fatal("Invalid number of arguments for insert");
        insert(argv[optind + 1], argv[optind + 2]);
    } else if (strncmp(argv[optind], "insert-exec", 10) == 0) {
        if (argc != optind + 3) fatal("Invalid number of argument for insert-exec");
        insert_exec(argv[optind + 1], argv[optind + 2]);
    } else if (strncmp(argv[optind], "list", 4) == 0) {
        if (argc != optind + 1) fatal("Invalid number of argument for list");
        list();
    } else if (strncmp(argv[optind], "exec", 4) == 0) {
        if (argc != optind + 2) fatal("Invalid number of argument for exec");
        exec(argv[optind + 1]);
    }
}

void check_data_dir(const char *path) {
    struct stat ds;
    if (stat(path, &ds) == -1) {
        if (errno == ENOENT) {
            printf("Initializing data directory %s\n", path);
            if (mkdir(path, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
                perror("Unable to create directory");
                exit(1);
            }
        } else {
            perror("Unable to access data directory");
            exit(1);
        }
    }
}

void read_config(char *cfg_path) {
    FILE *fp = fopen(cfg_path, "r");
    if (fp == NULL) {
        perror("FATAL: Unable to read config:");
        exit(1);
    }

    char *line = NULL;
    ssize_t len = 0;
    while(getline(&line, &len, fp) != -1) {
        if (strncmp(line, "path", 4) == 0) {
            char *path = strstr(line, "=") + 1;
            size_t cnt = 0;
            while (path[strlen(path) - 1 - cnt] == '\n' || path[strlen(path) - 1 - cnt] == '\r') {
                cnt++;
            }
            memset(path + strlen(path) - cnt, 0, cnt);
            if (path[0] != '/') {
                char *home = getenv("HOME");
                if (home == NULL) {
                    home = getpwuid(getuid())->pw_dir;
                }
                char *tmp = malloc(strlen(home) + strlen(path) + 2);
                strncpy(tmp, home, strlen(home));
                strncpy(tmp + strlen(home), "/", 1);
                strncpy(tmp + strlen(home) + 1, path, strlen(path) + 1);
                config.path = tmp;
            } else {
                config.path = malloc(strlen(path) + 1);
                strncpy(config.path, path, len + 1);
            }
        }
    }
    free(line);

    // check that the config is valid
    if (config.path == NULL) {
        fatal("Unable to read path from config file");
    }

    fclose(fp);
}

int insert(const char *name, const char *cmd) {
    int len = strlen(config.path) + strlen(name) + 2;
    char *full_path = malloc(len);
    memset(full_path, 0, len);
    strncat(full_path, config.path, strlen(config.path) + 1);
    strncat(full_path, "/", 2);
    strncat(full_path, name, strlen(name));
    printf("Inserting file %s\n", &full_path[0]);
    FILE *fp = fopen(full_path, "w");
    if (fp == NULL) {
        perror("Unable to create data file");
        exit(1);
    } 
    fprintf(fp, "%s", cmd);
    fclose(fp);
    free(full_path);
    return 0;
}

int exec(const char *name) {
    int len = strlen(config.path) + strlen(name) + 2;
    char *path = calloc(sizeof(char), len);
    strncat(path, config.path, strlen(config.path) + 1);
    strncat(path, "/", 2);
    strncat(path, name, strlen(path) + 1);
    printf("Reading crumb %s\n", path);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fatal("Crumb does not exist");
    }
    fseek(fp, 0, SEEK_END);
    int numBytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *cmd = malloc(numBytes);
    fread(cmd, sizeof(char), numBytes, fp);
    printf("Executing %s...\n", cmd);
    int ret = system(cmd);
    fclose(fp);
    free(path);
    return 0;
}

int insert_exec(const char *name, const char *cmd) {
    return 0;
}

int list() {
    return 0;
};

void fatal(const char *msg) {
    printf("FATAL: %s\n", msg);
    exit(1);
}

void show_help(char *progr_name) {
    printf("%s v%s\n", PROGRAM_NAME, PROGRAM_VERSION);
    printf("Crumb is a command bookmarking system for the console\n");
    printf("Usage: %s [options] <action>\n", progr_name);
    printf("\nActions:\n");
    printf("\tinsert <name> <cmdline>\t\tAdd a new crumb\n");
    printf("\tinsert-exec <name> <cmdline>\tExecute and add a new crumb\n");
    printf("\tlist\t\t\t\tList the current crumbs\n");
    printf("\texec <name>\t\t\tExecute a crumb\n");
    printf("\nOptions:\n");
    printf("\t-h/--help\t\tShow this help dialog and exit\n");
}
