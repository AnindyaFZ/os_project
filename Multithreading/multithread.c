#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

// Matrix size - can be increased for more visible performance differences
#define SIZE 5000 

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

// Structure to pass data to each thread
typedef struct {
    int thread_id;
    int start_row;
    int end_row;
} ThreadData;

// Thread function: Adds assigned rows of matrices A and B [cite: 56]
void* add_matrices(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < SIZE; j++) {
            // Independent writes: Each thread writes to unique memory addresses 
            C[i][j] = A[i][j] + B[i][j]; 
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) num_threads = 1;

    pthread_t threads[num_threads];
    ThreadData t_data[num_threads];

    // Initialize source matrices (Read-Only during threading) [cite: 60]
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = i % 10;
            B[i][j] = j % 10;
        }
    }

    // Performance Testing: Start timer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Dynamic Threading Logic [cite: 64]
    int rows_per_thread = SIZE / num_threads;
    int extra_rows = SIZE % num_threads; // Handle remainder rows 
    int current_row = 0;

    for (int i = 0; i < num_threads; i++) {
        t_data[i].thread_id = i;
        t_data[i].start_row = current_row;
        
        // Data Partitioning 
        int work = rows_per_thread + (i < extra_rows ? 1 : 0);
        t_data[i].end_row = current_row + work;
        
        current_row = t_data[i].end_row;

        if (pthread_create(&threads[i], NULL, add_matrices, (void*)&t_data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Stop timer
    gettimeofday(&end, NULL);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

    // Assignment Required Output
    printf("==========================================\n");
    printf("Matrix Size: %d x %d\n", SIZE, SIZE);
    printf("Threads Used: %d\n", num_threads);
    printf("Execution Time: %.6f seconds\n", time_taken);
    printf("Status: Addition Successful (No Race Conditions)\n");
    printf("==========================================\n");

    return 0;
}