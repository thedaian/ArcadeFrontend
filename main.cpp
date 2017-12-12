#include "game_list_state.hpp"
#include "data.hpp"

#include <cstring>
#include <ctime>

int main()
{
    /**
        The program uses printf to log errors, as well as output information such as how many games were successfully loaded into the program, and what games were chosen.
        We redirect the output of stdout so that the file run.log contains those messages from this point forward. We also log what time the program started running, as well as the end time
    */
    freopen ("run.log","w",stdout);

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [10];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,10,"%H:%M:%S",timeinfo);
    fprintf(stdout, "Arcade Frontend startup time: %s\n", buffer);

    //loads game data. See data.hpp for further information
    if(!game_data::init())
    {
        return EXIT_FAILURE;
    }

    core * state = new arcade_list();

	/**
        Fairly basic game loop. The core.hpp class handles basic game/window functions, see that file for more information.
	*/
    while (state->isOpen())
    {
        sf::Event event;
        while(state->getApp()->pollEvent(event))
        {
            state->handleEvent(event);
        }
        state->update();
        state->clearScreen();
        state->draw();
        state->display();
    }

    //cleans up game data
    game_data::unload();

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,10,"%H:%M:%S",timeinfo);
    fprintf(stdout, "\nArcade Frontend end time: %s", buffer);

    fclose(stdout);

    return EXIT_SUCCESS;
}
