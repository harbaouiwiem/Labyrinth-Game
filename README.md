1. Introduction:
This project involves generating a maze, represented as a grid, in which a player must navigate from a starting point to an endpoint.

In this context, we focused on algorithmic challenges, particularly:

Modeling a grid as a graph, where each cell is a node and its neighbors are connected by edges.
Exhaustively exploring possible paths using a breadth-first search (BFS).
Randomly injecting words into certain paths and searching for these words in the user’s chosen trajectory.
The code, written in C++, includes multiple difficulty levels (Easy, Medium, and Hard). Each level affects the grid size, the number of allowed attempts, the maximum number of moves, the minimum number of words to integrate, and the time limit to exit the maze. Additionally, the program allows the user to navigate through the grid and earn points based on the efficiency of their path (length relative to the shortest path) and the words found.

2. Grid Modeling as a Graph:
2.1. Data Structure:
We use a 2D array (vector<vector<Cell>> grid) to store all the cells of the maze. Each Cell contains:

x, y: coordinates of the cell in the grid.
type: a character representing either a wall (WALL = '#'), an empty space (EMPTY = '.'), the start (START = '*'), the end (END = '&'), or a letter (A-Z).
visited: a boolean marking whether the cell has been visited during an exploration (BFS).
neighbors: a list of pointers to neighboring cells.
2.2. Adding Neighbors:
After constructing the grid, we perform a systematic traversal to add adjacent cells (top, bottom, left, right, diagonals) to each cell. This step is crucial for the breadth-first search algorithm, allowing it to find all possible path alternatives.

3. Automatic Maze Generation:
3.1. Adding Random Walls:
To introduce difficulty and variability, we randomly place walls in the grid with a 25% probability. However, we ensure that neither the starting cell (0,0) nor the ending cell (N-1, N-1) is blocked.

3.2. Injecting Letters:
In all cells that are neither walls nor the start or end points, we randomly assign a letter (A-Z). This step prepares the grid for the possible insertion of words via our algorithms.

4. Path Searching (BFS):
4.1. Breadth-First Search (BFS) Principle:
We apply BFS to identify all valid paths between the start and end points. We store complete paths in a queue (queue<vector<Cell*>>). During each expansion, we visit an unvisited neighbor and add it to a copy of the current path. When we reach the endpoint, the found path is validated and archived.

4.2. Finding the Shortest Path:
We implement the function findShortestPath to determine the shortest route between the start and endpoint. This function follows the same BFS logic but retains and returns the first (and thus shortest) path that reaches the final cell.

5. Word Integration and Search:
5.1. File Reading and Difficulty-Based Distribution:
Word lists (easy.txt, medium.txt, hard.txt) are loaded at the beginning of the program. Depending on the difficulty level chosen by the user, the program uses the corresponding list for random word injection.

5.2. Injecting Words into Valid Paths:
Once all valid paths have been identified (validPaths), we insert or modify letters in some cells with characters from the selected words. The process follows these steps:

A path from the validPaths list is selected.
A random number (0 to 3) of words to insert is determined.
For each word, we try to find a subsequence in the path where insertion is possible (avoiding modifications to the start and end cells and ensuring no word overlap).
The targeted cells are sequentially assigned the letters of the word.
5.3. Detecting Words Found by the User:
Once the user completes their own path, we analyze the sequence of letters they traveled through. We then compare each word from the difficulty file to the sequence of letters in the path (pathLetters). Matches are detected (find), allowing the user to earn bonus points.

