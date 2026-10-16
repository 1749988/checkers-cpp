#include "joc.hpp"
#include "GraphicManager.h"
#include <algorithm>
#include <stdexcept>
#include <vector>  // para std::vector
#include "moviment.hpp"

Joc::Joc()
    : mode_(MODE_JOC_NONE),
    jugadorActual_(0),
    fitxaSeleccionada_(false)
{}

Joc::~Joc() {
    // Saving is explicit: destructors must not throw.
}

void Joc::inicialitza(ModeJoc mode,
    const std::string& nomFitxerTauler,
    const std::string& nomFitxerMoviments)
{
    mode_ = mode;
    nomFitxerMoviments_ = nomFitxerMoviments;
    jugadorActual_ = 0;
    fitxaSeleccionada_ = false;
    posicionsValides_.clear();
    cua_.vaciar();
    mousePressed_ = false;

    // Carrega el tauler des del fitxer
    tauler_.inicialitza(nomFitxerTauler);

    // Si és mode replay, carreguem la cua de moviments
    if (mode_ == MODE_JOC_REPLAY) {
        cua_.cargaDeFichero(nomFitxerMoviments_);
    }
}

bool Joc::pixelACasella(int x, int y, Posicio& out) const {
    int relX = x - POS_X_TAULER - CASELLA_INICIAL_X;
    int relY = y - POS_Y_TAULER - CASELLA_INICIAL_Y;
    if (relX < 0 || relY < 0) return false;
    int c = relX / AMPLADA_CASELLA;
    int f = relY / ALCADA_CASELLA;
    if (f < 0 || f >= NUM_FILES_TAULER || c < 0 || c >= NUM_COLS_TAULER)
        return false;
    out = Posicio(f, c);
    return true;
}

bool Joc::actualitza(int mouseX, int mouseY, bool mouseStatus)
{
    GraphicManager* gm = GraphicManager::getInstance();

    // ─── DIBUIX ────────────────────────────────
    gm->drawSprite(GRAFIC_FONS, 0, 0);
    gm->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    // 1) Dibuixar totes les fitxes
    for (int f = 0; f < NUM_FILES_TAULER; ++f) {
        for (int c = 0; c < NUM_COLS_TAULER; ++c) {
            Fitxa* p = tauler_.getFitxa(f, c);
            if (!p || p->isEmpty())
                continue;

            IMAGE_NAME img;
            // triar sprite segons tipus/color
            if (p->getTipus() == TIPUS_DAMA) {
                img = (p->getColor() == COLOR_BLANC
                    ? GRAFIC_DAMA_BLANCA
                    : GRAFIC_DAMA_NEGRA);
            }
            else {
                img = (p->getColor() == COLOR_BLANC
                    ? GRAFIC_FITXA_BLANCA
                    : GRAFIC_FITXA_NEGRA);
            }

            int px = POS_X_TAULER + CASELLA_INICIAL_X + c * AMPLADA_CASELLA;
            int py = POS_Y_TAULER + CASELLA_INICIAL_Y + f * ALCADA_CASELLA;
            gm->drawSprite(img, px, py);
        }
    }

    // 2) Highlights de moviments vàlids
    if (fitxaSeleccionada_) {
        for (std::vector<Posicio>::const_iterator itPos = posicionsValides_.begin();
            itPos != posicionsValides_.end();
            ++itPos)
        {
            const Posicio& dest = *itPos;
            int px = POS_X_TAULER + CASELLA_INICIAL_X
                + dest.getColumna() * AMPLADA_CASELLA;
            int py = POS_Y_TAULER + CASELLA_INICIAL_Y
                + dest.getFila() * ALCADA_CASELLA;
            gm->drawSprite(GRAFIC_POSICIO_VALIDA, px, py);
        }
    }

    // ─── LÒGICA INPUT ──────────────────────────
    const bool newClick = mouseStatus && !mousePressed_;
    mousePressed_ = mouseStatus;
    if (!newClick || mode_ == MODE_JOC_NONE) {
        // sempre redraw
        return true;
    }

    Posicio clic(0, 0);
    if (!pixelACasella(mouseX, mouseY, clic))
        return true;

    if (mode_ == MODE_JOC_NORMAL) {
        // Selecció
        if (!fitxaSeleccionada_) {
            Fitxa* selected = tauler_.getFitxa(clic);
            const auto color = jugadorActual_ == 0 ? COLOR_BLANC : COLOR_NEGRE;
            if (!selected || selected->getColor() != color) return true;
            Posicio dests[64];
            int n = 0;
            tauler_.getPosicionsPossibles(clic, n, dests);
            if (n > 0) {
                fitxaSeleccionada_ = true;
                origenSeleccionat_ = clic;
                posicionsValides_.assign(dests, dests + n);
            }
        }
        else {
            // Moviment
            std::vector<Posicio>::iterator it = std::find(
                posicionsValides_.begin(),
                posicionsValides_.end(),
                clic
            );
            if (it != posicionsValides_.end()) {
                if (tauler_.mouFitxa(origenSeleccionat_, clic)) {
                    cua_.encola(tauler_.getUltimMoviment());
                    jugadorActual_ = 1 - jugadorActual_;
                }
            }
            // Desseleccionem sempre després del clic
            fitxaSeleccionada_ = false;
            posicionsValides_.clear();
        }
    }
    else { // MODE_JOC_REPLAY
        if (!cua_.estaVacia()) {
            Moviment m = cua_.desencola();
            std::vector<Posicio> seq = m.getSeq();
            Posicio origen = seq.front();
            Posicio desti = seq.back();
            const Fitxa* piece = tauler_.getFitxa(origen);
            const auto color = jugadorActual_ == 0 ? COLOR_BLANC : COLOR_NEGRE;
            if (!piece || piece->getColor() != color ||
                !(tauler_.mouFitxa(m) || (seq.size() == 2 && tauler_.mouFitxa(origen, desti))))
                throw std::runtime_error("Invalid replay move");
            jugadorActual_ = 1 - jugadorActual_;
        }
    }

    return true;
}

void Joc::finalitza()
{
    if (mode_ == MODE_JOC_NORMAL) {
        cua_.guardaEnFichero(nomFitxerMoviments_);
    }
}
