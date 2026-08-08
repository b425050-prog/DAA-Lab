#include <stdio.h>
#include <stdlib.h>

static int integer_log2(int n) {
    int result = 0;
    while (n > 1) {
        n /= 2;
        ++result;
    }
    return result;
}

int main(void) {
    const int sizes[] = {
        16, 32, 64, 128, 256, 512, 1024,
        2048, 4096, 8192, 16384, 32768, 65536
    };
    const int count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    const char *data_file = "q1_dictionary_operations.dat";
    const char *plot_file = "q1_dictionary_operations.gp";
    FILE *file = fopen(data_file, "w");

    if (file == NULL) {
        perror(data_file);
        return EXIT_FAILURE;
    }

    fprintf(file, "# n constant logarithmic linear\n");
    for (int i = 0; i < count; ++i) {
        const int n = sizes[i];
        fprintf(file, "%d %d %d %d\n", n, 1, integer_log2(n), n);
    }
    fclose(file);

    puts("Q1 graph data prepared.");
    puts("Generating q1_dictionary_operations.svg with GNUPlot…");

    char command[256];
    snprintf(command, sizeof(command), "gnuplot %s", plot_file);
    const int status = system(command);

    if (status != 0) {
        puts("GNUPlot could not be started. Keep the generated .dat file and run the .gp script after GNUPlot is available.");
        return EXIT_FAILURE;
    }

    remove(data_file);
    puts("Graph generated successfully: q1_dictionary_operations.svg");
    return 0;
}
