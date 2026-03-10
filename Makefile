all:
	cmake -S . -B build
	cmake --build build

run: all
	./build/Debug/window_template
