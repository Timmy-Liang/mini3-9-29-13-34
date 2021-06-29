#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <array>
#include <vector>

#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};
/*
class BOARD{
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;


}*/

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> b,int player){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j]=b[i][j];
                if(board[i][j]==BLACK)disc_count[BLACK]++;
                else if(board[i][j]==WHITE)disc_count[WHITE]++;
                else disc_count[EMPTY]++;
            }
        }
        cur_player=player;
        next_valid_spots = get_valid_spots();
        done=false;
        winner=-1;

    }
    OthelloBoard(const OthelloBoard &boa){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j]=boa.board[i][j];
            }
        }
        disc_count[EMPTY]=boa.disc_count[EMPTY];
        disc_count[BLACK]=boa.disc_count[BLACK];
        disc_count[WHITE]=boa.disc_count[WHITE];
        cur_player=boa.cur_player;
        next_valid_spots=boa.next_valid_spots;
        done=boa.done;
        winner=boa.winner;
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        /*
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }*/
        return true;
    }
    /*
    std::string encode_player(int state) {
        if (state == BLACK) return "O";
        if (state == WHITE) return "X";
        return "Draw";
    }
    std::string encode_spot(int x, int y) {
        if (is_spot_valid(Point(x, y))) return ".";
        if (board[x][y] == BLACK) return "O";
        if (board[x][y] == WHITE) return "X";
        return " ";
    }
    std::string encode_output(bool fail=false) {
        int i, j;
        std::stringstream ss;
        ss << "Timestep #" << (8*8-4-disc_count[EMPTY]+1) << "\n";
        ss << "O: " << disc_count[BLACK] << "; X: " << disc_count[WHITE] << "\n";
        if (fail) {
            ss << "Winner is " << encode_player(winner) << " (Opponent performed invalid move)\n";
        } else if (next_valid_spots.size() > 0) {
            ss << encode_player(cur_player) << "'s turn\n";
        } else {
            ss << "Winner is " << encode_player(winner) << "\n";
        }
        ss << "+---------------+\n";
        for (i = 0; i < SIZE; i++) {
            ss << "|";
            for (j = 0; j < SIZE-1; j++) {
                ss << encode_spot(i, j) << " ";
            }
            ss << encode_spot(i, j) << "|\n";
        }
        ss << "+---------------+\n";
        ss << next_valid_spots.size() << " valid moves: {";
        if (next_valid_spots.size() > 0) {
            Point p = next_valid_spots[0];
            ss << "(" << p.x << "," << p.y << ")";
        }
        for (size_t i = 1; i < next_valid_spots.size(); i++) {
            Point p = next_valid_spots[i];
            ss << ", (" << p.x << "," << p.y << ")";
        }
        ss << "}\n";
        ss << "=================\n";
        return ss.str();
    }
    std::string encode_state() {
        int i, j;
        std::stringstream ss;
        ss << cur_player << "\n";
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE-1; j++) {
                ss << board[i][j] << " ";
            }
            ss << board[i][j] << "\n";
        }
        ss << next_valid_spots.size() << "\n";
        for (size_t i = 0; i < next_valid_spots.size(); i++) {
            Point p = next_valid_spots[i];
            ss << p.x << " " << p.y << "\n";
        }
        return ss.str();
    }*/
};

//////////////////////////////////////////////////////////////////////////
int player;
int dep;
Point pp;
Point test(5,4);
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> boardarr;
std::vector<Point> next_vspots;
int n_valid_spots;
#define corner 15
const int mapvalue[8][8]={
    corner,1,5,5,5,5,1,corner,
    1,1,1,1,1,1,1,1,
    5,1,5,5,5,5,1,5,
    5,1,5,1,1,5,1,5,
    5,1,5,1,1,5,1,5,
    5,1,5,5,5,5,1,5,
    1,1,1,1,1,1,1,1,
    corner,1,5,5,5,5,1,corner
};

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> boardarr[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_vspots.push_back({x, y});
    }
}

int heuristic(OthelloBoard BD){
    int edge=0;
    int mpv=0;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(BD.board[i][j]==player){
                mpv+=mapvalue[i][j];
            }
            if(BD.board[i][j]==3-player){
                mpv-=mapvalue[i][j];
            }
        }
    }


    int win=0;
    int nospot=0;
    if(BD.cur_player==player&&BD.next_valid_spots.size()==0)nospot-=30;
    else if(BD.cur_player==3-player&&BD.next_valid_spots.size()==0)nospot+=30;

    int valid=0;
    if(BD.cur_player==player){
        valid=BD.next_valid_spots.size();
    }
    int diff=BD.disc_count[player]-BD.disc_count[3-player];
//
// +3*valid 2*diff
    return mpv+valid*2;
}

int alphabeta(const OthelloBoard &board,int depth,int alpha,int beta,int pl){
    if(depth==0||board.disc_count[EMPTY]==0||board.disc_count[BLACK]==0||board.disc_count[WHITE]==0){
        return heuristic(board);
    }
    if(pl==player){
        int bsize=board.next_valid_spots.size();
        if(bsize!=0){
            int value=-100000;
            for(int i=0;i<bsize;i++){
                OthelloBoard tb=board;
                tb.put_disc(board.next_valid_spots[i]);

                value=max(value,alphabeta(tb,depth-1,alpha,beta,tb.cur_player));
                alpha=max(value,alpha);
                if(alpha>=beta)break;
            }
            return value;
        }
        else{
            return -100;
        }
    }
    else {
        int bsize=board.next_valid_spots.size();
        if(bsize!=0){
            int value=100000;
            for(int i=0;i<bsize;i++){
                OthelloBoard tb=board;
                tb.put_disc(board.next_valid_spots[i]);
                value=min(value,alphabeta(tb,depth-1,alpha,beta,tb.cur_player));
                beta=min(value,beta);
                if(beta<=alpha)break;
            }
            return value;
        }else{
            return 100;
        }
    }
}

void write_valid_spot(std::ofstream& fout) {
//set

    if(n_valid_spots!=0){
        pp=next_vspots[0];
        fout << pp.x << " " << pp.y << std::endl;
        fout.flush();
    }
    
    OthelloBoard curboard(boardarr,player);
    int value=-100000;
    for(int i=0;i<n_valid_spots;i++){
        OthelloBoard tb=curboard;
        tb.put_disc(next_vspots[i]);
        int tv=alphabeta(tb,3,-100000,100000,player);
        if(value<tv){
            value=tv;
            pp=next_vspots[i];
            fout << pp.x << " " << pp.y << std::endl;
            fout.flush();
        }

    }
    
//flip
//compare max
//fout

    /*
    int n_valid_spots = next_vspots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_vspots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
    
    
    */
}




int main(int , char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}


