#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

const char script[] = ""
"from re import findall\n"
"from os import pipe, dup2, fork, execlp, waitpid, fdopen, close\n"
"from sys import argv, stderr\n"
"\n"
"fd_out, fd_in = pipe()\n"
"pid = fork()\n"
"if pid == 0:\n"
"    dup2(fd_in, stderr.fileno())\n"
"    close(fd_in)\n"
"    execlp('gcc', 'gcc', '-fsyntax-only', '-x', 'c', argv[1])\n"
"close(fd_in)\n"
"error_log = fdopen(fd_out).read()\n"
"waitpid(pid, 0)\n"
"print(len(set(findall(r'.*:([0-9]+):[0-9]+: error: .*', error_log))))\n"
"print(len(set(findall(r'.*:([0-9]+):[0-9]+: warning: .*', error_log))))\n";

int main(int argc, char **argv)
{
    execlp("python3", "python3", "-c", script, argv[1], NULL);
    perror("exec");
    exit(1);
}