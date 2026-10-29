/**
 * Avatar navigation controller on top of gneural-net.
 *
 * Player controls are the four diagonals the SDL bindings already use
 * (up -1,-1, down +1,+1, left -1,+1, right +1,-1), in 64-unit steps.
 * The reference map supplies coordinates and shape types. Training labels
 * are the control a player would take on the shortest walk to the target.
 */

#include "navigation/AvatarNavController.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

extern "C" {
#include "defines.h"
#include "error.h"
#include "feedforward.h"
#include "network.h"
}

namespace Ultima {
namespace NPC {
namespace Navigation {

namespace {

constexpr int kFeatures = 8;
constexpr int kDirs = 4;
constexpr int kDir[kDirs][2] = {
    {-1, -1},  // up
    { 1,  1},  // down
    {-1,  1},  // left
    { 1, -1},  // right
};

enum class OutAct { Id, Tanh };
enum class Method { Gradient, Random };

struct TrainConfig {
    const char* name;
    Method method;
    int hidden;
    OutAct output;
    int iterations;
    double step;
    int nxw;
    double accuracy;
};

// Direct linear readouts sit next to a small hidden layer. Gradient descent
// on the skip connections can fit the player controls; the hidden units are
// extra capacity the search is free to use.
const TrainConfig kCandidates[] = {
    {"linear_gradient", Method::Gradient, 0, OutAct::Id, 80, 0.05, 20, 0.02},
    {"linear_gradient_wide", Method::Gradient, 0, OutAct::Id, 40, 0.2, 10, 0.05},
    {"hidden_tanh", Method::Gradient, 6, OutAct::Tanh, 40, 0.04, 16, 0.08},
    {"linear_random", Method::Random, 0, OutAct::Id, 300, 0.0, 0, 0.05},
};

activation_function toActivation(OutAct act) {
    return act == OutAct::Tanh ? TANH : ID;
}

bool buildNet(network* nn, int hidden, OutAct output) {
    if (hidden < 0 || kFeatures + hidden > MAX_IN) return false;
    const int total = kFeatures + hidden + 2;
    if (total > MAX_NUM_NEURONS) return false;
    if (network_set_neuron_number(nn, static_cast<unsigned long>(total)) != 0) return false;
    if (network_set_layer_number(nn, hidden > 0 ? 3 : 2) != 0) return false;

    nn->layers[0].num_of_neurons = kFeatures;
    nn->layers[0].neurons = &nn->neurons[0];
    int outLayer = 1;
    if (hidden > 0) {
        nn->layers[1].num_of_neurons = hidden;
        nn->layers[1].neurons = &nn->neurons[kFeatures];
        outLayer = 2;
    }
    nn->layers[outLayer].num_of_neurons = 2;
    nn->layers[outLayer].neurons = &nn->neurons[kFeatures + hidden];

    for (int i = 0; i < kFeatures; ++i) {
        nn->neurons[i].activation = ID;
        nn->neurons[i].discriminant = LINEAR;
    }
    for (int h = 0; h < hidden; ++h) {
        neuron* ne = &nn->neurons[kFeatures + h];
        ne->activation = TANH;
        ne->discriminant = LINEAR;
        if (network_neuron_set_connection_number(ne, kFeatures) != 0) return false;
        for (int k = 0; k < kFeatures; ++k) {
            ne->connection[k] = &nn->neurons[k];
            ne->w[k] = 0.0;
        }
    }
    const int outInputs = kFeatures + hidden;
    for (int o = 0; o < 2; ++o) {
        neuron* ne = &nn->neurons[kFeatures + hidden + o];
        ne->activation = toActivation(output);
        ne->discriminant = LINEAR;
        if (network_neuron_set_connection_number(ne, outInputs) != 0) return false;
        for (int k = 0; k < kFeatures; ++k) {
            ne->connection[k] = &nn->neurons[k];
            ne->w[k] = 0.0;
        }
        for (int h = 0; h < hidden; ++h) {
            ne->connection[kFeatures + h] = &nn->neurons[kFeatures + h];
            ne->w[kFeatures + h] = 0.0;
        }
    }
    return true;
}

}  // namespace

struct AvatarNavController::Net {
    network* nn = nullptr;
    network_config* config = nullptr;
    int hidden = 0;

