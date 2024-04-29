# Custom Shell Program

This project implements a simple shell program in C, allowing users to execute commands and log them into a file. The shell supports basic command execution using fork and exec family functions without using the system() function. Additionally, it logs all executed commands with timestamps in a file named "log.txt".

## Learning Objectives

- Practice fork/exec relationships
- Understand the multi-process environment and communication through shared objects
- Gain insights into the details of a shell program
- Recognize the need for inter-process communication mechanisms

## Features

- Starts with the sign `$` for autograding purposes
- Executes given commands using fork and exec family functions
- Handles command not found errors by searching for the command's path
- Logs executed commands in "log.txt" with timestamps

## Usage

1. Compile the source code.
2. Run the compiled executable.
3. Enter commands at the `$` prompt and press Enter to execute.

## Commands

- To exit the shell, type `exit`.
- To view command history, type `history`.

## Logging

All executed commands are logged in "log.txt" with timestamps in the following format.

