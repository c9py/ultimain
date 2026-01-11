/*
 * Ultima Engines Unified Launcher
 * 
 * This launcher provides a unified interface to:
 * - Exult (Ultima VII: The Black Gate / Serpent Isle)
 * - ScummVM Ultima8 (Ultima VIII: Pagan)
 * 
 * Copyright (C) 2026 Ultima Integration Project
 * Licensed under GPL-2.0
 */

#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Game definitions
struct GameInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string engine;
    std::string executable;
    std::string dataPath;
    bool available;
};

class UltimaLauncher {
public:
    UltimaLauncher();
    ~UltimaLauncher();
    
    bool init();
    void run();
    void shutdown();
    
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<GameInfo> games;
    int selectedGame = 0;
    bool running = true;
    
    void detectGames();
    void render();
    void handleEvents();
    void launchGame(const GameInfo& game);
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    void drawGameList();
    void drawHeader();
    void drawFooter();
};

UltimaLauncher::UltimaLauncher() {
    // Initialize game list
    games = {
        {
            "u7bg",
            "Ultima VII: The Black Gate",
            "The Avatar returns to Britannia to investigate a series of ritual murders.",
            "exult",
            "exult",
            "ultima7",
            false
        },
        {
            "u7si",
            "Ultima VII Part Two: Serpent Isle",
            "Continue the Avatar's journey to the mysterious Serpent Isle.",
            "exult",
            "exult",
            "serpentisle",
            false
        },
        {
            "u8",
            "Ultima VIII: Pagan",
            "The Avatar is banished to the dark world of Pagan.",
            "scummvm",
            "scummvm --path=",
            "ultima8",
            false
        }
    };
}

UltimaLauncher::~UltimaLauncher() {
    shutdown();
}

bool UltimaLauncher::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        "Ultima Engines Launcher",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    detectGames();
    return true;
}

void UltimaLauncher::detectGames() {
    // Check for game data directories
    std::vector<std::string> searchPaths = {
        ".",
        "./games",
        "/usr/share/games",
        "/usr/local/share/games",
        std::string(getenv("HOME") ? getenv("HOME") : "") + "/games",
        std::string(getenv("HOME") ? getenv("HOME") : "") + "/.local/share/exult",
        std::string(getenv("HOME") ? getenv("HOME") : "") + "/.local/share/scummvm"
    };
    
    for (auto& game : games) {
        for (const auto& basePath : searchPaths) {
            fs::path gamePath = fs::path(basePath) / game.dataPath;
            if (fs::exists(gamePath) && fs::is_directory(gamePath)) {
                game.available = true;
                game.dataPath = gamePath.string();
                std::cout << "Found " << game.name << " at " << gamePath << std::endl;
                break;
            }
        }
    }
}

void UltimaLauncher::render() {
    // Clear screen with dark blue background
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);
    
    drawHeader();
    drawGameList();
    drawFooter();
    
    SDL_RenderPresent(renderer);
}

void UltimaLauncher::drawHeader() {
    // Draw title bar
    SDL_FRect titleBar = {0, 0, 800, 80};
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    
    // Title would be drawn here with proper font rendering
    // For now, we'll use a placeholder
}

void UltimaLauncher::drawGameList() {
    int y = 100;
    int index = 0;
    
    for (const auto& game : games) {
        SDL_FRect gameBox = {50, (float)y, 700, 100};
        
        if (index == selectedGame) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 160, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 80, 255);
        }
        SDL_RenderFillRect(renderer, &gameBox);
        
        // Border
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
        SDL_RenderRect(renderer, &gameBox);
        
        // Availability indicator
        SDL_FRect indicator = {60, (float)(y + 40), 20, 20};
        if (game.available) {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &indicator);
        
        y += 120;
        index++;
    }
}

void UltimaLauncher::drawFooter() {
    SDL_FRect footerBar = {0, 520, 800, 80};
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderFillRect(renderer, &footerBar);
    
    // Instructions would be drawn here
}

void UltimaLauncher::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
                
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_UP:
                        if (selectedGame > 0) selectedGame--;
                        break;
                    case SDLK_DOWN:
                        if (selectedGame < (int)games.size() - 1) selectedGame++;
                        break;
                    case SDLK_RETURN:
                        if (games[selectedGame].available) {
                            launchGame(games[selectedGame]);
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                }
                break;
        }
    }
}

void UltimaLauncher::launchGame(const GameInfo& game) {
    std::string command;
    
    if (game.engine == "exult") {
        command = "exult";
        if (game.id == "u7si") {
            command += " --si";
        }
    } else if (game.engine == "scummvm") {
        command = "scummvm --path=\"" + game.dataPath + "\" ultima8";
    }
    
    std::cout << "Launching: " << command << std::endl;
    
    // Hide launcher window
    SDL_HideWindow(window);
    
    // Launch game
    int result = system(command.c_str());
    
    // Show launcher window again
    SDL_ShowWindow(window);
    
    if (result != 0) {
        std::cerr << "Game exited with code: " << result << std::endl;
    }
}

void UltimaLauncher::run() {
    while (running) {
        handleEvents();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void UltimaLauncher::shutdown() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    std::cout << "Ultima Engines Unified Launcher" << std::endl;
    std::cout << "================================" << std::endl;
    
    UltimaLauncher launcher;
    
    if (!launcher.init()) {
        std::cerr << "Failed to initialize launcher" << std::endl;
        return 1;
    }
    
    launcher.run();
    
    return 0;
}
