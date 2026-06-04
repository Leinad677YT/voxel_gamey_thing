#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <SDL3/SDL_main.h>

#include <leinad/data/control_shortcuts.h>
#include <leinad/data/app.h>
#include "data/globals.c"

int main(int argc, char* argv[]){

    Uint64 current_ns, previous_ns;

  { // initialize SDL subsystem

    SDL_SetAppMetadata(
        LEINAD_APP_NAME,
        LEINAD_APP_VERSION,
        LEINAD_APP_IDENTIFIER
    );

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!NET_Init()) {
        SDL_Log("Couldn't initialize SDL_net: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
  }

  { // init server

    server_addr = NET_ResolveHostname(LEINAD_SERVER_ADDR);

    NET_WaitUntilResolved(server_addr, -1);

    server = NET_CreateServer(server_addr, LEINAD_SERVER_PORT);
    
    NET_WaitUntilInputAvailable((void*) &server, 1, -1);
    NET_AcceptClient(server, &client_sock);
  }

    current_ns = SDL_GetTicksNS();
    previous_ns = SDL_GetTicksNS();


    loop() {
    
      { // check the time since last tick
        current_ns = SDL_GetTicksNS();

        if (current_ns - previous_ns < LEINAD_TICK_RANGE_NS) continue;
      }

        char buffer[100] = {0};
        int rcvd = 0;
        
        if (NET_GetConnectionStatus(client_sock) == NET_FAILURE) {
            SDL_Log("Closed connection");
            return SDL_APP_SUCCESS;
        }

        rcvd = NET_ReadFromStreamSocket(client_sock, buffer, 99);

        if (rcvd == -1) continue;
        if (rcvd) SDL_Log("%s", buffer);

        if (0 == SDL_strcmp(buffer, "close")) goto close;

        previous_ns = current_ns;

    }

close:
    NET_DestroyStreamSocket(client_sock);
    NET_DestroyServer(server);

    NET_Quit();
    SDL_Quit();

}