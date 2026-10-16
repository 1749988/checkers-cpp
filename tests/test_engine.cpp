#include "tauler.hpp"
#include "cuamoviments.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <chrono>

namespace {
std::filesystem::path temp;
void require(bool value,const char* why) { if (!value) throw std::runtime_error(why); }
std::string fixture(const std::string& name,const std::string& text) {
    const auto path=(temp/name).string(); std::ofstream(path)<<text; return path;
}
Tauler board(const std::string& text) { Tauler b; b.inicialitza(fixture("board.txt",text)); return b; }
void expect_error(const std::function<void()>& call) {
    bool raised=false; try { call(); } catch (const std::exception&) { raised=true; }
    require(raised,"expected exception");
}
}
int main() {
    temp=std::filesystem::temp_directory_path()/("checkers-test-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(temp);
    int passed=0;
    auto test=[&](const char* name,const std::function<void()>& run) {run(); ++passed; std::cout<<"PASS "<<name<<'\n';};
    try {
        test("coordinate parsing",[]{require(Posicio("a1")==Posicio(7,0),"a1 mapping");expect_error([]{Posicio p("z9");});});
        test("empty board and invalid coordinates",[]{Tauler b; require(!b.getFitxa(Posicio()),"invalid access");require(!b.mouFitxa(Posicio(),Posicio("b2")),"invalid move");require(!b.teMoviments(COLOR_BLANC),"empty board has no moves");});
        test("forward moves",[]{auto b=board("O c3\nX f6\n");require(!b.mouFitxa(Posicio("c3"),Posicio("b2")),"backwards step");require(b.mouFitxa(Posicio("c3"),Posicio("b4")),"white forward");require(b.mouFitxa(Posicio("f6"),Posicio("e5")),"black forward");});
        test("backwards capture",[]{auto b=board("O c3\nX d2\n");require(b.mouFitxa(Posicio("c3"),Posicio("e1")),"back capture");require(!b.getFitxa(Posicio("d2")),"remove victim");});
        test("mandatory capture without removing friendly pieces",[]{auto b=board("O c3\nO g3\nX d4\n");const auto before=b.toString();require(!b.mouFitxa(Posicio("g3"),Posicio("h4")),"must capture");require(before==b.toString(),"invalid move mutates board");require(b.mouFitxa(Posicio("c3"),Posicio("e5")),"capture");});
        test("longest complete capture",[]{auto b=board("O a3\nX b4\nX d6\n");auto moves=b.getMovimentsPossibles(Posicio("a3"));require(moves.size()==1 && moves[0].longitud()==3,"full route only");require(!b.mouFitxa(Posicio("a3"),Posicio("c5")),"cannot stop early");require(b.mouFitxa(Posicio("a3"),Posicio("e7")),"double capture");require(!b.getFitxa(Posicio("b4")) && !b.getFitxa(Posicio("d6")),"both victims removed");});
        test("global maximum capture",[]{auto b=board("O a3\nO g1\nX b4\nX d6\nX f2\n");require(b.getMovimentsPossibles(Posicio("g1")).empty(),"shorter capture not allowed");require(b.mouFitxa(Posicio("a3"),Posicio("e7")),"longest route");});
        test("king slide is not a capture",[]{auto b=board("D a1\nO g3\nX h8\n");require(b.mouFitxa(Posicio("g3"),Posicio("h4")),"long slide incorrectly forces capture");require(b.mouFitxa(Posicio("a1"),Posicio("f6")),"flying king");});
        test("flying king capture",[]{auto b=board("D a1\nX c3\n");require(b.mouFitxa(Posicio("a1"),Posicio("g7")),"king capture");require(!b.getFitxa(Posicio("c3")),"king victim");});
        test("promotion both colors",[]{auto b=board("O b6\nX c7\n");require(b.mouFitxa(Posicio("b6"),Posicio("d8")),"promotion capture");require(b.getFitxa(Posicio("d8"))->getTipus()==TIPUS_DAMA,"white king");b=board("X g3\nO f2\n");require(b.mouFitxa(Posicio("g3"),Posicio("e1")),"black promotion");require(b.getFitxa(Posicio("e1"))->getTipus()==TIPUS_DAMA,"black king");});
        test("copy and reload independent",[]{auto b=board("O a3\n");Tauler copy=b;require(copy.mouFitxa(Posicio("a3"),Posicio("b4")),"copy move");require(b.getFitxa(Posicio("a3")),"copy aliases original");b.inicialitza(fixture("other.txt","X h6\n"));require(!b.getFitxa(Posicio("a3")),"reload retained piece");});
        test("invalid board preserves state",[]{auto b=board("O a3\n");auto before=b.toString();expect_error([&]{b.inicialitza(fixture("bad.txt","O a3\nX z9\n"));});require(b.toString()==before,"failed load mutated state");expect_error([&]{b.inicialitza(fixture("bad.txt","Q b4\n"));});expect_error([&]{b.inicialitza(fixture("bad.txt","O a2\n"));});});
        test("full replay path roundtrip",[]{CuaMoviments q;Moviment m{Posicio("a3")};m.afegeix(Posicio("c5"));m.afegeix(Posicio("e7"));q.encola(m);const auto f=(temp/"moves.txt").string();q.guardaEnFichero(f);CuaMoviments loaded;loaded.cargaDeFichero(f);loaded.cargaDeFichero(f);require(loaded.desencola().getSeq()==m.getSeq(),"path lost");require(loaded.estaVacia(),"reload appends");expect_error([&]{loaded.desencola();});});
        test("invalid replay rejected",[]{CuaMoviments q;expect_error([&]{q.encola(Moviment());});expect_error([&]{q.cargaDeFichero(fixture("broken.txt","a3\n"));});});
        std::filesystem::remove_all(temp); std::cout<<passed<<" tests passed.\n"; return 0;
    } catch (const std::exception& error) { std::cerr<<"FAIL: "<<error.what()<<'\n';std::filesystem::remove_all(temp);return 1; }
}
