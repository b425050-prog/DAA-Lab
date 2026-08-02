#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define OUTPUT_FILE "growth_order.dat"
#define TEMP_PLOT_PREFIX "q1_plot"
#define SVG_FILE "growth_order.svg"
#define MIN_N 2.0L
#define MAX_N 50.0L
#define STEP 0.1L

static const char PLOT_COMMANDS[] =
    "set encoding utf8\n"
    "set terminal svg size 3000,1100 dynamic enhanced font \"Arial,12\" background rgb \"white\"\n"
    "set output \"growth_order.svg\"\n"
    "set multiplot\n"
    "\n"
    "# Main graph: kept large so adding the order table does not compress the curves.\n"
    "set origin 0.00,0.00\n"
    "set size 0.66,1.00\n"
    "set border linewidth 1.3 linecolor rgb \"#334155\"\n"
    "set grid xtics ytics mytics back linecolor rgb \"#dbe4ee\" linewidth 1\n"
    "set tics textcolor rgb \"#334155\"\n"
    "set key outside top center horizontal maxrows 3 opaque box linewidth 0.7 spacing 1.1 font \",10\"\n"
    "set title \"Question 1 · Growth of all given functions\" font \",19\"\n"
    "set xlabel \"n\"\n"
    "set ylabel \"Function value · logarithmic scale\"\n"
    "set xrange [2:50]\n"
    "set yrange [1e-2:1e25]\n"
    "set logscale y 10\n"
    "set format y \"10^{%L}\"\n"
    "set style line 1  linecolor rgb \"#1d4ed8\" linewidth 3\n"
    "set style line 2  linecolor rgb \"#dc2626\" linewidth 3\n"
    "set style line 3  linecolor rgb \"#059669\" linewidth 3\n"
    "set style line 4  linecolor rgb \"#d97706\" linewidth 3\n"
    "set style line 5  linecolor rgb \"#7c3aed\" linewidth 3\n"
    "set style line 6  linecolor rgb \"#0891b2\" linewidth 3\n"
    "set style line 7  linecolor rgb \"#be185d\" linewidth 3 dashtype 2\n"
    "set style line 8  linecolor rgb \"#4d7c0f\" linewidth 3 dashtype 2\n"
    "set style line 9  linecolor rgb \"#c2410c\" linewidth 3 dashtype 2\n"
    "set style line 10 linecolor rgb \"#4338ca\" linewidth 3 dashtype 3\n"
    "set style line 11 linecolor rgb \"#0f766e\" linewidth 3 dashtype 3\n"
    "set style line 12 linecolor rgb \"#991b1b\" linewidth 3 dashtype 3\n"
    "plot \"growth_order.dat\" using 1:2 with lines ls 1 title \"1/n\", \\\n"
    "     \"\" using 1:3 with lines ls 2 title \"log₂ n\", \\\n"
    "     \"\" using 1:4 with lines ls 3 title \"12√n\", \\\n"
    "     \"\" using 1:5 with lines ls 4 title \"50√n\", \\\n"
    "     \"\" using 1:6 with lines ls 5 title \"n⁰·⁵¹\", \\\n"
    "     \"\" using 1:7 with lines ls 6 title \"2³²n\", \\\n"
    "     \"\" using 1:8 with lines ls 7 title \"n log₂ n\", \\\n"
    "     \"\" using 1:($9 > 0 ? $9 : 1/0) with lines ls 8 title \"n² − 324\", \\\n"
    "     \"\" using 1:10 with lines ls 9 title \"100n² + 6n\", \\\n"
    "     \"\" using 1:11 with lines ls 10 title \"2n³\", \\\n"
    "     \"\" using 1:12 with lines ls 11 title \"nˡᵒᵍ₂ⁿ\", \\\n"
    "     \"\" using 1:13 with lines ls 12 title \"3ⁿ\"\n"
    "\n"
    "# One-row order table on the right.\n"
    "unset logscale y\n"
    "unset grid\n"
    "unset key\n"
    "unset title\n"
    "unset xlabel\n"
    "unset ylabel\n"
    "unset border\n"
    "unset xtics\n"
    "unset ytics\n"
    "unset format\n"
    "set origin 0.67,0.13\n"
    "set size 0.32,0.70\n"
    "set xrange [0:1]\n"
    "set yrange [0:1]\n"
    "set object 1 rect from graph 0.02,0.18 to graph 0.98,0.82 front fillcolor rgb \"#f8fafc\" fillstyle solid 1.0 border rgb \"#94a3b8\" linewidth 1.5\n"
    "set object 2 rect from graph 0.02,0.63 to graph 0.98,0.82 front fillcolor rgb \"#e2e8f0\" fillstyle solid 1.0 border rgb \"#94a3b8\" linewidth 1.5\n"
    "set label 1 \"Increasing order for sufficiently large n\" at graph 0.50,0.725 center front font \",16\" textcolor rgb \"#0f172a\"\n"
    "set label 2 \"1/n < log₂ n < 12√n < 50√n < n⁰·⁵¹ < 2³²n < n log₂ n < n² − 324 < 100n² + 6n < 2n³ < nˡᵒᵍ₂ⁿ < 3ⁿ\" at graph 0.50,0.405 center front font \",9\" textcolor rgb \"#0f172a\"\n"
    "plot NaN notitle\n"
    "\n"
    "unset multiplot\n";


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

int main(int argc, char *argv[])
{
    FILE *file;
    int point;
    int total_points = (int) ((MAX_N - MIN_N) / STEP + 0.5L) + 1;

    if (argc > 0)
        move_to_executable_folder(argv[0]);

    file = fopen(OUTPUT_FILE, "w");
    if (file == NULL) {
        printf("Could not create %s\n", OUTPUT_FILE);
        return 1;
    }

    fprintf(file, "# n one_over_n log2_n twelve_sqrt_n fifty_sqrt_n n_power_0_51 two_power_32_n n_log2_n n2_minus_324 hundred_n2_plus_6n two_n3 n_power_log2_n three_power_n\n");

    for (point = 0; point < total_points; point++) {
        long double n = MIN_N + point * STEP;
        long double log2_n = log2l(n);

        fprintf(file,
                "%.2Lf %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le %.12Le\n",
                n,
                1.0L / n,
                log2_n,
                12.0L * sqrtl(n),
                50.0L * sqrtl(n),
                powl(n, 0.51L),
                powl(2.0L, 32.0L) * n,
                n * log2_n,
                n * n - 324.0L,
                100.0L * n * n + 6.0L * n,
                2.0L * n * n * n,
                powl(n, log2_n),
                powl(3.0L, n));
    }

    fclose(file);

    printf("Increasing order for sufficiently large n:\n");
    printf("1/n < log2(n) < 12*sqrt(n) < 50*sqrt(n) < n^0.51\n");
    printf("< 2^32*n < n*log2(n) < n^2 - 324 < 100*n^2 + 6*n\n");
    printf("< 2*n^3 < n^log2(n) < 3^n\n");
    printf("%d plotting points written to %s.\n", total_points, OUTPUT_FILE);

    generate_and_open_graph(PLOT_COMMANDS, TEMP_PLOT_PREFIX, SVG_FILE);
    return 0;
}
