#include "game_list_state.hpp"
#include "data.hpp"

#define PICTURE_HEIGHT  270
#define PICTURE_WIDTH   480
#define BG_COLOR_FULL sf::Color(128, 128, 128, 255)
#define BG_COLOR_TRANSPARENT sf::Color(128, 128, 128, 255)
#define PLAY_BUTTON sf::Color(128, 32, 32, 255)
#define FONT_COLOR sf::Color(0, 0, 0, 255)
#define PLAY_COLOR sf::Color(32, 128, 32, 255)

/**
    Yes, this code uses switch statements as a basic state machine.
    It's not great, but the program is meant to be fairly simple, so it doesn't need a lot of added complexity.
*/


/******************************************************************************************************************************************************

                                                        BEGIN EVENT AND INPUT CODE

******************************************************************************************************************************************************/


/**
    Wherever possible, successful events should call other functions,
    as multiple buttons and input devices may/should be used during the course of the arcade cabinet software
*/
void arcade_list::handleEvent(sf::Event event)
{
    if(event.type == sf::Event::KeyReleased)
    {
        this->resetInputTimer();
        switch(event.key.code)
        {
        case sf::Keyboard::Escape:
            if(this->mode == MODES::SINGLE_GAME || this->mode == MODES::TO_SINGLE_GAME)
            {
                this->exitSingleGame();
            }
            break;
        case sf::Keyboard::Down:
            this->scrollDown();
            break;
        case sf::Keyboard::Up:
            this->scrollUp();
            break;
        case sf::Keyboard::W:
            this->scrollUp();
            break;
        case sf::Keyboard::A:
            this->scrollUp();
            break;
        case sf::Keyboard::S:
            this->scrollDown();
            break;
        case sf::Keyboard::D:
            this->scrollDown();
            break;
        case sf::Keyboard::Z:
            //reset the game list and screenshot views to default
            this->game_list_view = sf::View(sf::Vector2f(this->app.getSize().x/2, this->app.getSize().y/2), sf::Vector2f(this->app.getSize().x, this->app.getSize().y));
            this->screenshot_view = sf::View(sf::Vector2f(this->app.getSize().x/2, this->app.getSize().y/2), sf::Vector2f(this->app.getSize().x, this->app.getSize().y));
            break;
        default:
            break;
        }
    }
    if(event.type == sf::Event::MouseWheelMoved)
    {
        this->resetInputTimer();
        if(event.mouseWheel.delta > 0)
        {
            this->scrollUp();
        } else if(event.mouseWheel.delta < 0)
        {
            this->scrollDown();
        }
    }
    if(event.type == sf::Event::MouseMoved)
    {
        if(this->updateSelection(event.mouseMove.x, event.mouseMove.y))
        {
            this->resetInputTimer();
        }
    }
    if(event.type == sf::Event::MouseButtonReleased)
    {
        if(this->updateSelection(event.mouseButton.x, event.mouseButton.y))
        {
            this->resetInputTimer();
        }
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            this->select();
        }
    }
    core::handleEvent(event);
}

void arcade_list::scrollDown()
{
    switch(this->mode)
    {
    case MODES::LIST:
        if(this->games_end < this->sprite_list.size())
        {
            this->games_start += this->width;
            this->games_end += this->width;
            this->moveView = PICTURE_HEIGHT + 10 + this->moveView;
        }
        break;
    case MODES::SINGLE_GAME:
        if(this->shots_start < this->screenshots.size() - 1)
        {
            this->shots_start++;
            this->shots_end++;
        } else {
            this->shots_start = 0;
            this->shots_end = 1;
            this->screenshot_view.move(-((screenshot_width + 10) * (int)this->screenshots.size()), 0);
        }
        this->moveView = screenshot_width + 10 + this->moveView;
        break;
    }
}

void arcade_list::scrollUp()
{
    switch(this->mode)
    {
    case MODES::LIST:
        if(this->games_start > 0)
        {
            this->games_start -= this->width;
            this->games_end -= this->width;
            this->moveView = -PICTURE_HEIGHT - 10 + this->moveView;
        }
        break;
    case MODES::SINGLE_GAME:
        if(this->shots_start > 0)
        {
            this->shots_start--;
            this->shots_end--;
        } else {
            this->shots_start = this->screenshots.size() - 1;
            this->shots_end = this->screenshots.size();
            this->screenshot_view.move(((screenshot_width + 10) * (int)this->screenshots.size()), 0);
        }
        this->moveView = -screenshot_width - 10 + this->moveView;
        break;
    }
}

