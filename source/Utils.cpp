
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <list>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Utils.h"
#include "Leaderboard.h"

SDL_Texture *renderText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect)
{
  SDL_Surface *surface;
  SDL_Texture *texture;

  surface = TTF_RenderText_Solid(font, text, color);
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  rect->w = surface->w;
  rect->h = surface->h;

  SDL_FreeSurface(surface);

  return texture;
}

int randRange(int min, int max)
{
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void saveScore(int score, std::string name)
{
  std::ifstream file(scorePath);
  std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  file.close();
  nlohmann::json jfile = nlohmann::json::parse(content);
  std::string key = MY_SECRET;
  std::string msg = std::to_string(score);
  jfile[name].push_back(encryptScore(msg, key));
  std::ofstream inputFileBis(scorePath);
  inputFileBis << std::setw(4) << jfile << std::endl;
  inputFileBis.close();
}