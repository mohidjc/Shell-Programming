# Custom Shell Implementation-xv6

This project is a basic shell written in C, designed to execute commands, handle redirection, and support piping between processes. It provides functionality for executing multiple commands in a single line, input/output redirection, and chaining commands with pipes.

## Features

- **Command Execution**: Run commands with arguments just like a regular shell.
- **Redirection**:
  - Input redirection using `<`
  - Output redirection using `>`
- **Pipes**:
  - Supports one or more pipes (`|`) to connect multiple commands.
- **Multiple Commands**: Ability to run multiple commands in a single line separated by semicolons (`;`).
- **Change Directory**: The `cd` command allows users to change directories.

## Files

- `main.c`: Contains the main loop for reading user input and executing commands.
- `redir()`: Handles input and output redirection for commands.
- `redir_pipe()`: Similar to `redir()`, but handles redirection when pipes are present.
- `exe_cmd()`: Executes a single command without pipes.
- `exe_pipes()`: Executes commands connected by one or more pipes.
- `check_pipes()`: Parses the input to identify and handle pipes.
- `check_line()`: Parses each command and its arguments.
- `check_no_cmds()`: Identifies multiple commands separated by semicolons.
  
## How to Use

1. **Compile** the program:
    ```bash
    make
    ```
    Or, if there's no `Makefile`:
    ```bash
    gcc main.c -o my_shell
    ```

2. **Run** the shell:
    ```bash
    ./my_shell
    ```

3. **Commands**:
   - Run a command:
     ```bash
     >>> ls
     ```
   - Redirect input or output:
     ```bash
     >>> cat < input.txt
     >>> echo "hello" > output.txt
     ```
   - Pipe commands:
     ```bash
     >>> ls | grep .c
     >>> cat file.txt | grep word | wc -l
     ```
   - Chain multiple commands:
     ```bash
     >>> ls; echo "hello"; cat file.txt
     ```

4. **Exit the shell**:
    Simply close the terminal or press `Ctrl+C`.

## Limitations

- The current implementation supports up to two pipes. Expanding this to handle more pipes is possible with some modifications.
- Redirection and pipes are currently limited in scope and might not cover all edge cases.

## Future Improvements

- Implement additional shell features such as background task execution (`&`).
- Add support for environment variables.
- Improve error handling for malformed commands.
