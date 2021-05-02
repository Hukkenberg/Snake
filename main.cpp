#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>

#include "SDL_utils.h"
#include "Game.h"
#include "Gallery.h"
#include <SDL_image.h>

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const char WINDOW_TITLE[] = "Snake Game";

const int BOARD_WIDTH = 30;
const int BOARD_HEIGHT = 20;
const int CELL_SIZE = 30;

const SDL_Color BOARD_COLOR = {0, 0, 0};
const SDL_Color LINE_COLOR = {128, 128, 128};

double STEP_DELAY;
#define CLOCK_NOW chrono::system_clock::now
typedef chrono::duration<double> ElapsedTime;

SDL_Surface* loadSplashScreen(std::string path, SDL_Surface* gScreenSurface);
void renderGamePlay(SDL_Renderer*, const Game& game);
void renderGameOver(SDL_Renderer* renderer, const Game& game);
void interpretEvent(SDL_Event e, Game& game);
int updateRankingTable(const Game& game, int a);
void show_score(const Game& game, int a);
bool play_again();
double set_speed(double b, SDL_Event choice, SDL_Window* window);

Gallery* gallery = nullptr; // global picture manager

int high_score = 0;

int main(int argc, char* argv[])
{
    srand(time(0));
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    waitUntilKeyPressed();
    gallery = new Gallery(renderer);
    Game game(BOARD_WIDTH, BOARD_HEIGHT);
    SDL_Event e{};
 
    STEP_DELAY = set_speed(STEP_DELAY, e, window);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    surface = loadSplashScreen("introduction.png", surface);
    auto start = CLOCK_NOW();
    renderGamePlay(renderer, game);

    while (game.isGameRunning()) 
    {
        while (SDL_PollEvent(&e)) 
        {
            interpretEvent(e, game);
        }

        auto end = CLOCK_NOW();
        ElapsedTime elapsed = end-start;
        if (elapsed.count() > STEP_DELAY) 
        {
            game.nextStep();
            renderGamePlay(renderer, game);
            start = end;
        }
        SDL_Delay(1);
    }
    renderGameOver(renderer, game);
    high_score = updateRankingTable(game, high_score);
    show_score(game, high_score);

    delete gallery;
    quitSDL(window, renderer);

    /**playing next time**/
    bool new_game = play_again();
    while (new_game == true)
    {
        srand(time(0));
        SDL_Window* window;
        SDL_Renderer* renderer;
        initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
        gallery = new Gallery(renderer);
        Game game(BOARD_WIDTH, BOARD_HEIGHT);
        SDL_Event e{};

        STEP_DELAY = set_speed(STEP_DELAY, e, window);
        SDL_Surface* surface = SDL_GetWindowSurface(window);
        surface = loadSplashScreen("introduction.png", surface);
        waitUntilKeyPressed();
        auto start = CLOCK_NOW();
        renderGamePlay(renderer, game);

        while (game.isGameRunning()) 
        {
            while (SDL_PollEvent(&e)) 
            {
                interpretEvent(e, game);
            }

            auto end = CLOCK_NOW();
            ElapsedTime elapsed = end - start;
            if (elapsed.count() > STEP_DELAY) 
            {
                game.nextStep();
                renderGamePlay(renderer, game);
                start = end;
            }
            SDL_Delay(1);
        }
        renderGameOver(renderer, game);
        high_score = updateRankingTable(game, high_score);
        show_score(game, high_score);

        delete gallery;
        quitSDL(window, renderer);

        new_game = play_again();
    }
    return 0;
}

float generateRandomNumber()
{
    return (float) rand() / RAND_MAX;
}

SDL_Surface* loadSplashScreen(std::string path, SDL_Surface *gScreenSurface)
{
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

void drawCell(SDL_Renderer* renderer, int left, int top, Position pos, SDL_Texture* texture)
{
	SDL_Rect cell; 
	cell.x = left + pos.x * CELL_SIZE + 5;
	cell.y = top + pos.y * CELL_SIZE + 5;
	cell.w = CELL_SIZE-10;
	cell.h = CELL_SIZE-10;
	SDL_RenderCopy(renderer, texture, NULL, &cell);
}

void drawCherry(SDL_Renderer* renderer, int left, int top, Position pos)
{	
    drawCell(renderer, left, top, pos, gallery->getImage(PIC_CHERRY));
}

void drawSnake(SDL_Renderer* renderer, int left, int top, vector<Position> pos)
{
	// snake's head
	drawCell(renderer, left, top, pos[pos.size()-1], gallery->getImage(PIC_SNAKE_HEAD));
	
	// snake's body
    for (int i = pos.size() - 2; i >= 0; i--) {
        SDL_Texture* texture = gallery->getImage(pos[i].y == pos[i+1].y ? PIC_SNAKE_HORIZONTAL : PIC_SNAKE_VERTICAL);
        drawCell(renderer, left, top, pos[i], texture);
    }
}

void renderGamePlay(SDL_Renderer* renderer, const Game& game)
{
    int top = 0, left = 0;
    SDL_SetRenderDrawColor(renderer, BOARD_COLOR.r, BOARD_COLOR.g, BOARD_COLOR.b, 0);
    SDL_RenderClear(renderer);

    drawCherry(renderer, left, top, game.getCherryPosition());
    drawSnake(renderer, left, top, game.getSnakePositions());
    
    SDL_RenderPresent(renderer);
}

void renderGameOver(SDL_Renderer* renderer, const Game& game)
{
    cout << "GAME OVER!" << endl;
    waitUntilKeyPressed();
}

void interpretEvent(SDL_Event e, Game& game)
{
    if (e.type == SDL_KEYUP) 
    {
        switch (e.key.keysym.sym) 
        {
        	case SDLK_UP: game.processUserInput(UP); break;
        	case SDLK_DOWN: game.processUserInput(DOWN); break;
        	case SDLK_LEFT: game.processUserInput(LEFT); break;
        	case SDLK_RIGHT: game.processUserInput(RIGHT); break;
        }
    }
}

int updateRankingTable(const Game& game, int a)
{
    int score = game.getScore();
    if (score > a)
        a = score;
    return a;
}

bool play_again()
{
    bool again;
    cout << "Wanna play? (Y/N) " << endl;
    char c;
    cin >> c;
    switch (c)
    {
        case 'Y':
            return true;
        case 'N':
            return false;
        default:
            cout << "Invalid answer.";
            play_again();
    }
}

void show_score(const Game& game, int a)
{
    int score = game.getScore();
    cout << "SCORE: " << score << endl;
    cout << "HIGH SCORE: " << a << endl;
}

double set_speed(double b, SDL_Event choice, SDL_Window *window)
{
    SDL_Surface* welcome = SDL_GetWindowSurface(window);
    welcome = loadSplashScreen("set_difficulty.png", welcome);
    
    if (choice.type == SDL_KEYDOWN)
    {
        switch (choice.key.keysym.sym)
        {
            case SDLK_1:
            {
                b = 0.3;
                break;
            }
            case SDLK_2:
            {
                b = 0.2;
                break;
            }
            case SDLK_3:
            {
                b = 0.1;
                break;
            }
            default:
            {
                return 0;
            }
        }
    }

    return b;
}