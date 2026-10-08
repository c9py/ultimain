/*
 * Ultima Engines Unified Launcher
 * 
 * This launcher provides a unified interface to:
 * - Exult (Ultima VII: The Black Gate / Serpent Isle)
 * - ScummVM Ultima8 (Ultima VIII: Pagan)
 * 
 * Features:
 * - Keyboard and mouse navigation
 * - Responsive layout that adapts to window size
 * - Game data path configuration when data is missing
 * 
 * Copyright (C) 2026 Ultima Integration Project
 * Licensed under GPL-2.0
 */

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdio>

namespace fs = std::filesystem;

struct GameInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string engine;
    std::string executable;
    std::string dataPath;
    std::string configuredPath;
    bool available;
};

enum class UIState {
    MAIN_MENU,
    PATH_INPUT,
    MESSAGE_BOX
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
    TTF_Font* fontLarge = nullptr;
    TTF_Font* fontMedium = nullptr;
    TTF_Font* fontSmall = nullptr;
    
    std::vector<GameInfo> games;
    int selectedGame = 0;
    bool running = true;
    
    UIState uiState = UIState::MAIN_MENU;
    std::string pathInputBuffer;
    std::string messageBoxTitle;
    std::string messageBoxText;
    
    int windowWidth = 800;
    int windowHeight = 600;
    
    static constexpr int HEADER_HEIGHT = 80;
    static constexpr int FOOTER_HEIGHT = 80;
    static constexpr int CARD_HEIGHT = 90;
    static constexpr int CARD_SPACING = 10;
    static constexpr int CARD_MARGIN = 50;
    
    bool loadFonts();
    void detectGames();
    void checkGameAvailability(GameInfo& game);
    void render();
    void handleEvents();
    void launchGame(const GameInfo& game);
    
    void drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font, bool centered = false);
    void drawGameList();
    void drawHeader();
    void drawFooter();
    void drawPathInputDialog();
    void drawMessageBox();
    
    int getCardYPosition(int index) const;
    int hitTestCard(int mouseY) const;
    void showMessage(const std::string& title, const std::string& text);
    void showPathInput();
    void applyPathInput();
    std::string findPentagramBinary() const;
    std::string findPaganData(const std::string& configured) const;
    bool paganLayout(const fs::path& root) const;
    void writePentagramIni(const std::string& dataRoot) const;
    
    std::string findFontPath() const;
    std::vector<std::string> getSearchPaths() const;
};

UltimaLauncher::UltimaLauncher() {
    games = {
        {
            "u7bg",
            "Ultima VII: The Black Gate",
            "The Avatar returns to Britannia to investigate a series of ritual murders.",
            "exult",
            "exult",
            "ultima7",
            "",
            false
        },
        {
            "u7si",
            "Ultima VII Part Two: Serpent Isle",
            "Continue the Avatar's journey to the mysterious Serpent Isle.",
            "exult",
            "exult",
            "serpentisle",
            "",
            false
        },
        {
            "u8",
            "Ultima VIII: Pagan",
            "The Avatar is banished to the dark world of Pagan.",
            "scummvm",
            "scummvm --path=",
            "ultima8",
            "",
            false
        },
        {
            "exult_studio",
            "Exult Studio",
            "Map and content editor for Ultima VII games.",
            "exult_studio",
            "exult_studio",
            "",
            "",
            false
        },
        {
            "pentagram",
            "Pentagram (Native)",
            "Native Pentagram engine for Ultima VIII (alternative to ScummVM).",
            "pentagram",
            "pentagram",
            "ultima8",
            "",
            false
        }
    };
}

UltimaLauncher::~UltimaLauncher() {
    shutdown();
}

bool UltimaLauncher::paganLayout(const fs::path& root) const {
    if (!fs::is_directory(root)) {
        return false;
    }
    bool usecode = false;
    bool gumps = false;
    std::error_code ec;
    fs::path usecodeDir = root / "usecode";
    if (fs::is_directory(usecodeDir, ec)) {
        for (const auto& entry : fs::directory_iterator(usecodeDir, ec)) {
            std::string name = entry.path().filename().string();
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            if (name == "eusecode.flx" || name == "fusecode.flx" ||
                name == "gusecode.flx" || name == "jusecode.flx") {
                usecode = true;
            }
        }
    }
    fs::path staticDir = root / "static";
    if (fs::is_directory(staticDir, ec)) {
        for (const auto& entry : fs::directory_iterator(staticDir, ec)) {
            std::string name = entry.path().filename().string();
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            if (name == "u8gumps.flx") {
                gumps = true;
            }
        }
    }
    /* Same relative names GameDetector reads through lowercase virtual paths. */
    return usecode && gumps;
}

