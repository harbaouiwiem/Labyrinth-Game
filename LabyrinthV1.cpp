#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

const int N = 10; // Grid size
const char START = '.';
const char END = '.';
const char WALL = '#';
const char EMPTY = '.';

const vector<string> WORDS = { "CAT", "DOG", "BAT", "RAT", "ART", "CAR", "BAR", "BAG", "BIG", "BIT" };
bool loop;
struct Cell {
    int x, y;
    char type;
    bool visited;
    vector<Cell*> neighbors;
};

vector<vector<Cell>> grid(N, vector<Cell>(N));
vector<vector<Cell*>> validPaths;

void initializeGrid() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            grid[i][j] = { i, j, EMPTY, false, {} };
        }
    }
}

void addNeighbors() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i > 0) grid[i][j].neighbors.push_back(&grid[i - 1][j]);
            if (i < N - 1) grid[i][j].neighbors.push_back(&grid[i + 1][j]);
            if (j > 0) grid[i][j].neighbors.push_back(&grid[i][j - 1]);
            if (j < N - 1) grid[i][j].neighbors.push_back(&grid[i][j + 1]);
        }
    }
}

void addWalls() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if ((i == 0 && j == 0) || (i == N - 1 && j == N - 1)) continue;
            if (rand() % 4 == 0) grid[i][j].type = WALL;
        }
    }
}

void addLetters() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j].type == EMPTY) {
                grid[i][j].type = 'A' + rand() % 26;
            }
        }
    }
}

void resetGrid() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            grid[i][j].visited = false;
        }
    }
}

void findAllPaths(Cell* start, Cell* end) {
    queue<vector<Cell*>> q;
    q.push({ start });
    start->visited = true;

    while (!q.empty()) {
        vector<Cell*> currentPath = q.front();
        q.pop();
        Cell* current = currentPath.back();

        if (current == end) {
            validPaths.push_back(currentPath);
            continue;
        }

        for (Cell* neighbor : current->neighbors) {
            if (!neighbor->visited && neighbor->type != WALL) {
                neighbor->visited = true;
                vector<Cell*> newPath = currentPath;
                newPath.push_back(neighbor);
                q.push(newPath);
            }
        }
    }
    resetGrid();
}

void insertWords() {
    if (validPaths.empty()) return;

    for (vector<Cell*>& path : validPaths) {
        int numWords = rand() % 3 + 1; // Insert up to 3 words
        for (int w = 0; w < numWords; ++w) {
            string word = WORDS[rand() % WORDS.size()];
            if (path.size() < word.size()) continue;

            int startIdx = rand() % (path.size() - word.size() + 1);
            cout << "Injecting word: " << word << " into path segment: ";

            for (size_t i = 0; i < word.size(); ++i) {
                path[startIdx + i]->type = word[i];
            }

            string pathLetters;
            for (Cell* cell : path) {
                pathLetters += cell->type;
            }
            cout << pathLetters << endl;
        }
    }
}

void printGrid() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << grid[i][j].type << " ";
        }
        cout << endl;
    }
}

int main() {
    while (true) {
        srand(time(0));

        initializeGrid();
        addNeighbors();
        addWalls();
        addLetters();

        grid[0][0].type = START;
        grid[N - 1][N - 1].type = END;

        findAllPaths(&grid[0][0], &grid[N - 1][N - 1]);

        if (!validPaths.empty()) {
            insertWords();
            cout << "\nGrid with Injected Words:" << endl;
            printGrid();
            break;
        }
    }
    return 0;
}