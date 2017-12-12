#include "data.hpp"
#include "json-c/json.h"
#include <cstring>
#include <windows.h>

std::vector<game_info> game_data::games_list;
sf::Font game_data::font;
int game_data::playable_minutes;

#define DEFAULT_PLAYTIME 15

/**
    Loads a list of games from a file. This uses json.h,v 1.6 to parse the file, and it's expected to be valid JSON.
    Invalid JSON files will result in the first error message.
    Unfortunately, providing detailed error messages is... annoying? Not obvious? unless you skip the convenience function.

    I use JSON because it's a lot less wordy than XML, which makes it a bit easier to read and understand by humans.
    It's not perfect, and a missing comma, or one in the wrong place can easily cause errors.
*/
bool loadGameData(std::string filename)
{
    json_object * jsonDoc = json_object_from_file(filename.c_str());

    if(jsonDoc == NULL)
    {
        printf("ERROR: gameslist.json is NULL.");
        return false;
    }

    //seriously, it needs to be a JSON array
    if(!json_object_is_type(jsonDoc, json_type_array))
    {
        printf("ERROR: gameslist.json is not an array.");
        return false;
    }

    int i = 0;

    for(; i < json_object_array_length(jsonDoc); i++)
    {
        printf("Loading gameinfo %i of %i.\n", i, json_object_array_length(jsonDoc));
        game_info temp;
        temp.commandLine = "";
        temp.playtime = DEFAULT_PLAYTIME;
        bool loaded = true;
        json_object * game = json_object_array_get_idx(jsonDoc, i);
        {
            json_object_object_foreach(game, key, val)
            {
                if(strcmp(key, "name") == 0)
                {
                    temp.name = json_object_get_string(val);
                } else if(strcmp(key, "description") == 0)
                {
                    temp.description = json_object_get_string(val);
                } else if(strcmp(key, "playtime") == 0)
                {
                    temp.playtime = json_object_get_int(val);
                } else if(strcmp(key, "directory") == 0)
                {
                    temp.directory = json_object_get_string(val);
                    //we want to make sure any directories listed in the definition file end in \, otherwise, we get errors when launching the program
                    if(temp.directory.back() != '\\')
                    {
                        temp.directory.push_back('\\');
                    }
                } else if(strcmp(key, "exe") == 0)
                {
                    temp.exe = json_object_get_string(val);
                } else if(strcmp(key, "arguments") == 0)
                {
                    temp.commandLine = json_object_get_string(val);
                } else if(strcmp(key, "icon") == 0)
                {
                    if(!temp.icon.loadFromFile(json_object_get_string(val)))
                    {
                        loaded = false;
                    }
                } else if(strcmp(key, "screenshots") == 0)
                {
                    //we load any screenshots into textures here.
                    //they'll sit in memory and be ready to go upon the user selecting the game from the list
                    for(int s = 0; s < json_object_array_length(val); s++)
                    {
                        sf::Texture temp_screenshot;
                        if(temp_screenshot.loadFromFile(json_object_get_string(json_object_array_get_idx(val, s))))
                        {
                            temp.screenshots.push_back(temp_screenshot);
                        } else {
                            printf("Error: Failed to load screenshot %i. Filename: %s\n", s, json_object_get_string(json_object_array_get_idx(val, s)));
                        }
                    }
                }
            }
        }
        //if everything else went well, we check the fullpath of the file and make sure it's not a directory.
        //This'll prevent the user from even having the option to select an invalid game
        if(loaded)
        {
            std::string fullpath = temp.directory + temp.exe;

            DWORD dwAttrib = GetFileAttributes(fullpath.c_str());

            if(dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
            {
                game_data::games_list.push_back(temp);
            } else {
                printf("ERROR: Failed to find exe file at %s.\n", fullpath.c_str());
            }
        }
    }
    if(game_data::games_list.size() > 0)
    {
        printf("Loaded %i of %i games successfully.", game_data::games_list.size(), i);
        return true;
    } else {
        printf("ERROR: Tried to load %i games. No game successfully loaded.", i);
        return false;
    }
}

//all these values are hard coded. I apologize.
bool game_data::init()
{
    game_data::playable_minutes = DEFAULT_PLAYTIME;
    game_data::font.loadFromFile("OpenSans.ttf");

    if(loadGameData("gameslist.json"))
    {
        return true;
    }
    return false;
}

//get rid of any lingering data
void game_data::unload()
{
    game_data::games_list.clear();
}

#include <sstream>

/**
    the purpose of this function is to take a string and add carriage returns after anything detected as a word, up to a certain number of characters
    the main purpose of this is to keep long blocks of text to fit within a certain width of screenspace
    this needs to be passed a character length value, as opposed to pixel values
    so any calculations of how wide something should be need to occur prior to calling this function
*/
std::string split_by_words(std::string const & in, uint8_t length)
{
    std::string::size_type current, next_space, next_nl;
    current = 0;

    std::stringstream out;

    while(in.length() > current)
    {
        next_nl = in.find("\n", current);
        next_space = in.find(" ", current + length);
        if( (next_nl == std::string::npos) && (next_space == std::string::npos) )
        {
            out<<in.substr(current);
            current = in.length();
        } else if(next_nl > next_space)
        {
            out<<in.substr(current, next_space - current)<<"\n";
            current = next_space + 1;
        } else {
            out<<in.substr(current, next_nl - current + 1);
            current = next_nl + 1;
        }
    }

    return out.str();
}
