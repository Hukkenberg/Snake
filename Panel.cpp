#include "Panel.h"
#include "SDL_utils.h"
#include <SDL_image.h>

Panel::Panel(SDL_Renderer* renderer_) : renderer(renderer_)
{
    loadPanel();
}

Panel::~Panel()
{
    for (SDL_Texture* texture : items)
        SDL_DestroyTexture(texture);
}

SDL_Texture* Panel::loadPanelTexture(std::string path)
{
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
        logSDLError(std::cout, "Unable to load image " + path + " SDL_image Error: " + IMG_GetError());
    else
    {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
            logSDLError(std::cout, "Unable to create texture from " + path + " SDL Error: " + SDL_GetError());
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void Panel::loadPanel()
{
    items[SCORE] = loadPanelTexture("score.png");
    items[HIGH_SCORE] = loadPanelTexture("high_score.png");
}
