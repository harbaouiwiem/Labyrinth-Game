#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
//#include <SFML/Graphics.hpp>

using namespace std;


struct DifficultySettings {
    int gridSize;
    int attempts;
    int maxMoves;
    int requiredWords;
};
enum Difficulty {
    Easy,
    Medium,
    Hard
};

//function to aks the user which difficulty level they want 
Difficulty chooseDifficulty() {
    cout<<"Select Difficulty Level:\n";
    cout<<"1) Easy\n";
    cout<<"2) Medium\n";
    cout<<"3) Hard\n";
    cout<<"Choice: ";
    int choice;
    cin>>choice;

    switch (choice) {
        case 1: return Easy;
        case 2: return Medium;
        case 3: return Hard;
        default:cout<<"Invalid choice, defaulting to EASY.\n";
        return Easy;
    }
}
//function returns the DifficultySettings based on the user's choice
DifficultySettings getDifficultySettings(Difficulty diff) {
    DifficultySettings ds;
    switch (diff) {
        case Easy:
            ds.gridSize      = 10;   
            ds.attempts      = 5;   
            ds.maxMoves      = 20;  
            ds.requiredWords = 0;   
            break;
        case Medium:
            ds.gridSize      = 30;
            ds.attempts      = 4;
            ds.maxMoves      = 18;
            ds.requiredWords = 2;
            break;
        case Hard:
            ds.gridSize      = 50;  
            ds.attempts      = 3;   
            ds.maxMoves      = 16;  
            ds.requiredWords = 3;   
            break;
    }
    return ds;
}

static const char START = '*'; // Marker for the start cell 
static const char END   = '*'; // Marker for the end cell 
static const char WALL  = '#'; // Marker for walls 
static const char EMPTY = '.'; // Marker for empty cells 

// List of words to inject into the grid
const vector<string> WORDS = { "CAT", "DOG", "BAT", "RAT", "ART", "CAR", "BAR", "BAG", "BIG", "BIT" };

// Structure to represent a cell in the grid
struct Cell {
    int x, y;           // Coordinates of the cell
    char type;          // Type of cell (START, END, WALL, EMPTY, or a letter)
    bool visited;       // Flag to track if the cell has been visited during BFS
    vector<Cell*> neighbors; // List of adjacent cells (neighbors)
};


// We'll use a global vector-of-vector, but re-allocate it once we know gridSize
vector<vector<Cell>> grid; // 2D grid of cells
vector<vector<Cell*>> validPaths; // Stores all valid paths from start to end / each path = list of pointers to cells

// Function that depend on gridSize at runtime
void initializeGrid(int N) {
    //resize the global grid to NxN
    grid.clear();
    grid.resize(N, vector<Cell>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            grid[i][j] = { i, j, EMPTY, false, {} }; // Initialize each cell
        }
    }
}

// Function to add neighbors (adjacent cells) to each cell
void addNeighbors(int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i > 0) grid[i][j].neighbors.push_back(&grid[i - 1][j]); //top
            if (i < N - 1) grid[i][j].neighbors.push_back(&grid[i + 1][j]); //bottom
            if (j > 0) grid[i][j].neighbors.push_back(&grid[i][j - 1]); //left 
            if (j < N - 1) grid[i][j].neighbors.push_back(&grid[i][j + 1]); //right
            if (i > 0 && j > 0) {
                grid[i][j].neighbors.push_back(&grid[i - 1][j - 1]); //up-left
            }
            if (i > 0 && j < N - 1) {
                grid[i][j].neighbors.push_back(&grid[i - 1][j + 1]); //up-right
            }
            if (i < N - 1 && j > 0) {
                grid[i][j].neighbors.push_back(&grid[i + 1][j - 1]); //down-left
            }
            if (i < N - 1 && j < N - 1) {
                grid[i][j].neighbors.push_back(&grid[i + 1][j + 1]); //down-right
            }
        }
    }
}

// Function to randomly add walls to the grid
void addWalls(int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if ((i == 0 && j == 0) || (i == N - 1 && j == N - 1)) continue; // Skip the start and end cells
            if (rand() % 4 == 0) grid[i][j].type = WALL; //(25% chance of walls)
        }
    }
}

// Function to add random letters to non-wall cells
void addLetters(int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j].type == EMPTY) {
                grid[i][j].type = 'A' + rand() % 26;
            }
        }
    }
}

// Function to reset the visited flag of all cells
void resetGrid(int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            grid[i][j].visited = false;
        }
    }
}