    ~Net() {
        if (nn) network_free(nn);
        if (config) network_config_free(config);
    }
};

AvatarNavController::~AvatarNavController() {
    delete net_;
}

const NavCell* AvatarNavController::cellAt(int x, int y) const {
    for (const NavCell& cell : cells_) {
        if (cell.x == x && cell.y == y) return &cell;
    }
    return nullptr;
}

double AvatarNavController::worldDist(int x, int y, int tx, int ty) const {
    const double dx = static_cast<double>(tx - x);
    const double dy = static_cast<double>(ty - y);
    return std::sqrt(dx * dx + dy * dy);
}

bool AvatarNavController::loadReferenceMap(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "AvatarNav: cannot open " << path << std::endl;
        return false;
    }
    cells_.clear();
    samples_.clear();
    std::string tag;
    while (in >> tag) {
        if (tag[0] == '#') {
            std::string rest;
            std::getline(in, rest);
            continue;
        }
        if (tag == "start") {
            in >> startX_ >> startY_;
        } else if (tag == "target") {
            in >> targetX_ >> targetY_;
        } else if (tag == "cell") {
            NavCell cell;
            int walk = 0;
            in >> cell.x >> cell.y >> cell.shape >> walk;
            cell.walkable = walk != 0;
            cells_.push_back(cell);
        } else {
            std::string rest;
            std::getline(in, rest);
        }
    }
    return !cells_.empty() && cellAt(startX_, startY_) &&
           cellAt(startX_, startY_)->walkable && cellAt(targetX_, targetY_) &&
           cellAt(targetX_, targetY_)->walkable;
}

void AvatarNavController::recordPlayerControl(int x, int y, int targetX,
                                              int targetY, PlayerControl control) {
    samples_.push_back(Sample{x, y, targetX, targetY, control.dx, control.dy});
}

PlayerControl AvatarNavController::expertStep(int x, int y, int tx, int ty) const {
    if (x == tx && y == ty) return {};
    struct Node {
        int x;
        int y;
        PlayerControl first;
    };
    std::queue<Node> pending;
    std::vector<std::pair<int, int>> seen;
    auto known = [&](int px, int py) {
        for (const auto& item : seen) {
            if (item.first == px && item.second == py) return true;
        }
        return false;
    };
    pending.push(Node{x, y, {}});
    seen.push_back({x, y});
    while (!pending.empty()) {
        const Node cur = pending.front();
        pending.pop();
        for (int d = 0; d < kDirs; ++d) {
            const int nx = cur.x + kDir[d][0] * kStep;
            const int ny = cur.y + kDir[d][1] * kStep;
            const NavCell* next = cellAt(nx, ny);
            if (!next || !next->walkable || known(nx, ny)) continue;
            const PlayerControl taken = (cur.x == x && cur.y == y)
                                            ? PlayerControl{kDir[d][0], kDir[d][1]}
                                            : cur.first;
            if (nx == tx && ny == ty) return taken;
            pending.push(Node{nx, ny, taken});
            seen.push_back({nx, ny});
        }
    }
    return {};
}

int AvatarNavController::demonstratePlayerRoute(int targetX, int targetY) {
    samples_.clear();
    targetX_ = targetX;
    targetY_ = targetY;
    int recorded = 0;
    for (const NavCell& cell : cells_) {
        if (!cell.walkable) continue;
        const PlayerControl control = expertStep(cell.x, cell.y, targetX, targetY);
        recordPlayerControl(cell.x, cell.y, targetX, targetY, control);
        ++recorded;
    }
    return recorded;
}

void AvatarNavController::fillFeatures(int x, int y, int targetX, int targetY,
                                       double* features) const {
    const double here = worldDist(x, y, targetX, targetY);
    features[0] = 1.0;
    features[1] = std::max(-1.0, std::min(1.0, (targetX - x) / 1024.0));
    features[2] = std::max(-1.0, std::min(1.0, (targetY - y) / 1024.0));
    for (int d = 0; d < kDirs; ++d) {
        const int nx = x + kDir[d][0] * kStep;
        const int ny = y + kDir[d][1] * kStep;
        const NavCell* next = cellAt(nx, ny);
        if (!next || !next->walkable) {
            features[3 + d] = -1.0;
        } else {
            features[3 + d] = (here - worldDist(nx, ny, targetX, targetY)) / static_cast<double>(kStep);
        }
    }
    const NavCell* under = cellAt(x, y);
    features[7] = under ? under->shape / 256.0 : 0.0;
}

PlayerControl AvatarNavController::predict(int x, int y, int targetX, int targetY) {
    PlayerControl stay;
    if (!net_ || !net_->nn) return stay;
    if (x == targetX && y == targetY) return stay;

    double features[kFeatures];
    fillFeatures(x, y, targetX, targetY, features);
    for (int i = 0; i < kFeatures; ++i) {
        net_->nn->layers[0].neurons[i].output = features[i];
    }
    feedforward(net_->nn);

    const int hidden = net_->hidden;
    const double ox = net_->nn->neurons[kFeatures + hidden].output;
    const double oy = net_->nn->neurons[kFeatures + hidden + 1].output;

    int best = -1;
    double bestDot = 0.2;
    for (int d = 0; d < kDirs; ++d) {
        const int nx = x + kDir[d][0] * kStep;
        const int ny = y + kDir[d][1] * kStep;
        const NavCell* next = cellAt(nx, ny);
        if (!next || !next->walkable) continue;
        const double dot = kDir[d][0] * ox + kDir[d][1] * oy;
        if (dot > bestDot) {
            bestDot = dot;
            best = d;
        }
    }
    if (best < 0) return stay;
    return PlayerControl{kDir[best][0], kDir[best][1]};
}

bool AvatarNavController::reachable(int targetX, int targetY, int maxSteps) {
    int x = startX_;
    int y = startY_;
    for (int step = 0; step < maxSteps; ++step) {
        if (x == targetX && y == targetY) return true;
        const PlayerControl control = predict(x, y, targetX, targetY);
        if (control.dx == 0 && control.dy == 0) return false;
        x += control.dx * kStep;
        y += control.dy * kStep;
    }
    return x == targetX && y == targetY;
}

bool AvatarNavController::moveToTarget(int startX, int startY, int targetX,
                                       int targetY, std::vector<NavCell>* path,
                                       int maxSteps) {
    int x = startX;
    int y = startY;
    if (path) path->clear();
    for (int step = 0; step <= maxSteps; ++step) {
        if (path) {
            const NavCell* cell = cellAt(x, y);
            NavCell mark;
            mark.x = x;
            mark.y = y;
            mark.shape = cell ? cell->shape : 0;
            mark.walkable = cell && cell->walkable;
            path->push_back(mark);
        }
        if (x == targetX && y == targetY) return true;
        const PlayerControl control = predict(x, y, targetX, targetY);
        if (control.dx == 0 && control.dy == 0) return false;
        x += control.dx * kStep;
        y += control.dy * kStep;
    }
    return false;
}

bool AvatarNavController::trainOptimal(int targetX, int targetY) {
    if (samples_.empty()) return false;
    targetX_ = targetX;
    targetY_ = targetY;

    delete net_;
    net_ = nullptr;
    report_ = NavReport{};

    const int count = std::min(static_cast<int>(samples_.size()), MAX_TRAINING_POINTS);
    report_.samples = count;

    double bestError = 1.0e300;
    bool bestReached = false;

    for (const TrainConfig& candidate : kCandidates) {
        Net* trial = new Net();
        trial->hidden = candidate.hidden;
        trial->nn = network_alloc();
        trial->config = network_config_alloc_default();
        if (!trial->nn || !trial->config ||
            !buildNet(trial->nn, candidate.hidden, candidate.output)) {
            delete trial;
            continue;
        }

        trial->config->num_points = static_cast<unsigned int>(count);
        trial->config->error_type = L2;
        trial->config->wmin = -1.0;
        trial->config->wmax = 1.0;
        trial->config->verbosity = OFF;
        const int out0 = kFeatures + candidate.hidden;
        const int out1 = out0 + 1;
        for (int n = 0; n < count; ++n) {
            double features[kFeatures];
            fillFeatures(samples_[n].x, samples_[n].y, samples_[n].targetX,
                         samples_[n].targetY, features);
            for (int i = 0; i < kFeatures; ++i) {
                trial->config->points_x[n][i][0] = features[i];
            }
            trial->config->points_y[n][out0] = static_cast<double>(samples_[n].dx);
            trial->config->points_y[n][out1] = static_cast<double>(samples_[n].dy);
        }

        if (candidate.method == Method::Gradient) {
            trial->config->optimization_type = GRADIENT_DESCENT;
            trial->config->nxw = candidate.nxw;
            trial->config->maxiter = candidate.iterations;
            trial->config->gamma = candidate.step;
            trial->config->accuracy = candidate.accuracy;
        } else {
            trial->config->optimization_type = RANDOM_SEARCH;
            trial->config->nmax = candidate.iterations;
            trial->config->accuracy = candidate.accuracy;
        }

        network_run_algorithm(trial->nn, trial->config);
        const double err = error(trial->nn, trial->config);

        Net* previous = net_;
        net_ = trial;
        const bool reached = std::isfinite(err) && reachable(targetX, targetY, 48);
        const bool better = (reached && (!bestReached || err < bestError)) ||
                            (!reached && !bestReached && std::isfinite(err) && err < bestError);
        std::cout << "AvatarNav " << candidate.name
                  << " hidden=" << candidate.hidden
                  << " error=" << err
                  << " reached=" << (reached ? "yes" : "no") << std::endl;
        if (better) {
            delete previous;
            bestError = err;
            bestReached = reached;
            report_.method = candidate.name;
            report_.hidden = candidate.hidden;
            report_.error = err;
            report_.reached = reached;
            report_.samples = count;
        } else {
            net_ = previous;
            delete trial;
        }
    }
    if (net_) {
        std::cout << "AvatarNav kept " << report_.method
                  << " error=" << report_.error
                  << " reached=" << (report_.reached ? "yes" : "no") << std::endl;
    }
    return report_.reached;
}

bool AvatarNavController::aimAt(int x, int y) {
    const NavCell* cell = cellAt(x, y);
    if (!cell || !cell->walkable) return false;
    if (net_ && report_.reached && x == targetX_ && y == targetY_) return true;

    const int previousX = targetX_;
    const int previousY = targetY_;
    std::vector<Sample> previous = samples_;
    demonstratePlayerRoute(x, y);
    if (trainOptimal(x, y)) return true;

    samples_ = std::move(previous);
    if (!samples_.empty() && (previousX != x || previousY != y)) {
        trainOptimal(previousX, previousY);
    }
    return false;
}

}  // namespace Navigation
}  // namespace NPC
}  // namespace Ultima
