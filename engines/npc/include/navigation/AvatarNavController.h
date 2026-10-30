/**
 * Avatar navigation controller.
 *
 * Trains a gneural-net on player control inputs. Each sample pairs the
 * reference map (coordinates and shape types around the avatar) with the
 * control a player pressed. Several network configurations are trained and
 * the one that actually reaches the target is kept.
 */

#ifndef ULTIMA_NPC_AVATAR_NAV_CONTROLLER_H
#define ULTIMA_NPC_AVATAR_NAV_CONTROLLER_H

#include <string>
#include <vector>

namespace Ultima {
namespace NPC {
namespace Navigation {

// One step of the four player diagonals, in units of the 64-world step.
// (0, 0) means stand.
struct PlayerControl {
    int dx = 0;
    int dy = 0;
};

struct NavCell {
    int x = 0;
    int y = 0;
    int shape = 0;
    bool walkable = false;
};

struct NavReport {
    std::string method;
    int hidden = 0;
    double error = 0.0;
    bool reached = false;
    int samples = 0;
};

class AvatarNavController {
public:
    static constexpr int kStep = 64;

    ~AvatarNavController();

    bool loadReferenceMap(const std::string& path);

    int startX() const { return startX_; }
    int startY() const { return startY_; }
    int targetX() const { return targetX_; }
    int targetY() const { return targetY_; }

    const NavCell* cellAt(int x, int y) const;
    const std::vector<NavCell>& cells() const { return cells_; }

    // Remember one player control taken at (x, y) while aiming at the target.
    void recordPlayerControl(int x, int y, int targetX, int targetY,
                             PlayerControl control);

    // Walk every reference cell the way a player would (shortest path) and
    // record those controls. Returns how many controls were stored.
    int demonstratePlayerRoute(int targetX, int targetY);

    // Train several gneural configurations on the recorded controls and keep
    // the one that reaches the target.
    bool trainOptimal(int targetX, int targetY);

    // Record the player route to a walkable cell and train for that target.
    // A failed training leaves the previous route in place.
    bool aimAt(int x, int y);

    PlayerControl predict(int x, int y, int targetX, int targetY);

    // Follow the trained net from start until the target, or maxSteps.
    bool moveToTarget(int startX, int startY, int targetX, int targetY,
                      std::vector<NavCell>* path, int maxSteps);

    const NavReport& report() const { return report_; }
    int sampleCount() const { return static_cast<int>(samples_.size()); }

private:
    struct Sample {
        int x, y, targetX, targetY;
        int dx, dy;
    };

    PlayerControl expertStep(int x, int y, int targetX, int targetY) const;
    void fillFeatures(int x, int y, int targetX, int targetY, double* features) const;
    bool reachable(int targetX, int targetY, int maxSteps);
    double worldDist(int x, int y, int tx, int ty) const;

    std::vector<NavCell> cells_;
    std::vector<Sample> samples_;
    int startX_ = 0;
    int startY_ = 0;
    int targetX_ = 0;
    int targetY_ = 0;

    struct Net; // gneural objects, owned in the .cpp
    Net* net_ = nullptr;
    NavReport report_;
};

}  // namespace Navigation
}  // namespace NPC
}  // namespace Ultima

#endif
