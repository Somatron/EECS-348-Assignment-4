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

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Board: a 9x9 grid. 0 means an empty cell.
// ---------------------------------------------------------------------------
class Board {
public:
    static const int N = 9;

    Board() { cells_.fill(0); }

    // Reads 81 tokens: digits 1-9 or '_' for blank. Returns false on bad input.
    bool load(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) return false;
        std::string tok;
        int count = 0;
        while (count < N * N && in >> tok) {
            if (tok == "_") cells_[count++] = 0;
            else if (tok.size() == 1 && tok[0] >= '1' && tok[0] <= '9')
                cells_[count++] = tok[0] - '0';
            else return false;
        }
        return count == N * N;
    }

    int get(int r, int c) const { return cells_[r * N + c]; }
    void set(int r, int c, int v) { cells_[r * N + c] = v; }

    // Can digit v be placed at (r,c) without breaking row/column/block rules?
    bool canPlace(int r, int c, int v) const {
        for (int i = 0; i < N; ++i) {
            if (get(r, i) == v) return false;
            if (get(i, c) == v) return false;
        }
        int br = (r / 3) * 3, bc = (c / 3) * 3;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (get(br + i, bc + j) == v) return false;
        return true;
    }

    // Are the given clues consistent (no duplicates in any row/col/block)?
    bool cluesConsistent() {
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c) {
                int v = get(r, c);
                if (v == 0) continue;
                set(r, c, 0);
                bool ok = canPlace(r, c, v);
                set(r, c, v);
                if (!ok) return false;
            }
        return true;
    }

    // Finds the first empty cell; returns false if the board is full.
    bool findEmpty(int& r, int& c) const {
        for (r = 0; r < N; ++r)
            for (c = 0; c < N; ++c)
                if (get(r, c) == 0) return true;
        return false;
    }

    void print(std::ostream& os) const {
        for (int r = 0; r < N; ++r) {
            if (r == 3 || r == 6) {
                std::cout << "-------+--------+--------" << "\n";
            }
            for (int c = 0; c < N; ++c) {
                if (c == 3 || c == 6) {
                    std::cout << " | ";
                }
                int v = get(r, c);
                os << (v ? char('0' + v) : '_') << ' ';
            }
            os << '\n';
        }
    }

private:
    std::array<int, N * N> cells_;
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
            solutions_.push_back(board_);
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
    std::vector<Board> solutions_;
};

// ---------------------------------------------------------------------------
// PuzzleRunner: handles one file end to end (load, solve, report).
// ---------------------------------------------------------------------------
class PuzzleRunner {
public:
    explicit PuzzleRunner(const std::string& filename) : filename_(filename) {}

    void run() {
        std::cout << "========================================" << "\n";
        std::cout << "Puzzle file: " << filename_ << "\n";
        Board puzzle;
        if (!puzzle.load(filename_)) {
            std::cout << "Could not read a valid puzzle from this file.\n\n";
            return;
        }
        std::cout << "Original Puzzle:\n";
        puzzle.print(std::cout);

        Solver solver(puzzle);
        const std::vector<Board>& sols = solver.solveAll();

        std::cout << "\nSolutions found: " << sols.size() << "\n";

        if (sols.empty()) {
            std::cout << "\nNo solution found\n";
        } else {
            for (size_t i = 0; i < sols.size(); ++i) {
                std::cout << "\nSolution " << (i + 1) << ":\n";
                sols[i].print(std::cout);
            }
        }
        std::cout << "\n----------------------------------------\n\n";
    }

private:
    std::string filename_;         
};

int main(int argc, char* argv[]) {
    std::vector<std::string> files;
    for (int i = 1; i < argc; ++i) files.push_back(argv[i]);
    if (files.empty())
        for (int i = 1; i <= 5; ++i)
            files.push_back("puzzle" + std::to_string(i) + ".txt");

    for (const std::string& f : files) PuzzleRunner(f).run();
    return 0;
}
