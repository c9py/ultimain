/*
 * SDL3 view of the avatar navigation controller.
 *
 * Loads the reference map (coordinates and shape types), trains gneural-net
 * on player controls, and walks the avatar to the target. Arrow keys are the
 * same four diagonals the game uses. Space steps the trained network.
 * --auto plays the trained route and leaves the avatar on the target.
 */

#include "navigation/AvatarNavController.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

using Ultima::NPC::Navigation::AvatarNavController;
using Ultima::NPC::Navigation::NavCell;
using Ultima::NPC::Navigation::PlayerControl;

#ifndef NAV_MAP_PATH
#define NAV_MAP_PATH "dock_nav.map"
#endif

struct Rgb {
    Uint8 r, g, b;
};

Rgb shapeColor(int shape, bool walkable) {
    switch (shape) {
        case 164: return Rgb{186, 142, 78};
        case 153: return Rgb{96, 62, 34};
        case 154: return Rgb{72, 80, 66};
        case 155: return Rgb{46, 74, 58};
        case 156: return Rgb{62, 68, 84};
        default: break;
    }
    if (walkable) return Rgb{140, 126, 90};
    return Rgb{28, 30, 36};
}

struct View {
    int minX = 0;
    int minY = 0;
    int spanX = 1;
    int spanY = 1;
    float originX = 36.f;
    float originY = 36.f;
    float mapW = 760.f;
    float mapH = 640.f;
};

View makeView(const std::vector<NavCell>& cells) {
    View view;
    int maxX = cells.front().x;
    int maxY = cells.front().y;
    view.minX = cells.front().x;
    view.minY = cells.front().y;
    for (const NavCell& cell : cells) {
        view.minX = std::min(view.minX, cell.x);
        view.minY = std::min(view.minY, cell.y);
        maxX = std::max(maxX, cell.x);
        maxY = std::max(maxY, cell.y);
    }
    view.spanX = (maxX - view.minX) + AvatarNavController::kStep;
    view.spanY = (maxY - view.minY) + AvatarNavController::kStep;
    return view;
}

SDL_FRect cellRect(const View& view, int x, int y) {
    SDL_FRect rect;
    rect.w = AvatarNavController::kStep * view.mapW / static_cast<float>(view.spanX) - 2.f;
    rect.h = AvatarNavController::kStep * view.mapH / static_cast<float>(view.spanY) - 2.f;
    rect.x = view.originX + (x - view.minX) * view.mapW / static_cast<float>(view.spanX);
    rect.y = view.originY + (y - view.minY) * view.mapH / static_cast<float>(view.spanY);
    return rect;
}