// Function to find all valid paths from start to end using BFS
void findAllPaths(Cell* start, Cell* end, int N) {
    validPaths.clear(); 
    queue<vector<Cell*>> q; //store paths
    q.push({ start }); // Start with the initial path containing only the start cell
    start->visited = true; // Mark the start cell as visited

    while (!q.empty()) {
        auto currentPath = q.front(); // Get the current path
        q.pop();
        Cell* current = currentPath.back(); // Get the last cell in the current path

        // If the current cell is the end, store the path
        if (current == end) {
            validPaths.push_back(currentPath);
            continue;
        }

        // Explore all neighbors of the current cell
        for (Cell* neighbor : current->neighbors) {
            if (!neighbor->visited && neighbor->type != WALL) {
                neighbor->visited = true; // Mark the neighbor as visited
                vector<Cell*> newPath = currentPath; // Copy the current path
                newPath.push_back(neighbor); // Add the neighbor to the new path
                q.push(newPath); // Add the new path to the queue
            }
        }
    }
    resetGrid(N);// Reset visited flags for future use / next BFS calls
}

// Function to inject words into valid paths
void insertWords() {
    if (validPaths.empty()) return; // If no valid paths, exit

    for (vector<Cell*>& path : validPaths) {
        int numWords = rand() % 4 + 1; // Randomly choose 1 to 3 words to inject
        for (int w = 0; w < numWords; ++w) {
            string word = WORDS[rand() % WORDS.size()]; // Pick a random word
            if (path.size() < word.size()) continue; // Skip if the path is too short

            int startIdx = rand() % (path.size() - word.size() + 1); // Choose a random starting index
            cout<<"Injecting word: " << word << " into path segment: ";

            // Replace the letters in the path with the word
            for (size_t i = 0; i < word.size(); ++i) {
                path[startIdx + i]->type = word[i];
            }

            // Print the modified path / for debugging purposes
            string pathLetters;
            for (Cell* cell : path) {
                pathLetters.push_back(cell->type);
            }
            cout << pathLetters << endl;
        }
    }
}

// Function to print the grid
void printGrid(int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << grid[i][j].type << " ";
        }
        cout << endl;
    }
}
//return the length of the shortest valid path based on validPaths
int findShortestPathLength() {
    if(validPaths.empty()) return -1;
    int minLen = (int)validPaths[0].size();
    for (auto &vp : validPaths)
    {
        if ((int)vp.size() < minLen) {
            minLen = (int)vp.size();
        }
    }
    return minLen;
}
//check whether the player's chosen path is actually in validpaths / Compare pointer by pointer
bool isPathInValidPaths(const vector<Cell*>& userPath) {
    for (const auto &vp : validPaths) {
        if (vp.size() == userPath.size()) {
            bool same = true;
            for (int i=0; i<(int)vp.size(); i++) {
                if (vp[i]!=userPath[i]) {
                    same = false;
                    break;
                }
            }
            if (same) return true; // Found a matching path
        }
    }
    return false;
}
//count how many words from WORDS appear as a contigious substring in the user's path
int countWordsInPath(const vector<Cell*>& userPath) {
    //build the string of letters encoutered along the path
    string pathLetters;
    for(auto*c:userPath){
        pathLetters.push_back(c->type); 
    }
    //for reach possible word in WORDSn count how many times it appears in pathLetters
    int totalFound = 0;
    for (auto &word : WORDS) {
        //find all occurences of word in pathLetters
        size_t startPos = 0;
        while(true) {
            size_t idx = pathLetters.find(word, startPos);
            if (idx == string::npos) {
                break;
            }
            totalFound++;
            //move startPos so we can find overalapping occurences if any
            startPos = idx +1;
        }
    }
    return totalFound;
}
//let the user build a path from start with move-limit checking
vector<Cell*>getUserPath(int N, int maxMoves) {
    Cell* current = &grid[0][0]; //always start at the start cell
    vector<Cell*> path;
    path.push_back(current);
    int moveCount = 0;

    while(true) {
        cout << "\nCurrently at cell (" << current->x << "," << current->y<< ") containing '" << current->type << "'\n";
        cout << "Possible moves (up: U, down: D, left: L, right: R, Up-Left: Q, Up-Right: E, Down-Left: Z, Down-Right: X ), or S=Stop. Moves left: "<< (maxMoves - moveCount) << "\nChoice: ";

        char choice;
        cin >> choice;
        choice = toupper(choice);

        if (choice == 'S') {
            if (current->type == END) {
                cout<< "You have reached the end of the maze\n";       
            } else {
                cout << "You stopped before reaching the end of the maze\n";
            }
            break;
        }

        int nx = current->x;
        int ny = current->y;
        if (choice == 'U') nx--;
        if (choice == 'D') nx++;
        if (choice == 'L') ny--;
        if (choice == 'R') ny++;
        if (choice == 'Q') {nx--; ny--;}
        if (choice == 'E') {nx--; ny++;}
        if (choice == 'Z') {nx++; ny--;}
        if (choice == 'X') {nx++; ny++;}

        //check if movement is valid (within bounds and not a wall)
        if (nx < 0 || nx >= N || ny < 0 || ny >= N) {
            cout << "Invalid move: out of bounds. Try again.\n";
            continue;
        }
        if (grid[nx][ny].type == WALL) {
            cout << "Invalid move: that's a wall. Try again.\n";
            continue;
        }

        //move to the next cell
        moveCount++;
        current = &grid[nx][ny];
        path.push_back(current);

        //if we reached the end cell, we ask him if he wants to stop 
        if (current->type == END) {
            cout << "You are on the END cell! Stop here? (Y/N): ";
            char stopChoice;
            cin >> stopChoice;
            stopChoice = toupper(stopChoice);
            if (stopChoice == 'Y') {
                cout << "Stopping at END cell.\n";
                break;
            } else {
                // The user wants to keep going, which likely invalidates
                //continue moves anyway
                cout << "Continuing beyond the END cell...\n";
            }
        }
        return path;
    }
}

