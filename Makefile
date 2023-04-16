#
# Makefile to drive cmake operations
# https://github.com/d-e-e-p/fixed-size-string-buffer
# Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
# 

.PHONY: *
.DEFAULT_GOAL := help
INSTALL_LOCATION := ~/.local
CPM_SOURCE_CACHE:= .cache/CPM
#MAKEFLAGS += -j4
export CMAKE_BUILD_PARALLEL_LEVEL:= 4
# SHELL=/bin/bash -vx # for debug

include cmake/utils.mk

# Use the COMPARE_VERSIONS function to set the CMAKE_FRESH variable
# CMAKE_FRESH := $(shell python3 -c "$(COMPARE_VERSIONS_PY)")

BROWSER := python3 -c "$$BROWSER_PYSCRIPT"

help: ## this message
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

all: clean debug release test bench coverage install validate docs

clean: ## remove build dir
	rm -rf build/

debug: ## create debug version of standalone examples
	rm -rf ./build/debug/CMakeCache.txt
	cmake -S standalone -B build/debug -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug --config Debug 
	./build/debug/bin/demo

release: ## create optimized version of examples
	rm -rf ./build/release/CMakeCache.txt
	cmake -S standalone -B build/release -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release --config Release 
	./build/release/bin/demo

test: ## exercise all queue operations 
	rm -rf ./build/test/CMakeCache.txt
	cmake -S test -B build/test -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/test --config Debug
	cd build/test && ctest -C Debug -VV

bench: ## run benchmark on push operation 
	rm -rf ./build/bench/CMakeCache.txt
	cmake -S bench -B build/bench -DCMAKE_BUILD_TYPE=Release
	cmake --build build/bench --config Release --target unit_bench
	./build/bench/bin/unit_bench

coverage: ## check code coverage 
	rm -rf ./build/coverage/CMakeCache.txt
	rm -rf ./build/coverage/CMakeFiles/unit_tests.dir
	cmake -S test -B build/coverage -DENABLE_COVERAGE=1
	cmake --build build/coverage --config Debug
	cd build/coverage && ctest -C Debug -VV
	cd build/coverage && make coverage
	-open build/coverage/coverage/index.html

install: ## copy include files to install location
	rm -rf ./build/install/CMakeCache.txt
	cmake -S install -B build/install -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build/install --config Release --target install

validate: ## execute from the installed version
	rm -rf ./build/validate/CMakeCache.txt
	#cmake -S validate -B build/validate -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION) -DCMAKE_RULE_MESSAGES=OFF -DCMAKE_VERBOSE_MAKEFILE=ON
	cmake -S validate -B build/validate -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build/validate --config Release
	build/validate/bin/basic_example_fp
	build/validate/bin/basic_example_cpm

docs: ## generate Doxygen HTML documentation, including API docs
	rm -rf ./build/docs/CMakeCache.txt
	rm -rf ./build/docs/doxygen
	cmake -S docs -B build/docs
	cmake --build build/docs --target GenerateDocs -v
	-open build/docs/doxygen/html/index.html

windows_unicode_fix: ## needed for unicode output on windows
	chcp.com 65001



