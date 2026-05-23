#include "../data/app.h"

#include "../data/globals.h"

SDL_GPUShader *LoadShader (
    SDL_GPUDevice *gpu_device,
    const char *filename,
    Uint32 sampler_count,
    Uint32 uniform_buffer_count,
    Uint32 storage_buffer_count,
    Uint32 storage_texture_count
) {
	SDL_GPUShaderStage stage; // pipeline stage (vertex, fragment...)

    SDL_GPUShaderFormat backend_formats;    // valid shader formats     (spirv, dxil, msl)
    SDL_GPUShaderFormat format;             // provided shader format   (spirv, dxil, msl)

    char fullpath[LEINAD_MAX_PATH_LENGTH];         // full path of the compiled shader file
	const char *entrypoint;     // entry point of the file (`main` or `main0`)
	const char *basepath;       // directory on which it was launched
    
    size_t code_size;   // will hold the size of the shader to load
    void *code;         // loaded file obtained from SDL_LoadFile();
    
    // SDL_GPUShaderCreateInfo shader_info; // declared later to allow for struct initialization
    SDL_GPUShader *shader;                  // pointer to the returned shader
    
    
    // get pipeline stage

    if (SDL_strstr(filename, ".vert")) {
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
    else if (SDL_strstr(filename, ".frag")) {
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
    else {
		SDL_Log("Unknown shader type: %s", filename);
		return NULL;
	}

	// get shader format
    
    backend_formats = SDL_GetGPUShaderFormats(gpu_device);
	format = SDL_GPU_SHADERFORMAT_INVALID;
	
    basepath = SDL_GetBasePath();

	if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullpath, sizeof(fullpath), "%sresources/shaders/bin/%s.spv", basepath, filename);
		entrypoint = "main";
		format = SDL_GPU_SHADERFORMAT_SPIRV;
	}
	else if (backend_formats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullpath, sizeof(fullpath), "%sresources/shaders/bin/%s.dxil", basepath, filename);
		entrypoint = "main";
		format = SDL_GPU_SHADERFORMAT_DXIL;
	}
	else if (backend_formats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullpath, sizeof(fullpath), "%sresources/shaders/bin/%s.msl", basepath, filename);
		entrypoint = "main0";
		format = SDL_GPU_SHADERFORMAT_MSL;
	}
	else {
		SDL_Log("No supported shader format found!");
		return NULL;
	}


    // load the file

    code = SDL_LoadFile(fullpath, &code_size);
    if (!code) {
        SDL_Log("Couldn't load shader file: %s", SDL_GetError());
        return NULL;
    }

    // create the shader

	SDL_GPUShaderCreateInfo shader_info = {
		.code = code,                                   // compiled shader file
		.code_size = code_size,                         // size of compiled shader file
        .entrypoint = entrypoint,                       // entry point of the shader
		.format = format,                               // type of shader (svl for vulkan)
		.stage = stage,                                 // vertex, fragment...
		.num_samplers = sampler_count,                  // idk
		.num_uniform_buffers = uniform_buffer_count,    // idk
		.num_storage_buffers = storage_buffer_count,    // idk
		.num_storage_textures = storage_texture_count,  // idk
	};

	shader = SDL_CreateGPUShader(gpu_device, &shader_info);
	if (!shader) {
		SDL_Log("Couldn't create shader: %s", SDL_GetError());
	}

	SDL_free(code);
	return shader;
}

SDL_GPUTexture *CreateDepthTexture(Uint32 drawablew, Uint32 drawableh)
{
	SDL_GPUTexture *result;
    SDL_GPUTextureCreateInfo createinfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
	    .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
	    .width = drawablew,
	    .height = drawableh,
	    .layer_count_or_depth = 1,
	    .num_levels = 1,
	    .sample_count = 0,
	    .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
	    .props = 0
    };

	result = SDL_CreateGPUTexture(device, &createinfo);
	if (!result) {
		SDL_Log("Failed to create depth texture: %s", SDL_GetError());
		return NULL;
	}

	return result;
}
