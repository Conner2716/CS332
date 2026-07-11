/*
Name: Conner McKelvy

BlazerId: mckelvyc

Project: Homework 2

To compile: gcc -Wall -o homework mckelvyc_HW02.c
To run: ./homework [-S] [-s <size>] [-f <pattern> <depth>] [-t f|d] [directory]

*/

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

static int  opt_S = 0;
static int  opt_s = 0;
static long opt_s_value = 0;
static int  opt_f = 0;
static char opt_f_pattern[PATH_MAX] = "";
static int  opt_f_depth = 0;
static int  opt_t = 0;
static char opt_t_value = '\0';

static int filters_active(void) {
    return opt_s || opt_f || opt_t;
}


typedef int (*Predicate)(const char *name, int depth, const struct stat *st);
typedef void (*Printer)(const char *display_path, const char *name,
                         const struct stat *st, int is_symlink,
                         const char *link_target, int indent);


static int pred_size(const char *name, int depth, const struct stat *st) {
    (void)name; (void)depth;
    if (S_ISDIR(st->st_mode))
        return 1; 
    return st->st_size <= opt_s_value;
}

static int pred_pattern_depth(const char *name, int depth, const struct stat *st) {
    (void)st;
    if (depth > opt_f_depth)
        return 0;
    return strstr(name, opt_f_pattern) != NULL;
}

static int pred_type(const char *name, int depth, const struct stat *st) {
    (void)name; (void)depth;
    if (opt_t_value == 'f')
        return S_ISREG(st->st_mode);
    if (opt_t_value == 'd')
        return S_ISDIR(st->st_mode);
    return 1;
}

static Predicate active_predicates[8];
static int       n_active_predicates = 0;

static int passes_all_filters(const char *name, int depth, const struct stat *st) {
    for (int i = 0; i < n_active_predicates; i++) {
        if (!active_predicates[i](name, depth, st))
            return 0;
    }
    return 1;
}

//helpers

static void mode_to_string(mode_t mode, char *buf) {
    buf[0] = S_ISDIR(mode)  ? 'd' :
             S_ISLNK(mode)  ? 'l' : '-';
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
}

static void format_atime(time_t t, char *buf, size_t buflen) {
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    strftime(buf, buflen, "%b %d %H:%M:%S %Y", &tm_info);
}

static void print_name(const char *name, int is_symlink, const char *link_target) {
    if (is_symlink)
        printf("%s (%s)", name, link_target);
    else
        printf("%s", name);
}

//printers
static void print_plain(const char *display_path, const char *name,
                         const struct stat *st, int is_symlink,
                         const char *link_target, int indent) {
    (void)display_path; (void)st;
    for (int i = 0; i < indent; i++)
        putchar('\t');
    print_name(name, is_symlink, link_target);
    putchar('\n');
}

static void print_detailed(const char *display_path, const char *name,
                            const struct stat *st, int is_symlink,
                            const char *link_target, int indent) {
    (void)display_path;
    char perms[11];
    char atime_buf[64];
    long size = S_ISDIR(st->st_mode) ? 0 : (long)st->st_size;

    mode_to_string(st->st_mode, perms);
    format_atime(st->st_atime, atime_buf, sizeof(atime_buf));

    for (int i = 0; i < indent; i++)
        putchar('\t');
    print_name(name, is_symlink, link_target);
    printf(" (%ld, %s, %s)\n", size, perms, atime_buf);
}

static Printer active_printer = print_plain;

//traversal

static int visit_entry(const char *dir_path, const char *name, int depth,
                        struct stat *st_out, int *is_symlink_out,
                        char *link_target_out, size_t link_target_len) {
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, name);

    if (lstat(full_path, st_out) != 0) {
        perror(full_path);
        return 0;
    }

    *is_symlink_out = S_ISLNK(st_out->st_mode);
    link_target_out[0] = '\0';
    if (*is_symlink_out) {
        ssize_t n = readlink(full_path, link_target_out, link_target_len - 1);
        if (n >= 0)
            link_target_out[n] = '\0';
    }

    int should_print = filters_active()
        ? passes_all_filters(name, depth, st_out)
        : 1;

    if (should_print)
        active_printer(full_path, name, st_out, *is_symlink_out,
                        link_target_out, depth);

    
    return S_ISDIR(st_out->st_mode) && !*is_symlink_out;
}

static void traverse_dir(const char *dir_path, int depth) {
    DIR *dp = opendir(dir_path);
    if (dp == NULL) {
        perror(dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        struct stat st;
        int is_symlink;
        char link_target[PATH_MAX];

        int recurse = visit_entry(dir_path, entry->d_name, depth, &st,
                                   &is_symlink, link_target, sizeof(link_target));

        if (recurse) {
            char child_path[PATH_MAX];
            snprintf(child_path, sizeof(child_path), "%s/%s", dir_path, entry->d_name);
            traverse_dir(child_path, depth + 1);
        }
    }

    closedir(dp);
}

static void run_search(const char *start_dir) {
    struct stat st;
    if (lstat(start_dir, &st) != 0) {
        perror(start_dir);
        exit(EXIT_FAILURE);
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "%s: not a directory\n", start_dir);
        exit(EXIT_FAILURE);
    }

    if (!filters_active() || passes_all_filters(start_dir, 0, &st))
        active_printer(start_dir, start_dir, &st, 0, "", 0);

    traverse_dir(start_dir, 1);
}

//argument parsing / main

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [-S] [-s size] [-f pattern depth] [-t f|d] [directory]\n",
        prog);
}

int main(int argc, char *argv[]) {
    int opt;

   
    while ((opt = getopt(argc, argv, "Ss:f:t:")) != -1) {
        switch (opt) {
            case 'S':
                opt_S = 1;
                break;
            case 's':
                opt_s = 1;
                opt_s_value = atol(optarg);
                break;
            case 'f':
                opt_f = 1;
                strncpy(opt_f_pattern, optarg, sizeof(opt_f_pattern) - 1);
                if (optind >= argc) {
                    fprintf(stderr, "-f requires a <pattern> and a <depth>\n");
                    usage(argv[0]);
                    return EXIT_FAILURE;
                }
                opt_f_depth = atoi(argv[optind]);
                optind++;
                break;
            case 't':
                opt_t = 1;
                opt_t_value = optarg[0];
                if (opt_t_value != 'f' && opt_t_value != 'd') {
                    fprintf(stderr, "-t requires 'f' or 'd'\n");
                    usage(argv[0]);
                    return EXIT_FAILURE;
                }
                break;
            default:
                usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (opt_s)
        active_predicates[n_active_predicates++] = pred_size;
    if (opt_f)
        active_predicates[n_active_predicates++] = pred_pattern_depth;
    if (opt_t)
        active_predicates[n_active_predicates++] = pred_type;

    active_printer = opt_S ? print_detailed : print_plain;

    const char *start_dir = ".";
    if (optind < argc)
        start_dir = argv[optind];

    run_search(start_dir);

    return EXIT_SUCCESS;
}