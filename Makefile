.PHONY: all setup test clean

all:
	@echo "Run 'make setup' to setup the project"
	@echo "Run 'make test' to run the tests"
	@echo "Run 'make clean' to clean the project"

setup:
	@mkdir -p build \
	&& cd build \
	&& cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. \
	&& cmake --build .
	@echo "==================== Setup Finished ===================="
	@echo "Run './build/src/deeplang' to run the program"

test:
	@mkdir -p build \
	&& cd build \
	&& cmake -DBUILD_TESTS=ON .. \
	&& cmake --build . \
	&& ctest

clean:
	rm -rf build