bool arcade_list::updateSelection(int mX, int mY)
{
    if(this->mode == MODES::LIST)
    {
        sf::Vector2f m = this->app.mapPixelToCoords(sf::Vector2i(mX, mY), this->game_list_view);
        for(int i = 0; i < this->sprite_list.size(); i++)
        {
            if(sprite_list[i].getGlobalBounds().contains(m))
            {
                this->selected = i;
                this->game_name.setString(game_data::games_list[i].name);
                return true;
            }
        }
        this->selected = -1;
    } else {
        if(this->play_bg.getGlobalBounds().contains(mX, mY))
        {
            if(!this->overPlayButton)
            {
                this->play.setColor(PLAY_COLOR);
                this->overPlayButton = true;
            }
        } else {
            if(this->overPlayButton)
            {
                this->play.setColor(FONT_COLOR);
                this->overPlayButton = false;
            }
        }
    }
    return false;
}

void arcade_list::select()
{
    switch(this->mode)
    {
    case MODES::LIST:
        if(this->selected >= 0)
        {
            this->mode = MODES::TO_SINGLE_GAME;
            this->moveView = 0;
            this->to_clock.restart();
            this->bg.setFillColor(BG_COLOR_FULL);
            this->description.setString(game_data::games_list[this->selected].description);
            this->selected_game_card.setTexture(game_data::games_list[this->selected].icon);
        }
        break;
    case MODES::SINGLE_GAME:
        if(!this->overPlayButton)
        {
            this->exitSingleGame();
        } else {
            //run program!!
            game_data::playable_minutes = game_data::games_list[this->selected].playtime;
            this->runGame(game_data::games_list[this->selected]);
        }
        break;
    }

}

/******************************************************************************************************************************************************

                                                        BEGIN NON-INPUT CODE

******************************************************************************************************************************************************/

#define TO_COUNT 16.0
#define PLAYME_TEXT_FADE 8

void arcade_list::update()
{
    switch(this->mode)
    {
    case MODES::TO_LIST:
        if(this->to_clock.getElapsedTime().asMilliseconds() > 20)
        {
            if(this->moveView <= TO_COUNT)
            {
                this->to_clock.restart();
                this->transitionTo(1 - (float)this->moveView/TO_COUNT);
                this->moveView++;
            } else {
                this->mode = MODES::LIST;
                this->moveView = 0;
            }
        }
        break;
    case MODES::TO_SINGLE_GAME:
        if(this->to_clock.getElapsedTime().asMilliseconds() > 20)
        {
            if(this->moveView <= TO_COUNT)
            {
                this->to_clock.restart();
                this->transitionTo((float)this->moveView/TO_COUNT);
                this->moveView++;
            } else {
                this->mode = MODES::SINGLE_GAME;
                this->moveView = 0;
                this->loadScreenshots();
            }
        }

        break;
    case MODES::LIST:
        {
            if((this->moveView > 8) || (this->moveView < -8))
            {
                this->game_list_view.move(0, this->moveView/8);
                this->moveView -= this->moveView/8;
            } else if(this->moveView != 0)
            {
                this->game_list_view.move(0, this->moveView);
                this->moveView = 0;
            }
            this->m_shader.setParameter("wave_phase", clock.getElapsedTime().asSeconds());
            if(this->no_input_timer.getElapsedTime().asSeconds() > 60)
            {
                this->playMeActive = true;
                this->playMeTransition = true;
            }
            //fading the "select a game" text in or out
            if(this->playMeTransition)
            {
                sf::Color mchg = this->playMe.getColor();
                if(this->playMeActive)
                {
                    if(mchg.a < 255 - PLAYME_TEXT_FADE)
                    {
                        mchg.a += PLAYME_TEXT_FADE;
                    } else if(mchg.a >= 255 - PLAYME_TEXT_FADE)
                    {
                        mchg.a = 255;
                        this->playMeTransition = false;
                    }
                } else {
                    if(mchg.a > PLAYME_TEXT_FADE)
                    {
                        mchg.a -= PLAYME_TEXT_FADE;
                    } else if(mchg.a <= PLAYME_TEXT_FADE + 1)
                    {
                        mchg.a = 0;
                        this->playMeTransition = false;
                    }
                }
                this->playMe.setColor(mchg);
            }

            break;
        }

    case MODES::SINGLE_GAME:
        if((this->moveView > 8) || (this->moveView < -8))
        {
            this->screenshot_view.move(this->moveView/8, 0);
            this->moveView -= this->moveView/8;
        } else if(this->moveView != 0)
        {
            this->screenshot_view.move(this->moveView, 0);
            this->moveView = 0;
        }
        break;
    }
}

