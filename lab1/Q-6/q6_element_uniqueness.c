#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

static int contains_duplicate(const int array[], int n)
{
    int i;
    int j;

    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (array[i] == array[j])
                return 1;
        }
    }

    return 0;
}

int main(void)
{
    int *array;
    int n;
    int maximum_value;
    int i;

    printf("Enter the number of random elements: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid value of n.\n");
        return 1;
    }

    printf("Enter the maximum random value: ");
    if (scanf("%d", &maximum_value) != 1 || maximum_value < 0 || maximum_value >= RAND_MAX) {
        printf("Enter a maximum value from 0 to %d.\n", RAND_MAX - 1);
        return 1;
    }

    array = malloc((size_t) n * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand((unsigned int) time(NULL));
    for (i = 0; i < n; i++)
        array[i] = rand() % (maximum_value + 1);

    if (n <= 50) {
        printf("Generated numbers:\n");
        for (i = 0; i < n; i++)
            printf("%d%c", array[i], (i == n - 1) ? '\n' : ' ');
    }

    if (contains_duplicate(array, n))
        printf("Duplicate element found.\n");
    else
        printf("All elements are unique.\n");

    free(array);
    return 0;
}
