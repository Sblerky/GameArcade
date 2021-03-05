#include <time.h>
#include <unistd.h>
#include <iostream>
#include <list>
#include <fstream>
#include <iomanip>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include "Leaderboard.h"
#include "Utils.h"
#include "json.hpp"
#include "encrypt.h"

std::string encryptScore(std::string &msg, std::string &key)
{
  return encrypt(msg, key);
}

std::string decryptScore(std::string &msg, std::string &key)
{
  return decrypt(msg, key);
}

void initLeaderboard(SDL_Renderer *renderer, SDL_Color colors[], SDL_Event event)
{

  std::vector<SDL_Texture *> textVectBall;
  std::vector<SDL_Texture *> textVectFlag;
  std::vector<std::string> ballScores;
  std::vector<std::string> flagScores;
  std::string key = MY_SECRET;
  std::string msg;
  int exit_requested = 0;
  int ballLength = 0;
  int flagLength = 0;
  nlohmann::json jfile;

  SDL_Texture *bg_tex = NULL;

  SDL_Rect ballScore_pos = {375, 125, 0, 0}, flagScore_pos = {893, 125, 0, 0}, bg_pos = {0, 0, 0, 0};

  // load font from romfs
  TTF_Font *font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);

  // render sprites
  SDL_Surface *bg = IMG_Load("data/leaderboard_bg.png"); // 1280 x 720 image
  if (bg)
  {
    bg_pos.w = bg->w;
    bg_pos.h = bg->h;
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg);
    SDL_FreeSurface(bg);
  }

  try
  {
    std::ifstream file(scorePath);
    std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();
    jfile = nlohmann::json::parse(content);
  }
  catch (const std::exception &exc)
  {
    std::cout << exc.what() << std::endl;
    return;
  }

  ballLength = jfile["Ball"].size();
  flagLength = jfile["Flagman"].size();

  // Fill Scores Vectors
  for (int i = 0; i < ballLength; i++)
  {
    msg = jfile["Ball"][i];
    ballScores.push_back(decryptScore(msg, key));
  }
  for (int i = 0; i < flagLength; i++)
  {
    msg = jfile["Flagman"][i];
    flagScores.push_back(decryptScore(msg, key));
  }

  // Sort descending
  std::sort(ballScores.begin(), ballScores.end(), [](std::string a, std::string b) {
    return std::stoi(a) > std::stoi(b);
  });
  std::sort(flagScores.begin(), flagScores.end(), [](std::string a, std::string b) {
    return std::stoi(a) > std::stoi(b);
  });

  if (ballLength > 10)
  {
    ballLength = 10;
  }

  if (flagLength > 10)
  {
    flagLength = 10;
  }

  //Initialize textures
  for (int i = 0; i < ballLength; i++)
  {
    ballScore_pos.y = 125 + i * 54;
    textVectBall.push_back(renderText(renderer, ballScores[i].c_str(), font, colors[1], &ballScore_pos));
  }

  for (int i = 0; i < flagLength; i++)
  {
    flagScore_pos.y = 125 + i * 54;
    textVectFlag.push_back(renderText(renderer, flagScores[i].c_str(), font, colors[1], &flagScore_pos));
  }

  while (!exit_requested)
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

    SDL_RenderCopy(renderer, bg_tex, NULL, &bg_pos);

    for (int i = 0; i < flagLength; i++)
    {
      flagScore_pos.y = 125 + i * 54;
      SDL_RenderCopy(renderer, textVectFlag[i], NULL, &flagScore_pos);
    }

    for (int i = 0; i < ballLength; i++)
    {
      ballScore_pos.y = 125 + i * 54;
      SDL_RenderCopy(renderer, textVectBall[i], NULL, &ballScore_pos);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(25);
  }
}
