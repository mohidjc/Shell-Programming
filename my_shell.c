#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXLINE 512

void redir(char *args[]) {
    int out = -10;
    int more_redir=0;
    
   
   for (int j = 0; args[j]!=0;j++){
     if (strcmp(args[j], "<")  == 0|| strcmp(args[j], ">") == 0) {
        more_redir+=1; // if there is any redirection increment to keep track of how mnay redirections in one command
        }
    }


    for (int j = 0; args[j] != 0; j++) {
        if (strcmp(args[j], "<") == 0) {
            if (args[j + 1] != 0) {
                int in = open(args[j + 1], O_RDONLY); // open the file
                if (in < 0) {
                    fprintf(2, "cannot open %s for input\n", args[j + 1]);
                    exit(1);
                }
                close(0); // closes standard input
                dup(in); // dup ffd
                close(in); // close fd
                args[j] = 0; // set current arg to null
                if (more_redir==1){
                    args[j + 1] = 0; // if there is only one redir then set the next arg to 0 aswwell
                }
                
            }
        }
        if (strcmp(args[j], ">") == 0) {
            if (args[j + 1] != 0) {
                out = open(args[j + 1], O_CREATE | O_WRONLY | O_TRUNC);
                if (out < 0) {
                    fprintf(2, "cannot open %s for output\n", args[j + 1]);
                    exit(1);
                } 
                close(1); // close standard output
                dup(out); 
                close(out);
                args[j] = 0; // setting current and next arg to null
                args[j + 1] = 0;
            }
        }
    }
}


void redir_pipe(char *args[],int pipe_pos) { // same as normal redirection only pipe position to do any redirection either side of the pipes
    for (int j = pipe_pos + 1; args[j] != 0; j++) {
        if (strcmp(args[j], "<") == 0) {
            if (args[j + 1] != 0) {
                int in = open(args[j + 1], O_RDONLY);
                if (in < 0) {
                    fprintf(2, "cannot open %s for input\n", args[j + 1]);
                    exit(1);
                }
                close(0); 
                dup(in);
                close(in);
                args[j] = 0;
                args[j + 1] = 0;
            }
        }
        if (strcmp(args[j], ">") == 0) {
            if (args[j + 1] != 0) {
                int out = open(args[j + 1], O_CREATE | O_WRONLY | O_TRUNC);
                if (out < 0) {
                    fprintf(2, "cannot open %s for output\n", args[j + 1]);
                    exit(1);
                } 
                close(1);
                dup(out);
                close(out);
                args[j] = 0;
                args[j + 1] = 0;
            }
        }
    }
}




void exe_cmd(char *args[], int num_of_pipes) { // normal execution
    int pid = fork();
    if (pid == 0) {
        redir(args);
        exec(args[0], args);
        fprintf(2, "xv6: exec failed\n");
        exit(1);
    } else if (pid > 0) {
        wait(0);
    } else {
        fprintf(2, "xv6: fork failed\n");
    }
}


