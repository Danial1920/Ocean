#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Ocean.h"
#include "EntityType.h"

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    int oceanWidth = 80;
    int oceanHeight = 40;
    const int cellSize = 15;

    int windowWidth = oceanWidth * cellSize;
    int windowHeight = oceanHeight * cellSize + 50; 

    SDL_Window* window = SDL_CreateWindow(
        "Ocean Simulation (SDL2)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = nullptr;
    const char* fontPaths[] = {
        "arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };

    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, 20);
        if (font != nullptr) {
            std::cout << "Successfully loaded font from: " << path << std::endl;
            break;
        }
    }

    if (font == nullptr) {
        std::cerr << "Fatal: Could not load Arial.ttf from any known path. TTF_Error: " << TTF_GetError() << std::endl;
        std::cerr << "Please ensure Arial.ttf is in the same directory as the executable, or available in system font paths." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Ocean ocean(oceanWidth, oceanHeight);
    ocean.randomFill(oceanWidth * oceanHeight / 10,
                     oceanWidth * oceanHeight / 50,
                     oceanWidth * oceanHeight / 150);

    Uint32 lastTickTime = SDL_GetTicks();
    const float tickIntervalMs = 75.0f;
    long long ticksCount = 0;

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quit = true;
                }
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTickTime >= tickIntervalMs) {
            ocean.tick();
            ticksCount++;
            lastTickTime = currentTime;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255); 
        SDL_RenderClear(renderer);

        for (int y = 0; y < ocean.getHeight(); ++y) {
            for (int x = 0; x < ocean.getWidth(); ++x) {
                SDL_Rect cellRect = {x * cellSize, y * cellSize, cellSize, cellSize};

                switch (ocean.getCellType(x, y)) {
                    case EntityType::Sand:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 70, 255); 
                        break;
                    case EntityType::Algae:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
                        break;
                    case EntityType::HerbivoreFish:
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); 
                        break;
                    case EntityType::PredatorFish:
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
                        break;
                }
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }

        std::string stats = "Tick: " + std::to_string(ticksCount) +
                            "   Algae: " + std::to_string(ocean.countEntities(EntityType::Algae)) +
                            "   Herbivores: " + std::to_string(ocean.countEntities(EntityType::HerbivoreFish)) +
                            "   Predators: " + std::to_string(ocean.countEntities(EntityType::PredatorFish));

        SDL_Color textColor = {255, 255, 255, 255}; 
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, stats.c_str(), textColor);
        if (textSurface == nullptr) {
            std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        } else {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture == nullptr) {
                std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
            } else {
                SDL_Rect textRect = {10, windowHeight - 40, textSurface->w, textSurface->h}; 
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(renderer); 
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}