shaders: 
	cd "./resources/shaders/src" && shadercross "DepthOutline.vert.hlsl" -o "../bin/DepthOutline.vert.spv"
	cd "./resources/shaders/src" && shadercross "DepthOutline.frag.hlsl" -o "../bin/DepthOutline.frag.spv"
	cd "./resources/shaders/src" && shadercross "TexturedQuad.vert.hlsl" -o "../bin/TexturedQuad.vert.spv"
	cd "./resources/shaders/src" && shadercross "TexturedQuad.frag.hlsl" -o "../bin/TexturedQuad.frag.spv"
	cd "./resources/shaders/src" && shadercross "TransparentTexturedQuad.vert.hlsl" -o "../bin/TransparentTexturedQuad.vert.spv"
	cd "./resources/shaders/src" && shadercross "TransparentTexturedQuad.frag.hlsl" -o "../bin/TransparentTexturedQuad.frag.spv"
	cd "./resources/shaders/src" && shadercross "MergeTransparency.vert.hlsl" -o "../bin/MergeTransparency.vert.spv"
	cd "./resources/shaders/src" && shadercross "MergeTransparency.frag.hlsl" -o "../bin/MergeTransparency.frag.spv"
	cd "./resources/shaders/src" && shadercross "Sky.vert.hlsl" -o "../bin/Sky.vert.spv"
	cd "./resources/shaders/src" && shadercross "Sky.frag.hlsl" -o "../bin/Sky.frag.spv"

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