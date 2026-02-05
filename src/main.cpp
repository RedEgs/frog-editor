#define SDL_MAIN_USE_CALLBACKS
#include <iostream>
#include <SDL3/SDL_main.h>

#include "Game.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::cout << "Pre-initialisation" << std::endl;
    const auto game = new Game{argc, argv};
    *appstate = game;

    std::cout << "Initialising Game" << std::endl;
    return game->Init();

}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto game = static_cast<Game*>(appstate);
    return game->Iterate();

}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto game = static_cast<Game*>(appstate);
    return game->Event(event);
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

    const auto game = static_cast<Game*>(appstate);
    game->Quit(result);
}