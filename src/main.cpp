#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(AnalyzerPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) {
            return false;
        }

        log::info("ReuHUB Frame Analyzer active!");
        return true;
    }
};
