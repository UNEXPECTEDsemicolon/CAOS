#!/usr/bin/python
from os import getenv
print(getenv("HTTP_HOST"), getenv("QUERY_STRING"),
      getenv('REQUEST_METHOD'), getenv('SCRIPT_NAME'))
