.PHONY: all setup test clean run rc exe docs

all:
	@echo "Run 'make setup' to setup the project"
	@echo "Run 'make test' to run the tests"
	@echo "Run 'make clean' to clean the project"

docs:
	@cd docs && doxygen Doxyfile

setup:
	@mkdir -p build \
	&& cd build \
	&& cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. \
	&& cmake --build .
	@echo "==================== Setup Finished ===================="
	@echo "Execute 'make run', or 'make rc #run clean' or './build/src/deeplang' to run the program"

test:
	@mkdir -p build \
	&& cd build \
	&& cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release .. \
	&& cmake --build . --config Release \
	&& ctest -C Release

clean:
	rm -rf build

run:
	@echo ">> ./build/src/deeplang"
	@./build/src/deeplang

# run clean
rc:
	@clear
	@make run

exe: setup rc

# not tidy (ignore clang-tidy error)
ntidy_commit:
	git add .
	IGNORE_CLANG_TIDY_ERROR=1 git commit
