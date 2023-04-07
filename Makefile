#
# Makefile to drive cmake operations
# https://github.com/d-e-e-p/fixed-size-string-buffer
# Copyright (c) 2023 Sandeep <deep@tensorfield.ag>
# 

.PHONY: *
.DEFAULT_GOAL := help
INSTALL_LOCATION := ~/.local
# SHELL=/bin/bash -vx # for debug
#CCMAKE_COLORS='s=39:p=220:c=207:n=196:y=46'

include cmake/utils.mk

# Use the COMPARE_VERSIONS function to set the CMAKE_FRESH variable
# CMAKE_FRESH := $(shell python3 -c "$(COMPARE_VERSIONS_PY)")

BROWSER := python3 -c "$$BROWSER_PYSCRIPT"

help: ## this message
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

clean: ## remove build dir
	rm -rf build/

debug: ## create slow debug version of standalone example
	rm -rf ./build/debug/CMakeCache.txt
	cmake -S standalone -B build/debug -DCMAKE_BUILD_TYPE=Debug 
	cmake --build build/debug --config Debug 
	which -s build/debug/fixed_size_string_buffer && build/debug/fixed_size_string_buffer

release: ## create optimized release version of example
	rm -rf ./build/release/CMakeCache.txt
	cmake -S standalone -B build/release -DCMAKE_BUILD_TYPE=Release 
	cmake --build build/release --config release 
	which -s build/release/fixed_size_string_buffer && build/release/fixed_size_string_buffer

bench: ## run benchmark on push operation under bench/sources
	rm -rf ./build/bench/CMakeCache.txt
	 cmake -S bench -B build/bench 
	 cmake --build build/bench 
	 which -s ./build/bench/unit_bench && ./build/bench/unit_bench

test: ## exercise all queue operations under test/sources
	rm -rf ./build/test/CMakeCache.txt
	cmake -S test -B build/test
	cmake --build build/test --config Debug
	cd build/test && ctest -C Debug -VV

coverage: ## check code coverage 
	rm -rf build/coverage/CMakeCache.txt
	cmake -S test -B build/coverage -DENABLE_COVERAGE=1
	cmake --build build/coverage --config Debug
	cd build/coverage && ctest -C Debug -VV
	cd build/coverage && make coverage
	$(BROWSER) build/coverage/coverage/index.html

install: ## move libs to install location
	rm -rf ./build/release/CMakeCache.txt
	cmake -S install -B build/install -DCMAKE_INSTALL_PREFIX=$(INSTALL_LOCATION)
	cmake --build build/install --config Release --target install

docs: ## generate Doxygen HTML documentation, including API docs
	rm -rf ./build/docs/CMakeCache.txt
	cmake -S docs -B build/docs
	cmake --build build/docs --target GenerateDocs -v
	open build/docs/doxygen/html/index.html


