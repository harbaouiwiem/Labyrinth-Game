#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <unordered_set>
#include <string>
#include <chrono>
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
    cout << "Select Difficulty Level:\n";
    cout << "1) Easy\n";
    cout << "2) Medium\n";
    cout << "3) Insane\n";
    cout << "Choice: ";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1: return Easy;
    case 2: return Medium;
    case 3: return Hard;
    default:cout << "Invalid choice, defaulting to EASY.\n";
        return Easy;
    }
}
//function returns the DifficultySettings based on the user's choice
DifficultySettings getDifficultySettings(Difficulty diff) {
    DifficultySettings ds;
    switch (diff) {
    case Easy:
        ds.gridSize = 6;
        ds.attempts = 3;
        ds.maxMoves = 40;
        ds.requiredWords = 0;
        break;
    case Medium:
        ds.gridSize = 9;
        ds.attempts = 2;
        ds.maxMoves = 35;
        ds.requiredWords = 1;
        break;
    case Hard:
        ds.gridSize = 18;
        ds.attempts = 1;
        ds.maxMoves = 25;
        ds.requiredWords = 1;
        break;
    }
    return ds;
}

static const char START = '*'; // Marker for the start cell 
static const char END = '&'; // Marker for the end cell 
static const char WALL = '#'; // Marker for walls 
static const char EMPTY = '.'; // Marker for empty cells 

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


//Fonction to Get words from files
std::vector<std::string> readfromfile(const std::string& nomFichier) {
    std::vector<std::string> mots;
    std::ifstream fichier(nomFichier);

    if (!fichier) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        return mots;  // Retourne une liste vide
    }

    std::string mot;
    while (fichier >> mot) {  // Lire mot par mot
        mots.push_back(mot);
    }

    return mots;
}

//Read words from files
std::vector<std::string> easyword = readfromfile("easy.txt");
std::vector<std::string> mediumword = readfromfile("medium.txt");
std::vector<std::string> hardword = readfromfile("hard.txt");


// Function to inject words into valid paths
void insertWords(int diff) {
    cout << "insertWords() is executing...\n";
    if (validPaths.empty()) return; // If no valid paths, exit

    // Select the appropriate word list based on difficulty
    std::vector<std::string>* wordList;
    if (diff == 0) {
        wordList = &easyword;
        cout << "insertWords() is executing... Easy\n";
    }
    else if (diff == 1) {
        wordList = &mediumword;
        cout << "insertWords() is executing...Medium\n";
    }
    else if (diff == 2) {
        wordList = &hardword;
        cout << "insertWords() is executing...Hard\n";
    }
    else {
        wordList = &easyword; // Default to general words if difficulty is undefined
    }

    for (vector<Cell*>& path : validPaths) {
        cout << "Processing path...\n";
        int numWords = rand() % 4; // Randomly choose 0 to 3 words to inject
        unordered_set<int> occupiedIndices; // Track occupied positions

        for (int w = 0; w < numWords; ++w) {
            string word = (*wordList)[rand() % wordList->size()]; // Pick a random word from the list
            if (path.size() < word.size()) continue; // Skip if the path is too short

            int startIdx;
            bool validInsertion = false;

            // Find a valid starting index where START/END are not affected and words don't overlap
            for (int attempts = 0; attempts < 10; ++attempts) { // Try multiple times
                startIdx = rand() % (path.size() - word.size() + 1);

                // Ensure the word does not overlap START, END, or previously placed words
                bool conflict = false;
                for (size_t i = 0; i < word.size(); ++i) {
                    if (path[startIdx + i]->type == START || path[startIdx + i]->type == END || occupiedIndices.count(startIdx + i)) {
                        conflict = true;
                        break;
                    }
                }

                if (!conflict) {
                    validInsertion = true;
                    break;
                }
            }
            if (!validInsertion) continue; // Skip this word if no valid position is found

            cout << "Injecting word: " << word << " into path segment: ";

            // Replace the letters in the path with the word
            for (size_t i = 0; i < word.size(); ++i) {
                int idx = startIdx + i;
                if (path[idx]->type == START || path[idx]->type == END) continue; // Ensure START/END remain unchanged
                path[idx]->type = word[i];
                occupiedIndices.insert(idx); // Mark index as occupied
            }

            // Print the modified path for debugging
            string pathLetters;
            for (Cell* cell : path) {
                pathLetters.push_back(cell->type);
            }
            cout << pathLetters << endl;
        }
    }
}