void exe_pipes(char *args[], int pipe_pos[], int num_of_pipes) {
    int pipe_fd[MAXARG][2]; // 2d array for file descriptors can have more than 2 sets of descriptors but I only go up to 2 in my code
    for (int i = 0; i < num_of_pipes; i++) {
        if (pipe(pipe_fd[i]) < 0) {
            fprintf(2, "xv6: pipe failed\n");
            exit(1);
        }
    }
    int pid1 = fork();
    if (pid1 == 0) {
        close(1); // closing standard output
        dup(pipe_fd[0][1]); // dup the write end of first pipe
        for (int j = 0; j < num_of_pipes; j++) {
            // closing read and write end of each pipe
            close(pipe_fd[j][0]); 
            close(pipe_fd[j][1]); 
        }
        redir(args); // handle redirections
        exec(args[0], args);
        fprintf(2, "xv6: exec failed\n");
        exit(1);
    } else if (pid1 > 0) {
        int pid2 = fork();
        if (pid2 == 0) {
            close(0); // closing standard input
            dup(pipe_fd[0][0]); // dup read end of first pipe
            if (num_of_pipes == 2){
                close(1);
                dup(pipe_fd[1][1]); // dup write end of second pipe
            }
            for (int j = 0; j < num_of_pipes; j++) {
                close(pipe_fd[j][0]);
                close(pipe_fd[j][1]);
            }
            int p = pipe_pos[0]; //gets position of first pipe
            redir_pipe(args, p);
            exec(args[pipe_pos[0] + 1], args + pipe_pos[0] + 1);
            fprintf(2, "xv6: exec failed\n");
            exit(1);
        } else if (pid2 > 0) {
            close(pipe_fd[0][0]); // closing read and write end of first pipe
            close(pipe_fd[0][1]);
            if (num_of_pipes==2){
                int pid3 = fork();
                if (pid3 == 0) {
                    close(0); 
                    dup(pipe_fd[1][0]); // dup read end of second pipe
                    close(pipe_fd[1][0]);
                    close(pipe_fd[1][1]);
                    int p = pipe_pos[1];
                    redir_pipe(args, p);
                    exec(args[pipe_pos[1] + 1], args + pipe_pos[1] + 1);
                    fprintf(2, "xv6: exec failed\n");
                    exit(1);
                } else if (pid3 > 0) {
                    close(pipe_fd[1][0]); // closing read and write end of second pipe
                    close(pipe_fd[1][1]);
                    wait(0); //wwaiting for child processes
                    wait(0);
                    wait(0);
                } else {
                    fprintf(2, "xv6: fork failed\n");
                }
            }
            wait(0);
            wait(0);
        } else {
            fprintf(2, "xv6: fork failed\n");
        }
        wait(0); 
    } else {
        fprintf(2, "xv6: fork failed\n");
    }
}


void change_dir(char *args[]){
    if (strcmp(args[0], "cd") == 0) {
        if (chdir(args[1]) < 0) {
            fprintf(2, "cannot cd %s\n", args[1]);
        }
    }
}


void check_pipes(char *args[], int i){
    int no_of_pipes = 0; // keeping track of how many pipes in the command
    int pipe_pos[MAXARG]; // array to hold positions of all pipes
    for (int j = 0; j < i; j++) {
        if (strcmp(args[j], "|") == 0) {
            args[j] = 0;
            pipe_pos[no_of_pipes] = j; // stores position of pipes 
            no_of_pipes++;
        }
    }

    change_dir(args); // cd case

    if (no_of_pipes == 0) {
        exe_cmd(args, no_of_pipes); // if there is no pipes execute normally
        
    } else {
        exe_pipes(args, pipe_pos, no_of_pipes); // execute the pipe function if there is even one pipe
    }
}

void check_line(char *no_of_commands[], int count){
    for (int i = 0; i<count;i++){
        char *c;
        char *commandi = no_of_commands[i]; 
        c = commandi; // points at the first character of the first command from the array of commands
        char *args[MAXARG];
        int i = 0;
        for (int i = 0; i < MAXARG; i++) { 
            args[i] = 0;
        }

        while (*c != '\0') {
            while (*c == ' ' || *c == '\n') {
                *c = '\0'; // SEtting then spaces and newline to \0
                c++;
            }
            if (*c != '\0') {
                args[i] = c; // store in the array as an argument
                i++;
            }
            while (*c != '\0' && *c != ' ' && *c != '\n') {
                c++;
            }
        }
        if (i == 0) {
            exit(1);
        }
        check_pipes(args, i); 
    }
}

void check_no_cmds(char *thisLine) {

    char *d = thisLine; 
    char *no_of_commands[MAXARG];
    int count = 0;
    char *start = d;

    while (*d != '\0'){ // while there is something at the position of pointer
        if (*d == ';' || *d == '\n'){ 
            *d = '\0'; // set to \0 if ; introduces new command or end of line is reached
            no_of_commands[count] = start; // appends the commands to this array every time
            count++;
            start = d+1;
        }
        d++;
    }
    check_line(no_of_commands, count); // parsing the line
}




int main(int argc, char *argv[]) {
    char buf[MAXLINE]; // Initialising the buffer to hold the whole line for the command
    while (1) { // looping until the user exits
        printf(">>>");
        gets(buf, sizeof(buf)); // getting the user input
        if (buf[0] == '\n' || buf[0] == '\0') {
            continue; // if the input is empty of there is new line do next iteration
        }
        check_no_cmds(buf); //execute the command using the buffer
    }
    exit(0);
}