std::string UltimaLauncher::findPentagramBinary() const {
    std::vector<std::string> candidates;
    if (const char* env = std::getenv("PENTAGRAM_BIN")) {
        candidates.push_back(env);
    }
    candidates.push_back(std::string(LAUNCHER_SOURCE_DIR) + "/../engines/ultima8/build/pentagram");
    candidates.push_back("engines/ultima8/build/pentagram");
    candidates.push_back("/usr/local/bin/pentagram");
    candidates.push_back("/usr/bin/pentagram");
    for (const auto& candidate : candidates) {
        std::error_code ec;
        if (!candidate.empty() && fs::is_regular_file(candidate, ec)) {
            return fs::absolute(candidate).string();
        }
    }
    if (system("which pentagram > /dev/null 2>&1") == 0) {
        FILE* pipe = popen("which pentagram", "r");
        if (pipe) {
            char buf[512];
            if (fgets(buf, sizeof(buf), pipe)) {
                std::string path(buf);
                while (!path.empty() && (path.back() == '\n' || path.back() == '\r')) {
                    path.pop_back();
                }
                pclose(pipe);
                if (!path.empty()) {
                    return path;
                }
            } else {
                pclose(pipe);
            }
        }
    }
    return "";
}

std::string UltimaLauncher::findPaganData(const std::string& configured) const {
    std::vector<std::string> candidates;
    if (!configured.empty()) {
        candidates.push_back(configured);
    }
    if (const char* env = std::getenv("PENTAGRAM_GAME_PATH")) {
        candidates.push_back(env);
    }
    candidates.push_back(std::string(LAUNCHER_SOURCE_DIR) + "/../engines/ultima8/gamedata");
    candidates.push_back("engines/ultima8/gamedata");
    for (const auto& base : getSearchPaths()) {
        candidates.push_back((fs::path(base) / "ultima8").string());
        candidates.push_back((fs::path(base) / "Ultima8").string());
        candidates.push_back((fs::path(base) / "Ultima VIII").string());
    }
    for (const auto& candidate : candidates) {
        if (paganLayout(candidate)) {
            return fs::absolute(candidate).string();
        }
    }
    return "";
}

