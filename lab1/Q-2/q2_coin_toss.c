#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define OUTPUT_FILE "coin_toss.dat"
#define TEMP_PLOT_PREFIX "q2_plot"
#define SVG_FILE "coin_comparison.svg"
#define MAX_TOSSES 50000
#define BIASED_HEAD_PROBABILITY 0.70

static const char PLOT_COMMANDS[] =
    "set encoding utf8\n"
    "set terminal svg size 1800,1050 dynamic enhanced font \"Arial,12\" background rgb \"white\"\n"
    "set output \"coin_comparison.svg\"\n"
    "set border linewidth 1.3 linecolor rgb \"#334155\"\n"
    "set grid xtics ytics mxtics back linecolor rgb \"#dbe4ee\" linewidth 1\n"
    "set tics textcolor rgb \"#334155\"\n"
    "set key opaque box linewidth 0.7 spacing 1.15 font \",11\"\n"
    "set title \"Question 2 · Fair and biased coin simulation\" font \",19\"\n"
    "set xlabel \"Number of tosses · logarithmic scale\"\n"
    "set ylabel \"Fair coin · observed P(HEAD)\"\n"
    "set y2label \"Biased coin · observed P(HEAD)\"\n"
    "set logscale x 10\n"
    "set xrange [1:50000]\n"
    "set yrange [0.35:1.05]\n"
    "set y2range [0.45:1.05]\n"
    "set ytics 0.10\n"
    "set y2tics 0.10\n"
    "set format y \"%.2f\"\n"
    "set format y2 \"%.2f\"\n"
    "set style line 1 linecolor rgb \"#2563eb\" linewidth 3.2\n"
    "set style line 2 linecolor rgb \"#dc2626\" linewidth 3.2\n"
    "set style line 3 linecolor rgb \"#2563eb\" linewidth 2 dashtype 2\n"
    "set style line 4 linecolor rgb \"#dc2626\" linewidth 2 dashtype 2\n"
    "plot \"coin_toss.dat\" using 1:2 axes x1y1 with lines ls 1 title \"Fair coin · observed\", \\\n"
    "     \"\" using 1:3 axes x1y2 with lines ls 2 title \"Biased coin · observed\", \\\n"
    "     0.50 axes x1y1 with lines ls 3 title \"Fair target 0.50\", \\\n"
    "     0.70 axes x1y2 with lines ls 4 title \"Biased target 0.70\"\n";


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

static uint32_t state = 425050u;

static uint32_t next_random(void)
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

static double random_unit(void)
{
    return (double) next_random() / 4294967296.0;
}

static int should_record(int toss)
{
    if (toss <= 100)
        return 1;
    if (toss <= 1000 && toss % 10 == 0)
        return 1;
    if (toss <= 10000 && toss % 100 == 0)
        return 1;
    if (toss % 500 == 0)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *file;
    int fair_heads = 0;
    int biased_heads = 0;
    int toss;

    if (argc > 0)
        move_to_executable_folder(argv[0]);

    file = fopen(OUTPUT_FILE, "w");
    if (file == NULL) {
        printf("Could not create %s\n", OUTPUT_FILE);
        return 1;
    }

    fprintf(file, "# tosses fair_probability biased_probability\n");

    for (toss = 1; toss <= MAX_TOSSES; toss++) {
        if (random_unit() < 0.50)
            fair_heads++;
        if (random_unit() < BIASED_HEAD_PROBABILITY)
            biased_heads++;

        if (should_record(toss)) {
            fprintf(file, "%d %.8f %.8f\n",
                    toss,
                    (double) fair_heads / toss,
                    (double) biased_heads / toss);
        }
    }

    fclose(file);

    printf("After %d tosses:\n", MAX_TOSSES);
    printf("Fair coin P(HEAD)   = %.6f\n", (double) fair_heads / MAX_TOSSES);
    printf("Biased coin P(HEAD) = %.6f\n", (double) biased_heads / MAX_TOSSES);
    printf("Data written to %s\n", OUTPUT_FILE);

    generate_and_open_graph(PLOT_COMMANDS, TEMP_PLOT_PREFIX, SVG_FILE);
    return 0;
}
