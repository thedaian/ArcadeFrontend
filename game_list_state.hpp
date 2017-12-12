#include "core.hpp"
#include "data.hpp"

/**
    Contains both the game list code, and single game screen code.
    Also contains all the code surrounding the launching of games (much of which is specific to Windows).
*/
class arcade_list : public core {
private:
    std::vector<sf::Sprite> sprite_list;
    std::vector<sf::Sprite> screenshots;
    sf::Sprite selected_game_card;
    sf::View game_list_view, screenshot_view;
    sf::Text game_name, description, play, screenshot_txt;
    sf::Text playMe;
    sf::Shader m_shader;
    sf::RectangleShape bg, description_bg, screenshot_bg, play_bg, single_game_bg;//, back_bg;
    sf::Clock to_clock, clock, no_input_timer;
    int games_start, games_end, shots_start, shots_end, width, height, moveView, selected, mode, screenshot_width, screenshot_height;

    bool overPlayButton, playMeTransition, playMeActive;

    void resetInputTimer();
    void exitSingleGame();
    void scrollUp();
    void scrollDown();
    bool updateSelection(int mX, int mY);
    void select();
    void loadScreenshots();
    void runGame(game_info &);

    void transitionTo(float percent);

    enum MODES {
        TO_LIST,
        LIST,
        TO_SINGLE_GAME,
        SINGLE_GAME
    };
public:
    arcade_list();
    ~arcade_list();

    void handleEvent(sf::Event event);
    void update();
    void draw();
};