void UltimaLauncher::writePentagramIni(const std::string& dataRoot) const {
    const char* homeEnv = std::getenv("HOME");
    fs::path home = homeEnv ? fs::path(homeEnv) / ".pentagram" : fs::path(".pentagram");
    std::error_code ec;
    fs::create_directories(home, ec);
    fs::path iniPath = home / "pentagram.ini";

    std::string existing;
    {
        std::ifstream in(iniPath);
        if (in) {
            existing.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        }
    }
    if (existing.empty()) {
        std::ofstream out(iniPath);
        out << "[pentagram]\n";
        out << "defaultgame=u8\n";
        out << "fullscreen=no\n";
        out << "scaler=bilinear\n";
        out << "width=640\n";
        out << "height=480\n";
        out << "bpp=32\n";
        out << "ttf=no\n";
        out << "midi_driver=timidity\n";
        out << "skipstart=yes\n";
        out << "\n[u8]\n";
        out << "path=" << dataRoot << "\n";
        return;
    }

    std::string outText;
    std::string section;
    bool sawU8 = false;
    bool wrotePath = false;
    bool sawPentagram = false;
    bool sawMidi = false;
    bool sawScaler = false;
    bool sawTtf = false;
    bool sawSkip = false;
    bool sawDefault = false;
    std::string line;
    std::istringstream input(existing);
    while (std::getline(input, line)) {
        if (!line.empty() && line[0] == '[') {
            if (section == "pentagram") {
                if (!sawDefault) outText += "defaultgame=u8\n";
                if (!sawScaler) outText += "scaler=bilinear\n";
                if (!sawTtf) outText += "ttf=no\n";
                if (!sawMidi) outText += "midi_driver=timidity\n";
                if (!sawSkip) outText += "skipstart=yes\n";
            }
            if (section == "u8" && !wrotePath) {
                outText += "path=" + dataRoot + "\n";
                wrotePath = true;
            }
            section = line.substr(1, line.find(']') - 1);
            std::transform(section.begin(), section.end(), section.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            if (section == "u8") sawU8 = true;
            if (section == "pentagram") sawPentagram = true;
            outText += line + "\n";
            continue;
        }
        if (section == "u8" && line.rfind("path=", 0) == 0) {
            outText += "path=" + dataRoot + "\n";
            wrotePath = true;
            continue;
        }
        if (section == "pentagram") {
            if (line.rfind("midi_driver=", 0) == 0) sawMidi = true;
            if (line.rfind("scaler=", 0) == 0) sawScaler = true;
            if (line.rfind("ttf=", 0) == 0) sawTtf = true;
            if (line.rfind("skipstart=", 0) == 0) sawSkip = true;
            if (line.rfind("defaultgame=", 0) == 0) sawDefault = true;
        }
        outText += line + "\n";
    }
    if (section == "u8" && !wrotePath) {
        outText += "path=" + dataRoot + "\n";
        wrotePath = true;
    }
    if (section == "pentagram") {
        if (!sawDefault) outText += "defaultgame=u8\n";
        if (!sawScaler) outText += "scaler=bilinear\n";
        if (!sawTtf) outText += "ttf=no\n";
        if (!sawMidi) outText += "midi_driver=timidity\n";
        if (!sawSkip) outText += "skipstart=yes\n";
    }
    if (!sawPentagram) {
        outText += "\n[pentagram]\ndefaultgame=u8\nscaler=bilinear\nttf=no\nmidi_driver=timidity\nskipstart=yes\n";
    }
    if (!sawU8) {
        outText += "\n[u8]\npath=" + dataRoot + "\n";
    }
    std::ofstream out(iniPath);
    out << outText;
}

std::string UltimaLauncher::findFontPath() const {
    std::vector<std::string> fontPaths = {
        std::string(LAUNCHER_SOURCE_DIR) + "/assets/DejaVuSans.ttf",
        std::string(LAUNCHER_ASSET_DIR) + "/DejaVuSans.ttf",
        "./assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    
    for (const auto& path : fontPaths) {
        if (fs::exists(path)) {
            return path;
        }
    }
    return "";
}

bool UltimaLauncher::loadFonts() {
    std::string fontPath = findFontPath();
    if (fontPath.empty()) {
        std::cerr << "Could not find any suitable font file" << std::endl;
        return false;
    }
    
    std::cout << "Loading font from: " << fontPath << std::endl;
    
    fontLarge = TTF_OpenFont(fontPath.c_str(), 28);
    fontMedium = TTF_OpenFont(fontPath.c_str(), 18);
    fontSmall = TTF_OpenFont(fontPath.c_str(), 14);
    
    if (!fontLarge || !fontMedium || !fontSmall) {
        std::cerr << "Failed to load fonts: " << SDL_GetError() << std::endl;
        return false;
    }
    
    return true;
}

bool UltimaLauncher::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        "Ultima Engines Launcher",
        windowWidth, windowHeight,
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
    
    if (!loadFonts()) {
        std::cerr << "Failed to load fonts - text will not be displayed" << std::endl;
    }
    
    detectGames();
    return true;
}

std::vector<std::string> UltimaLauncher::getSearchPaths() const {
    std::vector<std::string> paths = {
        ".",
        "./games",
        "./data"
    };
    
#ifdef _WIN32
    paths.push_back("C:/Games");
    paths.push_back("C:/GOG Games");
    paths.push_back("C:/Program Files/GOG.com");
    paths.push_back("C:/Program Files (x86)/GOG.com");
    paths.push_back("C:/Program Files/Steam/steamapps/common");
    paths.push_back("C:/Program Files (x86)/Steam/steamapps/common");
    
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) {
        paths.push_back(std::string(userProfile) + "/Games");
        paths.push_back(std::string(userProfile) + "/Documents/Ultima");
    }
#else
    paths.push_back("/usr/share/games");
    paths.push_back("/usr/local/share/games");
    
    const char* home = std::getenv("HOME");
    if (home) {
        paths.push_back(std::string(home) + "/games");
        paths.push_back(std::string(home) + "/Games");
        paths.push_back(std::string(home) + "/.local/share/exult");
        paths.push_back(std::string(home) + "/.local/share/scummvm");
        paths.push_back(std::string(home) + "/.local/share/ultima");
        paths.push_back(std::string(home) + "/GOG Games");
    }
    
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
    if (xdgDataHome) {
        paths.push_back(std::string(xdgDataHome) + "/exult");
        paths.push_back(std::string(xdgDataHome) + "/scummvm");
    }
#endif
    
    return paths;
}

