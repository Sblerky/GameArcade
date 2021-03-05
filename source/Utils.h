// some switch buttons
#define JOY_A 0
#define JOY_B 1
#define JOY_X 2
#define JOY_Y 3
#define JOY_PLUS 10
#define JOY_MINUS 11
#define JOY_LEFT 12
#define JOY_UP 13
#define JOY_RIGHT 14
#define JOY_DOWN 15
#define LSTICK_LEFT 16
#define LSTICK_UP 17
#define LSTICK_RIGHT 18
#define LSTICK_DOWN 19

#define SCREEN_W 1280
#define SCREEN_H 720

#define MY_SECRET "XXXXXXX"

#include <fstream>
#include <iomanip>
#include <vector>
#include "json.hpp"

static const std::string appDir = "sdmc:/switch/Sblerky";
static const std::string scorePath = appDir + "/scores.json";

SDL_Texture *renderText(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color, SDL_Rect *rect);
int randRange(int min, int max);
void saveScore(int score, std::string name);