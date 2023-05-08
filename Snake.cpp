#include <iostream>
#include <SDL.h>
#include <time.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <string>

using namespace std;
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 2150 / SCREEN_FPS;
const int MOVE_SPEED = 10;

TTF_Font* Font = NULL;
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Renderer* renderer = NULL;

int close = 0;
int score = 0;
int bodys = 0;
int Exit  = 0;

class head {
public:
    int posx, posy, velx, vely, direction;
        head(int x, int y){
            posx = x;
            posy = y;
            velx = 0;
            vely = 0;
            direction = 0;
        }
        void draw(SDL_Renderer *renderer) {
            SDL_Rect fillRect = { posx, posy, 10, 10};
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &fillRect);
            if ((!(posx + velx > SCREEN_WIDTH - 10) && !(posx + velx < 0)) && (!(posy + vely > SCREEN_HEIGHT - 10) && !(posy + vely < 0))) {
                posx += velx;
                posy += vely;
            }
            else {
                close = 1;
            }
        }

};

class fruit {
public:
    int posx, posy;
    fruit(int x, int y) {
        posx = x;
        posy = y;
    }
    void draw(SDL_Renderer* renderer) {
        SDL_Rect fillRect = { posx, posy, 10, 10 };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
        SDL_RenderFillRect(renderer, &fillRect);
    }
    
};

class body {
    public:
        int posx, posy, offsetx, offsety;
        body(int x, int y, int ox, int oy) {
            posx = x;
            posy = y;
            offsetx = ox;
            offsety = oy;
        }
        void draw(SDL_Renderer* renderer) {
            //posx -= offsetx;
            //posy -= offsety;
            SDL_Rect fillRect = { posx, posy, 10, 10 };
            SDL_SetRenderDrawColor(renderer, 0, 250, 0, 255);
            SDL_RenderFillRect(renderer, &fillRect);
        }
};

class Timer {
public:
    unsigned int StartTick, PausedTick;

    bool Paused, Started;

    Timer() {
        StartTick = 0;
        PausedTick = 0;
        Paused = false;
        Started = false;
    }
    void start() {
        Started = true;
        Paused = false;
        StartTick = SDL_GetTicks();
        PausedTick = 0;
    }
    void stop() {
        Started = false;
        Paused = false;
        StartTick = 0;
        PausedTick = 0;
    }
    void pause() {
        if (Started && !Paused) {
            Paused = true;
            PausedTick = SDL_GetTicks() - StartTick;
            StartTick = 0;
        }
    }
    void unpause() {
        if (Started && Paused) {
            Paused = false;
            StartTick = SDL_GetTicks() - PausedTick;
            PausedTick = 0;
        }
    }
    Uint32 getTicks() {
        Uint32 time = 0;
        if (Started) {
            if (Paused) {
                time = PausedTick;
            }
            else {
                time = SDL_GetTicks() - StartTick;
            }
        }
        return time;
    }
};