void UltimaLauncher::checkGameAvailability(GameInfo& game) {
    game.available = false;
    
    if (game.id == "pentagram") {
        game.available = false;
        std::string binary = findPentagramBinary();
        if (binary.empty()) {
            return;
        }
        game.executable = binary;
        std::string data = findPaganData(game.configuredPath);
        if (data.empty()) {
            return;
        }
        game.dataPath = data;
        game.available = true;
        return;
    }

    if (!game.configuredPath.empty() && fs::exists(game.configuredPath) && fs::is_directory(game.configuredPath)) {
        game.available = true;
        game.dataPath = game.configuredPath;
        return;
    }
    
    if (game.id == "exult_studio") {
        std::string command = "which " + game.executable + " > /dev/null 2>&1";
#ifdef _WIN32
        command = "where " + game.executable + " > nul 2>&1";
#endif
        if (system(command.c_str()) == 0) {
            game.available = true;
        }
        return;
    }
    
    auto searchPaths = getSearchPaths();
    for (const auto& basePath : searchPaths) {
        fs::path gamePath = fs::path(basePath) / game.dataPath;
        if (fs::exists(gamePath) && fs::is_directory(gamePath)) {
            game.available = true;
            game.dataPath = gamePath.string();
            std::cout << "Found " << game.name << " at " << gamePath << std::endl;
            return;
        }
    }
}

void UltimaLauncher::detectGames() {
    for (auto& game : games) {
        checkGameAvailability(game);
    }
}

void UltimaLauncher::drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font, bool centered) {
    if (!font || text.empty()) return;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_DestroySurface(surface);
        return;
    }
    
    SDL_FRect destRect;
    destRect.w = (float)surface->w;
    destRect.h = (float)surface->h;
    
    if (centered) {
        destRect.x = (float)(x - surface->w / 2);
    } else {
        destRect.x = (float)x;
    }
    destRect.y = (float)y;
    
    SDL_RenderTexture(renderer, texture, nullptr, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

int UltimaLauncher::getCardYPosition(int index) const {
    return HEADER_HEIGHT + CARD_SPACING + index * (CARD_HEIGHT + CARD_SPACING);
}

int UltimaLauncher::hitTestCard(int mouseY) const {
    for (int i = 0; i < (int)games.size(); ++i) {
        int cardTop = getCardYPosition(i);
        int cardBottom = cardTop + CARD_HEIGHT;
        if (mouseY >= cardTop && mouseY <= cardBottom) {
            return i;
        }
    }
    return -1;
}

void UltimaLauncher::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);
    
    drawHeader();
    drawGameList();
    drawFooter();
    
    if (uiState == UIState::PATH_INPUT) {
        drawPathInputDialog();
    } else if (uiState == UIState::MESSAGE_BOX) {
        drawMessageBox();
    }
    
    SDL_RenderPresent(renderer);
}

void UltimaLauncher::drawHeader() {
    SDL_FRect titleBar = {0, 0, (float)windowWidth, (float)HEADER_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderFillRect(renderer, &titleBar);
    
    SDL_Color white = {255, 255, 255, 255};
    drawText("Ultima Engines Launcher", windowWidth / 2, 20, white, fontLarge, true);
    
    SDL_Color gray = {180, 180, 200, 255};
    drawText("Select a game to play", windowWidth / 2, 52, gray, fontSmall, true);
}

void UltimaLauncher::drawGameList() {
    int cardWidth = windowWidth - 2 * CARD_MARGIN;
    
    for (int i = 0; i < (int)games.size(); ++i) {
        const auto& game = games[i];
        int y = getCardYPosition(i);
        
        if (y + CARD_HEIGHT > windowHeight - FOOTER_HEIGHT) {
            break;
        }
        
        SDL_FRect gameBox = {(float)CARD_MARGIN, (float)y, (float)cardWidth, (float)CARD_HEIGHT};
        
        if (i == selectedGame) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 160, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 80, 255);
        }
        SDL_RenderFillRect(renderer, &gameBox);
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
        SDL_RenderRect(renderer, &gameBox);
        
        SDL_FRect indicator = {(float)(CARD_MARGIN + 15), (float)(y + 35), 20, 20};
        if (game.available) {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &indicator);
        
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color gray = {180, 180, 200, 255};
        SDL_Color statusColor = game.available ? SDL_Color{100, 255, 100, 255} : SDL_Color{255, 150, 150, 255};
        
        drawText(game.name, CARD_MARGIN + 50, y + 10, white, fontMedium, false);
        
        std::string desc = game.description;
        if (desc.length() > 80) {
            desc = desc.substr(0, 77) + "...";
        }
        drawText(desc, CARD_MARGIN + 50, y + 35, gray, fontSmall, false);
        
        std::string statusText = game.available ? "Ready to play" : "Game data not found - Press Enter to configure";
        drawText(statusText, CARD_MARGIN + 50, y + 60, statusColor, fontSmall, false);
    }
}

