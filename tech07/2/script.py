from os import stat, execve, environ
from stat import S_IXUSR
from os.path import exists
from sys import stderr, stdout

query, cmd = input().split(' ')[:2]
if '?' in cmd:
    path, args = cmd.split('?')
else:
    path = cmd
    args = ''
host = input().split(' ')[-1]
environ['HTTP_HOST'] = host
environ['QUERY_STRING'] = args
environ['REQUEST_METHOD'] = query
environ['SCRIPT_NAME'] = path

if (query != 'GET'):
    print("Only GET method is allowed", file=stderr)
    exit(1)
if not exists(path):
    print('HTTP/1.1 404 ERROR\n')
    exit()
if not (stat(path).st_mode & S_IXUSR):
    print('HTTP/1.1 403 ERROR\n')
    exit()
envs = dict([pair.split('=') for pair in args.split('&')])
print('HTTP/1.1 200 OK')
stdout.flush()
execve(path, [path], envs)