void arcade_list::draw()
{
    switch(this->mode)
    {
    default:
    case MODES::LIST:
    case MODES::TO_LIST:
    case MODES::TO_SINGLE_GAME:
        this->app.setView(this->game_list_view);
        for(int i = 0; i < this->sprite_list.size(); i++)
        {
            this->app.draw(sprite_list[i]);
        }
        this->app.setView(this->app.getDefaultView());
        if(this->mode == MODES::LIST)
        {
            this->app.draw(this->bg);
            this->app.draw(this->game_name);
            if(this->playMeActive || this->playMeTransition)
            {
                this->app.draw(this->playMe, &this->m_shader);
            }
            break;
        }
        this->app.draw(this->single_game_bg);

        this->app.draw(this->bg);
        this->app.draw(this->game_name);
        this->app.draw(this->screenshot_bg);
        this->app.draw(this->description_bg);
        this->app.draw(this->play_bg);
        break;
    case MODES::SINGLE_GAME:
        this->app.draw(this->bg);
        this->app.draw(this->game_name);

        this->app.draw(this->screenshot_bg);
        this->app.setView(this->screenshot_view);
        if(this->screenshots.size() > 0)
        {
            this->app.draw(screenshots[this->shots_start]);
        }

        this->app.setView(this->app.getDefaultView());
        this->app.draw(this->description_bg);
        this->app.draw(this->selected_game_card);
        this->app.draw(this->description);
        this->app.draw(this->play_bg);
        this->app.draw(this->play);
        break;
    }
}

/**
    I'm not a huge fan of how the screenshots are handled at the moment.
    Different sized screenshots can cause bugs in the positioning, and
    trying to fit screenshots into a specific space just makes them look a bit ugly
    I don't have a particularly good solution to this at the moment, so for now, this is what I've got
*/
void arcade_list::loadScreenshots()
{
    this->screenshots.clear();

    if(game_data::games_list[this->selected].screenshots.size() > 0)
    {
        sf::Vector2u size = game_data::games_list[this->selected].screenshots.front().getSize();
        //int total = game_data::games_list[this->selected].screenshots.size();

        sf::Vector2f factor = sf::Vector2f((float)screenshot_width/size.x, (float)screenshot_height/size.y);

        int count = 0;
        for(auto tx = game_data::games_list[this->selected].screenshots.begin(), last = game_data::games_list[this->selected].screenshots.end(); tx != last; tx++)
        {
            this->screenshots.push_back(sf::Sprite((*tx)));
            this->screenshots.back().setPosition(this->screenshot_bg.getGlobalBounds().left + 15 + count * (screenshot_width + 10), this->screenshot_bg.getPosition().y + 20);
            this->screenshots.back().setScale(factor);
            count++;
        }
    }
    this->shots_start = 0;
    this->shots_end = 1;

    printf("loaded %i screenshots", this->screenshots.size());
}

void arcade_list::exitSingleGame()
{
    this->mode = MODES::TO_LIST;
    this->moveView = 0;
    this->to_clock.restart();
    this->bg.setFillColor(BG_COLOR_TRANSPARENT);
}

//move to single game screen. pass in 1 - percent to get the list screen
//essentially, 0% is the list screen, 100% is the single game view
void arcade_list::transitionTo(float percent)
{
    this->description_bg.setPosition((-this->description_bg.getGlobalBounds().width * (1.0 - percent)), this->description_bg.getPosition().y);
    this->screenshot_bg.setPosition(this->app.getSize().x - ((this->description_bg.getGlobalBounds().width - 50) * percent), this->screenshot_bg.getPosition().y);

    this->play_bg.setPosition(this->app.getSize().x/2 - 100, this->app.getSize().y - (80 * percent));

    this->bg.setPosition(0, (15 * (percent)) + ((this->app.getSize().y - 75) * (1 - percent)));
    this->game_name.setPosition(this->app.getSize().x/2 - 100, this->bg.getPosition().y + 10);

    this->single_game_bg.setFillColor(sf::Color(0, 0, 0, percent * 255));
}

/******************************************************************************************************************************************************

                                                        BEGIN STARTUP CODE

******************************************************************************************************************************************************/

const std::string shaderCode = R"(uniform float wave_phase;

void main()
{
    vec4 vertex = gl_Vertex;
    vertex.x += cos(gl_Vertex.y * 0.02 + wave_phase * 3.8)
              + sin(gl_Vertex.y * 0.02 + wave_phase * 6.3) * 0.3;
    vertex.y += sin(gl_Vertex.x * 0.02 + wave_phase * 2.4) * 20
              + cos(gl_Vertex.x * 0.02 + wave_phase * 5.2) * 0.3;

	gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
})";

