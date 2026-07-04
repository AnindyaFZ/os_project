#include <stdio.h>
#include <string.h>

typedef struct {
    char name[10];
    int e;          // Execution Time
    int p;          // Period
    int remaining;  // Time left to run in current period
    int deadline;   // Absolute Deadline
    int priority;   // Static priority (for RM)
} Task;

// Helper to find GCD for the Hyper-period calculation
int gcd(int a, int b) {
    while (b) { a %= b; int t = a; a = b; b = t; }
    return a;
}

// Function to run the simulation
void simulate(Task tasks[], int n, char* mode) {
    int hyper_period = tasks[0].p;
    for (int i = 1; i < n; i++) 
        hyper_period = (hyper_period * tasks[i].p) / gcd(hyper_period, tasks[i].p);

    // RM Logic: Sort by period (Simple Bubble Sort)
    if (strcmp(mode, "RM") == 0) {
        for (int i = 0; i < n-1; i++)
            for (int j = 0; j < n-i-1; j++)
                if (tasks[j].p > tasks[j+1].p) {
                    Task temp = tasks[j]; tasks[j] = tasks[j+1]; tasks[j+1] = temp;
                }
    }

    printf("\n--- Starting %s Simulation (Limit: %d) ---\n", mode, hyper_period);

    for (int t = 0; t < hyper_period; t++) {
        int best_idx = -1;

        for (int i = 0; i < n; i++) {
            // Task Arrival (New Period)
            if (t % tasks[i].p == 0) {
                tasks[i].remaining = tasks[i].e;
                tasks[i].deadline = t + tasks[i].p;
            }

            // Selection Logic
            if (tasks[i].remaining > 0) {
                if (best_idx == -1) {
                    best_idx = i;
                } else {
                    if (strcmp(mode, "EDF") == 0) {
                        if (tasks[i].deadline < tasks[best_idx].deadline) best_idx = i;
                    } else {
                        // For RM, the list is already sorted by period (priority)
                        if (i < best_idx) best_idx = i;
                    }
                }
            }
        }

        if (best_idx != -1) {
            printf("Time %d: %s running\n", t, tasks[best_idx].name);
            tasks[best_idx].remaining--;
            if (t + 1 > tasks[best_idx].deadline && tasks[best_idx].remaining > 0)
                printf("  !! DEADLINE MISSED: %s !!\n", tasks[best_idx].name);
        } else {
            printf("Time %d: IDLE\n", t);
        }
    }
}

int main() {
    Task task_set[] = { {"T1", 2, 5}, {"T2", 4, 10}, {"T3", 1, 20} };
    int n = 3;

    // Schedulability Check
    float utilization = 0;
    for(int i=0; i<n; i++) utilization += (float)task_set[i].e / task_set[i].p;
    
    printf("Total Utilization: %.2f\n", utilization);
    if (utilization > 1.0) printf("System is NOT schedulable.\n");

    // We copy the set because RM sorting changes the array order
    Task rm_tasks[3], edf_tasks[3];
    memcpy(rm_tasks, task_set, sizeof(task_set));
    memcpy(edf_tasks, task_set, sizeof(task_set));

    simulate(rm_tasks, n, "RM");
    simulate(edf_tasks, n, "EDF");

    return 0;
}