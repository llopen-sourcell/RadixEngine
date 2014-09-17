#include "Game.hpp"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_timer.h>
#include <stdexcept>
#include <string>
#include <cstdio>
#include <iostream>

#include "engine/env/ConfigFileParser.hpp"
#include "engine/env/Environment.hpp"
#include <engine/env/System.hpp>
#include "Input.hpp"

namespace glPortal {

Game::Game() :closed(false) {
  window.createFromConfig();

  try {
      world.create();
    }
  catch (std::runtime_error &e) {
    System::log(std::string("Runtime Error: " + std::string(e.what())));
    }
  update();
}

void Game::update() {
  SDL_Event event;
  //Time the next update should be run to conform to UPDATE_RATE
  unsigned int nextUpdate = SDL_GetTicks();
  //Time of last FPS count
  unsigned int lastFpsTime = SDL_GetTicks();
  //Amount of times rendering has been skipped in favour of updating
  int skipped;
  //Amount of times rendered so far, gets reset each second
  int rendered = 0;

  while (not closed) {
    skipped = 0;
    //Count the FPS
    if (SDL_GetTicks() - lastFpsTime > 1000) {
      lastFpsTime = SDL_GetTicks();
      int fps = rendered;
      std::cout << fps << std::endl;
      rendered = 0;
    }

    //Update the game if it is time
    while (SDL_GetTicks() > nextUpdate && skipped < MAX_SKIP) {
      while (SDL_PollEvent(&event)) {
        handleEvent(event);
      }
      world.update();
      nextUpdate += SKIP_TIME;
      skipped++;
    }
    world.render();
    rendered++;
    window.swapBuffers();
  }
  world.destroy();
  window.close();
}

void Game::close() {
  closed = true;
}

void Game::handleEvent(SDL_Event event){
  if (event.type == SDL_QUIT) {
    closed = 1;
  }
  if (event.type == SDL_KEYDOWN) {
    int key = event.key.keysym.sym;
    int mod = event.key.keysym.mod;
    
    Input::keyPressed(key, mod);
    
    if (key == 'q') {
      close();
    }
  }
  if (event.type == SDL_KEYUP) {
    int key = event.key.keysym.sym;
    int mod = event.key.keysym.mod;
    
    Input::keyReleased(key, mod);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      world.shootPortal(1);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      world.shootPortal(2);
    }
  }
}

} /* namespace glPortal */

using namespace glPortal;

Window window;

int main(int argc, char *argv[]) {
  Environment::init(argc, argv);
  Game game;

  return 0;
}
