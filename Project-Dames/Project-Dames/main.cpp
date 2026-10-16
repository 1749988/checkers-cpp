#include <iostream>
#define SDL_MAIN_HANDLED
#include "libreria.h"
#include "joc.hpp"
#include "info_joc.hpp"

int main(int argc, char* argv[]) {
    try {
        Screen screen(TAMANY_PANTALLA_X,TAMANY_PANTALLA_Y);
        screen.show();
        Joc game;
        const bool replay=argc==3 && std::string(argv[1])=="--replay";
        game.inicialitza(replay?MODE_JOC_REPLAY:MODE_JOC_NORMAL,
                         "tauler_inicial.txt",replay?argv[2]:"recorded_moves.txt");
        do {
            screen.processEvents();
            game.actualitza(Mouse_getX(),Mouse_getY(),Mouse_getBtnLeft());
            screen.update();
        } while (!screen.isExit() && !Keyboard_GetKeyTrg(KEYBOARD_ESCAPE));
        game.finalitza();
        return 0;
    } catch (const std::exception& error) {
        std::cerr<<error.what()<<'\n'; return 1;
    }
}