// Main function
int main() {
    srand(time(0)); // Seed the random number generator

    Difficulty diff = chooseDifficulty();
    DifficultySettings ds = getDifficultySettings(diff);

    int N = ds.gridSize;
    int attempts = ds.attempts;
    int maxMoves = ds.maxMoves;
    int requiredWords = ds.requiredWords;

    //we'll create and fill the grid for each round, we might want to regenrate a new labyrinth each time the user fails an attempt
    //we'll do one labyrinth per entire game session, until we reach the max number of attempts
        // Initialize the grid and add walls, letters, and neighbors
        initializeGrid(N);
        addNeighbors(N);
        addWalls(N);
        addLetters(N);

        // Set the start and end cells
        grid[0][0].type = START;
        grid[N - 1][N - 1].type = END;

        // BFS to find all validPaths
        findAllPaths(&grid[0][0], &grid[N - 1][N - 1],N);
        // If no valid paths 
        if(validPaths.empty()) {
            cout << "No valid paths\n";
            return 0;
        }
        // If valid paths exist, inject words and print the grid
        insertWords();
        cout << "\nGrid with Injected Words:" << endl;
        printGrid(N);
        //let the user interact with the grid to build a path up to attempts times
        bool userWon = false;
        while(attempts > 0) {
            cout << "\nYou have " << attempts << " attempt(s) remaining.\n";
            //build user path 
            vector<Cell*> userPath = getUserPath(N, maxMoves);
            //check if user's path is valid
            bool isValid = isPathInValidPaths(userPath);
            if (!isValid) {
                cout << "\nYour path is not a valid path from S to E. You lose!\n\n";
                attempts--;
                if(attempts == 0) {
                    cout << "No attempts left. Game Over!\n";
                }
                continue;
            } else {
                //if path is valid check the final score
                int shortestLen = findShortestPathLength();
                int userLen = (int)userPath.size();
                int score = 50;

                //if user used more cells than shortest path, penalize
                if(userLen > shortestLen) {
                    score -= 2 * (userLen - shortestLen);
                }
                //Count how many words from WORDS appear in user’s path letters
                int wordsFound = countWordsInPath(userPath);
                score +=10 * wordsFound;
                //check if required words are found
                bool enoughWords = wordsFound >= requiredWords;
                cout << "\nCongratulations! Your path is valid.\n";
                cout << "Path length = " << userLen << ", shortest path length = " << shortestLen << "\n"; 
                cout << "Words found = " << wordsFound << " (need at least " << requiredWords << "for full success)";
                cout << "Final Score = " << score << "\n";

                if (!enoughWords) {
                    cout << "(But you have NOT found enough words for a perfect success at this difficulty.)\n";
                } else
                {
                    cout << "(You have met the word requirement!)\n";
                }
                
                userWon = true;
                break;        
            }
    }

    if (userWon) {
        cout << "\nYou ran out of attempts. Better luck next time!\n";
    } else {
        cout << "\nThanks for playing!\n";
    }
    return 0;
}