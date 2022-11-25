from re import findall
from os import pipe, dup2, fork, execlp, waitpid, fdopen, close
from sys import argv, stdin, stderr

fd_out, fd_in = pipe()
pid = fork()
if pid == 0:
    dup2(fd_in, stderr.fileno())
    close(fd_in)
    execlp('gcc', 'gcc', '-fsyntax-only', '-x', 'c', argv[1])
close(fd_in)
waitpid(pid, 0)
error_log = fdopen(fd_out).read()
print(len(set(findall(r'.*:([0-9]+):[0-9]+: error: .*', error_log))))
print(len(set(findall(r'.*:([0-9]+):[0-9]+: warning: .*', error_log))))
