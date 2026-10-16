#include "cuamoviments.hpp"
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include "moviment.hpp"


// Constructor: comença amb cua buida
CuaMoviments::CuaMoviments()
    : cap(nullptr), cua(nullptr) {}

// Destructor: allibera tots els nodes pendents
CuaMoviments::~CuaMoviments() {
    vaciar();
}

void CuaMoviments::encola(const Moviment& m) {
    if (m.longitud() < 2) throw std::invalid_argument("Move needs at least two positions");
    Nodo* nou = new Nodo(m);
    if (cua) {
        cua->seguent = nou;
    }
    else {
        cap = nou;
    }
    cua = nou;
}

Moviment CuaMoviments::desencola() {
    if (!cap) {
        throw std::runtime_error("Cua buida: no hi ha moviments per reproducir");
    }
    Nodo* actual = cap;
    Moviment m = actual->moviment;
    cap = cap->seguent;
    if (!cap) {
        cua = nullptr;
    }
    delete actual;
    return m;
}

bool CuaMoviments::estaVacia() const {
    return cap == nullptr;
}

void CuaMoviments::vaciar() {
    while (cap) {
        Nodo* seguent = cap->seguent;
        delete cap;
        cap = seguent;
    }
    cua = nullptr;
}

void CuaMoviments::guardaEnFichero(const std::string& nomFitxer) {
    std::ofstream out(nomFitxer);
    if (!out) {
        throw std::runtime_error("No es pot obrir fitxer per escriure: " + nomFitxer);
    }
    for (Nodo* it = cap; it; it = it->seguent) {
        const std::vector<Posicio>& seq = it->moviment.getSeq();
        for (std::size_t i=0;i<seq.size();++i) {
            if (i) out << ' ';
            out << seq[i].toString();
        }
        out << '\n';
    }
    if (!out) throw std::runtime_error("Cannot write moves: " + nomFitxer);
}

void CuaMoviments::cargaDeFichero(const std::string& nomFitxer) {
    std::ifstream in(nomFitxer);
    if (!in) {
        throw std::runtime_error("No es pot obrir fitxer per llegir: " + nomFitxer);
    }
    std::vector<Moviment> loaded;
    std::string line;
    while (std::getline(in,line)) {
        std::istringstream row(line);
        std::string square;
        Moviment move;
        while (row >> square) move.afegeix(Posicio(square));
        if (move.longitud()==0) continue;
        if (move.longitud()<2) throw std::runtime_error("Incomplete replay move");
        loaded.push_back(move);
    }
    vaciar();
    for (const auto& move:loaded) encola(move);
}
