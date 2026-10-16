#ifndef TAULER_HPP
#define TAULER_HPP
#include "posicio.hpp"
#include "fitxa.hpp"
#include <array>
#include <string>
#include <vector>

static const int N_FILES = 8;
static const int N_COLUMNES = 8;

// Value ownership: copying or reloading a board cannot leak or double-delete pieces.
class Tauler {
    std::array<std::array<Fitxa, N_COLUMNES>, N_FILES> m_tauler{};
    Moviment ultim_;
    void aplica(const Moviment& moviment);
public:
    void inicialitza(const std::string& nomFitxer);
    void actualitzaMovimentsValids();
    // Legacy API: caller supplies room for 64 destinations.
    void getPosicionsPossibles(const Posicio& origen, int& n, Posicio posicions[]);
    std::vector<Moviment> getMovimentsPossibles(const Posicio& origen);
    bool teMoviments(ColorFitxa color);
    bool mouFitxa(const Posicio& origen, const Posicio& desti);
    bool mouFitxa(const Moviment& moviment);
    const Moviment& getUltimMoviment() const { return ultim_; }
    Fitxa* getFitxa(const Posicio& pos);
    const Fitxa* getFitxa(const Posicio& pos) const;
    Fitxa* getFitxa(int fila, int columna) { return getFitxa(Posicio(fila, columna)); }
    std::string toString() const;
};
#endif
