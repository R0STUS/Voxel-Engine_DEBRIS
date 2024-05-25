#ifndef FRONTEND_MENU_MENU_H_
#define FRONTEND_MENU_MENU_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../content/ContentPack.h"
#include "../../engine.h"


namespace gui {
    class Panel;
}

class Engine;
class LevelController;

using packconsumer = std::function<void(const ContentPack& pack)>;

namespace menus {
    // implemented in menu_settings.cpp 
    extern void create_settings_panel(Engine* engine);

    // implemented in menu_create_world.cpp
    extern void create_new_world_panel(Engine* engine);

    // implemented in menu_create_world.cpp
    extern void create_world_generators_panel(Engine* engine);

    // implemented in menu_pause.cpp
    extern void create_pause_panel(Engine* engine, LevelController* controller);
    extern std::shared_ptr<gui::Panel> create_packs_panel(
        const std::vector<ContentPack>& packs, 
        Engine* engine, 
        bool backbutton, 
        packconsumer callback,
        packconsumer remover
    );

    /// @brief Load world, convert if required and set to LevelScreen.
    /// @param name world name
    /// @param engine engine instance
    /// @param confirmConvert automatically confirm convert if requested
    void open_world(std::string name, Engine* engine, bool confirmConvert);
    void open_dimension(std::string name, Engine* engine, bool confirmConvert);

    /// @brief Create development version label at the top-right screen corner
    void create_version_label(Engine* engine);
    void create_menus(Engine* engine);
    void refresh_menus(Engine* engine);
}

#endif // FRONTEND_MENU_MENU_H_
