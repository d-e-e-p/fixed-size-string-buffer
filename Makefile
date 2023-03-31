.PHONY: help clean debug release test bench coverage docs install format
.DEFAULT_GOAL := help
INSTALL_LOCATION := ~/.local

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

BROWSER := python3 -c "$$BROWSER_PYSCRIPT"

help: ## this message
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

clean: ## remove build dir
	rm -rf build/

debug: ## create slow debug version
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DCMAKE_BUILD_TYPE="Debug"
	cmake --build build --config Debug

release: ## create optimized release version
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DCMAKE_BUILD_TYPE="Release" 
	cmake --build build --config Release

test: ## run tests under test/ dir
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DENABLE_UNIT_TESTING=1 -DCMAKE_BUILD_TYPE="Release" 
	cmake --build build --config Release
	cd build/ && ctest -C Release -VV

bench: ## run benchmark under bench/ dir
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DENABLE_BENCH=1 -DCMAKE_BUILD_TYPE="Release" 
	cmake --build build --config Release

coverage: ## check code coverage 
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DENABLE_COVERAGE=1 -DENABLE_UNIT_TESTING=1 -DCMAKE_BUILD_TYPE="Debug" --fresh
	cmake --build build --config Debug
	cd build/ && ctest -C Debug -VV
	cd build/ && make coverage
	$(BROWSER) build/coverage/index.html

docs: ## generate Doxygen HTML documentation, including API docs
	rm -rf docs/
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DENABLE_DOXYGEN=1 --fresh
	cmake --build build --config Release
	cmake --build build --target doxygen-docs
	$(BROWSER) docs/html/index.html

install: ## install the package to the INSTALL_LOCATION={INSTALL_LOCATION}
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build --config Release
	cmake --build build --target install --config Release

format: ## format the project sources
	rm -rf build/
	cmake -Bbuild -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build --target clang-format

#gen_single_include: ## generate an include file combining definition and implementation
#	@python utils/amalgamate/amalgamate.py -c utils/single_include.json -s .
