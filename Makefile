shaders: 
	shadercross "resources/shaders/src/DepthOutline.vert.hlsl" -o "resources/shaders/DepthOutline.vert.spv"
	shadercross "resources/shaders/src/DepthOutline.frag.hlsl" -o "resources/shaders/DepthOutline.frag.spv"
	shadercross "resources/shaders/src/TexturedQuad.vert.hlsl" -o "resources/shaders/TexturedQuad.vert.spv"
	shadercross "resources/shaders/src/TexturedQuad.frag.hlsl" -o "resources/shaders/TexturedQuad.frag.spv"

all: shaders
#	cmake -S . -B build
	cmake --build build
	cp -r -t ./build/Debug ./resources

run: all
	./build/Debug/window_template

clean:
	rm -rf ./build/Debug

test: all
	gcc test/region_n_block.c -lSDL3
	./a.out
	rm -f ./a.out