#ifndef JOC_H
#define JOC_H

#include <vector>
#include <string>
#include "info_joc.hpp"
#include "tauler.hpp"
#include "cuamoviments.hpp"
#include "moviment.hpp"
#include "posicio.hpp"

class Joc {
public:
    Joc();
    ~Joc();

    /**
     * Inicialitza el joc (MODE_JOC_NORMAL o MODE_JOC_REPLAY),
     * carregant l’estat inicial del tauler i, si és replay, la cua de moviments.
     */
    void inicialitza(ModeJoc mode,
        const std::string& nomFitxerTauler,
        const std::string& nomFitxerMoviments);

    /**
     * Processa un clic del ratolí i dibuixa tot el que cal:
     *  - fons, tauler, peces, highlights, text
     * Retorna true si volem redraw (sempre true).
     */
    bool actualitza(int mousePosX, int mousePosY, bool mouseStatus);

    /**
     * Quan tanquem en mode normal, desar la cua a fitxer.
     */
    void finalitza();

private:
    ModeJoc               mode_;
    Tauler                tauler_;
    CuaMoviments          cua_;
    int                   jugadorActual_;      // 0=blanques,1=negres

    bool                  fitxaSeleccionada_;
    bool                  mousePressed_ = false;
    Posicio               origenSeleccionat_;
    std::vector<Posicio>  posicionsValides_;
    std::string           nomFitxerMoviments_;

    /** Converteix coordenades de píxel a casella (fila,columna). */
    bool pixelACasella(int x, int y, Posicio& out) const;
};

#endif // JOC_H
