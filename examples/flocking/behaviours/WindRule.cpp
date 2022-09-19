#include "WindRule.h"
#include <imgui.h>
#include "../gameobjects/Boid.h"
#include "../gameobjects/World.h"

Vector2 WindRule::computeForce(const std::vector<Boid*>& neighborhood, Boid* boid) {
    // todo: add a wind force here
    // hint: use the windAngle variable

    Vector2 heading = Vector2::zero();

    heading = Vector2(cos(windAngle), sin(windAngle)).normalized();

    return heading;
}

bool WindRule::drawImguiRuleExtra() {
    ImGui::SetCurrentContext(world->engine->imGuiContext);
    bool valueHasChanged = false;

    if (ImGui::SliderAngle("Wind Direction", &windAngle, 0)) {
        valueHasChanged = true;
    }

    return valueHasChanged;
}