// Function to print the grid
void printGrid(int N, int currentX = -1, int currentY = -1) {
    // Move the cursor to the top of the grid
    cout << "\033[4;1H"; // Move to row 4, column 1

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == currentX && j == currentY) {
                // Highlight the current position with blinking text
                cout << "\033[5;31m" << grid[i][j].type << "\033[0m" << " "; // Blinking red text
            }
            else {
                cout << grid[i][j].type << " ";
            }
        }
        cout << "\n"; // Move to the next line
    }
}
//return the length of the shortest valid path based on validPaths
vector<Cell*> findShortestPath(int N) {

    if (grid.empty() || grid[0][0].type == WALL) {
        cout << "Grid is empty or start position is blocked!\n";
        return {};
    }


    queue<pair<Cell*, vector<Cell*>>> q;
    vector<vector<bool>> visited(N, vector<bool>(N, false));

    // Start from (0,0)
    Cell* start = &grid[0][0];
    q.push({ start, {start} });
    visited[0][0] = true;

    while (!q.empty()) {

        pair<Cell*, vector<Cell*>> front = q.front();
        Cell* current = front.first;
        vector<Cell*> path = front.second;
        q.pop();


        // If reached the END, return the shortest path
        if (current->type == END) {
            return path;
        }

        // Possible moves: Up, Down, Left, Right, and Diagonals
        vector<pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},  // Up, Down, Left, Right
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1} // Diagonals
        };

        for (const pair<int, int>& dir : directions) {
            int nx = current->x + dir.first;
            int ny = current->y + dir.second;
            // Ensure the move is within bounds and not a wall
            if (nx >= 0 && nx < N && ny >= 0 && ny < N && !visited[nx][ny] && grid[nx][ny].type != WALL) {
                visited[nx][ny] = true;
                vector<Cell*> newPath = path;
                newPath.push_back(&grid[nx][ny]);
                q.push({ &grid[nx][ny], newPath });
            }
        }
    }

    return {}; // No valid path found
}

//count how many words from WORDS appear as a contigious substring in the user's path
int countWordsInPath(const vector<Cell*>& userPath) {
    //build the string of letters encoutered along the path
    string pathLetters;
    for (auto* c : userPath) {
        pathLetters.push_back(c->type);
    }
    //for reach possible word in WORDSn count how many times it appears in pathLetters
    int totalFound = 0;
    for (auto& word : easyword) {
        //find all occurences of word in pathLetters
        size_t startPos = 0;
        while (true) {
            size_t idx = pathLetters.find(word, startPos);
            if (idx == string::npos) {
                break;
            }
            totalFound++;
            //move startPos so we can find overalapping occurences if any
            startPos = idx + 1;
        }
    }
    return totalFound;
}


