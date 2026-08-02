#include <stdio.h>
#include <stdlib.h>

static int is_valid_partitioned_array(const int array[], int n)
{
    int seen_one = 0;
    int i;

    for (i = 0; i < n; i++) {
        if (array[i] != 0 && array[i] != 1)
            return 0;
        if (array[i] == 1)
            seen_one = 1;
        else if (seen_one)
            return 0;
    }

    return 1;
}

static int find_first_one(const int array[], int n)
{
    int low = 0;
    int high = n;

    while (low < high) {
        int middle = low + (high - low) / 2;

        if (array[middle] == 0)
            low = middle + 1;
        else
            high = middle;
    }

    return low;
}

int main(void)
{
    int *array;
    int n;
    int i;
    int partition_point;

    printf("Enter the number of elements: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid array size.\n");
        return 1;
    }

    array = malloc((size_t) n * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d elements containing 0s followed by 1s:\n", n);
    for (i = 0; i < n; i++) {
        if (scanf("%d", &array[i]) != 1) {
            printf("Invalid input.\n");
            free(array);
            return 1;
        }
    }

    if (!is_valid_partitioned_array(array, n)) {
        printf("The array must contain only a run of 0s followed by a run of 1s.\n");
        free(array);
        return 1;
    }

    partition_point = find_first_one(array, n);

    if (partition_point == n)
        printf("No 0-to-1 transition exists because the array contains no 1.\n");
    else if (partition_point == 0)
        printf("The first 1 is at index 0; there is no preceding run of 0s.\n");
    else
        printf("The transition occurs between indices %d and %d.\n",
               partition_point - 1, partition_point);

    printf("First index containing 1: %d\n", partition_point);

    free(array);
    return 0;
}