void UltimaLauncher::drawFooter() {
    SDL_FRect footerBar = {0, (float)(windowHeight - FOOTER_HEIGHT), (float)windowWidth, (float)FOOTER_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderFillRect(renderer, &footerBar);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {180, 180, 200, 255};
    
    drawText("Navigation: Up/Down or Mouse | Select: Enter or Double-Click | Quit: Escape", 
             windowWidth / 2, windowHeight - FOOTER_HEIGHT + 20, gray, fontSmall, true);
    
    if (selectedGame >= 0 && selectedGame < (int)games.size()) {
        const auto& game = games[selectedGame];
        if (game.available) {
            drawText("Press Enter or Double-Click to launch " + game.name, 
                     windowWidth / 2, windowHeight - FOOTER_HEIGHT + 45, white, fontSmall, true);
        } else {
            drawText("Press Enter to set the path to " + game.name + " game data", 
                     windowWidth / 2, windowHeight - FOOTER_HEIGHT + 45, white, fontSmall, true);
        }
    }
}

void UltimaLauncher::drawPathInputDialog() {
    int dialogWidth = std::min(600, windowWidth - 100);
    int dialogHeight = 200;
    int dialogX = (windowWidth - dialogWidth) / 2;
    int dialogY = (windowHeight - dialogHeight) / 2;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect overlay = {0, 0, (float)windowWidth, (float)windowHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    SDL_FRect dialogBg = {(float)dialogX, (float)dialogY, (float)dialogWidth, (float)dialogHeight};
    SDL_SetRenderDrawColor(renderer, 30, 30, 70, 255);
    SDL_RenderFillRect(renderer, &dialogBg);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderRect(renderer, &dialogBg);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {180, 180, 200, 255};
    SDL_Color yellow = {255, 255, 150, 255};
    
    drawText("Set Game Data Path", windowWidth / 2, dialogY + 20, white, fontMedium, true);
    
    const auto& game = games[selectedGame];
    drawText("Enter the path to " + game.name + " data folder:", windowWidth / 2, dialogY + 55, gray, fontSmall, true);
    
    SDL_FRect inputBox = {(float)(dialogX + 20), (float)(dialogY + 85), (float)(dialogWidth - 40), 30};
    SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
    SDL_RenderFillRect(renderer, &inputBox);
    SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
    SDL_RenderRect(renderer, &inputBox);
    
    std::string displayText = pathInputBuffer + "_";
    drawText(displayText, dialogX + 30, dialogY + 90, white, fontSmall, false);
    
    drawText("Press Enter to confirm, Escape to cancel", windowWidth / 2, dialogY + 130, gray, fontSmall, true);
    drawText("Example: /home/user/games/ultima7 or C:\\Games\\Ultima7", windowWidth / 2, dialogY + 155, yellow, fontSmall, true);
}

void UltimaLauncher::drawMessageBox() {
    int dialogWidth = std::min(500, windowWidth - 100);
    int dialogHeight = 150;
    int dialogX = (windowWidth - dialogWidth) / 2;
    int dialogY = (windowHeight - dialogHeight) / 2;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_FRect overlay = {0, 0, (float)windowWidth, (float)windowHeight};
    SDL_RenderFillRect(renderer, &overlay);
    
    SDL_FRect dialogBg = {(float)dialogX, (float)dialogY, (float)dialogWidth, (float)dialogHeight};
    SDL_SetRenderDrawColor(renderer, 30, 30, 70, 255);
    SDL_RenderFillRect(renderer, &dialogBg);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderRect(renderer, &dialogBg);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {180, 180, 200, 255};
    
    drawText(messageBoxTitle, windowWidth / 2, dialogY + 30, white, fontMedium, true);
    drawText(messageBoxText, windowWidth / 2, dialogY + 70, gray, fontSmall, true);
    drawText("Press Enter or Escape to close", windowWidth / 2, dialogY + 110, gray, fontSmall, true);
}

void UltimaLauncher::showMessage(const std::string& title, const std::string& text) {
    messageBoxTitle = title;
    messageBoxText = text;
    uiState = UIState::MESSAGE_BOX;
}

void UltimaLauncher::showPathInput() {
    pathInputBuffer.clear();
    if (selectedGame >= 0 && selectedGame < (int)games.size()) {
        pathInputBuffer = games[selectedGame].configuredPath;
    }
    uiState = UIState::PATH_INPUT;
    SDL_StartTextInput(window);
}

void UltimaLauncher::applyPathInput() {
    SDL_StopTextInput(window);
    
    if (pathInputBuffer.empty()) {
        uiState = UIState::MAIN_MENU;
        return;
    }
    
    fs::path inputPath(pathInputBuffer);
    if (!fs::exists(inputPath)) {
        showMessage("Path Not Found", "The specified path does not exist: " + pathInputBuffer);
        return;
    }
    
    if (!fs::is_directory(inputPath)) {
        showMessage("Invalid Path", "The specified path is not a directory.");
        return;
    }
    
    games[selectedGame].configuredPath = pathInputBuffer;
    checkGameAvailability(games[selectedGame]);
    
    if (games[selectedGame].available) {
        showMessage("Success", "Game data found! You can now launch " + games[selectedGame].name);
    } else {
        showMessage("Data Not Found", "Could not find valid game data in the specified path.");
    }
}

void UltimaLauncher::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
                
            case SDL_EVENT_WINDOW_RESIZED:
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                break;
                
            case SDL_EVENT_TEXT_INPUT:
                if (uiState == UIState::PATH_INPUT) {
                    pathInputBuffer += event.text.text;
                }
                break;
                
            case SDL_EVENT_KEY_DOWN:
                if (uiState == UIState::PATH_INPUT) {
                    switch (event.key.key) {
                        case SDLK_RETURN:
                            applyPathInput();
                            break;
                        case SDLK_ESCAPE:
                            SDL_StopTextInput(window);
                            uiState = UIState::MAIN_MENU;
                            break;
                        case SDLK_BACKSPACE:
                            if (!pathInputBuffer.empty()) {
                                pathInputBuffer.pop_back();
                            }
                            break;
                    }
                } else if (uiState == UIState::MESSAGE_BOX) {
                    if (event.key.key == SDLK_RETURN || event.key.key == SDLK_ESCAPE) {
                        uiState = UIState::MAIN_MENU;
                    }
                } else {
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
                            } else {
                                showPathInput();
                            }
                            break;
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                    }
                }
                break;
                
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (uiState == UIState::MAIN_MENU) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int clickedCard = hitTestCard((int)event.button.y);
                        if (clickedCard >= 0) {
                            if (clickedCard == selectedGame && event.button.clicks >= 2) {
                                if (games[selectedGame].available) {
                                    launchGame(games[selectedGame]);
                                } else {
                                    showPathInput();
                                }
                            } else {
                                selectedGame = clickedCard;
                            }
                        }
                    }
                } else if (uiState == UIState::MESSAGE_BOX) {
                    uiState = UIState::MAIN_MENU;
                }
                break;
                
            case SDL_EVENT_MOUSE_MOTION:
                if (uiState == UIState::MAIN_MENU) {
                    int hoveredCard = hitTestCard((int)event.motion.y);
                    if (hoveredCard >= 0 && hoveredCard != selectedGame) {
                    }
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
    } else if (game.engine == "exult_studio") {
        command = "exult_studio";
    } else if (game.engine == "pentagram") {
        if (game.executable.empty() || game.dataPath.empty()) {
            showMessage("Launch Failed", "Pentagram binary or Pagan data path is missing.");
            return;
        }
        writePentagramIni(game.dataPath);
        command = "\"" + game.executable + "\" --game u8";
    }
    
    std::cout << "Launching: " << command << std::endl;
    showMessage("Launching", "Starting " + game.name + "...");
    render();
    
    SDL_HideWindow(window);
    
    int result = system(command.c_str());
    
    SDL_ShowWindow(window);
    SDL_RaiseWindow(window);
    
    if (result != 0) {
        showMessage("Launch Failed", "Game exited with error code: " + std::to_string(result));
    }
}

void UltimaLauncher::run() {
    while (running) {
        handleEvents();
        render();
        SDL_Delay(16);
    }
}

void UltimaLauncher::shutdown() {
    if (fontLarge) {
        TTF_CloseFont(fontLarge);
        fontLarge = nullptr;
    }
    if (fontMedium) {
        TTF_CloseFont(fontMedium);
        fontMedium = nullptr;
    }
    if (fontSmall) {
        TTF_CloseFont(fontSmall);
        fontSmall = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
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
