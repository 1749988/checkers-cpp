#include "tauler.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
using Board = std::array<std::array<Fitxa, N_COLUMNES>, N_FILES>;
const std::pair<int,int> directions[] = {{1,1},{1,-1},{-1,1},{-1,-1}};
bool inside(int r, int c) { return r>=0 && r<8 && c>=0 && c<8; }
struct Candidate { Moviment move; int captures; };

void captures(const Board& board, const Fitxa& piece, const Moviment& path,
              int count, std::vector<Candidate>& result) {
    const auto start = path.getSeq().back();
    const int row = start.getFila(), col = start.getColumna();
    bool extended = false;
    for (const auto& dir : directions) {
        const int dr=dir.first, dc=dir.second;
        int r=row+dr, c=col+dc;
        if (piece.getTipus()==TIPUS_DAMA) {
            while (inside(r,c) && board[r][c].isEmpty()) { r+=dr; c+=dc; }
        }
        if (!inside(r,c) || board[r][c].isEmpty() || board[r][c].getColor()==piece.getColor()) continue;
        const int victimRow=r, victimCol=c;
        r+=dr; c+=dc;
        while (inside(r,c) && board[r][c].isEmpty()) {
            Board next=board;
            next[row][col].setEmpty();
            next[victimRow][victimCol].setEmpty();
            next[r][c]=piece;
            Moviment route=path;
            route.afegeix(Posicio(r,c));
            extended=true;
            captures(next,piece,route,count+1,result);
            if (piece.getTipus()==TIPUS_NORMAL) break;
            r+=dr; c+=dc;
        }
    }
    // A capture sequence must be completed, not stopped at an intermediate square.
    if (!extended && count>0) result.push_back({path,count});
}
}

void Tauler::inicialitza(const std::string& file) {
    std::ifstream in(file);
    if (!in) throw std::runtime_error("Cannot open board: "+file);
    Board next{};
    std::string line;
    int number=0;
    while (std::getline(in,line)) {
        ++number;
        if (line.find_first_not_of(" \t\r")==std::string::npos) continue;
        std::istringstream row(line);
        char symbol; std::string square, extra;
        if (!(row>>symbol>>square) || (row>>extra) || std::string("OXDR").find(symbol)==std::string::npos)
            throw std::runtime_error("Invalid board row "+std::to_string(number));
        Posicio p(square);
        if ((p.getFila()+p.getColumna())%2!=1 || !next[p.getFila()][p.getColumna()].isEmpty())
            throw std::runtime_error("Invalid or duplicate square: "+square);
        next[p.getFila()][p.getColumna()]=Fitxa::fromChar(symbol);
    }
    m_tauler=next;
    ultim_=Moviment();
}

Fitxa* Tauler::getFitxa(const Posicio& p) {
    if (!inside(p.getFila(),p.getColumna())) return nullptr;
    auto& piece=m_tauler[p.getFila()][p.getColumna()];
    return piece.isEmpty()?nullptr:&piece;
}
const Fitxa* Tauler::getFitxa(const Posicio& p) const {
    if (!inside(p.getFila(),p.getColumna())) return nullptr;
    const auto& piece=m_tauler[p.getFila()][p.getColumna()];
    return piece.isEmpty()?nullptr:&piece;
}

void Tauler::actualitzaMovimentsValids() {
    for (auto& row:m_tauler) for (auto& piece:row) piece.netejaMoviments();
    std::array<std::array<std::vector<Candidate>,8>,8> all{};
    int maximum[2]={0,0};
    for (int r=0;r<8;++r) for (int c=0;c<8;++c) {
        const Fitxa& piece=m_tauler[r][c];
        if (piece.isEmpty()) continue;
        auto& moves=all[r][c];
        captures(m_tauler,piece,Moviment(Posicio(r,c)),0,moves);
        for (const auto& move:moves) maximum[piece.getColor()]=std::max(maximum[piece.getColor()],move.captures);
        for (const auto& dir:directions) {
            const int dr=dir.first,dc=dir.second;
            if (piece.getTipus()==TIPUS_NORMAL && dr!=(piece.getColor()==COLOR_BLANC?-1:1)) continue;
            int rr=r+dr,cc=c+dc;
            while (inside(rr,cc) && m_tauler[rr][cc].isEmpty()) {
                Moviment move{Posicio(r,c)}; move.afegeix(Posicio(rr,cc));
                moves.push_back({move,0});
                if (piece.getTipus()==TIPUS_NORMAL) break;
                rr+=dr; cc+=dc;
            }
        }
    }
    for (int r=0;r<8;++r) for (int c=0;c<8;++c) {
        Fitxa& piece=m_tauler[r][c];
        if (piece.isEmpty()) continue;
        for (const auto& candidate:all[r][c])
            if (candidate.captures==maximum[piece.getColor()]) piece.afegeixMoviment(candidate.move);
    }
}

std::vector<Moviment> Tauler::getMovimentsPossibles(const Posicio& origin) {
    actualitzaMovimentsValids();
    const Fitxa* piece=getFitxa(origin);
    return piece?piece->getMoviments():std::vector<Moviment>{};
}
void Tauler::getPosicionsPossibles(const Posicio& origin,int& count,Posicio positions[]) {
    count=0;
    for (const auto& move:getMovimentsPossibles(origin)) {
        const auto destination=move.getSeq().back();
        if (std::find(positions,positions+count,destination)==positions+count) positions[count++]=destination;
    }
}
bool Tauler::teMoviments(ColorFitxa color) {
    actualitzaMovimentsValids();
    for (const auto& row:m_tauler) for (const auto& piece:row)
        if (!piece.isEmpty() && piece.getColor()==color && !piece.getMoviments().empty()) return true;
    return false;
}

void Tauler::aplica(const Moviment& move) {
    const auto& route=move.getSeq();
    Fitxa piece=*getFitxa(route.front());
    m_tauler[route.front().getFila()][route.front().getColumna()].setEmpty();
    for (std::size_t i=1;i<route.size();++i) {
        int r=route[i-1].getFila(),c=route[i-1].getColumna();
        const int dr=route[i].getFila()>r?1:-1,dc=route[i].getColumna()>c?1:-1;
        r+=dr; c+=dc;
        while (r!=route[i].getFila()) { m_tauler[r][c].setEmpty(); r+=dr; c+=dc; }
    }
    const auto end=route.back();
    if ((piece.getColor()==COLOR_BLANC && end.getFila()==0) || (piece.getColor()==COLOR_NEGRE && end.getFila()==7)) piece.converteixDama();
    piece.netejaMoviments();
    m_tauler[end.getFila()][end.getColumna()]=piece;
    ultim_=move;
}

bool Tauler::mouFitxa(const Posicio& origin,const Posicio& destination) {
    for (const auto& move:getMovimentsPossibles(origin))
        if (move.getSeq().back()==destination) { aplica(move); return true; }
    return false;
}
bool Tauler::mouFitxa(const Moviment& wanted) {
    if (wanted.longitud()<2) return false;
    for (const auto& move:getMovimentsPossibles(wanted.getSeq().front()))
        if (move.getSeq()==wanted.getSeq()) { aplica(move); return true; }
    return false;
}
std::string Tauler::toString() const {
    std::ostringstream out;
    for (int r=0;r<8;++r) {
        out<<8-r<<": ";
        for (int c=0;c<8;++c) out<<m_tauler[r][c].toChar()<<(c==7?'\n':' ');
    }
    out<<"   a b c d e f g h\n";
    return out.str();
}
