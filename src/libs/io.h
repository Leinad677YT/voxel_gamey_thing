#pragma once

#include <SDL3/SDL.h>

#define LEINAD_MAX_LINE_LENGTH 197
// buffers should hold LENGTH+1 for the final '\0'
// if not enough, make it 219304 :works_as_intended:

static int leinad_io_readline(SDL_IOStream* io, char output[LEINAD_MAX_LINE_LENGTH]){
    Sint64 offset;
    size_t read_chars;
    int i, j;
    char line[LEINAD_MAX_LINE_LENGTH+1] = {0};

    // GET CURRENT OFFSET
    offset = SDL_TellIO(io);

    read_chars = SDL_ReadIO(io, line, LEINAD_MAX_LINE_LENGTH);

    for (i = 0;i < read_chars; i++) {
        if (line[i] == '\0' || line[i] == '\n') break;
    }

    for (j = 0; j < i;j++) {
        output[j] = line[j];
    }

    output[j] = '\0';

    // remove the line from the IO offset
    SDL_SeekIO(io, offset+i+1, SDL_IO_SEEK_SET);

    return i;
}