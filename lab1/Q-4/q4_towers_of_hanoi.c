#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define OUTPUT_FILE "hanoi.dat"
#define TEMP_PLOT_PREFIX "q4_plot"
#define SVG_FILE "hanoi_moves.svg"
#define MAX_DISCS 25

static const char PLOT_COMMANDS[] =
    "set encoding utf8\n"
    "set terminal svg size 1600,900 dynamic enhanced font \"Arial,12\" background rgb \"white\"\n"
    "set output \"hanoi_moves.svg\"\n"
    "set border linewidth 1.2 linecolor rgb \"#334155\"\n"
    "set grid xtics ytics mxtics mytics back linecolor rgb \"#dbe4ee\" linewidth 1\n"
    "set tics textcolor rgb \"#334155\"\n"
    "set key opaque box linewidth 0.7 spacing 1.15 font \",11\"\n"
    "set title \"Question 4 · Towers of Hanoi move count\" font \",18\"\n"
    "set xlabel \"Number of discs n\"\n"
    "set ylabel \"Total number of moves\"\n"
    "set xrange [1:25]\n"
    "set xtics 1\n"
    "set format y \"%.0s%c\"\n"
    "set style line 1 linecolor rgb \"#2563eb\" linewidth 3 pointtype 7 pointsize 0.8\n"
    "set style line 2 linecolor rgb \"#dc2626\" linewidth 2 dashtype 2\n"
    "plot \"hanoi.dat\" using 1:2 with points ls 1 title \"Recursive simulation\", \\\n"
    "     \"\" using 1:3 with lines ls 2 title \"2ⁿ − 1\"\n";


#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define CHANGE_DIRECTORY _chdir
#else
#include <unistd.h>
#define CHANGE_DIRECTORY chdir
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

static int get_executable_path(char path[], size_t size, const char *program_path)
{
#ifdef _WIN32
    DWORD length = GetModuleFileNameA(NULL, path, (DWORD) size);
    if (length > 0 && (size_t) length < size) {
        path[length] = '\0';
        return 1;
    }
#elif defined(__APPLE__)
    uint32_t buffer_size = (uint32_t) size;
    if (_NSGetExecutablePath(path, &buffer_size) == 0)
        return 1;
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", path, size - 1);
    if (length > 0 && (size_t) length < size) {
        path[length] = '\0';
        return 1;
    }
#endif

    if (program_path != NULL && program_path[0] != '\0' && strlen(program_path) < size) {
        strcpy(path, program_path);
        return 1;
    }

    return 0;
}

static void move_to_executable_folder(const char *program_path)
{
    char path[4096];
    char *slash;
    char *backslash;
    char *separator;

    if (!get_executable_path(path, sizeof(path), program_path))
        return;

    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    separator = slash;

    if (backslash != NULL && (separator == NULL || backslash > separator))
        separator = backslash;

    if (separator == NULL)
        return;

    if (separator == path || (separator == path + 2 && path[1] == ':'))
        separator[1] = '\0';
    else
        *separator = '\0';

    if (CHANGE_DIRECTORY(path) != 0)
        printf("Warning: could not switch to the executable folder.\n");
}

static unsigned long current_process_id(void)
{
#ifdef _WIN32
    return (unsigned long) GetCurrentProcessId();
#else
    return (unsigned long) getpid();
#endif
}

static void generate_and_open_graph(const char *plot_commands,
                                    const char *temporary_prefix,
                                    const char *svg_file)
{
    char temporary_script[256];
    char command[1024];
    FILE *script_file;
    int result;

    if (getenv("DAA_SKIP_PLOT") != NULL)
        return;

    snprintf(temporary_script, sizeof(temporary_script), "%s_%lu.tmp",
             temporary_prefix, current_process_id());

    script_file = fopen(temporary_script, "w");
    if (script_file == NULL) {
        printf("Could not create the temporary GNUPlot command file.\n");
        return;
    }

    fputs(plot_commands, script_file);
    if (fclose(script_file) != 0) {
        remove(temporary_script);
        printf("Could not finish writing the temporary GNUPlot command file.\n");
        return;
    }

    snprintf(command, sizeof(command), "gnuplot \"%s\"", temporary_script);
    result = system(command);
    remove(temporary_script);

    if (result != 0) {
        printf("GNUPlot could not generate the graph. Make sure GNUPlot is installed and added to PATH.\n");
        printf("No external .plt file is required; the plotting commands are embedded in this program.\n");
        return;
    }

    printf("SVG graph generated: %s\n", svg_file);

    if (getenv("DAA_NO_OPEN") != NULL)
        return;

#ifdef _WIN32
    snprintf(command, sizeof(command), "start \"\" \"%s\"", svg_file);
#elif __APPLE__
    snprintf(command, sizeof(command), "open \"%s\" >/dev/null 2>&1 &", svg_file);
#else
    snprintf(command, sizeof(command), "xdg-open \"%s\" >/dev/null 2>&1 &", svg_file);
#endif

    (void) system(command);
}

static unsigned long long moves;

static void solve_hanoi(int discs, char source, char helper, char destination, int show_moves)
{
    if (discs == 0)
        return;

    solve_hanoi(discs - 1, source, destination, helper, show_moves);
    moves++;
    if (show_moves)
        printf("Move disc %d from %c to %c\n", discs, source, destination);
    solve_hanoi(discs - 1, helper, source, destination, show_moves);
}

int main(int argc, char *argv[])
{
    FILE *file;
    int n;

    if (argc > 0)
        move_to_executable_folder(argv[0]);

    file = fopen(OUTPUT_FILE, "w");
    if (file == NULL) {
        printf("Could not create %s\n", OUTPUT_FILE);
        return 1;
    }

    printf("Towers of Hanoi demonstration for 3 discs:\n");
    moves = 0;
    solve_hanoi(3, 'A', 'B', 'C', 1);
    printf("Total moves: %llu\n\n", moves);

    fprintf(file, "# discs simulated_moves theoretical_moves\n");
    for (n = 1; n <= MAX_DISCS; n++) {
        unsigned long long theoretical_moves = (1ULL << n) - 1ULL;
        moves = 0;
        solve_hanoi(n, 'A', 'B', 'C', 0);
        fprintf(file, "%d %llu %llu\n", n, moves, theoretical_moves);
    }

    fclose(file);
    printf("Data written to %s\n", OUTPUT_FILE);

    generate_and_open_graph(PLOT_COMMANDS, TEMP_PLOT_PREFIX, SVG_FILE);
    return 0;
}
