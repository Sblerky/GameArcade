#include <time.h>
#include <unistd.h>
#include <iostream>
#include <list>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include "Flag.h"
#include "Utils.h"
#include "Leaderboard.h"

void initGameFlagman(SDL_Renderer *renderer, SDL_Color colors[], SDL_Event event)
{

  // Game variables
  std::vector<int> flags;
  int lives = 3;
  int time = 703;
  int pressed = 0;
  int tmpScore = 0;
  bool demo = true;
  bool startingScreen = true;
  bool gameOverScreen = false;
  bool hasPressed = false;
  bool hasDecreasedLives = false;
  bool hasPlayedSound = false;
  bool hasResetTime = false;
  int delayFlagman = 45;
  int delayGameplay = 45;
  int indiceFlags = 0;
  int score = 0;

  // SDL Stuff

  SDL_Texture *flag_tex = NULL, *bButton_tex = NULL, *aButton_tex = NULL, *xButton_tex = NULL,
              *yButton_tex = NULL, *bg_tex = NULL, *dead_tex = NULL, *neutre_tex = NULL, *score_tex = NULL, *demo_tex = NULL,
              *intro_tex = NULL, *gameover_tex = NULL;

  SDL_Rect bg_pos = {0, 0, 0, 0}, button_pos = {384, 110, 100, 100},
           flag_pos = {SCREEN_W - SCREEN_W / 6, 5, 10, 10}, time_rect = {59, 10, 45, time};

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

  int exit_requested = 0;

  // load font from romfs
  TTF_Font *font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);

  SDL_Rect score_rect = {910, 627, 0, 0};
  score_tex = renderText(renderer, "Score : 0", font, colors[1], &score_rect);

  // render sprites
  SDL_Surface *bButton = IMG_Load("data/b_button.png");
  if (bButton)
  {
    button_pos.w = bButton->w;
    button_pos.h = bButton->h;
    bButton_tex = SDL_CreateTextureFromSurface(renderer, bButton);
    SDL_FreeSurface(bButton);
  }

  SDL_Surface *aButton = IMG_Load("data/a_button.png");
  if (aButton)
  {
    button_pos.w = aButton->w;
    button_pos.h = aButton->h;
    aButton_tex = SDL_CreateTextureFromSurface(renderer, aButton);
    SDL_FreeSurface(aButton);
  }

  SDL_Surface *yButton = IMG_Load("data/y_button.png");
  if (yButton)
  {
    button_pos.w = yButton->w;
    button_pos.h = yButton->h;
    yButton_tex = SDL_CreateTextureFromSurface(renderer, yButton);
    SDL_FreeSurface(yButton);
  }

  SDL_Surface *xButton = IMG_Load("data/x_button.png");
  if (xButton)
  {
    button_pos.w = xButton->w;
    button_pos.h = xButton->h;
    xButton_tex = SDL_CreateTextureFromSurface(renderer, xButton);
    SDL_FreeSurface(xButton);
  }

  SDL_Surface *dead = IMG_Load("data/dead.png");
  if (dead)
  {
    button_pos.w = dead->w;
    button_pos.h = dead->h;
    dead_tex = SDL_CreateTextureFromSurface(renderer, dead);
    SDL_FreeSurface(dead);
  }

  SDL_Surface *neutre = IMG_Load("data/neutre.png");
  if (neutre)
  {
    button_pos.w = neutre->w;
    button_pos.h = neutre->h;
    neutre_tex = SDL_CreateTextureFromSurface(renderer, neutre);
    SDL_FreeSurface(neutre);
  }

  SDL_Surface *bg = IMG_Load("data/flagman_main.png"); // 1280 x 720 image
  if (bg)
  {
    bg_pos.w = bg->w;
    bg_pos.h = bg->h;
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg);
    SDL_FreeSurface(bg);
  }

  SDL_Surface *demoS = IMG_Load("data/flagman_demo.png"); // 1280 x 720 image
  if (demoS)
  {
    demo_tex = SDL_CreateTextureFromSurface(renderer, demoS);
    SDL_FreeSurface(demoS);
  }

  SDL_Surface *intro = IMG_Load("data/flagman_intro.png"); // 1280 x 720 image
  if (intro)
  {
    intro_tex = SDL_CreateTextureFromSurface(renderer, intro);
    SDL_FreeSurface(intro);
  }

  SDL_Surface *gameover = IMG_Load("data/flagman_gameover.png"); // 1280 x 720 image
  if (gameover)
  {
    gameover_tex = SDL_CreateTextureFromSurface(renderer, gameover);
    SDL_FreeSurface(gameover);
  }

  SDL_Surface *flag = IMG_Load("data/flag.png"); // 50 x 50 image
  if (flag)
  {
    flag_pos.w = flag->w;
    flag_pos.h = flag->h;
    flag_tex = SDL_CreateTextureFromSurface(renderer, flag);
    SDL_FreeSurface(flag);
  }
  Mix_Chunk *sound[5] = {NULL};
  sound[0] = Mix_LoadWAV("data/flag2.wav");
  sound[1] = Mix_LoadWAV("data/flag1.wav");
  sound[2] = Mix_LoadWAV("data/ball.wav");
  sound[3] = Mix_LoadWAV("data/flag3.wav");
  sound[4] = Mix_LoadWAV("data/flagdead.wav");

  Mix_Music *music = NULL;
  music = Mix_LoadMUS("data/flagman.mp3");
  Mix_PlayMusic(music, -1);

  while (!exit_requested)
  {

    // Starting screen
    while (startingScreen && !exit_requested)
    {
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_QUIT)
          exit_requested = 1;

        // use joystick
        if (event.type == SDL_JOYBUTTONDOWN)
        {
          switch (event.jbutton.button)
          {
          case (JOY_PLUS):
            exit_requested = 1;
            break;

          case (JOY_A):
            startingScreen = false;
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, intro_tex, NULL, &bg_pos);
      SDL_RenderPresent(renderer);
      SDL_Delay(25);
    }

    // Remplissage de la liste
    flags.push_back(randRange(0, 3));
    indiceFlags = 0;
    delayFlagman = 55;

    // Début du jeu
    while (demo && !exit_requested)
    {
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_QUIT)
          exit_requested = 1;

        // use joystick
        if (event.type == SDL_JOYBUTTONDOWN)
        {
          switch (event.jbutton.button)
          {
          case (JOY_PLUS):
            exit_requested = 1;
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, demo_tex, NULL, &bg_pos);

      // Render Flagman
      if (delayFlagman > 15 && delayFlagman < 45)
      {
        switch (flags[indiceFlags])
        {
        case (0):
          SDL_RenderCopy(renderer, aButton_tex, NULL, &button_pos);
          if (!hasPlayedSound)
          {
            Mix_PlayChannel(-1, sound[0], 0);
            hasPlayedSound = true;
          }
          break;
        case (1):
          SDL_RenderCopy(renderer, bButton_tex, NULL, &button_pos);
          if (!hasPlayedSound)
          {
            Mix_PlayChannel(-1, sound[1], 0);
            hasPlayedSound = true;
          }
          break;
        case (2):
          SDL_RenderCopy(renderer, xButton_tex, NULL, &button_pos);
          if (!hasPlayedSound)
          {
            Mix_PlayChannel(-1, sound[2], 0);
            hasPlayedSound = true;
          }
          break;
        case (3):
          SDL_RenderCopy(renderer, yButton_tex, NULL, &button_pos);
          if (!hasPlayedSound)
          {
            Mix_PlayChannel(-1, sound[3], 0);
            hasPlayedSound = true;
          }
          break;
        }
      }
      else
      {
        SDL_RenderCopy(renderer, neutre_tex, NULL, &button_pos);
      }

      if (delayFlagman > 0)
      {
        delayFlagman--;
      }
      else
      {
        if (indiceFlags < flags.size() - 1)
        {
          indiceFlags++;
          hasPlayedSound = false;
          delayFlagman = 45;
        }
        else
        {
          demo = false;
        }
      }

      SDL_RenderPresent(renderer);
      SDL_Delay(25);
    }

    indiceFlags = 0;
    delayGameplay = 45;
    time = 703;
    hasPlayedSound = false;

    // Gameplay
    while (!demo && !exit_requested)
    {

      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_QUIT)
          exit_requested = 1;

        // use joystick
        if (event.type == SDL_JOYBUTTONDOWN)
        {
          switch (event.jbutton.button)
          {
          case (JOY_PLUS):
            exit_requested = 1;
            break;

          case (JOY_A):
            if (indiceFlags < flags.size())
            {
              pressed = 0;
              hasPressed = true;
              hasDecreasedLives = false;
              hasResetTime = false;
              indiceFlags++;
              delayFlagman = 45;
            }
            break;

          case (JOY_B):
            if (indiceFlags < flags.size())
            {
              pressed = 1;
              hasPressed = true;
              hasDecreasedLives = false;
              hasResetTime = false;
              indiceFlags++;
              delayFlagman = 45;
            }
            break;

          case (JOY_X):
            if (indiceFlags < flags.size())
            {
              pressed = 2;
              hasPressed = true;
              hasDecreasedLives = false;
              hasResetTime = false;
              indiceFlags++;
              delayFlagman = 45;
            }
            break;

          case (JOY_Y):
            if (indiceFlags < flags.size())
            {
              pressed = 3;
              hasPressed = true;
              hasDecreasedLives = false;
              hasResetTime = false;
              indiceFlags++;
              delayFlagman = 45;
            }
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, bg_tex, NULL, &bg_pos);

      // Render Flagman
      if (hasPressed && delayFlagman > 15)
      {
        if (pressed == flags[indiceFlags - 1] && indiceFlags > 0 && !hasDecreasedLives)
        {
          switch (pressed)
          {
          case (0):
            SDL_RenderCopy(renderer, aButton_tex, NULL, &button_pos);
            if (!hasResetTime)
            {
              time = 703;
              hasResetTime = true;
              score++;
              Mix_PlayChannel(-1, sound[0], 0);
            }
            break;
          case (1):
            SDL_RenderCopy(renderer, bButton_tex, NULL, &button_pos);
            if (!hasResetTime)
            {
              time = 703;
              hasResetTime = true;
              score++;
              Mix_PlayChannel(-1, sound[1], 0);
            }
            break;
          case (2):
            SDL_RenderCopy(renderer, xButton_tex, NULL, &button_pos);
            if (!hasResetTime)
            {
              time = 703;
              hasResetTime = true;
              score++;
              Mix_PlayChannel(-1, sound[2], 0);
            }
            break;
          case (3):
            SDL_RenderCopy(renderer, yButton_tex, NULL, &button_pos);
            if (!hasResetTime)
            {
              time = 703;
              hasResetTime = true;
              score++;
              Mix_PlayChannel(-1, sound[3], 0);
            }
            break;
          }
        }
        else
        {
          SDL_RenderCopy(renderer, dead_tex, NULL, &button_pos);
          if (lives > 0 && !hasDecreasedLives)
          {
            lives--;
            hasDecreasedLives = true;
            Mix_PlayChannel(-1, sound[4], 0);
            if (indiceFlags > 0)
            {
              indiceFlags--;
            }
          }
        }
      }
      else
      {
        SDL_RenderCopy(renderer, neutre_tex, NULL, &button_pos);
      }

      if (delayFlagman > 0)
      {
        delayFlagman--;
      }

      // Render time
      if (time > 0)
      {
        if (indiceFlags != flags.size())
        {
          time = time - 1;
        }
      }
      else
      {
        lives--;
        time = 703;
        SDL_RenderCopy(renderer, dead_tex, NULL, &button_pos);
      }
      time_rect.h = time;
      time_rect.y = 10 + 703 - time;
      SDL_RenderFillRect(renderer, &time_rect);

      // Render lives
      for (int i = 0; i < lives; i++)
      {
        flag_pos.x = SCREEN_W - SCREEN_W / 6 + i * 55;
        SDL_RenderCopy(renderer, flag_tex, NULL, &flag_pos);
      }

      //Render score
      if (tmpScore != score)
      {
        char score_string[32];
        sprintf(score_string, "%d", score);
        char full_string[64] = "Score : ";
        strcat(full_string, score_string);
        score_tex = renderText(renderer, full_string, font, colors[1], &score_rect);
        tmpScore = score;
      }
      SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);

      if (lives == 0)
      {
        //gameOver
        gameOverScreen = true;
        demo = true;
      }

      if (indiceFlags == flags.size())
      {
        delayGameplay--;
      }

      if (delayGameplay <= 0)
      {
        demo = true;
        hasPressed = false;
        hasPlayedSound = false;
      }

      SDL_RenderPresent(renderer);
      //std::cout << "Indice" << indiceFlags << std::endl;
      SDL_Delay(25);
    }

    // GameOver screen
    while (gameOverScreen && !exit_requested)
    {
      while (SDL_PollEvent(&event))
      {
        if (event.type == SDL_QUIT)
          exit_requested = 1;

        // use joystick
        if (event.type == SDL_JOYBUTTONDOWN)
        {
          switch (event.jbutton.button)
          {
          case (JOY_PLUS):
            exit_requested = 1;
            break;

          case (JOY_A):
            flags.clear();
            saveScore(score, "Flagman");
            score = 0;
            gameOverScreen = false;
            demo = true;
            hasPressed = false;
            hasPlayedSound = false;
            lives = 3;
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, gameover_tex, NULL, &bg_pos);
      SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
      SDL_RenderPresent(renderer);
      SDL_Delay(25);
    }
  }

  // no need to keep the font loaded
  TTF_CloseFont(font);

  Mix_FadeOutMusic(500);
  Mix_FreeMusic(music);

  for (int snd = 0; snd < 5; snd++)
    if (sound[snd])
      Mix_FreeChunk(sound[snd]);

  if (flag_tex)
    SDL_DestroyTexture(flag_tex);

  if (bg_tex)
    SDL_DestroyTexture(bg_tex);

  if (bButton_tex)
    SDL_DestroyTexture(bButton_tex);

  if (aButton_tex)
    SDL_DestroyTexture(aButton_tex);

  if (xButton_tex)
    SDL_DestroyTexture(xButton_tex);

  if (yButton_tex)
    SDL_DestroyTexture(yButton_tex);

  if (dead_tex)
    SDL_DestroyTexture(dead_tex);

  if (neutre_tex)
    SDL_DestroyTexture(neutre_tex);

  if (score_tex)
    SDL_DestroyTexture(score_tex);

  if (demo_tex)
    SDL_DestroyTexture(demo_tex);

  if (intro_tex)
    SDL_DestroyTexture(intro_tex);

  if (gameover_tex)
    SDL_DestroyTexture(gameover_tex);

  // Save Score
  saveScore(score, "Flagman");
}