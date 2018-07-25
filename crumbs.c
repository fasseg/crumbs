#include "crumbs.h"

// configuration parameters are stored here
struct crumb_config config;

// data directory file descriptor
int data_dir_fd;

int main(int argc, char **argv) {
    int opt;
    int opt_idx = 0;

    char *config_path = DEFAULT_CONFIG;

    const struct option opts[] = {
        {"help", no_argument, 0, 'h'},
        {"config", required_argument, 0, 'c'}
    };

    // parse the command line options
    while((opt = getopt_long(argc, argv, "hc:", opts, &opt_idx)) > 0) {
        switch(opt) {
            case 'h':
                show_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 'c':
                config_path = optarg;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    // read the configuration from a file
    read_config(config_path);

    // make sure that the data directory for crumbs exists and is usable
    init_data_dir(config.path);

    // parse the action requested by the user
    if (strcmp(argv[optind], "insert") == 0) {
        if (argc != optind + 3) fatal("Invalid number of arguments for insert");
        insert(argv[optind + 1], argv[optind + 2]);
    } else if (strcmp(argv[optind], "insert-exec") == 0) {
        if (argc != optind + 3) fatal("Invalid number of argument for insert-exec");
        insert_exec(argv[optind + 1], argv[optind + 2]);
    } else if (strcmp(argv[optind], "list") == 0) {
        if (argc != optind + 1) fatal("Invalid number of argument for list");
        list(config.path);
    } else if (strcmp(argv[optind], "exec") == 0) {
        if (argc != optind + 2) fatal("Invalid number of argument for exec");
        exec(argv[optind + 1]);
    } else if (strcmp(argv[optind], "show") == 0) {
        if (argc != optind + 2) fatal("Invalid number of argument for show");
        view(argv[optind + 1]);
    } else if (strcmp(argv[optind], "delete") == 0) {
        if (argc != optind + 2) fatal("Invalid number of arguments for delete");
        delete_crumb(argv[optind + 1]);
    }
}

void init_data_dir(const char *path) {
    struct stat ds;

    // make sure that the directory exists and can be accessed
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

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Unable to access data directory");
        exit(1);
    }

    // store a file descritptot for later use in a global variable
    data_dir_fd = dirfd(dir);
    if (data_dir_fd == -1) {
        perror("Unable to access data directory");
        exit(1);
    }
}

void read_config(char *cfg_path) {

    // read the configuration data from a file
    FILE *fp = fopen(cfg_path, "r");
    if (fp == NULL) {
        perror("FATAL: Unable to read config:");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, fp) != -1) {

        // read the data path from the configuration file
        if (strncmp(line, "path", 4) == 0) {
            char *path = strstr(line, "=") + 1;
            size_t cnt = 0;
            while (path[strlen(path) - 1 - cnt] == '\n' || path[strlen(path) - 1 - cnt] == '\r') {
                cnt++;
            }
            memset(path + strlen(path) - cnt, 0, cnt);
            if (path[0] != '/') {
                // handle a relative path 
                char *home = getenv("HOME");
                if (home == NULL) {
                    home = getpwuid(getuid())->pw_dir;
                }
                char *tmp = malloc(strlen(home) + strlen(path) + 2);
                strncpy(tmp, home, strlen(home));
                strcpy(tmp + strlen(home), "/");
                strncpy(tmp + strlen(home) + 1, path, strlen(path) + 1);
                config.path = tmp;
            } else {
                // handle an absolute path
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

    check_name(name);

    // Make sure that only relative paths are used
    if (*(name) == '/') {
        fatal("Name must be a relative path");
        exit(1);
    }

    // parse the directories in a path
    int offset = 0;
    int len = strlen(name);
    const char **dirs = malloc(0);
    int dirs_len = 0;
    for (int i = 0; i < len; i++) {
        if (*(name + i) == '/') {
            // prepare a list of directories to create
            char *dir = malloc(i - offset + 1);
            memcpy(dir, (name + offset), i - offset);
            memset((dir + i - offset), 0, 1);
            dirs = realloc(dirs, sizeof(char *) * (dirs_len + 1));
            dirs[dirs_len++]=dir;
            offset = i + 1;					
        }
    }

    create_dirs(dirs_len, dirs);

    // create the absolute path of the target file
    char *abs_path = calloc(len + strlen(config.path) + 2, sizeof(char));
    strncat(abs_path, config.path, strlen(config.path) + 1);
    strncat(abs_path, "/", 2);
    strncat(abs_path, name, len);

    // make sure that no crumb with the same path exists
    int exists = crumb_exists(name);
    if (exists == ECRUMBEXISTS) {
        printf("WARN: The crumb %s already exists. Do you want to overwrite it? [y|N] ", name);
        int choice = getchar();
        if (choice != 'y' && choice != 'Y') {
            return 0;
        }
    } else if (exists == EDIREXISTS) {
        fprintf(stderr, "ERROR: The crumb name is already used as a directory name\n");
        exit(EXIT_FAILURE);
    }

    // create the new crumb file
    FILE *fp = fopen(abs_path, "w");
    if (fp == NULL) {
        perror("Unable to create crumb file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", cmd);
    free(abs_path);

    printf("Successfully added the crumb %s\n", name);
    return 0;
}

int crumb_exists(const char *name) {

    // create a path containing the parent directories
    size_t len_parent = strlen(config.path);
    size_t len_name = strlen(name);
    char path[len_parent + len_name + 2];
    memset(path, 0, len_parent + len_name + 2);
    strncat(path, config.path, len_parent);
    strcat(path, "/");
    strncat(path, name, len_name);
    struct stat path_stat;
    if (stat(path, &path_stat) < 0) {
        return 0;
    }

    // check if the crumb already exists or if there's already a directory with the same name
    if (S_ISDIR(path_stat.st_mode)) {
        return EDIREXISTS;
    } else {    
        return ECRUMBEXISTS;
    }
}

void create_dirs(const int len, const char **dirs) {

    // create a hierarchy of nonexisting directories for a crumb to be stored under
    char *prefix = calloc(0, sizeof(char));
    for (int i = 0; i < len; i++) {
        const char *name = *(dirs + i);
        create_dir(prefix, name);
        prefix = realloc(prefix, strlen(prefix) + strlen(name) + 2);
        strncat(prefix, name, strlen(name) + 1);
        strncat(prefix, "/", 2);
    }    
    free(prefix);
}

void create_dir(const char *prefix, const char *name) {

    // create a path for creating the directory
    int len_prefix = strlen(prefix);
    int len_name = strlen(name);
    char *path = calloc(len_prefix + len_name + 2, sizeof(char));
    strncat(path, prefix, len_prefix + 1);
    strncat(path, name, len_name + 1);

    struct stat ds;

    printf("Checking path %s\n", path);

    // try to create the directory at the given path
    if (fstatat(data_dir_fd, path, &ds, 0) == -1) {
        if (errno == ENOENT) {
            if (mkdirat(data_dir_fd, path, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
                perror("Unable to create directory");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("Unable to check directory");
            exit(EXIT_FAILURE);
        }
    }
    free(path);
}

void view(const char *name) {

    check_name(name);

    // read and show a command from a stored crumb file
    char *cmd = read_command(name);
    printf("%s\n", cmd);
    free(cmd);
}

char * read_command(const char *name) {

    // create the path of the crumb file to read the command from
    int len = strlen(config.path) + strlen(name) + 2;
    char *path = calloc(sizeof(char), len);
    strncat(path, config.path, strlen(config.path) + 1);
    strncat(path, "/", 2);
    strncat(path, name, strlen(path) + 1);

    // open the file for reading
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fatal("Crumb does not exist");
    }
    fseek(fp, 0, SEEK_END);
    int num_bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *cmd = malloc(num_bytes + 1);
    memset(cmd, 0, num_bytes);

    // read the command from the file into a variable
    fread(cmd, sizeof(char), num_bytes, fp);
    return cmd;
}


int exec(const char *name) {

    check_name(name);

    // read the command and execute it on the system
    char *cmd = read_command(name);
    printf("Executing %s\n", cmd);
    int ret = system(cmd);
    return ret;
}

int insert_exec(const char *name, const char *cmd) {

    check_name(name);

    // try to execute the command
    int ret = system(cmd);
    if (ret != EXIT_SUCCESS) {
        fprintf(stderr, "ERROR: The command failed and will not be added to crumbs\n");
        return EXIT_FAILURE;
    }

    // and store the crumb in a file
    return insert(name, cmd);
}

int list(const char *path) {
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path);

    // list all crumbs in a given directory
    if (dir) {
        while((ent = readdir(dir)) != NULL)  {
            if (strncmp(ent->d_name, ".", 1) != 0 && strncmp(ent->d_name, "..", 1) != 0) {
                if (ent->d_type == DT_REG) {
                    // read the crumb name from the path if it's a file
                    if (strlen(path) > strlen(config.path)) {
                        printf("%s/", path + strlen(config.path) + 1);
                    }
                    printf("%s\n", ent->d_name);
                } else if (ent->d_type == DT_DIR) {
                    // recurse in a sub directory and read all crumbs
                    size_t curr_len = strlen(path);
                    size_t sub_len = strlen(ent->d_name);
                    char *sub = malloc(curr_len + sub_len + 2);
                    memset(sub, 0, curr_len + sub_len + 2);
                    strncat(sub, path, curr_len);
                    strcat(sub, "/");
                    strncat(sub, ent->d_name, sub_len);
                    list(sub);
                    free(sub);
                }             
            }
        }
        closedir(dir);
    }
    return 0;
};

void delete_crumb(const char *name) {

    check_name(name);

    size_t len = strlen(name);
    size_t offset = 0;
    char **segments = NULL;
    size_t count_segments = 0;

    for (int i = 0; i < len; i++) {

        if (*(name + i) == '/') {

            // split the name at slashes    
            int seg_len = i - offset;
            segments = realloc(segments, ++count_segments * sizeof(char *));
            segments[count_segments - 1] = malloc(seg_len + 1);
            memcpy(segments[count_segments - 1], name + offset, seg_len);
            segments[count_segments - 1][seg_len] = '\0';

            // skip multiple slashes in a row
            while (*(name + i) == '/') {
                offset = ++i;
            }
        }
    }

    // terminate the segments double pointer with a NULL element for easy iteration
    segments = realloc(segments, (count_segments + 1) * sizeof(char *));
    segments[count_segments] = NULL;

    // the rest of the string is the file name
    int file_len = len - offset;
    char file_name[file_len + 1];
    memcpy(file_name, name + offset, file_len);
    file_name[file_len] = '\0';

    // delete the file
    delete_file(segments, file_name); 

    // delete the empty path segments

    // free the allocated memory
    for (int i = 0; i < count_segments; i++) {
        free(segments[i]);
    }
    free(segments);
}

int delete_file(char **path_segments, char *file) {
    
    // prepare the path from the configured base directory
    size_t home_len = strlen(config.path);
    size_t len = home_len + 1;
    char *path = malloc(len);
    strncat(path, config.path, len - 1);
    strcat(path, "/");

    // add all segments to the path
    for (char **seg = path_segments; *seg; seg++) {
        size_t seg_len = strlen(*seg);
        len += seg_len;
        path = realloc(path, len + 2);
        strncat(path, *seg, seg_len);
        strcat(path, "/");
    }

    // add the file name to the path
    size_t file_len = strlen(file);
    len += file_len + 1;
    path=realloc(path, len);
    strncat(path, file, file_len);

    // delete the file and all empty directories in the hierarchy
    while(strlen(path) > home_len) {
        if (remove(path) < 0) {
            break;
        }
        printf("Deleting %s\n", path);
        char *separator = strrchr(path, '/');
        *separator = '\0';
    }

    free(path);

    return 0;
}

void fatal(const char *msg) {
    printf("FATAL: %s\n", msg);
    exit(1);
}

void check_name(const char *name) {
    size_t len = strlen(name);
    if (len == 0) {
        fprintf(stderr, "ERROR: The name must have at least one character\n");
        exit(EXIT_FAILURE);
    }
    if (len > 4095) {
        fprintf(stderr, "ERROR: The name must not have more than 4095 characters\n");
        exit(EXIT_FAILURE);
    }
    if (name[0] == '/') {
        fprintf(stderr, "ERROR: The name must not start with a path separator\n");
        exit(EXIT_FAILURE);
    }
    if (name[len - 1] == '/') {
        fprintf(stderr, "ERROR: THe name must not end with a path separator\n");
        exit(EXIT_FAILURE);
    }
    for (const char *ch = name; *ch != '\0'; ch++) {
        if (*ch < 33 || *ch > 126) {
            fprintf(stderr, "ERROR: The name must only contain printable ascii characters\n");
            exit(EXIT_FAILURE);
        }
    }
}


void show_help(char *progr_name) {
    printf("%s v%s\n", PROGRAM_NAME, PROGRAM_VERSION);
    printf("Crumb is a command bookmarking system for the console\n\n");
    printf("Usage: %s [options] <action>\n", progr_name);
    printf("\nActions:\n");
    printf("\tlist\t\t\t\tList the current crumbs\n");
    printf("\tinsert <name> <cmdline>\t\tAdd a new crumb\n");
    printf("\tinsert-exec <name> <cmdline>\tExecute and add a new crumb\n");
    printf("\tshow <name>\t\t\tShow a crumb\n");
    printf("\texec <name>\t\t\tExecute a crumb\n");
    printf("\tdelete <name>\t\t\tDelete a crumb\n");
    printf("\nOptions:\n");
    printf("\t-h/--help\t\t\tShow this help dialog and exit\n");
}
