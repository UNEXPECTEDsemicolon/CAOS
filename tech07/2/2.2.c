#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

const char script[] = "\n\
from os import stat, execve, environ\n\
from stat import S_IXUSR\n\
from os.path import exists\n\
from sys import stderr, stdout\n\
\n\
query, cmd = input().split(' /')[:2]\n\
cmd = cmd.split(' ')[0]\n\
if '?' in cmd:\n\
    path, args = cmd.split('?')\n\
else:\n\
    path = cmd\n\
    args = ''\n\
host = input().split(' ')[-1]\n\
\n\
if (query != 'GET'):\n\
    print('Only GET method is allowed', file=stderr)\n\
    exit(1)\n\
if not exists(path):\n\
    print('HTTP/1.1 404 ERROR\\n')\n\
    exit()\n\
if not (stat(path).st_mode & S_IXUSR):\n\
    print('HTTP/1.1 403 ERROR\\n')\n\
    exit()\n\
print('HTTP/1.1 200 OK')\n\
stdout.flush()\n\
envs = {'HTTP_HOST': host,\n\
        'QUERY_STRING': args,\n\
        'REQUEST_METHOD': query,\n\
        'SCRIPT_NAME': path}\n\
execve(path, [path], envs)\n\
";

int main()
{
    execlp("python3", "python3", "-c", script, NULL);
    perror("exec");
    exit(1);
}