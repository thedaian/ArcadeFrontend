#ifndef GAME_DATA
#define GAME_DATA

#include <SFML/Graphics.hpp>
#include <vector>

//detailed comments for these functions are within data.cpp
//this structure, and this static class hold info about the games loaded from a JSON file
struct game_info {
    std::vector<sf::Texture> screenshots;
    std::string name, description, directory, exe, commandLine;
    sf::Texture icon;
    int playtime;
};

class game_data {
public:
    static sf::Font font;
    static int playable_minutes;
    static std::vector<game_info> games_list;
    static bool init();
    static void unload();
};

//utility function used in game_list_state.cpp. stored here to keep that file slightly cleaner (it's already a bit big)
std::string split_by_words(std::string const & in, uint8_t length);

#endif // GAME_DATA
