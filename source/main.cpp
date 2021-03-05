#include <time.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include "Flag.h"
#include "Ball.h"
#include "Utils.h"
#include "Leaderboard.h"

void initJson()
{
  nlohmann::json jfile;
  std::string msg = "0";
  std::string key = MY_SECRET;
  jfile["Ball"].push_back(encryptScore(msg, key));
  jfile["Flagman"].push_back(encryptScore(msg, key));

  std::ofstream inputFile(scorePath);
  inputFile << std::setw(4) << jfile << std::endl;
  inputFile.close();
}

int main(int argc, char **argv)
{
  socketInitializeDefault(); // Initialize sockets
  nxlinkStdio();

  // Creating a directory
  if (mkdir("Sblerky", 0777) == -1)
    std::cerr << "Error :  " << strerror(errno) << std::endl;

  else
    std::cout << "Directory created";

  //Check if leaderboard is here and valid json
  std::ifstream f(scorePath);
  if (f.good())
  {
    try
    {
      f.close();
      std::ifstream file(scorePath);
      std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
      file.close();
      nlohmann::json jfile = nlohmann::json::parse(content);
    }
    catch (const std::exception &exc)
    {
      initJson();
    }
  }
  else
  {
    initJson();
  }

  romfsInit();
  chdir("romfs:/");

  int exit_requested = 0;
  int trail = 0;
  int wait = 25;
  bool isOnA = true;

  SDL_Texture *gameA_tex = NULL, *gameB_tex = NULL, *arrowA_tex = NULL, *arrowB_tex = NULL, *bg_tex = NULL;
  SDL_Rect bg_pos = {0, 0, 0, 0};
  Mix_Music *music = NULL;
  SDL_Event event;

  SDL_Color colors[] = {
      {128, 128, 128, 0}, // gray
      {255, 255, 255, 0}, // white
      {255, 0, 0, 0},     // red
      {0, 255, 0, 0},     // green
      {0, 0, 255, 0},     // blue
      {255, 255, 0, 0},   // brown
      {0, 255, 255, 0},   // cyan
      {255, 0, 255, 0},   // purple
      {0, 0, 0, 0},       // Black
  };

  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  Mix_Init(MIX_INIT_OGG);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  SDL_Window *window = SDL_CreateWindow("SblerkyArcade", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

  SDL_Surface *bg = IMG_Load("data/main_menu.png");
  if (bg)
  {
    bg_pos.w = bg->w;
    bg_pos.h = bg->h;
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg);
    SDL_FreeSurface(bg);
  }

  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  SDL_JoystickEventState(SDL_ENABLE);
  SDL_JoystickOpen(0);

  // load font from romfs
  TTF_Font *font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);

  // render text as texture
  SDL_Rect gameA_rect = {570, 496, 0, 0};
  gameA_tex = renderText(renderer, "Flagman", font, colors[1], &gameA_rect);

  SDL_Rect gameB_rect = {602, 590, 0, 0};
  gameB_tex = renderText(renderer, "Ball", font, colors[1], &gameB_rect);

  SDL_Rect arrowA_rect = {570 - 50, 496, 0, 0};
  arrowA_tex = renderText(renderer, ">", font, colors[1], &arrowA_rect);

  SDL_Rect arrowB_rect = {602 - 50, 590, 0, 0};
  arrowB_tex = renderText(renderer, ">", font, colors[1], &arrowB_rect);

  // no need to keep the font loaded
  TTF_CloseFont(font);

  SDL_InitSubSystem(SDL_INIT_AUDIO);
  Mix_AllocateChannels(5);
  Mix_OpenAudio(48000, AUDIO_S16, 2, 4096);

  // load music and sounds from files
  music = Mix_LoadMUS("data/menu.mp3");
  if (music)
    Mix_PlayMusic(music, -1);

  while (!exit_requested && appletMainLoop())
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
        exit_requested = 1;

      // use joystick
      if (event.type == SDL_JOYBUTTONDOWN)
      {
        if (event.jbutton.button == JOY_UP || event.jbutton.button == LSTICK_UP)
        {
          isOnA = !isOnA;
        }

        if (event.jbutton.button == JOY_DOWN || event.jbutton.button == LSTICK_DOWN)
        {
          isOnA = !isOnA;
        }

        if (event.jbutton.button == JOY_PLUS)
          exit_requested = 1;

        if (event.jbutton.button == JOY_MINUS)
        {
          initLeaderboard(renderer, colors, event);
        }

        if (event.jbutton.button == JOY_B)
          trail = !trail;

        if (event.jbutton.button == JOY_A)
        {
          if (isOnA)
          {
            Mix_FadeOutMusic(500);
            initGameFlagman(renderer, colors, event);
            Mix_PlayMusic(music, -1);
          }
          else
          {
            Mix_FadeOutMusic(500);
            initGameBall(renderer, colors, event);
            Mix_PlayMusic(music, -1);
          }
        }
      }
    }

    if (bg_tex)
      SDL_RenderCopy(renderer, bg_tex, NULL, &bg_pos);

    // put text on screen
    if (gameA_tex)
      SDL_RenderCopy(renderer, gameA_tex, NULL, &gameA_rect);

    if (gameB_tex)
      SDL_RenderCopy(renderer, gameB_tex, NULL, &gameB_rect);

    if (arrowA_tex && isOnA)
      SDL_RenderCopy(renderer, arrowA_tex, NULL, &arrowA_rect);

    if (arrowB_tex && !isOnA)
      SDL_RenderCopy(renderer, arrowB_tex, NULL, &arrowB_rect);

    SDL_RenderPresent(renderer);

    SDL_Delay(wait);
  }

  // clean up your textures when you are done with them

  if (bg_tex)
    SDL_DestroyTexture(bg_tex);

  if (gameA_tex)
    SDL_DestroyTexture(gameA_tex);

  if (gameB_tex)
    SDL_DestroyTexture(gameB_tex);

  if (arrowA_tex)
    SDL_DestroyTexture(arrowA_tex);

  if (arrowB_tex)
    SDL_DestroyTexture(arrowB_tex);

  // stop sounds and free loaded data
  Mix_HaltChannel(-1);
  Mix_FreeMusic(music);

  IMG_Quit();
  Mix_CloseAudio();
  TTF_Quit();
  Mix_Quit();
  SDL_Quit();
  socketExit();
  romfsExit();
  return 0;
}
