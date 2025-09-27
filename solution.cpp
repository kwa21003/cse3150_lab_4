#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    // TODO: implement print_board here
    for (const auto& row : board) {
        for (auto cell : row) {
            if (cell == 0) cout << "0\t";
            else cout << cell << "\t";
        }
        cout << endl;
    }
    cout << endl;
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
    vector<pair<int,int>> empty;
    for (int r=0;r<4;r++)
        for (int c=0;c<4;c++)
            if (board[r][c]==0) empty.push_back({r, c});

    static std::mt19937 rng(time(nullptr));
    std::uniform_int_distribution<int> dist(0, empty.size()-1);
    std::uniform_int_distribution<int> val_dist(0, 9); // 10% chance for 4
    pair<int, int> random_cell = empty[dist(rng)];
    int r = random_cell.first;
    int c = random_cell.second;

    board[r][c] = val_dist(rng)== 0 ? 4 : 2;
}

// TODO: Compress a row, remove zeroes, and then pad with zeroes at the end
std::vector<int> compress_row(const std::vector<int>& row) {
    std::vector<int> out;
    std::copy_if(row.begin(), row.end(), std::back_inserter(out), [](int i){ return i!=0; });
    out.resize(4, 0);
    return out;
}

// TODO: Merge a row (assumes the row is already compressed)
std::vector<int> merge_row(std::vector<int> row) {
    for (size_t i=0; i+1<row.size(); ++i) {
        if (row[i] != 0 && row[i] == row[i+1]) {
            row[i] *= 2;
            row[i+1] = 0;
            ++i; // no double merge
        }
    }
    return compress_row(row);
}

// TODO: use copy_if and iterators
bool move_left(vector<vector<int>>& board){
    bool moved = false;
    for (int r=0;r<4;++r) {
        std::vector<int> orig = board[r];
        auto compressed = compress_row(board[r]);
        auto merged = merge_row(compressed);
        if (merged != orig) moved = true;
        board[r] = merged;
    }
    return moved;
}

// TODO: use reverse iterators
bool move_right(vector<vector<int>>& board){
    bool moved = false;
    for (int r=0;r<4;++r) {
        std::vector<int> orig = board[r];
        std::reverse(board[r].begin(), board[r].end());
        auto compressed = compress_row(board[r]);
        auto merged = merge_row(compressed);
        std::reverse(merged.begin(), merged.end());
        if (merged != orig) moved = true;
        board[r] = merged;
    }
    return moved;
}

// TODO: use column traversal
bool move_up(vector<vector<int>>& board){
    bool moved = false;
    for (int c=0;c<4;++c) {
        std::vector<int> col(4);
        for (int r=0;r<4;++r) col[r] = board[r][c];
        auto compressed = compress_row(col);
        auto merged = merge_row(compressed);
        for (int r=0;r<4;++r) {
            if (board[r][c] != merged[r]) moved = true;
            board[r][c] = merged[r];
        }
    }
    return moved;
}

// TODO: use column traversal with reverse
bool move_down(vector<vector<int>>& board){
    bool moved = false;
    for (int c=0;c<4;++c) {
        std::vector<int> col(4);
        for (int r=0;r<4;++r) col[r] = board[r][c];
        std::reverse(col.begin(), col.end());
        auto compressed = compress_row(col);
        auto merged = merge_row(compressed);
        std::reverse(merged.begin(), merged.end());
        for (int r=0;r<4;++r) {
            if (board[r][c] != merged[r]) moved = true;
            board[r][c] = merged[r];
        }
    }
    return moved;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: get the history and print the board and continue
            if (!history.empty()) {
                board = history.top();
                history.pop();
            }
            continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: Store the previous state here!
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
