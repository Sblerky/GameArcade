#include <time.h>
#include <unistd.h>
#include <iostream>
#include <list>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include "Ball.h"
#include "Utils.h"

void initGameBall(SDL_Renderer *renderer, SDL_Color colors[], SDL_Event event)
{
  // Game variables

  std::vector<std::vector<int>> outerBall{
      {366, 476},
      {364, 382},
      {376, 285},
      {423, 200},
      {495, 138},
      {584, 103},
      {674, 99},
      {769, 143},
      {834, 198},
      {890, 280},
      {886, 383},
      {882, 477}};
  std::vector<std::vector<int>> innerBall{
      {438, 470},
      {433, 387},
      {449, 298},
      {503, 227},
      {589, 191},
      {675, 189},
      {759, 228},
      {816, 304},
      {823, 391},
      {816, 470}};
  int ballmanStatus = 1;
  int resetDelay = 26;
  int delayInnerBall = resetDelay;
  int delayOuterBall = resetDelay + resetDelay / 2;
  int indiceInnerBall = 4;
  int indiceOuterBall = 5;
  int score = 0;
  int tmpScore = 0;
  bool innerGoRight;
  bool startingScreen = true;
  bool gameOverScreen = false;
  bool outerGoRight;
  bool hasChangedDelay = false;
  bool ballFallenRight = false;
  bool hasTouchedInnerBall = false;
  bool hasTouchedOuterBall = false;

  if (randRange(0, 100) > 50)
  { //Greater range for better results
    innerGoRight = true;
  }
  else
  {
    innerGoRight = false;
  }

  if (randRange(0, 100) > 50)
  {
    outerGoRight = true;
  }
  else
  {
    outerGoRight = false;
  }

  // SDL Stuff

  SDL_Texture *ballmanMiddle_tex = NULL, *ballmanLeft_tex = NULL, *ballmanRight_tex = NULL,
              *ballmanMiddleDead_tex = NULL, *ballmanLeftDead_tex = NULL, *ballmanRightDead_tex = NULL,
              *ball_tex = NULL, *bg_tex = NULL, *dead_tex = NULL, *score_tex = NULL,
              *intro_tex = NULL, *gameover_tex = NULL, *ballbroke_tex = NULL;

  SDL_Rect bg_pos = {0, 0, 0, 0}, ballman_pos = {350, 431, 100, 100},
           ball_pos = {0, 0, 0, 0}, ballbroke_pos = {0, 0, 0, 0};
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

  int exit_requested = 0;

  // load font from romfs
  TTF_Font *font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);

  SDL_Rect score_rect = {910, 627, 0, 0};
  score_tex = renderText(renderer, "Score : 0", font, colors[1], &score_rect);

  // render sprites
  SDL_Surface *bg = IMG_Load("data/ball_main.png"); // 1280 x 720 image
  if (bg)
  {
    bg_pos.w = bg->w;
    bg_pos.h = bg->h;
    bg_tex = SDL_CreateTextureFromSurface(renderer, bg);
    SDL_FreeSurface(bg);
  }

  SDL_Surface *ballmanMiddle = IMG_Load("data/ballman_middle.png");
  if (ballmanMiddle)
  {
    ballman_pos.w = ballmanMiddle->w;
    ballman_pos.h = ballmanMiddle->h;
    ballmanMiddle_tex = SDL_CreateTextureFromSurface(renderer, ballmanMiddle);
    SDL_FreeSurface(ballmanMiddle);
  }

  SDL_Surface *ballmanRight = IMG_Load("data/ballman_right.png");
  if (ballmanRight)
  {
    ballman_pos.w = ballmanRight->w;
    ballman_pos.h = ballmanRight->h;
    ballmanRight_tex = SDL_CreateTextureFromSurface(renderer, ballmanRight);
    SDL_FreeSurface(ballmanRight);
  }

  SDL_Surface *ballmanLeft = IMG_Load("data/ballman_left.png");
  if (ballmanLeft)
  {
    ballman_pos.w = ballmanLeft->w;
    ballman_pos.h = ballmanLeft->h;
    ballmanLeft_tex = SDL_CreateTextureFromSurface(renderer, ballmanLeft);
    SDL_FreeSurface(ballmanLeft);
  }

  SDL_Surface *ballmanMiddleDead = IMG_Load("data/ballman_middle_dead.png");
  if (ballmanMiddleDead)
  {
    ballman_pos.w = ballmanMiddleDead->w;
    ballman_pos.h = ballmanMiddleDead->h;
    ballmanMiddleDead_tex = SDL_CreateTextureFromSurface(renderer, ballmanMiddleDead);
    SDL_FreeSurface(ballmanMiddleDead);
  }

  SDL_Surface *ballmanRightDead = IMG_Load("data/ballman_right_dead.png");
  if (ballmanRightDead)
  {
    ballmanRightDead_tex = SDL_CreateTextureFromSurface(renderer, ballmanRightDead);
    SDL_FreeSurface(ballmanRightDead);
  }

  SDL_Surface *ballmanLeftDead = IMG_Load("data/ballman_left_dead.png");
  if (ballmanLeftDead)
  {
    ballmanLeftDead_tex = SDL_CreateTextureFromSurface(renderer, ballmanLeftDead);
    SDL_FreeSurface(ballmanLeftDead);
  }

  SDL_Surface *introS = IMG_Load("data/ball_intro.png");
  if (introS)
  {
    intro_tex = SDL_CreateTextureFromSurface(renderer, introS);
    SDL_FreeSurface(introS);
  }

  SDL_Surface *gameOver = IMG_Load("data/ball_gameover.png");
  if (gameOver)
  {
    gameover_tex = SDL_CreateTextureFromSurface(renderer, gameOver);
    SDL_FreeSurface(gameOver);
  }

  SDL_Surface *ballBroke = IMG_Load("data/ball_broke.png");
  if (ballBroke)
  {
    ballbroke_pos.w = ballBroke->w;
    ballbroke_pos.h = ballBroke->h;
    ballbroke_tex = SDL_CreateTextureFromSurface(renderer, ballBroke);
    SDL_FreeSurface(ballBroke);
  }

  SDL_Surface *ball = IMG_Load("data/ball.png");
  if (ball)
  {
    ball_pos.w = ball->w;
    ball_pos.h = ball->h;
    ball_tex = SDL_CreateTextureFromSurface(renderer, ball);
    SDL_FreeSurface(ball);
  }
  Mix_Music *music = NULL;
  music = Mix_LoadMUS("data/ball.mp3");
  Mix_PlayMusic(music, -1);

  Mix_Chunk *sound[4] = {NULL};
  sound[0] = Mix_LoadWAV("data/ball.wav");
  sound[1] = Mix_LoadWAV("data/flagdead.wav");

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
    while (!exit_requested && !gameOverScreen)
    {
      while (SDL_PollEvent(&event) && !exit_requested)
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
          case (LSTICK_RIGHT):
          case (JOY_RIGHT):
            if (ballmanStatus < 2)
            {
              ballmanStatus++;
            }
            break;
          case (LSTICK_LEFT):
          case (JOY_LEFT):
            if (ballmanStatus > 0)
            {
              ballmanStatus--;
            }
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, bg_tex, NULL, &bg_pos);

      //Render Ballman
      switch (ballmanStatus)
      {
      case (0):
        SDL_RenderCopy(renderer, ballmanLeft_tex, NULL, &ballman_pos);
        break;

      case (1):
        SDL_RenderCopy(renderer, ballmanMiddle_tex, NULL, &ballman_pos);
        break;

      case (2):
        SDL_RenderCopy(renderer, ballmanRight_tex, NULL, &ballman_pos);
        break;
      }

      // Check if hand under ball
      if (!hasTouchedInnerBall && (indiceInnerBall == innerBall.size() - 1 || indiceInnerBall == 0) && ballmanStatus == 1)
      {
        hasTouchedInnerBall = true;
        Mix_PlayChannel(-1, sound[0], 0);
      }

      if (!hasTouchedOuterBall && indiceOuterBall == outerBall.size() - 1 && ballmanStatus == 2)
      {
        hasTouchedOuterBall = true;
        Mix_PlayChannel(-1, sound[0], 0);
      }

      if (!hasTouchedOuterBall && indiceOuterBall == 0 && ballmanStatus == 0)
      {
        hasTouchedOuterBall = true;
        Mix_PlayChannel(-1, sound[0], 0);
      }

      if (delayInnerBall > 0)
      {
        delayInnerBall--;
      }
      else
      {

        switch (innerGoRight)
        {
        case (true):
          if (indiceInnerBall < innerBall.size() - 1)
          {
            indiceInnerBall++;
          }
          else
          {
            if (hasTouchedInnerBall)
            {

              innerGoRight = false;
              indiceInnerBall--;
              score++;
              hasTouchedInnerBall = false;
            }
            else
            {
              gameOverScreen = true;
              Mix_PlayChannel(-1, sound[1], 0);
              ballFallenRight = true;
            }
          }
          break;

        case (false):
          if (indiceInnerBall > 0)
          {
            indiceInnerBall--;
          }
          else
          {
            if (hasTouchedInnerBall)
            {
              innerGoRight = true;
              indiceInnerBall++;
              score++;
              hasTouchedInnerBall = false;
            }
            else
            {
              gameOverScreen = true;
              Mix_PlayChannel(-1, sound[1], 0);
              ballFallenRight = false;
            }
          }
          break;
        }

        delayInnerBall = resetDelay;
        delayOuterBall = resetDelay / 2;
      }

      if (delayOuterBall > 0)
      {
        delayOuterBall--;
      }
      else
      {
        switch (outerGoRight)
        {
        case (true):
          if (indiceOuterBall < outerBall.size() - 1)
          {
            indiceOuterBall++;
          }
          else
          {
            if (hasTouchedOuterBall)
            {
              outerGoRight = false;
              indiceOuterBall--;
              score++;
              hasTouchedOuterBall = false;
            }
            else
            {
              gameOverScreen = true;
              Mix_PlayChannel(-1, sound[1], 0);
              ballFallenRight = true;
            }
          }
          break;

        case (false):
          if (indiceOuterBall > 0)
          {
            indiceOuterBall--;
          }
          else
          {
            if (hasTouchedOuterBall)
            {
              outerGoRight = true;
              indiceOuterBall++;
              score++;
              hasTouchedOuterBall = false;
            }
            else
            {
              gameOverScreen = true;
              Mix_PlayChannel(-1, sound[1], 0);
              ballFallenRight = false;
            }
          }
          break;
        }
        delayOuterBall = resetDelay;
        delayInnerBall = resetDelay / 2;
      }

      if (score % 10 == 0 && !hasChangedDelay && score < 101)
      {
        resetDelay = resetDelay - 2;
        hasChangedDelay = true;
      }
      if (score % 10 == 1)
      {
        hasChangedDelay = false;
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

      //Render balls
      ball_pos.x = innerBall[indiceInnerBall][0];
      ball_pos.y = innerBall[indiceInnerBall][1];
      SDL_RenderCopy(renderer, ball_tex, NULL, &ball_pos);

      ball_pos.x = outerBall[indiceOuterBall][0];
      ball_pos.y = outerBall[indiceOuterBall][1];
      SDL_RenderCopy(renderer, ball_tex, NULL, &ball_pos);

      SDL_RenderPresent(renderer);
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
            saveScore(score, "Ball");
            score = 0;
            gameOverScreen = false;
            ballmanStatus = 1;
            resetDelay = 26;
            delayInnerBall = resetDelay;
            delayOuterBall = resetDelay + resetDelay / 2;
            indiceInnerBall = 4;
            indiceOuterBall = 5;
            ballFallenRight = false;
            hasTouchedInnerBall = false;
            hasTouchedOuterBall = false;
            break;
          }
        }
      }

      SDL_RenderCopy(renderer, gameover_tex, NULL, &bg_pos);

      //Render score
      char score_string[32];
      sprintf(score_string, "%d", score);
      char full_string[64] = "Score : ";
      strcat(full_string, score_string);
      score_tex = renderText(renderer, full_string, font, colors[1], &score_rect);
      SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);

      //Render Ballman
      switch (ballmanStatus)
      {
      case (0):
        SDL_RenderCopy(renderer, ballmanLeftDead_tex, NULL, &ballman_pos);
        break;

      case (1):
        SDL_RenderCopy(renderer, ballmanMiddleDead_tex, NULL, &ballman_pos);
        break;

      case (2):
        SDL_RenderCopy(renderer, ballmanRightDead_tex, NULL, &ballman_pos);
        break;
      }

      //Render Ball
      if (ballFallenRight)
      {
        ballbroke_pos.x = 825;
        ballbroke_pos.y = 602;
        SDL_RenderCopy(renderer, ballbroke_tex, NULL, &ballbroke_pos);
      }
      else
      {
        ballbroke_pos.x = 291;
        ballbroke_pos.y = 602;
        SDL_RenderCopy(renderer, ballbroke_tex, NULL, &ballbroke_pos);
      }

      SDL_RenderPresent(renderer);
      SDL_Delay(25);
    }
  }
  Mix_FadeOutMusic(500);
  Mix_FreeMusic(music);
  for (int snd = 0; snd < 4; snd++)
    if (sound[snd])
      Mix_FreeChunk(sound[snd]);
  // no need to keep the font loaded
  TTF_CloseFont(font);

  if (bg_tex)
    SDL_DestroyTexture(bg_tex);

  if (ballmanMiddle_tex)
    SDL_DestroyTexture(ballmanMiddle_tex);

  if (ballmanLeft_tex)
    SDL_DestroyTexture(ballmanLeft_tex);

  if (ballmanRight_tex)
    SDL_DestroyTexture(ballmanRight_tex);

  if (ballmanMiddleDead_tex)
    SDL_DestroyTexture(ballmanMiddleDead_tex);

  if (ballmanLeftDead_tex)
    SDL_DestroyTexture(ballmanLeftDead_tex);

  if (ballmanRightDead_tex)
    SDL_DestroyTexture(ballmanRightDead_tex);

  if (ball_tex)
    SDL_DestroyTexture(ball_tex);

  if (dead_tex)
    SDL_DestroyTexture(dead_tex);

  if (score_tex)
    SDL_DestroyTexture(score_tex);

  if (intro_tex)
    SDL_DestroyTexture(intro_tex);

  if (gameover_tex)
    SDL_DestroyTexture(gameover_tex);

  if (ballbroke_tex)
    SDL_DestroyTexture(ballbroke_tex);

  // Save Score
  saveScore(score, "Ball");
}