arcade_list::arcade_list()
{
    this->m_shader.loadFromMemory(shaderCode, sf::Shader::Vertex);
    this->mode = MODES::LIST;
    core::init();
    this->width = this->app.getSize().x/PICTURE_WIDTH;
    this->height = this->app.getSize().y/PICTURE_HEIGHT;
    this->moveView = 0;
    this->selected = -1;
    this->games_start = 0;
    this->shots_start = 0;
    this->shots_end = 0;
    this->playMeActive = true;
    this->playMeTransition = false;

    this->playMe.setFont(game_data::font);
    this->playMe.setCharacterSize(105);
    this->playMe.setColor(sf::Color::Red);
    this->playMe.setStyle(sf::Text::Bold);
    this->playMe.setString("CHOOSE A GAME!");
    this->playMe.setPosition(this->app.getSize().x/2 - this->playMe.getGlobalBounds().width/2, this->app.getSize().y/2);

    for(int i = 0; i < game_data::games_list.size(); i++)
    {
        game_data::games_list[i].description = split_by_words(game_data::games_list[i].description, (((float)this->app.getSize().x/2)/12) - 2);
        sf::Sprite temp;
        temp.setTexture(game_data::games_list[i].icon);

        temp.setPosition((PICTURE_WIDTH + 5) * (i % this->width) + 5, (PICTURE_HEIGHT + 5) * (i / this->height));
        this->sprite_list.push_back(temp);
    }
    if(this->sprite_list.size() > this->width * this->height)
    {
        this->games_end = this->width * this->height;
    } else {
        this->games_end = this->sprite_list.size();
    }
    this->game_list_view = sf::View(sf::Vector2f(this->app.getSize().x/2, this->app.getSize().y/2), sf::Vector2f(this->app.getSize().x, this->app.getSize().y));

    this->game_name.setFont(game_data::font);
    this->game_name.setCharacterSize(24);
    this->game_name.setColor(FONT_COLOR);
    this->game_name.setString("Select a Game!");

    this->play_bg.setSize(sf::Vector2f(200, 60));
    this->play_bg.setPosition(this->app.getSize().x/2 - 100, this->app.getSize().y - 80);
    this->play_bg.setFillColor(PLAY_BUTTON);

    this->bg.setSize(sf::Vector2f(this->app.getSize().x, 50));
    this->bg.setPosition(0, this->app.getSize().y - 75);
    this->bg.setFillColor(BG_COLOR_TRANSPARENT);

    this->single_game_bg.setSize(sf::Vector2f(this->app.getSize().x, this->app.getSize().x));
    this->single_game_bg.setPosition(0, 0);
    this->single_game_bg.setFillColor(sf::Color(0, 0, 0));

    this->description_bg.setSize(sf::Vector2f(this->app.getSize().x/2, this->app.getSize().y - this->bg.getGlobalBounds().height - this->play_bg.getGlobalBounds().height - 100));
    this->description_bg.setPosition(-this->description_bg.getGlobalBounds().width, 100);
    this->description_bg.setFillColor(BG_COLOR_FULL);

    this->selected_game_card.setPosition(20, this->description_bg.getGlobalBounds().top + 20);

    this->description.setFont(game_data::font);
    this->description.setCharacterSize(20);
    this->description.setColor(FONT_COLOR);
    this->description.setString("test game description");
    this->description.setPosition(30, this->description_bg.getGlobalBounds().top + 40 + PICTURE_HEIGHT);

    this->screenshot_bg.setSize(sf::Vector2f(this->app.getSize().x/2, this->description_bg.getGlobalBounds().height));
    this->screenshot_bg.setPosition(this->app.getSize().x, this->description_bg.getGlobalBounds().top);
    this->screenshot_bg.setFillColor(BG_COLOR_FULL);

    this->screenshot_width = this->screenshot_bg.getGlobalBounds().width - 80;
    this->screenshot_height = this->screenshot_bg.getGlobalBounds().height - 40;

    this->screenshot_view = sf::View(sf::Vector2f(this->app.getSize().x/2, this->app.getSize().y/2), sf::Vector2f(this->app.getSize().x, this->app.getSize().y));

    this->screenshot_txt.setFont(game_data::font);
    this->screenshot_txt.setCharacterSize(48);
    this->screenshot_txt.setColor(FONT_COLOR);
    this->screenshot_txt.setString("screeshots");
    this->screenshot_txt.setPosition(10, 10);

    this->play.setFont(game_data::font);
    this->play.setCharacterSize(32);
    this->play.setColor(FONT_COLOR);
    this->play.setString("Play Game");
    this->play.setPosition(this->play_bg.getPosition().x + 20, this->play_bg.getPosition().y + 6);

    this->game_name.setPosition(this->app.getSize().x/2 - 100, this->bg.getPosition().y + 10);
    this->overPlayButton = false;

//printf("\napp size: %i by %i. view size: %i by %i and centre: %i, %i", this->app.getSize().x, this->app.getSize().y, this->view.getSize().x, this->view.getSize().y, this->view.getCenter().x, this->view.getCenter().y);
}

