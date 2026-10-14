/*
Program name: Assignment4.cpp
Description: Program that solves sodoku problems when you plug in the file
Inputs: puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
Output: Sudoku solved problem. 
Collaborators: Som Javia
Other Sources: None
Author: Som Javia
Creation date: 10/13/2026
Revision date: 10/13/2026
Revisions: None
Source code: ClaudeAI

*/

#include <array> //nxn matrix
#include <fstream> //input output writing minipulation
#include <iostream> //input output
#include <string> //string
#include <vector> //array we use

// ---------------------------------------------------------------------------
// Board: a 9x9 grid. 0 means an empty cell.
// ---------------------------------------------------------------------------
class Board {
public:
    static const int N = 9; //3 x 3 grid would be total of 9 numbers

    Board() { cells_.fill(0); } //fill the whole sudoku puzzle in an 81 grid

    // Reads 81 tokens: digits 1-9 or '_' for blank. Returns false on bad input.
    bool load(const std::string& filename) { //load in txt file
        std::ifstream in(filename); //make sure file loads properly
        if (!in) return false; //if it doesnt quit
        std::string tok; //max count
        int count = 0; //begin at 0
        while (count < N * N && in >> tok) { //while we count the board
            if (tok == "_") cells_[count++] = 0; //if we detect an empty space fill it with a 0
            else if (tok.size() == 1 && tok[0] >= '1' && tok[0] <= '9') 
                cells_[count++] = tok[0] - '0'; //parse string into intergers
            else return false;
        }
        return count == N * N; //return nxn matrix
    }

    //both of these codes will try to convert the txt file string into an array of integers and convert the nxn matrix into a single array
    int get(int r, int c) const { return cells_[r * N + c]; }
    void set(int r, int c, int v) { cells_[r * N + c] = v; }

    // Can digit v be placed at (r,c) without breaking row/column/block rules?
    bool canPlace(int r, int c, int v) const {
        for (int i = 0; i < N; ++i) {
            if (get(r, i) == v) return false; //test v placing at r
            if (get(i, c) == v) return false; //test v placing at c
        }
        int br = (r / 3) * 3, bc = (c / 3) * 3; //calculate number
        for (int i = 0; i < 3; ++i) //add 1
            for (int j = 0; j < 3; ++j) //add 1
                if (get(br + i, bc + j) == v) return false;
        return true; //code run true
    }

    // Are the given clues consistent (no duplicates in any row/col/block)?
    bool cluesConsistent() {
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c) {
                int v = get(r, c); 
                if (v == 0) continue;
                set(r, c, 0); //add placeholder value at v
                bool ok = canPlace(r, c, v); //test if we can place number
                set(r, c, v); //if true then we place the number
                if (!ok) return false;
            }
        return true;
    }

    // Finds the first empty cell; returns false if the board is full.
    bool findEmpty(int& r, int& c) const {
        for (r = 0; r < N; ++r)
            for (c = 0; c < N; ++c)
                if (get(r, c) == 0) return true; //make sure number being found is valid
        return false;
    }

    void print(std::ostream& os) const {
        for (int r = 0; r < N; ++r) {
            if (r == 3 || r == 6) { //if detects it sees end of the nxn 3d matrix
                std::cout << "-------+--------+--------" << "\n";
            } //print this line which makes our matrix look cool
            for (int c = 0; c < N; ++c) { 
                if (c == 3 || c == 6) {
                    std::cout << " | "; //add a pipe between the end of our nxn matrix
                }
                int v = get(r, c); //find the number value
                os << (v ? char('0' + v) : '_') << ' '; //determine number or if it's blank
            }
            os << '\n'; //new line after done with the row
        }
    }

private:
    std::array<int, N * N> cells_; //array is private 
};

// ---------------------------------------------------------------------------
// Solver: recursive backtracking that collects ALL solutions.
// ---------------------------------------------------------------------------
class Solver {
public:
    explicit Solver(const Board& puzzle) : board_(puzzle) {}

    // Returns every solution found (empty vector => no solution).
    const std::vector<Board>& solveAll() {
        solutions_.clear();
        if (board_.cluesConsistent()) search();
        return solutions_;
    }

private:
    void search() {
        int r, c;
        if (!board_.findEmpty(r, c)) {      // k == 81: complete solution
            solutions_.push_back(board_); //show the board solution
            return;
        }
        // Try each candidate; no candidates => dead end, return (backtrack).
        for (int v = 1; v <= 9; ++v) {
            if (board_.canPlace(r, c, v)) {
                board_.set(r, c, v);
                search();                   // recursive call with k+1 filled
                board_.set(r, c, 0);        // undo and try next candidate
            }
        }
    }

    Board board_;
    std::vector<Board> solutions_; // new board which will be the solutions to the sudoku
};

// ---------------------------------------------------------------------------
// PuzzleRunner: handles one file end to end (load, solve, report).
// ---------------------------------------------------------------------------
class PuzzleRunner {
public:
    explicit PuzzleRunner(const std::string& filename) : filename_(filename) {}

    void run() {  //out put will print out this fancy thing where it shows the puzzle file, and show you the solution to the problem
        std::cout << "========================================" << "\n";
        std::cout << "Puzzle file: " << filename_ << "\n";
        Board puzzle;
        if (!puzzle.load(filename_)) { //if fail to load the file
            std::cout << "Could not read a valid puzzle from this file.\n\n";
            return;
        }
        std::cout << "Original Puzzle:\n";
        puzzle.print(std::cout);

        Solver solver(puzzle);
        const std::vector<Board>& sols = solver.solveAll(); //solve the puzzle in totallity

        std::cout << "\nSolutions found: " << sols.size() << "\n"; //find how big the matrix

        if (sols.empty()) {
            std::cout << "\nNo solution found\n";
        } else {
            for (size_t i = 0; i < sols.size(); ++i) {
                std::cout << "\nSolution " << (i + 1) << ":\n";
                sols[i].print(std::cout); //print the solution itself
            }
        }
        std::cout << "\n----------------------------------------\n\n";
    }

private:
    std::string filename_;         
};

int main(int argc, char* argv[]) { //load all txt files
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) files.push_back(argv[i]);
    if (files.empty()) //check if file is empty
        for (int i = 1; i <= 5; ++i)
            files.push_back("puzzle" + std::to_string(i) + ".txt"); //import txt file

    for (const std::string& f : files) PuzzleRunner(f).run(); //run sudoku solver
    return 0;
}
