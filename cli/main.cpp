#include "tauler.hpp"
#include "cuamoviments.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

int main(int argc,char* argv[]) {
    try {
        std::string board="tauler_inicial.txt",record="recorded_moves.txt",replay;
        for (int i=1;i<argc;++i) {
            const std::string arg=argv[i];
            if (arg=="--help") {
                std::cout<<"checkers [--board FILE] [--record FILE] [--replay FILE]\n"
                         <<"Enter a move such as a3 b4. Use moves a3, save or quit.\n";
                return 0;
            }
            if (i+1>=argc) throw std::invalid_argument("Missing option value");
            if (arg=="--board") board=argv[++i];
            else if (arg=="--record") record=argv[++i];
            else if (arg=="--replay") replay=argv[++i];
            else throw std::invalid_argument("Unknown option: "+arg);
        }
        Tauler game; game.inicialitza(board);
        CuaMoviments history;
        ColorFitxa turn=COLOR_BLANC;
        auto play=[&](const Moviment& move) {
            if (move.longitud()<2) return false;
            const auto& seq=move.getSeq();
            const Fitxa* piece=game.getFitxa(seq.front());
            if (!piece || piece->getColor()!=turn) return false;
            if (!game.mouFitxa(move) && !(seq.size()==2 && game.mouFitxa(seq.front(),seq.back()))) return false;
            history.encola(game.getUltimMoviment());
            turn=turn==COLOR_BLANC?COLOR_NEGRE:COLOR_BLANC;
            return true;
        };
        if (!replay.empty()) {
            CuaMoviments input; input.cargaDeFichero(replay);
            int count=0;
            while (!input.estaVacia()) {
                if (!play(input.desencola())) throw std::runtime_error("Illegal replay move "+std::to_string(count+1));
                ++count;
                std::cout<<"Move "<<count<<'\n'<<game.toString();
            }
            std::cout<<"Replay complete: "<<count<<" moves.\n";
            return 0;
        }
        std::cout<<"Checkers C++ | O/D: white; X/R: black | White moves first.\n";
        std::string line;
        while (true) {
            std::cout<<game.toString();
            if (!game.teMoviments(turn)) {
                std::cout<<(turn==COLOR_BLANC?"Black":"White")<<" wins: opponent has no legal move.\n";
                break;
            }
            std::cout<<(turn==COLOR_BLANC?"White":"Black")<<"> ";
            if (!std::getline(std::cin,line) || line=="quit") break;
            if (line=="save") { history.guardaEnFichero(record); std::cout<<"Saved to "<<record<<'\n'; continue; }
            try {
                std::istringstream input(line);
                std::string word; input>>word;
                if (word=="moves") {
                    std::string square; if (!(input>>square)) throw std::invalid_argument("Use moves a3");
                    Posicio origin(square);
                    const auto* piece=game.getFitxa(origin);
                    if (!piece || piece->getColor()!=turn) { std::cout<<"Choose one of your pieces.\n"; continue; }
                    for (const auto& move:game.getMovimentsPossibles(origin)) {
                        for (const auto& p:move.getSeq()) std::cout<<p<<' ';
                        std::cout<<'\n';
                    }
                    continue;
                }
                if (word.empty()) continue;
                Moviment move{Posicio(word)};
                while (input>>word) move.afegeix(Posicio(word));
                if (!play(move)) std::cout<<"Illegal move. Captures and the longest capture sequence are mandatory.\n";
            } catch (const std::invalid_argument& error) { std::cout<<error.what()<<'\n'; }
        }
        if (!history.estaVacia()) history.guardaEnFichero(record);
        return 0;
    } catch (const std::exception& error) { std::cerr<<error.what()<<'\n'; return 1; }
}
