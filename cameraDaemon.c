#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define ERR_TOO_FEW_ARGS 1
#define ERR_PROCESS_FORK 2
#define ERR_INVALID_DIR 3

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
// int processArgument(char* argument) {  
//     if (argument == "-i") {
//         printf("Take image.");
//     } else if (argument == "-v") {
//         printf("Take video.");
//     }
// }

//https://riptutorial.com/c/example/6514/iterating-over-the-characters-in-a-string
int removeSpaces(char* str) {
    return 0;
}

char* getCurrentTime(void) {

    // https://www.geeksforgeeks.org/time-h-header-file-in-c-with-examples/
    struct tm* timePtr = NULL;
    time_t currentTime;

    currentTime = time(NULL);

    timePtr = localtime(&currentTime);

    char* formattedTime = asctime(timePtr);

    //formattedTime = removeSpaces(formattedTime);

    return formattedTime;

}

// https://codeforwin.org/c-programming/c-program-check-file-or-directory-exists-not
int doesDirectoryExist(char* path) {

    // declare struct
    struct stat fileStats;

    // https://en.wikibooks.org/wiki/C_Programming/POSIX_Reference/sys/stat.h
    stat(path, &fileStats);

    int directoryExist = S_ISDIR(fileStats.st_mode);

    if (directoryExist) {

        return 1;

    } else {

        return -1;

    }

}

// Allocates enough memory to contain both strings (str1 and str2) into outputString
// The plus one is to account for the null-terminator at the end of the string
// int concatenateStrings(char* outputStr, char* str1, char* str2) {
//     outputStr = (char *) malloc(1 + strlen(str1) + strlen(str2));
//     // Copy str1
//     strcpy(outputStr, str1);
//     // Copy str2
//     strcpy(outputStr, str2);

//     return &outputStr;
// }

int main(int argc, char** argv) {

    // char* baseFilePath = "/home/raspberry/";
    // char* outputDirectory = argv[1];
    // strcat(outputDirectory, "/");


    // if (argc < 2) {
    //     fprintf(stderr, "Please present an output directory (starting from your current user directory)\n");
    //     exit(ERR_TOO_FEW_ARGS);
    // }

    // https://stackoverflow.com/questions/18468229/concatenate-two-char-strings-in-a-c-program
    // Allocates enough memory to contain the base directory path and the extension (outputDirectory)
    // The plus 1 is to account for the null-terminator at the end of the string
    // char* fullDirPath = (char *) malloc(1 + strlen(baseFilePath) + strlen(outputDirectory));

    // // Concatenate base file path
    // strcat(fullDirPath, baseFilePath);

    // // Concatenate output directory
    // strcat(fullDirPath, outputDirectory);
    // printf("%s", fullDirPath);
    
    // // Ensure the directory provided exists before appending output file name
    // checkErr(doesDirectoryExist(fullDirPath), "Invalid output directory.", ERR_INVALID_DIR, false);

    
    // gets current local time
    char* curTime = getCurrentTime();

    //printf("%s", curTime);


    // Allocate enough memory to contain the pre-established directory path and the time as the filename
    // The plus 1 is to account for the null-terminator at the end of the string
    // The plus 4 is to append a file extension (such as '.jpg')
    // TODO: Add more dynamic approach to file extension
    char* fullFilePath = (char *) malloc(1 + strlen(fullDirPath) + strlen(curTime) + 4);

    // Concatenate current time (intended to be file name)
    strcat(fullFilePath, curTime);

    // Concatenate fileExtension
    char* fileExtension = ".jpg";
    strcat(fullFilePath, fileExtension);

    printf("%s", fullFilePath);

    free(fullDirPath);

    // https://www.demo2s.com/g/c/for-c-what-are-command-line-options-how-can-you-implement-optional-flags-or-parameters-i.html
    // for (int i = 1; i < argc; i++) {

    //     char* currentArgument = argv[i];

    //     if (currentArgument[0] == "-") {
    //         processArgument(currentArgument);
    //     }
    // }

    
    //while (true) {

        pid_t pid = fork();

        checkErr(pid, "Error initializing process fork.", ERR_PROCESS_FORK, true);

        if (pid == 0) {
            execlp("libcamera-jpeg", "libcamera-jpeg", "-o", fullFilePath, NULL);
            wait(NULL);
            exit(0);
        }

    //}

    printf("I'M BACK!!!!!!!!!!!!!!!!!!!!!!!");
}