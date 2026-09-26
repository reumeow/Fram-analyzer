#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/file.hpp>

using namespace geode::prelude;

// Структура для хранения информации о клике из реплея
struct ReplayInput {
    int frame;
    bool down; // true = нажатие, false = отпускание
};

// Глобальное хранилище текущего загруженного реплея
static std::vector<ReplayInput> g_loadedReplay;
static bool g_replayLoaded = false;
static std::string g_replayName = "None";

// Функция для простого парсинга .gdr / .2gdr файлов
void parseReplayFile(std::filesystem::path const& path) {
    g_loadedReplay.clear();
    g_replayLoaded = false;

    auto readResult = utils::file::readData(path);
    if (!readResult) {
        log::info("Failed to read replay file");
        return;
    }

    auto data = readResult.unwrap();
    if (data.size() < 16) return; // Слишком маленький файл для валидного реплея

    // Упрощенный парсинг: ищем структуры вводов в бинарных данных реплея
    // Стандартные реплеи содержат последовательность кадров и действий
    // Для теста заполним заглушку или пройдемся по байтам
    
    // Пример симуляции чтения вводов для демонстрации работы интерфейса
    // (Полноценный .gdr парсер учитывает версию заголовка, fps и т.д.)
    for (size_t i = 0; i < data.size() - 8; i += 8) {
        int frame = *reinterpret_cast<int*>(data.data() + i);
        if (frame > 0 && frame < 1000000) {
            g_loadedReplay.push_back({ frame, true });
        }
    }

    g_replayLoaded = true;
    g_replayName = path.filename().string();
    log::info("Replay loaded successfully: {} (Inputs: {})", g_replayName, g_loadedReplay.size());
}

// Хук на меню паузы для добавления нашей кнопки выбора реплея
class $modify(AnalyzerPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        // Ищем меню справа или по центру на паузе
        auto winSize = CCDirector::get()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPosition({ 40, winSize.height - 40 });

        auto btnSprite = CircleButtonSprite::createWithSpriteName("geode.loader/gear.png", 0.8f, CircleBaseColor::Green, CircleTier::Base);
        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(AnalyzerPauseLayer::onSelectReplay)
        );

        menu->addChild(btn);
        this->addChild(menu, 100);
    }

    void onSelectReplay(CCObject*) {
        // Открываем проводник для выбора .gdr / .2gdr файлов
        utils::file::pick(
            utils::file::PickMode::OpenFile,
            utils::file::FilePickOptions{
                .filters = {
                    utils::file::FilePickOptions::Filter{
                        .description = "Geometry Dash Replays",
                        .files = { "*.gdr", "*.2gdr" }
                    }
                }
            }
        ).listen([] (utils::file::FilePickResult* result) {
            if (result && result->path()) {
                parseReplayFile(result->path().value());
                FLAlertLayer::create("ReuHUB Analyzer", "Replay loaded and analyzed successfully!", "OK")->show();
            }
        });
    }
};

// Хук на уровень для визуализации кадров во время игры
class $modify(AnalyzerPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        std::string statusText = g_replayLoaded ? "Loaded: " + g_replayName : "No Replay Loaded (Click gear in pause)";
        
        auto label = CCLabelBMFont::create(statusText.c_str(), "chatFont.fnt");
        label->setPosition({ winSize.width / 2, winSize.height - 15 });
        label->setScale(0.4f);
        label->setOpacity(200);
        this->addChild(label, 1000);

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!g_replayLoaded || !m_player1) return;

        int currentFrame = this->m_time * 240.0f; // Примерный расчет текущего кадра (при 240fps)

        // Здесь в следующем шаге мы пройдемся по g_loadedReplay, 
        // посчитаем фрейм-винды (1ф, 2-3ф, 4-6ф, 7-9ф) и нарисуем кружки на экране!
    }
};
