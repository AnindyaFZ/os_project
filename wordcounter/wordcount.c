#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILES 100
#define MAX_FILENAME_LEN 256

int count_words_in_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    int count = 0;
    char ch;
    int in_word = 0;

    while ((ch = fgetc(file)) != EOF) {
        if (isspace(ch)) {
            in_word = 0;
        } else {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        }
    }
    fclose(file);
    return count;
}

int main() {
    int num_files;
    char filenames[MAX_FILES][MAX_FILENAME_LEN];

    printf("Enter number of files: ");
    scanf("%d", &num_files);
    getchar();

    for (int i = 0; i < num_files; i++) {
        printf("Enter filename %d: ", i + 1);
        fgets(filenames[i], MAX_FILENAME_LEN, stdin);
        filenames[i][strcspn(filenames[i], "\n")] = '\0';
    }

    int pipes[MAX_FILES][2];
    pid_t pids[MAX_FILES];

    for (int i = 0; i < num_files; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(1);
        }

        pids[i] = fork();

        if (pids[i] == 0) { 
            close(pipes[i][0]);
            int count = count_words_in_file(filenames[i]);
            write(pipes[i][1], &count, sizeof(count));
            close(pipes[i][1]);
            exit(0);
        } else if (pids[i] < 0) {
            perror("fork failed");
            exit(1);
        }
        
        close(pipes[i][1]); 
    }

    int total_words = 0;
    for (int i = 0; i < num_files; i++) {
        int child_count;
        waitpid(pids[i], NULL, 0);
        read(pipes[i][0], &child_count, sizeof(child_count));
        close(pipes[i][0]);

        if (child_count == -1) {
            printf("Error: Could not open file %s\n", filenames[i]);
        } else {
            printf("File %s has %d words\n", filenames[i], child_count);
            total_words += child_count;
        }
    }

    printf("Total words across all files: %d\n", total_words);
    return 0;
}