void fillRect(SDL_Renderer* renderer, const SDL_FRect& rect, Rgb color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void drawText(SDL_Renderer* renderer, float x, float y, const std::string& text) {
    SDL_SetRenderDrawColor(renderer, 232, 224, 208, 255);
    SDL_RenderDebugText(renderer, x, y, text.c_str());
}

bool applyControl(AvatarNavController& nav, int& x, int& y, PlayerControl control,
                  std::vector<NavCell>* trail, bool record) {
    if (control.dx == 0 && control.dy == 0) return false;
    if (record) {
        nav.recordPlayerControl(x, y, nav.targetX(), nav.targetY(), control);
    }
    const int nx = x + control.dx * AvatarNavController::kStep;
    const int ny = y + control.dy * AvatarNavController::kStep;
    const NavCell* next = nav.cellAt(nx, ny);
    if (!next || !next->walkable) return false;
    x = nx;
    y = ny;
    if (trail) trail->push_back(*next);
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    std::string mapPath = NAV_MAP_PATH;
    bool automatic = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--auto") automatic = true;
        else mapPath = arg;
    }

    AvatarNavController nav;
    if (!nav.loadReferenceMap(mapPath)) {
        std::cerr << "AvatarNav SDL: failed to load " << mapPath << std::endl;
        return 1;
    }
    const int shown = nav.demonstratePlayerRoute(nav.targetX(), nav.targetY());
    std::cout << "AvatarNav SDL demonstrated " << shown << " player controls" << std::endl;
    if (!nav.trainOptimal(nav.targetX(), nav.targetY())) {
        std::cerr << "AvatarNav SDL: training did not reach the target" << std::endl;
        return 1;
    }

    const std::vector<NavCell>& cells = nav.cells();
    if (cells.empty()) {
        std::cerr << "AvatarNav SDL: reference map has no cells" << std::endl;
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Avatar navigation", 1100, 720, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowPosition(window, 360, 48);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const View view = makeView(cells);
    int avatarX = nav.startX();
    int avatarY = nav.startY();
    std::vector<NavCell> trail;
    if (const NavCell* start = nav.cellAt(avatarX, avatarY)) trail.push_back(*start);

    bool running = true;
    int tick = 0;
    int hold = 0;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
                PlayerControl control;
                bool stepped = false;
                switch (event.key.key) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_UP: control = {-1, -1}; stepped = true; break;
                    case SDLK_DOWN: control = {1, 1}; stepped = true; break;
                    case SDLK_LEFT: control = {-1, 1}; stepped = true; break;
                    case SDLK_RIGHT: control = {1, -1}; stepped = true; break;
                    case SDLK_SPACE:
                        control = nav.predict(avatarX, avatarY, nav.targetX(), nav.targetY());
                        stepped = true;
                        break;
                    case SDLK_A:
                        automatic = true;
                        tick = 320;
                        break;
                    case SDLK_RETURN:
                        nav.trainOptimal(nav.targetX(), nav.targetY());
                        break;
                    default: break;
                }
                if (stepped) {
                    applyControl(nav, avatarX, avatarY, control, &trail, event.key.key != SDLK_SPACE);
                }
            }
        }

        // Hold the start cell long enough for the window to appear, then step.
        if (automatic && tick > 300 &&
            (avatarX != nav.targetX() || avatarY != nav.targetY()) &&
            ((tick - 300) % 20) == 0) {
            const PlayerControl control = nav.predict(avatarX, avatarY, nav.targetX(), nav.targetY());
            if (!applyControl(nav, avatarX, avatarY, control, &trail, false)) {
                std::cerr << "AvatarNav SDL stopped at (" << avatarX << "," << avatarY << ")" << std::endl;
                automatic = false;
            }
        }
        if (automatic && avatarX == nav.targetX() && avatarY == nav.targetY()) {
            if (hold == 0) {
                std::cout << "AvatarNav SDL arrived (" << avatarX << "," << avatarY << ")" << std::endl;
            }
            ++hold;
        }

        SDL_SetRenderDrawColor(renderer, 18, 20, 28, 255);
        SDL_RenderClear(renderer);
        for (const NavCell& cell : cells) {
            fillRect(renderer, cellRect(view, cell.x, cell.y), shapeColor(cell.shape, cell.walkable));
        }
        for (const NavCell& cell : trail) {
            SDL_FRect rect = cellRect(view, cell.x, cell.y);
            rect.x += rect.w * 0.22f;
            rect.y += rect.h * 0.22f;
            rect.w *= 0.56f;
            rect.h *= 0.56f;
            fillRect(renderer, rect, {232, 196, 96});
        }
        {
            SDL_FRect goal = cellRect(view, nav.targetX(), nav.targetY());
            SDL_SetRenderDrawColor(renderer, 240, 208, 96, 255);
            SDL_RenderRect(renderer, &goal);
        }
        {
            SDL_FRect body = cellRect(view, avatarX, avatarY);
            body.x += 3.f;
            body.y += 3.f;
            body.w = std::max(4.f, body.w - 6.f);
            body.h = std::max(4.f, body.h - 6.f);
            fillRect(renderer, body, {176, 48, 40});
        }

        const std::string status = std::string("trained ") + nav.report().method +
            "  error " + std::to_string(nav.report().error);
        drawText(renderer, 820.f, 48.f, "Tenebrae dock");
        drawText(renderer, 820.f, 72.f, status);
        drawText(renderer, 820.f, 96.f,
                 "avatar " + std::to_string(avatarX) + "," + std::to_string(avatarY));
        drawText(renderer, 820.f, 112.f,
                 "target " + std::to_string(nav.targetX()) + "," + std::to_string(nav.targetY()));
        drawText(renderer, 820.f, 148.f, "timber  walk");
        drawText(renderer, 820.f, 164.f, "plank   blocked");
        drawText(renderer, 820.f, 180.f, "rail    blocked");
        drawText(renderer, 820.f, 220.f, "arrows  player step");
        drawText(renderer, 820.f, 236.f, "space   trained step");
        drawText(renderer, 820.f, 252.f, "a       play route");
        drawText(renderer, 820.f, 268.f, "enter   retrain");

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        ++tick;
    }

    const bool arrived = avatarX == nav.targetX() && avatarY == nav.targetY();
    std::cout << "AvatarNav SDL final (" << avatarX << "," << avatarY << ")"
              << (arrived ? " on target" : " short of target") << std::endl;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return arrived || !automatic ? 0 : 1;
}
