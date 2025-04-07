#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define ERR_TOO_FEW_OR_MANY_ARGS 1
#define ERR_PROCESS_FORK 2
#define ERR_INVALID_DIR 3
#define DELAY_MS 100

// MAIN AT BOTTOM

// Simplifies error checking
void checkErr(int varToCheck, char* errorDesc, int errCode, bool ignore) {

    if (varToCheck == -1) {
        fprintf(stderr, "Error: %s\n", errorDesc);

        //perror("Error");

        if (!ignore) {
            exit(errCode);
        }
    }

}

// Not in use for now
char* processArgument(char* argument) {  

    // strcmp function returns 0 when the two strings are the same (which is kinda weird ngl)
    int isNotVideoArgument = (strcmp(argument, "-v"));

    if (!isNotVideoArgument) {
        return ".h264";
    } else {
        return ".jpg";
    }
}

char* getCurrentTime(void) {

    // https://www.geeksforgeeks.org/time-h-header-file-in-c-with-examples/
    struct tm* timePtr = NULL;
    time_t currentTime;

    // gets current time
    // NULL is for purpose of NULL pointers
    currentTime = time(NULL);

    // A pointer to an area in memory containing the local time
    timePtr = localtime(&currentTime);

    // takes the above poitner and converts it to a format with a full date: WWW MMM DD HH:MM:SS YYYY
    char* formattedTime = asctime(timePtr);

    //formattedTime = removeSpaces(formattedTime);

    return formattedTime;

}

// https://www.geeksforgeeks.org/remove-spaces-from-a-given-string/

// This works by keeping two different counters, one which counts across the entire string and one that does not increment when encountering a space
// Therefore, we can replace the letter at the "count" position with the ith letter, effectively removing spaces
int removeSpaces(char* str) {

    int count = 0;

    for (int i = 0; i < strlen(str); i++) {

        // Current character in the string
        char currChar = str[i];

        // ASCII code for space character
        const char space = 0x00000020;

        // ASCII code for colon character
        const char colon = 0x0000003A;

        // ASCII code for newline character
        const char newline = 0x0000000A;

        // If the current character equals a space
        int isSpace = (currChar == space) || (currChar == colon) || (currChar == newline);

        if (!isSpace) {

            str[count] = str[i];

            count++;

        }
    }

    // appends null terminator to end
    str[count] = '\0';

    return 0;

}

// https://codeforwin.org/c-programming/c-program-check-file-or-directory-exists-not
int doesDirectoryExist(char* path) {

    // declare struct
    struct stat fileStats;

    // https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/sys/stat.h
    // this will return an error if the directory does not exist
    // therefore, it will effectively cause the line below to output an error
    stat(path, &fileStats);

    // checks if directory (see above comments for more details)
    int directoryExist = S_ISDIR(fileStats.st_mode);

    if (directoryExist) {

        return 1;

    } else {

        return -1;

    }

}

int main(int argc, char** argv) {

    // We only want one argument too avoid a bozo putting -i and -v
    if (argc != 2) {

        fprintf(stderr, "Too few or too many arguments, please pass in one argument.\n");

        exit(ERR_TOO_FEW_OR_MANY_ARGS);

    }

    char* fileExtension;

    char* argument = argv[0];

    int isArgumentDash = (strcmp(argument, "-v"));

    // "-" implies that an option/flag was passed
    if (isArgumentDash) {

        fileExtension = processArgument(argument);
        
    }

    // gets current local time
    char* curTime = getCurrentTime();

    removeSpaces(curTime);

    // Concatenate fileExtension
    char* fullFilename = strcat(curTime, fileExtension);

    // This is the daemon part
    while (true) {

        // Create child process
        pid_t pid = fork();

        // Check to see if errors occured during process creation
        checkErr(pid, "Error initializing process fork.", ERR_PROCESS_FORK, true);

        // Wait on the child process 
        waitpid(pid, NULL, 0);

        // Since the fork() command returns 0 to the child process and the child pid to the parent
        // We use the following condition to see whether we are in the child or parent process
        
        // If we are in the child process we will execute our desired command
        if (pid == 0) {
            printf("%i: We good.\n", getpid());
            //execlp("libcamera-jpeg", "libcamera-jpeg", "-o", fullFilename, NULL);
            usleep(2000000);
            //wait(NULL);
            exit(0);
        }

        // Does something if we are in the parent process
        if (pid != 0) {
            printf("%i: I'M BACK!!!!!!!!!!!!!!!!!!!!!!!\n", getpid());
        }

    }
}