//let the user build a path from start with move-limit checking
pair<int, vector<Cell*>> getUserPath(int N, int maxMoves, int& attempts, std::chrono::time_point<std::chrono::high_resolution_clock>& startTime, int totalTimeLimit) {
    Cell* current = &grid[0][0]; // Start at the beginning
    vector<Cell*> userPath;
    userPath.push_back(current);
    int moveCount = 0;

    // Calculate the row position for text below the grid
    int textRow = N + 6; // Adjust the offset (6) as needed

    // Print the initial layout
    cout << "\033[2J"; // Clear the screen
    cout << "Time remaining: " << totalTimeLimit << " seconds\n";
    //cout << "Currently at (" << current->x << "," << current->y << ") containing '" << current->type << "'\n";
    cout << "Moves left: " << maxMoves << "\n";
    printGrid(N, current->x, current->y);
    cout << "\033[" << textRow << ";1H"; // Move to the dynamically calculated row
    cout << "Possible moves (U=Up, D=Down, L=Left, R=Right, Q=Up-Left, E=Up-Right, Z=Down-Left, X=Down-Right)\n";
    cout << "S=Stop, A=Restart, E=Exit: ";
    cout << "\033[" << (textRow + 2) << ";1H"; // Move to the dynamically calculated row
    cout << "Attempts left: " << attempts << "\033[K"; // Display attempts left

    while (moveCount < maxMoves) {
        // Calculate remaining time
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        int remainingTime = totalTimeLimit - elapsedTime;
        if (remainingTime < 0) remainingTime = 0; // Ensure it doesn't go negative

        // Update time remaining
        cout << "\033[1;1H"; // Move to row 1, column 1
        cout << "Time remaining: " << remainingTime << " seconds\033[K"; // Clear the rest of the line

        // Update current position and moves left
        cout << "\033[2;1H"; // Move to row 2, column 1
        cout << "Currently at (" << current->x << "," << current->y << ") containing '" << current->type << "'\033[K"; // Clear the rest of the line
        cout << "\033[3;1H"; // Move to row 3, column 1
        cout << "Moves left: " << (maxMoves - moveCount) << "\033[K"; // Clear the rest of the line

        // Print the grid with the current position highlighted
        printGrid(N, current->x, current->y);

        // Prompt for input
        cout << "\033[" << textRow << ";1H"; // Move to the dynamically calculated row
        cout << "Possible moves (U=Up, D=Down, L=Left, R=Right, Q=Up-Left, E=Up-Right, Z=Down-Left, X=Down-Right)\n";
        cout << "S=Stop, A=Restart, E=Exit: ";

        char choice;
        cin >> choice;
        choice = toupper(choice);

        // Handle special commands: Exit or Restart
        if (choice == 'E') {
            cout << "\033[2J"; // Clear the screen
            cout << "\033[1;1H";
            cout << "Exiting the game...\n";
            return { -1, userPath }; // Use -1 to indicate exit
        }
        if (choice == 'A') { // Changed restart button to 'A'
            cout << "\033[2J"; // Clear the screen
            cout << "\033[1;1H";
            cout << "Restarting the game...\n";
            startTime = std::chrono::high_resolution_clock::now(); // Reset the timer
            return { -2, userPath }; // Use -2 to indicate restart
        }

        if (choice == 'S') { // Stop condition
            if (current->type == END) {
                cout << "\033[2J"; // Clear the screen
                cout << "\033[1;1H";
                cout << "You successfully reached the end!\n";
                return { 1, userPath }; // Success
            }
            else {
                cout << "\033[2J"; // Clear the screen
                cout << "\033[1;1H";
                cout << "You stopped early.\n";
                return { 0, userPath }; // Failed path
            }
        }

        int nx = current->x;
        int ny = current->y;

        // Determine new coordinates based on user input
        switch (choice) {
        case 'U': nx--; break;
        case 'D': nx++; break;
        case 'L': ny--; break;
        case 'R': ny++; break;
        case 'Q': nx--; ny--; break;
        case 'E': nx--; ny++; break;
        case 'Z': nx++; ny--; break;
        case 'X': nx++; ny++; break;
        default:
            cout << "\033[" << (textRow + 1) << ";1H"; // Move to the dynamically calculated row
            cout << "Invalid input! Enter a valid move.\033[K"; // Clear the rest of the line
            continue;
        }

        // Check if movement is valid
        if (nx < 0 || nx >= N || ny < 0 || ny >= N) {
            cout << "\033[" << (textRow + 1) << ";1H"; // Move to the dynamically calculated row
            cout << "Invalid move: out of bounds.\033[K"; // Clear the rest of the line
            attempts--; // Decrement attempts
            cout << "\033[" << (textRow + 2) << ";1H"; // Move to the dynamically calculated row
            cout << "Attempts left: " << attempts << "\033[K"; // Update attempts left dynamically
            if (attempts == 0) {
                cout << "\033[2J"; // Clear the screen
                cout << "\033[1;1H";
                cout << "You ran out of attempts! Game over.\n";
                return { 0, userPath }; // Failed path
            }
            // Reset position to start cell
            current = &grid[0][0];
            userPath.clear();
            userPath.push_back(current);
            moveCount = 0; // Reset move count
            continue; // Skip the rest of the loop and prompt for input again
        }
        if (grid[nx][ny].type == WALL) {
            cout << "\033[" << (textRow + 1) << ";1H"; // Move to the dynamically calculated row
            cout << "Invalid move: You hit a wall (#).\033[K"; // Clear the rest of the line
            attempts--; // Decrement attempts
            cout << "\033[" << (textRow + 2) << ";1H"; // Move to the dynamically calculated row
            cout << "Attempts left: " << attempts << "\033[K"; // Update attempts left dynamically
            if (attempts == 0) {
                cout << "\033[2J"; // Clear the screen
                cout << "\033[1;1H";
                cout << "You ran out of attempts! Game over.\n";
                return { 0, userPath }; // Failed path
            }
            // Reset position to start cell
            current = &grid[0][0];
            userPath.clear();
            userPath.push_back(current);
            moveCount = 0; // Reset move count
            continue; // Skip the rest of the loop and prompt for input again
        }

        // Move to the next cell
        moveCount++;
        current = &grid[nx][ny];
        userPath.push_back(current);

        // If we reached the end cell, return success
        if (current->type == END) {
            cout << "\033[2J"; // Clear the screen
            cout << "\033[1;1H";
            cout << "Congratulations! You reached the END cell!\n";
            cout << "User Path: ";
            for (Cell* cell : userPath) {
                cout << "(" << cell->x << "," << cell->y << ") -> ";
                cout << cell->type << " ";
            }
            cout << "END\n";
            return { 1, userPath }; // Success
        }
    }

    // If maxMoves exceeded
    cout << "\033[2J"; // Clear the screen
    cout << "You ran out of moves before reaching the end.\n";
    return { 0, userPath };
}
// Function to extract and display words found in the user's path
void displayFoundWords(const vector<Cell*>& userPath, const vector<string>& wordList) {
    // Build the string of letters encountered along the path
    string pathLetters;
    for (auto* cell : userPath) {
        pathLetters.push_back(cell->type);
    }

    // Check for each word in the word list
    vector<string> foundWords;
    for (const string& word : wordList) {
        if (pathLetters.find(word) != string::npos) {
            foundWords.push_back(word);
        }
    }

    // Display the found words
    if (!foundWords.empty()) {
        cout << "Words found in your path: ";
        for (const string& word : foundWords) {
            cout << word << "\t";
        }
    }
    else {
        cout << "No words found in your path.\n";
    }
}