int WinMain(int argc, char* argv[])
{   srand(time(0));
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }
    head snake_head(50 + (rand() % (SCREEN_WIDTH - 50) / 10) * 10, 50 + (rand() % (SCREEN_HEIGHT - 50) / 10) * 10);
    fruit snake_fruit(((rand() % SCREEN_WIDTH) / 10) * 10, ((rand() % SCREEN_HEIGHT) / 10) * 10);
    body* snake_body = (body*)malloc(sizeof(body) * (bodys + 1));
    body* temp_body;

    Timer fpsTimer;
    Timer capFrames;

    int countedFrames = 0;
    fpsTimer.start();
    float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {   window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        cout << "Could not create renderer! SDL error" << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    TTF_Font* Font = TTF_OpenFont("roboto.ttf", 28);
    if (!Font)
        cout << "Couldn't find/init open ttf font." << endl;
    SDL_Color White = { 255, 255, 255, 0 };
    string score_text = "Score: " + to_string(score);
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, score_text.c_str(), White);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;
    SDL_FreeSurface(surfaceMessage);
    SDL_Rect renderQuad = { 0, 0, text_width, text_height };
    SDL_Event event;
    int mousepos_x = -1;
    int mousepos_y = -1;
    while (!Exit) {
        while (!close) {

            string score_text = "Score: " + to_string(score);
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, score_text.c_str(), White);
            SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
            text_width = surfaceMessage->w;
            text_height = surfaceMessage->h;
            SDL_FreeSurface(surfaceMessage);
            SDL_Rect renderQuad = { 0, 0, text_width, text_height };

            SDL_SetRenderDrawColor(renderer, 60, 70, 20, 30);
            SDL_RenderClear(renderer);
            capFrames.start();
            avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    close = 1;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake_head.direction != 2) {
                            snake_head.vely = -MOVE_SPEED;
                            snake_head.velx = 0;
                            snake_head.direction = 1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (snake_head.direction != 1) {
                            snake_head.vely = MOVE_SPEED;
                            snake_head.velx = 0;
                            snake_head.direction = 2;
                        }
                        break;
                    case SDLK_LEFT:
                        if (snake_head.direction != 4) {
                            snake_head.vely = 0;
                            snake_head.velx = -MOVE_SPEED;
                            snake_head.direction = 3;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snake_head.direction != 3) {
                            snake_head.vely = 0;
                            snake_head.velx = MOVE_SPEED;
                            snake_head.direction = 4;
                        }
                        break;
                    }
                    break;
                }
            }
            if (bodys) {
                for (int i = bodys - 1; i >= 1; i--) {
                    snake_body[i].posx = snake_body[i - 1].posx;
                    snake_body[i].posy = snake_body[i - 1].posy;
                    snake_body[i].draw(renderer);
                }
                snake_body[0].posx = snake_head.posx;
                snake_body[0].posy = snake_head.posy;
                snake_body[0].draw(renderer);
            }
            snake_head.draw(renderer);
            snake_fruit.draw(renderer);

            SDL_RenderCopy(renderer, Message, NULL, &renderQuad);
            SDL_RenderPresent(renderer);

            if ((snake_head.posx == snake_fruit.posx) && (snake_head.posy == snake_fruit.posy)) {
                snake_fruit.posx = ((rand() % SCREEN_WIDTH) / 10) * 10;
                snake_fruit.posy = ((rand() % SCREEN_HEIGHT) / 10) * 10;
                temp_body = (body*)realloc(snake_body, sizeof(body) * (bodys + 2));
                if (temp_body != NULL) {
                    snake_body = temp_body;
                }
                else {
                    cout << "Realloc failed.";
                    return -1;
                }
                score += 10;

                switch (snake_head.direction) {
                case 1: {
                    snake_body[bodys] = body(snake_head.posx, snake_head.posy + 10 * (bodys + 1), 0, -MOVE_SPEED);
                    bodys++;
                    break;
                }
                case 2: {
                    snake_body[bodys] = body(snake_head.posx, snake_head.posy - 10 * (bodys + 1), 0, +MOVE_SPEED);
                    bodys++;
                    break;
                }
                case 3: {
                    snake_body[bodys] = body(snake_head.posx + 10 * (bodys + 1), snake_head.posy, -MOVE_SPEED, 0);
                    bodys++;
                    break;
                }
                case 4: {
                    snake_body[bodys] = body(snake_head.posx - 10 * (bodys + 1), snake_head.posy, +MOVE_SPEED, 0);
                    bodys++;
                    break;
                }
                }
            }

            for (int i = 1; i < bodys - 1; i++) {
                if ((snake_head.posx == snake_body[i].posx) && (snake_head.posy == snake_body[i].posy)) {
                    close = 1;
                }
            }

            ++countedFrames;

            int frameTicks = capFrames.getTicks();
            if (frameTicks < SCREEN_TICKS_PER_FRAME) {
                SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
            }
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONUP) {
                SDL_GetMouseState(&mousepos_x, &mousepos_y);
                if ((mousepos_x >= SCREEN_WIDTH / 2 - 10 * SCREEN_WIDTH / 100) && (mousepos_x <= SCREEN_WIDTH / 2 - 10 * SCREEN_WIDTH / 100 + 70))
                {
                    if ((mousepos_y <= SCREEN_HEIGHT / 2 + 37) && (mousepos_y >= SCREEN_HEIGHT / 2 + 2)) {
                        snake_head.posx = 50 + (rand() % (SCREEN_WIDTH - 50) / 10) * 10;
                        snake_head.posy = 50 + (rand() % (SCREEN_HEIGHT - 50) / 10) * 10;
                        snake_head.direction = 0;
                        snake_head.velx = 0;
                        snake_head.vely = 0;
                        snake_fruit.posx = ((rand() % SCREEN_WIDTH) / 10) * 10;
                        snake_fruit.posy = ((rand() % SCREEN_HEIGHT) / 10) * 10;
                        close = 0;
                        bodys = 0;
                        score = 0;
                        snake_body = (body*)malloc(sizeof(body) * (bodys + 1));

                    }
                }
                if ((mousepos_x >= SCREEN_WIDTH / 2 + 10 * SCREEN_WIDTH / 100) && (mousepos_x <= SCREEN_WIDTH / 2 + 10 * SCREEN_WIDTH / 100 + 70))
                {
                    if ((mousepos_y >= SCREEN_HEIGHT / 2 + 2) && (mousepos_y <= SCREEN_HEIGHT / 2 + 37))
                        Exit = 1;
                }
            }

            }
        SDL_SetRenderDrawColor(renderer, 60, 70, 20, 30);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, Message, NULL, &renderQuad);

        SDL_Surface* surfaceMessage_Retry = TTF_RenderText_Solid(Font, "Retry", White);
        SDL_Surface* surfaceMessage_Exit = TTF_RenderText_Solid(Font, "Exit", White);
        SDL_Texture* Retry_Text = SDL_CreateTextureFromSurface(renderer, surfaceMessage_Retry);
        SDL_Texture* Exit_Text = SDL_CreateTextureFromSurface(renderer, surfaceMessage_Exit);
        int text_width_Retry = surfaceMessage_Retry->w;
        int text_height_Retry = surfaceMessage_Retry->h;
        int text_width_Exit = surfaceMessage_Exit->w;
        int text_height_Exit = surfaceMessage_Exit->h;
        SDL_FreeSurface(surfaceMessage_Retry);
        SDL_FreeSurface(surfaceMessage_Exit);
        SDL_Rect render_Retry = { SCREEN_WIDTH / 2 - 10 * SCREEN_WIDTH / 100, SCREEN_HEIGHT / 2, text_width_Retry, text_height_Retry };
        SDL_Rect render_Exit = { SCREEN_WIDTH / 2 + 10 * SCREEN_WIDTH / 100, SCREEN_HEIGHT / 2, text_width_Exit, text_height_Exit };

        SDL_Rect fillRect1 = { SCREEN_WIDTH / 2 - 10 * SCREEN_WIDTH / 100, SCREEN_HEIGHT / 2 + 2, 70, 35 };
        SDL_Rect fillRect2 = { SCREEN_WIDTH / 2 + 10 * SCREEN_WIDTH / 100, SCREEN_HEIGHT / 2 + 2, 50, 35 };
        SDL_SetRenderDrawColor(renderer, 100, 250, 100, 255);
        SDL_RenderFillRect(renderer, &fillRect1);
        SDL_RenderFillRect(renderer, &fillRect2);
        SDL_RenderCopy(renderer, Retry_Text, NULL, &render_Retry);
        SDL_RenderCopy(renderer, Exit_Text, NULL, &render_Exit);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(Message);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
