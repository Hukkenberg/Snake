#ifndef PANEL_H
#define PANEL_H

#include <vector>
#include <string>
#include <SDL.h>

enum StartID
{
    DIFFICULTY,
    HIGH_SCORE,
    SCORE,
    ITEM_COUNT,
};

class Panel
{
    SDL_Texture* items[ITEM_COUNT];
    SDL_Renderer* renderer;
    SDL_Texture* loadPanelTexture(std::string path);
public:
    Panel(SDL_Renderer* renderer_);
    ~Panel();

    void loadPanel();
    SDL_Texture* getImage(StartID id) const { return items[id]; }
};

#endif
#pragma once
