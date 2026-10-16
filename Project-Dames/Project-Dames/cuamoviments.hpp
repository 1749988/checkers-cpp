#ifndef CUAMOVIMENTS_HPP
#define CUAMOVIMENTS_HPP

#include <string>
#include "moviment.hpp"

/**
 * @brief Estructura dinàmica de cua enllaçada per emmagatzemar Moviment.
 *
 * En Modo Normal es fa push de cada moviment després d'executar-se.
 * En Modo Replay es fa pop per reproduir-los en ordre.
 */
class CuaMoviments {
private:
    struct Nodo {
        Moviment moviment;
        Nodo* seguent;
        Nodo(const Moviment& m) : moviment(m), seguent(nullptr) {}
    };

    Nodo* cap;   ///< Punter al primer element
    Nodo* cua;   ///< Punter a l'últim element

public:
    CuaMoviments();
    ~CuaMoviments();
    CuaMoviments(const CuaMoviments&) = delete;
    CuaMoviments& operator=(const CuaMoviments&) = delete;

    /** Afegeix un Moviment al final de la cua. */
    void encola(const Moviment& m);

    /**
     * Retira i retorna el primer Moviment de la cua.
     * @throws std::runtime_error si la cua està buida.
     */
    Moviment desencola();

    /** Retorna true si no hi ha cap moviment a la cua. */
    bool estaVacia() const;

    /** Buida completament la cua, alliberant tota la memòria. */
    void vaciar();

    /**
     * Guarda tots els moviments de la cua en un fitxer.
     * Cada línia té format "origen [salts...] desti\n".
     * @throws std::runtime_error si no es pot obrir el fitxer.
     */
    void guardaEnFichero(const std::string& nomFitxer);

    /**
     * Carrega moviments des d'un fitxer i els encola.
     * Se suposa que cada línia està en format "origen [salts...] desti".
     * @throws std::runtime_error si no es pot obrir el fitxer.
     */
    void cargaDeFichero(const std::string& nomFitxer);
};

#endif // CUAMOVIMENTS_HPP
