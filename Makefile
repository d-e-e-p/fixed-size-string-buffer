#
# Makefile to drive cmake operations
# https://github.com/d-e-e-p/fixed-size-string-buffer
# Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
# 

.PHONY: *
.DEFAULT_GOAL := help
INSTALL_LOCATION := ~/.local
CPM_SOURCE_CACHE:= .cache/CPM
# SHELL=/bin/bash -vx # for debug
#CCMAKE_COLORS='s=39:p=220:c=207:n=196:y=46'

include cmake/utils.mk

# Use the COMPARE_VERSIONS function to set the CMAKE_FRESH variable
# CMAKE_FRESH := $(shell python3 -c "$(COMPARE_VERSIONS_PY)")

BROWSER := python3 -c "$$BROWSER_PYSCRIPT"

help: ## this message
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

all: clean debug release test bench coverage install docs

clean: ## remove build dir
	rm -rf build/

debug: ## create debug version of standalone examples
	rm -rf ./build/debug/CMakeCache.txt
	cmake -S standalone -B build/debug 
	cmake --build build/debug --config Debug 
	./build/debug/bin/demo

release: ## create optimized version of examples
	rm -rf ./build/release/CMakeCache.txt
	cmake -S standalone -B build/release 
	cmake --build build/release --config Release 
	./build/release/bin/demo

test: ## exercise all queue operations 
	rm -rf ./build/test/CMakeCache.txt
	cmake -S test -B build/test 
	cmake --build build/test --config Debug
	cd build/test && ctest -C Debug -VV

bench: ## run benchmark on push operation 
	rm -rf ./build/bench/CMakeCache.txt
	cmake -S bench -B build/bench 
	cmake --build build/bench --config Release
	./build/bench/bin/unit_bench

coverage: ## check code coverage 
	rm -rf ./build/coverage/CMakeCache.txt
	rm -rf ./build/coverage//CMakeFiles/unit_tests.dir
	cmake -S test -B build/coverage -DENABLE_COVERAGE=1
	cmake --build build/coverage --config Debug
	cd build/coverage && ctest -C Debug -VV
	cd build/coverage && make coverage
	-open build/coverage//coverage/index.html

install: ## copy include files to install location
	rm -rf ./build/release/CMakeCache.txt
	cmake -S install -B build/install -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build/install --config Release --target install

docs: ## generate Doxygen HTML documentation, including API docs
	rm -rf ./build/docs/CMakeCache.txt
	rm -rf ./build/docs/doxygen
	cmake -S docs -B build/docs
	cmake --build build/docs --target GenerateDocs -v
	-open build/docs/doxygen/html/index.html

windows_unicode_fix: ## needed for unicode output on windows
	chcp.com 65001



