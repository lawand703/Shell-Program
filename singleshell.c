
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_PATH_LENGTH 1024
#define MAX_HISTORY_LENGTH 100

// Bir komutun tam yolunu bulmak için belirtilen dizinleri arar 
char *get_path(char *command)
{
    char *path = getenv("PATH"); /* PATH değişkeninin değerini al */

    if (path == NULL) 
        return NULL;

    const char *delimiter = ":";
    const size_t command_length = strlen(command);

    char *path_copy = strdup(path);
    if (path_copy == NULL) /* memory allocation fail olursa */
        return NULL;

    char *path_token = strtok(path_copy, delimiter); /* Get the first directory token from PATH */

    while (path_token != NULL)
    {
        const size_t directory_length = strlen(path_token);
        const size_t file_path_length = directory_length + command_length + 2;
        char *file_path = malloc(file_path_length); /* Allocate memory for the file path (+1 for null terminator) */

        if (file_path == NULL)
        { /* If memory allocation failed */
            free(path_copy);
            return NULL;
        }

        snprintf(file_path, file_path_length, "%s/%s", path_token, command); /* Construct the full file path */

        if (access(file_path, F_OK) == 0)
        { /* Check if the file path exists */
            free(path_copy);
            return file_path;
        }

        free(file_path);
        path_token = strtok(NULL, delimiter); /* Get the next directory token from PATH */
    }

    free(path_copy);

    if (access(command, F_OK) == 0) /* Check if the command exists in the current directory */
        return strdup(command);

    return NULL; /* Return NULL if the command was not found*/
}

// log.txt dosyasına log kaydı tutar
void log_command(const char *command)
{
    FILE *log_file = fopen("log.txt", "a");
    if (log_file != NULL)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        fprintf(log_file, "%ld\t%s\n", tv.tv_sec, command);
        fclose(log_file);
    }
    else
    {
        perror("Error opening log file");
    }
}

// Command çalıştırma fonksiyonu. Command bulunamazsa hata mesajı döndürür
void execute_command(char *command)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // Child process
        char *args[4];
        args[0] = command;
        args[1] = NULL;

        char *full_command_path = get_path(command);
        // Try to execute the command
        execve(full_command_path, &command, NULL);

        // If not found in current path or system path, find the path
        char path[MAX_PATH_LENGTH];
        snprintf(path, sizeof(path), "which %s", command);
        FILE *fp = popen(path, "r");
        if (fp != NULL)
        {
            if (fgets(path, sizeof(path), fp) != NULL)
            {
                path[strcspn(path, "\n")] = 0; // Remove trailing newline
                printf("Found command path: %s\n", path);
                execve(path, &command, NULL);
            }
            pclose(fp);
        }

        // If still not found, print error
        printf("%s: command not found\n", command);
        exit(EXIT_FAILURE);
    }
    else
    { // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

int main()
{
    char command[MAX_COMMAND_LENGTH];
    char history[MAX_HISTORY_LENGTH][MAX_COMMAND_LENGTH];
    int history_count = 0;

    while (1)
    {
        printf("$ "); // $ işaretiyle kullanıcıdan input alma

        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            break;
        }

        // Remove trailing newline character
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "") == 0)
        {
            continue; // komut boşsa yeni komutu bekle
        }

        if (strcmp(command, "exit") == 0)
        {
            break;
        }
        else if (strcmp(command, "history") == 0)
        {
            // Display command history
            for (int i = 0; i < history_count; i++)
            {
                printf("%d: %s\n", i + 1, history[i]);
            }
            continue;
        }

        // yürütülen komutu log'a kaydetme
        log_command(command);

        // history komut ekleme
        if (history_count < MAX_HISTORY_LENGTH)
        {
            strcpy(history[history_count], command);
            history_count++;
        }
        else
        {
            // yeni komuta yer açmak
            for (int i = 0; i < MAX_HISTORY_LENGTH - 1; i++)
            {
                strcpy(history[i], history[i + 1]);
            }
            strcpy(history[MAX_HISTORY_LENGTH - 1], command);
        }

        execute_command(command);
    }

    return 0;
}
