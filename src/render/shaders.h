#pragma once

/**
 * Loads a shader
 * @param device gpu device to assign the shader to
 * @param filename name of the shader file without extension, relative to `resources/shaders/` and must contain on it's name .vert for vertex or .frag for fragment shaders respectively
 * @param sampler_count ?
 * @param uniform_buffer_count ?
 * @param storage_buffer_count ?
 * @param storage_texture_count ?
 * @return Returns the shader on success and NULL on failure, The shader has to be released after not being needed anymore
 */
SDL_GPUShader *LoadShader(
    SDL_GPUDevice *device,
    const char *filename,
    Uint32 sampler_count, 
    Uint32 uniform_buffer_count,
    Uint32 storage_buffer_count,
    Uint32 storage_texture_count
);


/**
 * Creates a depth texture to use later on
 * @param drawablew width of the drawable area
 * @param drawableh height of the drawable area
 * @return Returns the depth texture or NULL on failure
 */
SDL_GPUTexture *CreateDepthTexture(Uint32 drawablew, Uint32 drawableh);