void arcade_list::resetInputTimer()
{
    if(this->playMeActive || this->playMeTransition)
    {
        this->playMeActive = false;
        this->playMeTransition = true;
    }

    this->no_input_timer.restart();
}

arcade_list::~arcade_list()
{
}

/******************************************************************************************************************************************************

                                                        BEGIN GAME LAUNCHING CODE

******************************************************************************************************************************************************/

#include <windows.h>

//yes, these are global variables.
//no, it's not that big of a problem.
HWND g_HWND = NULL;
DWORD window_process_id = 0;

//used to get the window handle of whatever game we're running/monitoring
BOOL CALLBACK EnumWindowsProcMy(HWND hwnd,LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd,&lpdwProcessId);
    if(lpdwProcessId==lParam)
    {
        g_HWND=hwnd;
        return FALSE;
    }
    return TRUE;
}

#define SECONDS_TO_WAIT (60 * game_data::playable_minutes)

void arcade_list::runGame(game_info & game)
{
    std::string fullPath = game.directory + game.exe;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    //command lines start with the EXE name, then arguments are passed in after that.
    //we need to combine those two strings to get the real command line, but we only care if there's actually a commandline value to be passed into the program
    char *cmd = NULL;
    if(game.commandLine.length() > 0)
    {
        cmd = new char[fullPath.length() + 1 + game.commandLine.length() + 1];
        strcpy(cmd, fullPath.c_str());
        cmd[fullPath.length()] = ' ';
        cmd[fullPath.length() + 1] = ' ';
        strcpy(cmd + fullPath.length() + 1, game.commandLine.c_str());
        printf("\ncommand line argument is: %s", cmd);
    }

    //si and pi contain information about the process after it starts
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
printf("\ncreating process from %s", fullPath.c_str());
    // start the program up
    if(CreateProcess( fullPath.c_str(),   // the path
        cmd,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        game.directory.c_str(),           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    ))
    {
        delete [] cmd;
        //waits for the new window to be running
        if(WaitForInputIdle(pi.hProcess, INFINITE) == 0)
        {
            // Close process and thread handles.
            CloseHandle( pi.hThread );
            CloseHandle( pi.hProcess );
            window_process_id = pi.dwProcessId;
            EnumWindows(EnumWindowsProcMy, pi.dwProcessId);
            while(g_HWND == NULL)
            {
                sf::sleep(sf::seconds(1));printf("\nWindow was null, waiting.");
            }
            printf("\nWindow found. Now monitoring for changes.");
            /*HWINEVENTHOOK g_hook;

            g_hook = SetWinEventHook(
                EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,  // Range of events
                NULL,                                          // Handle to DLL.
                HandleWinEvent,                                // The callback.
                pi.dwProcessId, pi.dwThreadId,              // Process and thread IDs of interest (0 = all)
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.
*/
            for(int counter = 0; counter < SECONDS_TO_WAIT; counter++)
            {
                //we process events once per second to prevent windows from seeing this window as "Not Responding"
                //it also lets us handle any close events that might occur as a result of something else occurring.
                sf::Event event;
                while(this->app.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                    {
                        this->app.close();
                        g_HWND = NULL;
                        break;
                    }
                }
                sf::sleep(sf::seconds(1));
                //If the game got closed by the user in the game, we break out of this loop because we're done waiting
                //IsHungAppWindow requires using Visual Studio/Microsoft's compiler, but it'll check if a window is reported as "Not Responding"
                if(/*IsHungAppWindow(g_HWND) || */(IsWindow(g_HWND) == 0))
                {
                    g_HWND = NULL;
                printf("\nWINDOW closed manually!");
                    break;
                }
            }
            //if we've waited long enough, (default is 15 minutes), then we issue a close window command.
            //PostMessage is the safest way of doing it, as it lets the game exit gracefully.
            if(g_HWND != NULL)
            {
                if(PostMessage(g_HWND, WM_CLOSE, 0, 0) == 0)
                {
                    printf("\nCLOSE MESSAGE FAILED TO SEND!");
                }
                printf("\nWINDOW closed automatically!");
            }

        }

    } else {
        printf("\nERROR CREATING PROCESS!!!");
        delete [] cmd;
    }
}
