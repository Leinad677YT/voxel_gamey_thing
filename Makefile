shaders: 
	cd "./resources/shaders/src" && shadercross "Sky.vert.hlsl" -o "../bin/Sky.vert.spv"
	cd "./resources/shaders/src" && shadercross "Sky.frag.hlsl" -o "../bin/Sky.frag.spv"
	cd "./resources/shaders/src" && shadercross "OpaqueBlock.vert.hlsl" -o "../bin/OpaqueBlock.vert.spv"
	cd "./resources/shaders/src" && shadercross "OpaqueBlock.frag.hlsl" -o "../bin/OpaqueBlock.frag.spv"
	cd "./resources/shaders/src" && shadercross "TransparentBlock.vert.hlsl" -o "../bin/TransparentBlock.vert.spv"
	cd "./resources/shaders/src" && shadercross "TransparentBlock.frag.hlsl" -o "../bin/TransparentBlock.frag.spv"
	cd "./resources/shaders/src" && shadercross "FrontTransparentBlock.vert.hlsl" -o "../bin/FrontTransparentBlock.vert.spv"
	cd "./resources/shaders/src" && shadercross "FrontTransparentBlock.frag.hlsl" -o "../bin/FrontTransparentBlock.frag.spv"
	cd "./resources/shaders/src" && shadercross "DepthOutline.vert.hlsl" -o "../bin/DepthOutline.vert.spv"
	cd "./resources/shaders/src" && shadercross "DepthOutline.frag.hlsl" -o "../bin/DepthOutline.frag.spv"

all: shaders
#	cmake -S . -B build
	cmake --build build
	cp -r -t ./build/Debug ./resources

run: all
	./build/Debug/server_template &
	./build/Debug/window_template

clean:
	rm -rf ./build/Debug

test: all
	./build/Debug/test_regionnblock
	./build/Debug/test_enbt
