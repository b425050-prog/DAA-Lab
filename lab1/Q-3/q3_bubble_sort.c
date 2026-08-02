#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define OUTPUT_FILE "bubble_sort.dat"
#define TEMP_PLOT_PREFIX "q3_plot"
#define SVG_FILE "bubble_comparisons.svg"
#define MIN_SIZE 10
#define MAX_SIZE 200
#define STEP 5
#define REPETITIONS 100

static const char PLOT_COMMANDS[] =
    "set encoding utf8\n"
    "set terminal svg size 1800,1050 dynamic enhanced font \"Arial,12\" background rgb \"white\"\n"
    "set output \"bubble_comparisons.svg\"\n"
    "set border linewidth 1.3 linecolor rgb \"#334155\"\n"
    "set grid xtics ytics mxtics mytics back linecolor rgb \"#dbe4ee\" linewidth 1\n"
    "set tics textcolor rgb \"#334155\"\n"
    "set key opaque box linewidth 0.7 spacing 1.2 font \",11\"\n"
    "set title \"Question 3 · Bubble sort on random permutations\" font \",19\"\n"
    "set xlabel \"Array size n\"\n"
    "set ylabel \"Average number of comparisons\"\n"
    "set xrange [10:200]\n"
    "set yrange [0:20500]\n"
    "set style line 1 linecolor rgb \"#2563eb\" linewidth 4 pointtype 7 pointsize 0.75\n"
    "set style line 2 linecolor rgb \"#dc2626\" linewidth 4 dashtype 2 pointtype 9 pointsize 0.85\n"
    "plot \"bubble_sort.dat\" using 1:3 with linespoints ls 2 title \"Always completes n − 1 passes\", \\\n"
    "     \"\" using 1:2 with linespoints ls 1 title \"Stops when a pass makes no swap\"\n";


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

static void make_random_permutation(int array[], int n)
{
    int i;

    for (i = 0; i < n; i++)
        array[i] = i;

    for (i = n - 1; i > 0; i--) {
        int position = (int) (next_random() % (uint32_t) (i + 1));
        int temp = array[i];
        array[i] = array[position];
        array[position] = temp;
    }
}

static void copy_array(int destination[], const int source[], int n)
{
    int i;
    for (i = 0; i < n; i++)
        destination[i] = source[i];
}

static long long bubble_sort_early_stop(int array[], int n)
{
    long long comparisons = 0;
    int pass;

    for (pass = 0; pass < n - 1; pass++) {
        int swapped = 0;
        int j;

        for (j = 0; j < n - pass - 1; j++) {
            comparisons++;
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
                swapped = 1;
            }
        }

        if (swapped == 0)
            break;
    }

    return comparisons;
}

static long long bubble_sort_all_passes(int array[], int n)
{
    long long comparisons = 0;
    int pass;

    for (pass = 0; pass < n - 1; pass++) {
        int j;
        for (j = 0; j < n - pass - 1; j++) {
            comparisons++;
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }

    return comparisons;
}

int main(int argc, char *argv[])
{
    int *original;
    int *first_copy;
    int *second_copy;
    FILE *file;
    int n;

    if (argc > 0)
        move_to_executable_folder(argv[0]);

    original = malloc(MAX_SIZE * sizeof(int));
    first_copy = malloc(MAX_SIZE * sizeof(int));
    second_copy = malloc(MAX_SIZE * sizeof(int));

    if (original == NULL || first_copy == NULL || second_copy == NULL) {
        printf("Memory allocation failed.\n");
        free(original);
        free(first_copy);
        free(second_copy);
        return 1;
    }

    file = fopen(OUTPUT_FILE, "w");
    if (file == NULL) {
        printf("Could not create %s\n", OUTPUT_FILE);
        free(original);
        free(first_copy);
        free(second_copy);
        return 1;
    }

    fprintf(file, "# n early_stop_average always_complete_average\n");

    for (n = MIN_SIZE; n <= MAX_SIZE; n += STEP) {
        long long early_total = 0;
        long long complete_total = 0;
        int repetition;

        for (repetition = 0; repetition < REPETITIONS; repetition++) {
            make_random_permutation(original, n);
            copy_array(first_copy, original, n);
            copy_array(second_copy, original, n);

            early_total += bubble_sort_early_stop(first_copy, n);
            complete_total += bubble_sort_all_passes(second_copy, n);
        }

        fprintf(file, "%d %.3f %.3f\n",
                n,
                (double) early_total / REPETITIONS,
                (double) complete_total / REPETITIONS);
    }

    fclose(file);
    free(original);
    free(first_copy);
    free(second_copy);

    printf("Random permutations tested from n = %d to n = %d.\n",
           MIN_SIZE, MAX_SIZE);
    printf("Each point is averaged over %d random permutations.\n", REPETITIONS);
    printf("Data written to %s\n", OUTPUT_FILE);

    generate_and_open_graph(PLOT_COMMANDS, TEMP_PLOT_PREFIX, SVG_FILE);
    return 0;
}
