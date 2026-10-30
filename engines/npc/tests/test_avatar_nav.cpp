/*
 * test_avatar_nav.cpp
 *
 * Trains gneural-net on player controls over the Tenebrae dock reference
 * map and checks that the kept network walks to the target coordinate.
 */

#include "test_framework.h"
#include "navigation/AvatarNavController.h"

#include <iostream>
#include <string>
#include <vector>

using Ultima::NPC::Navigation::AvatarNavController;
using Ultima::NPC::Navigation::NavCell;
using Ultima::NPC::Navigation::PlayerControl;

#ifndef NAV_MAP_PATH
#define NAV_MAP_PATH "dock_nav.map"
#endif

bool test_reference_map_coordinates_and_shapes() {
    AvatarNavController nav;
    TEST_ASSERT(nav.loadReferenceMap(NAV_MAP_PATH));
    TEST_ASSERT_EQUAL(11808, nav.startX());
    TEST_ASSERT_EQUAL(5520, nav.startY());
    TEST_ASSERT_EQUAL(11936, nav.targetX());
    TEST_ASSERT_EQUAL(6160, nav.targetY());

    const NavCell* dock = nav.cellAt(11808, 5520);
    TEST_ASSERT_NOT_NULL(dock);
    TEST_ASSERT_EQUAL(164, dock->shape);
    TEST_ASSERT(dock->walkable);

    const NavCell* plank = nav.cellAt(11552, 5520);
    TEST_ASSERT_NOT_NULL(plank);
    TEST_ASSERT_EQUAL(153, plank->shape);
    TEST_ASSERT(!plank->walkable);

    const NavCell* goal = nav.cellAt(11936, 6160);
    TEST_ASSERT_NOT_NULL(goal);
    TEST_ASSERT_EQUAL(164, goal->shape);
    TEST_ASSERT(goal->walkable);
    return true;
}

bool test_player_controls_are_recorded() {
    AvatarNavController nav;
    TEST_ASSERT(nav.loadReferenceMap(NAV_MAP_PATH));
    const int shown = nav.demonstratePlayerRoute(nav.targetX(), nav.targetY());
    TEST_ASSERT(shown > 10);
    TEST_ASSERT_EQUAL(shown, nav.sampleCount());

    const int before = nav.sampleCount();
    nav.recordPlayerControl(nav.startX(), nav.startY(), nav.targetX(), nav.targetY(),
                            PlayerControl{1, 1});
    TEST_ASSERT_EQUAL(before + 1, nav.sampleCount());
    return true;
}

bool test_trained_controls_reach_target() {
    AvatarNavController nav;
    TEST_ASSERT(nav.loadReferenceMap(NAV_MAP_PATH));
    const int shown = nav.demonstratePlayerRoute(nav.targetX(), nav.targetY());
    TEST_ASSERT(shown > 10);

    const bool trained = nav.trainOptimal(nav.targetX(), nav.targetY());
    std::cout << "\n    kept " << nav.report().method
              << " hidden=" << nav.report().hidden
              << " error=" << nav.report().error
              << " samples=" << nav.report().samples << std::endl;
    TEST_ASSERT(trained);
    TEST_ASSERT(nav.report().reached);

    std::vector<NavCell> path;
    const bool arrived = nav.moveToTarget(nav.startX(), nav.startY(),
                                          nav.targetX(), nav.targetY(),
                                          &path, 40);
    if (!path.empty()) {
        std::cout << "    path";
        for (const NavCell& cell : path) {
            std::cout << " (" << cell.x << "," << cell.y << ")";
        }
        std::cout << std::endl;
    }
    TEST_ASSERT(arrived);
    TEST_ASSERT(!path.empty());
    TEST_ASSERT_EQUAL(nav.startX(), path.front().x);
    TEST_ASSERT_EQUAL(nav.startY(), path.front().y);
    TEST_ASSERT_EQUAL(nav.targetX(), path.back().x);
    TEST_ASSERT_EQUAL(nav.targetY(), path.back().y);
    for (const NavCell& cell : path) {
        TEST_ASSERT(cell.walkable);
        TEST_ASSERT_EQUAL(164, cell.shape);
    }
    return true;
}

bool test_aim_rejects_blocked_and_reaches_another_cell() {
    AvatarNavController nav;
    TEST_ASSERT(nav.loadReferenceMap(NAV_MAP_PATH));
    TEST_ASSERT(!nav.aimAt(11552, 5520));

    const int tx = 11616;
    const int ty = 4816;
    const NavCell* far = nav.cellAt(tx, ty);
    TEST_ASSERT_NOT_NULL(far);
    TEST_ASSERT(far->walkable);
    TEST_ASSERT_EQUAL(164, far->shape);

    const bool trained = nav.aimAt(tx, ty);
    std::cout << "\n    kept " << nav.report().method
              << " hidden=" << nav.report().hidden
              << " error=" << nav.report().error
              << " samples=" << nav.report().samples << std::endl;
    TEST_ASSERT(trained);
    TEST_ASSERT_EQUAL(tx, nav.targetX());
    TEST_ASSERT_EQUAL(ty, nav.targetY());

    std::vector<NavCell> path;
    const bool arrived = nav.moveToTarget(nav.startX(), nav.startY(), tx, ty, &path, 40);
    if (!path.empty()) {
        std::cout << "    path";
        for (const NavCell& cell : path) {
            std::cout << " (" << cell.x << "," << cell.y << ")";
        }
        std::cout << std::endl;
    }
    TEST_ASSERT(arrived);
    TEST_ASSERT_EQUAL(nav.startX(), path.front().x);
    TEST_ASSERT_EQUAL(nav.startY(), path.front().y);
    TEST_ASSERT_EQUAL(tx, path.back().x);
    TEST_ASSERT_EQUAL(ty, path.back().y);
    for (const NavCell& cell : path) {
        TEST_ASSERT(cell.walkable);
        TEST_ASSERT_EQUAL(164, cell.shape);
    }
    return true;
}

int main() {
    TEST_SUITE("Avatar navigation");

    RUN_TEST("Reference map coordinates and shapes", test_reference_map_coordinates_and_shapes);
    RUN_TEST("Player controls are recorded", test_player_controls_are_recorded);
    RUN_TEST("Trained controls reach the target", test_trained_controls_reach_target);
    RUN_TEST("Aim reaches another dock cell", test_aim_rejects_blocked_and_reaches_another_cell);

    TEST_SUMMARY();
}
