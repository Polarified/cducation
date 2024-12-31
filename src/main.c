#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void log_cpu_usage(int log_fd) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ps -p %d -o %%cpu --no-headers", getpid());
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        dprintf(log_fd, "[%s] Failed to get daemon CPU usage\n", timestamp);
        return;
    }

    char daemon_cpu_usage[16];
    if (fgets(daemon_cpu_usage, sizeof(daemon_cpu_usage), fp) != NULL) {
        daemon_cpu_usage[strcspn(daemon_cpu_usage, "\n")] = '\0';
    }
    pclose(fp);

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        dprintf(log_fd, "[%s] Failed to get total CPU usage\n", timestamp);
        return;
    }

    char buffer[1024];
    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
        unsigned long long int total = user + nice + system + idle + iowait + irq + softirq + steal;
        unsigned long long int busy = total - idle;
        double total_cpu_usage = (double)busy / total * 100.0;
        dprintf(log_fd, "[%s] Daemon CPU usage: %s%%, Total CPU usage: %.2f%%\n", timestamp, daemon_cpu_usage, total_cpu_usage);
    }
    fclose(fp);
}

int main(void) {
    pid_t pid, sid;

    // Fork the parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    // If we got a good PID, then we can exit the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Open any logs here

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Change the current working directory
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Open the log file
    int log_fd = open("/tmp/daemon.log", O_RDWR | O_CREAT, 0600);
    if (log_fd < 0) {
        exit(EXIT_FAILURE);
    }

    // Daemon-specific initialization goes here

    // The Big Loop
    while (1) {
        // Do some task here
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        // Remove the newline character from the end of the timestamp
        timestamp[strlen(timestamp) - 1] = '\0';

        dprintf(log_fd, "[%s] Daemon is running...\n", timestamp);
        log_cpu_usage(log_fd);
        sleep(30); // wait 30 seconds
    }

    close(log_fd);
    return 0;
}