
#
# utility functions for makefile
#


CMAKE_VERSION := $(shell cmake --version | grep 'cmake version' | sed 's/cmake version //')
# this gets collapsed into one line so needs semicolon
define COMPARE_VERSIONS_PY
import sys;
from packaging import version;
version_string = '$(CMAKE_VERSION)';
required_version = '3.24';
compare = version.parse(version_string) >= version.parse(required_version);
option = '--fresh' if compare else '';
print(option)
endef
export COMPARE_VERSIONS_PY

define BROWSER_PYSCRIPT
import os, webbrowser, sys

try:
    from urllib import pathname2url
except:
    from urllib.request import pathname2url

webbrowser.open("file://" + pathname2url(os.path.abspath(sys.argv[1])))
endef
export BROWSER_PYSCRIPT

define PRINT_HELP_PYSCRIPT
import re, sys

for line in sys.stdin:
    match = re.match(r'^([a-zA-Z_-]+):.*?## (.*)$$', line)
    if match:
        target, help = match.groups()
        help = help.replace("{INSTALL_LOCATION}","$(INSTALL_LOCATION)")
        print("%-20s %s" % (target, help))
endef
export PRINT_HELP_PYSCRIPT


