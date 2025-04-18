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
#define ERR_INVALID_ARG 2
#define ERR_PROCESS_FORK 3
#define ERR_INVALID_DIR 4
#define ERR_BASH_SCRIPT 5
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

// Processes an argument
// For now, it only accepts -i and -v
// If you wish to store additional info in another variable, provide another char* variable
char* processArgument(char* argument/*, char* argDescReturnVar*/) {  

    char argFirstChar = argument[0];

    // ASCII code for dash character
    const char dash = 0x0000002D;
    
    int isArgumentDash = (argFirstChar == dash);

    if (isArgumentDash) {

        
        
    } else {

        fprintf(stderr, "Invalid argument.\n");
        exit(ERR_INVALID_ARG);

    }

    // strcmp function returns 0 when the two strings are the same (which is kinda weird ngl)
    // However it's because the difference is ASCII values is 0 (kinda cool ngl)
    int isVideoArgument = !(strcmp(argument, "-v"));

    if (isVideoArgument) {
        //argDescReturnVar = "video";
        return ".h264";
    } else {
        return ".jpg";
    }
}

// Gets current time and returns it the time formatted as WWW MMM DD HH:MM:SS YYYY
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

        // If the current character equals a space (or a colon or newline)
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

/*
-- ARGS --
-i: take an image using the camera
-v: take a video using the camera
*/
int main(int argc, char** argv) {

    // We only want one argument too avoid a bozo putting -i and -v
    if (argc != 2) {

        fprintf(stderr, "Too few or too many arguments, please pass in one argument.\n");

        exit(ERR_TOO_FEW_OR_MANY_ARGS);

    }

    char* argument = argv[1];

    // Based on the argument passed
    // Error handling is within the processArgument() function
    char* fileExtension = processArgument(argument);

    // gets current local time
    char* curTime = getCurrentTime();


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

            char* curTime = getCurrentTime();

            // remove spaces from time to prevent an UGLY file name
            // trust me, you do NOT want to see the monstrousity that exists without this
            removeSpaces(curTime);

            // Concatenate fileExtension
            char* fullFilename = strcat(curTime, fileExtension);

            //printf("%i: We good.\n", getpid());
            //printf("%s\n", fileExtension);
            printf("%i\n", strcmp(fileExtension, ".jpg"));

            int isPhoto = !(strcmp(fileExtension, ".jpg"));

            if (isPhoto) {

                //printf("taking photo\n");
                int bashScript = execlp("libcamera-jpeg", "libcamera-jpeg", "-o", fullFilename, "-t", "500", NULL);

                checkErr(bashScript, "Error executing bash script.", ERR_BASH_SCRIPT, true);

            } else {

                //printf("taking video\n");
                int bashScript = execlp("libcamera-vid", "libcamera-vid", "-o", fullFilename, "-t", "2000", NULL);

                checkErr(bashScript, "Error executing bash script.", ERR_BASH_SCRIPT, true);

            }

            usleep(2000000);
            printf("Captured -- with filename %s\n", fullFilename);
            //wait(NULL);
            exit(0);
        }

        // Does something if we are in the parent process
        if (pid != 0) {
            NULL;
        }

    }
}