// Main function

int main() {
    srand(time(0)); // Seed the random number generator

    while (true) { // Outer loop for restarting the game
        std::cout << R"(
                        88888888888888888888888888888888888888888888888888888888888888888888888
                        88.._|      | `-.  | `.  -_-_ _-_  _-  _- -_ -  .'|   |.'|     |  _..88
                        88   `-.._  |    |`!  |`.  -_ -__ -_ _- _-_-  .'  |.;'   |   _.!-'|  88
                        88      | `-!._  |  `;!  ;. _______________ ,'| .-' |   _!.i'     |  88
                        88..__  |     |`-!._ | `.| |_______________||."'|  _!.;'   |     _|..88
                        88   |``"..__ |    |`";.| i|_|MMMMMMMMMMM|_|'| _!-|   |   _|..-|'    88
                        88   |      |``--..|_ | `;!|l|MMoMMMMoMMM|1|.'j   |_..!-'|     |     88
                        88   |      |    |   |`-,!_|_|MMMMP'YMMMM|_||.!-;'  |    |     |     88
                        88___|______|____!.,.!,.!,!|d|MMMo * loMM|p|,!,.!.,.!..__|_____|_____88
                        88      |     |    |  |  | |_|MMMMb,dMMMM|_|| |   |   |    |      |  88
                        88      |     |    |..!-;'i|r|MPYMoMMMMoM|r| |`-..|   |    |      |  88
                        88      |    _!.-j'  | _!,"|_|M<>MMMMoMMM|_||!._|  `i-!.._ |      |  88
                        88     _!.-'|    | _."|  !;|1|MbdMMoMMMMM|l|`.| `-._|    |``-.._  |  88
                        88..-i'     |  _.''|  !-| !|_|MMMoMMMMoMM|_|.|`-. | ``._ |     |``"..88
                        88   |      |.|    |.|  !| |u|MoMMMMoMMMM|n||`. |`!   | `".    |     88
                        88   |  _.-'  |  .'  |.' |/|_|MMMMoMMMMoM|_|! |`!  `,.|    |-._|     88
                        88  _!"'|     !.'|  .'| .'|[@]MMMMMMMMMMM[@] \|  `. | `._  |   `-._  88
                        88-'    |   .'   |.|  |/| /                 \|`.  |`!    |.|      |`-88
                        88      |_.'|   .' | .' |/                   \  \ |  `.  | `._-Lee|  88
                        88     .'   | .'   |/|  /                     \ |`!   |`.|    `.  |  88
                        88  _.'     !'|   .' | /                       \|  `  |  `.    |`.|  88
                        88 vanishing point 888888888888888888888888888888888888888888888(FL)888    
)" << '\n';
        cout << "                                                Welcome, adventurer! \n            You stand at the entrance of a vast labyrinth. Navigate its twists and turns to find your way out. \n\n";
        cout << "                     --------------------------------------------------------------------------\n";
        cout << "                     --------------------------------------------------------------------------\n\n";
        
       
        Difficulty diff = chooseDifficulty();
        DifficultySettings ds = getDifficultySettings(diff);

        int N = ds.gridSize;
        int attempts = ds.attempts; // Reset attempts to initial value
        int maxMoves = ds.maxMoves;
        int requiredWords = ds.requiredWords;

        // Set a total time limit for the game based on difficulty
        int totalTimeLimit;
        switch (diff) {
        case Easy:
            totalTimeLimit = 300; // 5 minutes for Easy
            break;
        case Medium:
            totalTimeLimit = 200; // ~3.5 minutes for Medium
            break;
        case Hard:
            totalTimeLimit = 120; // 2 minutes for Hard
            break;
        default:
            totalTimeLimit = 300; // Default to Easy time limit
            break;
        }

        // Retry mechanism to ensure valid paths exist
        bool validGrid = false;
        while (!validGrid) {
            // Initialize the grid and add walls, letters, and neighbors
            initializeGrid(N);
            addNeighbors(N);
            addWalls(N);
            addLetters(N);

            // Set the start and end cells
            grid[0][0].type = START;
            grid[N - 1][N - 1].type = END;

            // BFS to find all validPaths
            findAllPaths(&grid[0][0], &grid[N - 1][N - 1], N);

            // If no valid paths, retry grid initialization
            if (validPaths.empty()) {
                cout << "No valid paths found. Retrying grid initialization...\n";
                continue;
            }

            // If valid paths exist, proceed with the game
            validGrid = true;
        }

        // If valid paths exist, inject words and print the grid
        insertWords(diff);
        grid[0][0].type = START;
        grid[N - 1][N - 1].type = END;
        vector<Cell*> shortestPath = findShortestPath(N);

        printGrid(N);

        // Start the game timer
        auto startTime = std::chrono::high_resolution_clock::now();

        // Let the user interact with the grid to build a path up to attempts times
        bool userWon = false;
        vector<Cell*> theuserpath;

        while (attempts > 0) {
            cout << "\nYou have " << attempts << " attempt(s) remaining.\n";

            // Build user path in a pair of the path validation and a vector of paths
            pair<int, vector<Cell*>> Userpath_Validation_pair;
            int user_path_validation;
            Userpath_Validation_pair = getUserPath(N, maxMoves, attempts, startTime, totalTimeLimit);

            user_path_validation = Userpath_Validation_pair.first;

            // Handle exit or restart
            if (user_path_validation == -1) {
                cout << "\033[1;1H";
                cout << "Exiting the game. Goodbye!\n";
                return 0; // Exit the game
            }
            if (user_path_validation == -2) {
                cout << "Restarting the game...\n";
                break; // Restart the game
            }

            // Get the user path and then get the shortest path
            theuserpath = Userpath_Validation_pair.second;

            // Check if user's path is valid
            if (user_path_validation == 0) {
                cout << "\nAnd Your path is not a valid path from S to E. Try again!\n\n";
                attempts--;
                if (attempts == 0) {
                    cout << "Woops you won't be trying again, no attempts left. Game Over!\n";
                }
                continue;
            }
            else {
                // If path is valid, calculate the final score
                int shortestLen = shortestPath.size() - 1;
                int userLen = (int)theuserpath.size() - 1;
                int score = 50;

                // Penalize if the user used more moves than the shortest path
                if (userLen > shortestLen) {
                    score -= 2 * (userLen - shortestLen);
                }

                // Add bonus score for valid words in the user's path
                vector<string>* wordList;
                if (diff == Easy) {
                    wordList = &easyword;
                }
                else if (diff == Medium) {
                    wordList = &mediumword;
                }
                else if (diff == Hard) {
                    wordList = &hardword;
                }
                else {
                    wordList = &easyword; // Default to easy words if difficulty is undefined
                }

                // Build the string of letters encountered along the path
                string pathLetters;
                for (Cell* cell : theuserpath) {
                    pathLetters.push_back(cell->type);
                }

                // Check for valid words in the path
                int bonusScore = 0;
                for (const string& word : *wordList) {
                    if (pathLetters.find(word) != string::npos) {
                        bonusScore += 10; // Add 10 points for each valid word found
                    }
                }

                // Add the bonus score to the total score
                score += bonusScore;

                // Display the final score and words found
                cout << "\nYour path is valid.\n";
                cout << "Shortest Path Length: " << shortestLen << "\n";
                cout << "Your Path Length: " << userLen << "\n";
                cout << "Bonus Score (Valid Words): " << bonusScore << "\n";
                cout << "Final Score: " << score << "\n";

                // Display the words found in the user's path
                displayFoundWords(theuserpath, *wordList);

                userWon = true;
                break;
            }
        }

        // End the game timer
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

        if (userWon == false) {
            if (duration >= totalTimeLimit) {
                cout << "\nTime's up! Better luck next time!\n";
            }
            else {
                cout << "\nBetter luck next time!\n";
            }
        }
        else {
            cout << "\nThanks for playing!\n";
        }

        // Display the elapsed time
        cout << "Time taken: " << duration << " seconds\n";

        // Ask the user if they want to play again
        //cout << "\033[2J";
        //cout << "\033[1;1H";
        cout << "Do you want to play again? (Y/N): ";
        char playAgain;
        cin >> playAgain;
        cout << "\033[2J";
        cout << "\033[1;1H";
        if (toupper(playAgain) != 'Y') {
            cout << "Exiting the game. Goodbye!\n";
            break; // Exit the outer loop
        }
    }

    return 0;
}