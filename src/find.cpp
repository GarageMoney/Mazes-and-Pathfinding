#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <ctime>
using namespace std;

// Global Variables
const int n = 60;
    int screenWidth = n * 15; 
    int screenHeight = n * 15;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
bool maxSpeed = false;
stack<pair<int, int>> Track;

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////    SDL Functions    //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void clearScreen(SDL_Renderer* renderer)
{SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);}

void displayScreen(SDL_Renderer* renderer)
{SDL_RenderPresent(renderer);}

//Draw Current Grid State
void drawState(SDL_Renderer* renderer, char grid[][n])
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(grid[i][j] == ' ')
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            else if(grid[i][j] == 's')
                SDL_SetRenderDrawColor(renderer, 0, 255, 150, 255);
            else if(grid[i][j] == 'e')
                SDL_SetRenderDrawColor(renderer, 255, 0, 100, 255);   
            else if(grid[i][j] == '.')
                SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
            else if(grid[i][j] == 'p')
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else if(grid[i][j] == 'f')
                SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }
}

// Menu Text Renderer
SDL_Rect renderText(SDL_Renderer* renderer, SDL_Color color, int size, const char* text, int x, int y) {
    SDL_Rect textRect;
    TTF_Font* font = TTF_OpenFont("fonts/warownia-black-narrow.ttf", size);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = x - textRect.w / 2;
    textRect.y = y;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    return textRect;
}

// Display 
void Display(SDL_Renderer* renderer, char grid[][n]) {
    clearScreen(renderer);
    drawState(renderer, grid);
    displayScreen(renderer);
    if (!maxSpeed) SDL_Delay(5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////     Functions/Classes     //////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// RECURSION
bool checkBounds(int r, int c)
{
    if(r <= 0 || r >= n - 1 || c <= 0 || c >= n - 1)
        return false;
    return true;
}

void generatePaths(SDL_Renderer* renderer, char grid[][n], int r, int c)
{
    Display(renderer, grid);
    //check out of bounds
    if(!checkBounds(r, c)) return;
    
    //remove site
    grid[r][c] = ' ';
    
    //choose a valid neighboring cell
    vector<pair<int, int>> validCells;
    if(checkBounds(r - 1, c) && 
    grid[r - 1][c] != ' ' && grid[r - 1][c - 1] != ' ' && grid[r - 1][c + 1] != ' ' && 
    grid[r - 2][c] != ' ' && grid[r - 2][c - 1] != ' ' && grid[r - 2][c + 1] != ' ')
        validCells.push_back({r - 1, c});
    if(checkBounds(r + 1, c) && 
    grid[r + 1][c] != ' ' && grid[r + 1][c - 1] != ' ' && grid[r + 1][c + 1] != ' ' && 
    grid[r + 2][c] != ' ' && grid[r + 2][c - 1] != ' ' && grid[r + 2][c + 1] != ' ')
        validCells.push_back({r + 1, c});
    if(checkBounds(r, c - 1) && 
    grid[r][c - 1] != ' ' && grid[r - 1][c - 1] != ' ' && grid[r + 1][c - 1] != ' ' && 
    grid[r][c - 2] != ' ' && grid[r - 1][c - 2] != ' ' && grid[r + 1][c - 2] != ' ')
        validCells.push_back({r, c - 1});
    if(checkBounds(r, c + 1) && 
    grid[r][c + 1] != ' ' && grid[r - 1][c + 1] != ' ' && grid[r + 1][c + 1] != ' ' && 
    grid[r][c + 2] != ' ' && grid[r - 1][c + 2] != ' ' && grid[r + 1][c + 2] != ' ')
        validCells.push_back({r, c + 1});

    //if no valid neighbors and stack is empty then return
    //if not remove one stack history and retry
    //if valid neighbor(s) exist select one and generate paths 
    if(validCells.empty())
    {
        if(Track.size() <= 1)
            return;
        else
        {
            Track.pop();
            generatePaths(renderer, grid, Track.top().first, Track.top().second);
        }
    }
    else
    {
        Track.push({r, c});
        int random = rand() % validCells.size();
        generatePaths(renderer, grid, validCells.at(random).first, validCells.at(random).second);
    }
    return;
}

void generateRecursive(SDL_Renderer* renderer, char grid[][n], int r, int c, int &s, int &e)
{
    //generate paths
    generatePaths(renderer, grid, r, c);

    //add start point & end point
    for(int i = 0; i < n - 1; i++)
        if(grid[i][i] != 'X'){grid[i][i] = 's'; s = i; break;}
    for(int i = n - 2; i > 0; i--)
        if(grid[i][i] != 'X'){grid[i][i] = 'e'; e = i; break;}

    //empty global stack
    while(!Track.empty())
        Track.pop();

    Display(renderer, grid); SDL_Delay(1000);
}

// PRIMS
void generatePrims(SDL_Renderer* renderer, char grid[][n], int r0, int c0, int &s, int &e)
{
    //select start and store all adjacent walls
    grid[r0][c0] = ' ';
    vector<pair<int, int>> walls = {{r0 - 1, c0}, {r0 + 1, c0}, {r0, c0 - 1}, {r0, c0 + 1}};

    //while walls exist
    while (!walls.empty())
    {
        Display(renderer, grid);
        //Choose one wall at random
        int random = rand() % walls.size();
        int r = walls.at(random).first, c = walls.at(random).second;
        //if the wall is within bounds of the grid exterior
            //depending on the direction: if surrounding 5 cells (exclude behind) are walls
                //wall can be converted into a path
                //Store adjacent wall cells into the list 
        if (r > 0 && c > 0 && r < n - 1 && c < n - 1)
        {
            if(grid[r - 1][c] == ' ' && grid[r + 1][c] == 'X' && grid[r + 1][c - 1] == 'X' &&
               grid[r + 1][c + 1] == 'X' && grid[r][c - 1] == 'X' && grid[r][c + 1] == 'X') 
            {
                grid[r][c] = ' ';
                if(grid[r - 1][c] == 'X') walls.push_back({r - 1, c});
                if(grid[r + 1][c] == 'X') walls.push_back({r + 1, c});
                if(grid[r][c - 1] == 'X') walls.push_back({r, c - 1});
                if(grid[r][c + 1] == 'X') walls.push_back({r, c + 1});
            }
            else if(grid[r + 1][c] == ' ' && grid[r - 1][c] == 'X' && grid[r - 1][c - 1] == 'X' && 
                    grid[r - 1][c + 1] == 'X' && grid[r][c - 1] == 'X' && grid[r][c + 1] == 'X') 
            {
                grid[r][c] = ' ';
                if(grid[r - 1][c] == 'X') walls.push_back({r - 1, c});
                if(grid[r + 1][c] == 'X') walls.push_back({r + 1, c});
                if(grid[r][c - 1] == 'X') walls.push_back({r, c - 1});
                if(grid[r][c + 1] == 'X') walls.push_back({r, c + 1});
            }
            else if(grid[r][c - 1] == ' ' && grid[r][c + 1] == 'X' && grid[r - 1][c + 1] == 'X' && 
                    grid[r + 1][c + 1] == 'X' && grid[r - 1][c] == 'X' && grid[r + 1][c] == 'X') 
            {
                grid[r][c] = ' ';
                if(grid[r][c - 1] == 'X') walls.push_back({r, c - 1});
                if(grid[r][c + 1] == 'X') walls.push_back({r, c + 1});
                if(grid[r - 1][c] == 'X') walls.push_back({r - 1, c});
                if(grid[r + 1][c] == 'X') walls.push_back({r + 1, c});
            }
            else if(grid[r][c + 1] == ' ' && grid[r][c - 1] == 'X' && grid[r - 1][c - 1] == 'X' && 
                    grid[r + 1][c - 1] == 'X' && grid[r - 1][c] == 'X' && grid[r + 1][c] == 'X') 
            {
                grid[r][c] = ' ';
                if(grid[r][c - 1] == 'X') walls.push_back({r, c - 1});
                if(grid[r][c + 1] == 'X') walls.push_back({r, c + 1});
                if(grid[r - 1][c] == 'X') walls.push_back({r - 1, c});
                if(grid[r + 1][c] == 'X') walls.push_back({r + 1, c});
            }
        }
        //erase wall cell as it has been checked
        walls.erase(walls.begin() + random);
    }

    //add start point & end point
    for(int i = 0; i < n - 1; i++)
        if(grid[i][i] != 'X'){grid[i][i] = 's'; s = i; break;}
    for(int i = n - 2; i > 0; i--)
        if(grid[i][i] != 'X'){grid[i][i] = 'e'; e = i; break;}
    Display(renderer, grid); SDL_Delay(1000);
}

// KRUSKAL
class Set
{
    public:
        vector<pair<int, int>> elements;

        Set(pair<int, int> rc)
        {
            elements.push_back(rc);
        }
        void Add(pair<int, int> rc)
        {
            for(int i = 0; i < elements.size(); i++)
                if(elements.at(i) == rc) return;
            elements.push_back(rc);
        }
        bool Find(pair<int, int> rc)
        {
            for(int i = 0; i < elements.size(); i++)
                if(elements.at(i) == rc)
                    return true;
            return false;
        }
        void Merge(Set* other)
        {
            for(int i = 0; i < other->elements.size(); i++)
                elements.push_back(other->elements.at(i));
            other->elements.clear();
        }
};

void generateKruskal(SDL_Renderer* renderer, char grid[][n], int o, int p, int &s, int &e)
{
    //grid is initially filled entirely with walls
    vector<Set*> sets;
    vector<pair<int, int>> walls;
    vector<pair<int, int>> pot;
    for(int i = 1; i < n - 1; i++)
        for(int j = 1; j < n - 1; j++)
            walls.push_back({i, j});
    top:
    //while walls exist
    while(!walls.empty())
    {   
        Display(renderer, grid); 
        //select a random wall
        //remove wall from list of walls
        int random = rand() % walls.size(); 
        int r = walls.at(random).first, c = walls.at(random).second;
        walls.erase(walls.begin() + random);
        
        //if any corner cell is a path store it in a vector
        vector<pair<int, int>> corner = {{r - 1, c - 1}, {r - 1, c + 1}, {r + 1, c - 1}, {r + 1, c + 1}};
        vector<pair<int, int>> corPaths;
        for(int i = 0; i < corner.size(); i++)
            if(grid[corner.at(i).first][corner.at(i).second] == ' ')
                corPaths.push_back(corner.at(i));

        //if any adjacent cell is a path store it in a vector
        vector<pair<int, int>> adjacent = {{r - 1, c}, {r + 1, c}, {r, c - 1}, {r, c + 1}};
        vector<pair<int, int>> adjPaths;
        for(int i = 0; i < adjacent.size(); i++)
            if(grid[adjacent.at(i).first][adjacent.at(i).second] == ' ')
                adjPaths.push_back(adjacent.at(i));

        //if there are no adjacent or corner paths 
        if(adjPaths.empty())
        {
            if(corPaths.empty())
            {
                grid[r][c] = ' ';
                sets.push_back(new Set({r, c}));
            }
            else if(corPaths.size() < 3)
                pot.push_back({r, c});
        }
        //if there is one adjacent path
        else if(adjPaths.size() == 1)
        {
            //if corner path exists and it is opposite edge of 
            //adjacent path continue to next
            int dr = r - adjPaths.front().first;
            int dc = c - adjPaths.front().second;
            if(dr == 0)
                if(grid[r - 1][c + dc] == ' ' || grid[r + 1][c + dc] == ' ')
                    continue;
            if(dc == 0)
                if(grid[r + dr][c - 1] == ' ' || grid[r + dr][c + 1] == ' ')
                    continue;

            //replace wall with path and add new path to adjacent path set
            grid[r][c] = ' ';
            for(int i = 0; i < sets.size(); i++)
            {
                if(sets.at(i)->Find(adjPaths.front()))
                {
                    sets.at(i)->Add({r, c});
                    break;
                }
            }
        }
        //if there are two adjacent paths
        else if(adjPaths.size() == 2)
        {
            //if the two adjacent paths are diagonal 
            //to each other continue to next
            int dr = adjPaths.front().first - adjPaths.back().first;
            int dc = adjPaths.front().second - adjPaths.back().second;
            if(dr != 0 && dc != 0) continue;
            
            //find the two sets containing each adjacent path
            //if the two sets happen to be equal continue to next
            int a = -1, b = -2;
            for(int i = 0; i < sets.size(); i++)
            {
                if(sets.at(i)->Find(adjPaths.front())) a = i;
                if(sets.at(i)->Find(adjPaths.back())) b = i;
                if(a != -1 && b != -2) break;
            }
            if(a == b) continue;

            //replace wall with path and merge with both sets
            grid[r][c] = ' ';
            sets.at(a)->Add({r, c});
            sets.at(a)->Merge(sets.at(b));
            delete sets.at(b);
            sets.erase(sets.begin() + b); 
        }
    }

    //remove any disconnected islands
    if(sets.size() != 1 && s != 1)
    {
        walls.swap(pot);
        s++;
        goto top;
    }
    //add start point & end point
    for(int i = 0; i < n - 1; i++)
        if(grid[i][i] != 'X'){grid[i][i] = 's'; s = i; break;}
    for(int i = n - 2; i > 0; i--)
        if(grid[i][i] != 'X'){grid[i][i] = 'e'; e = i; break;}
    Display(renderer, grid); SDL_Delay(1000);

    //de memory
    for(Set* i : sets)
        delete i;
}

// RESET Functions
//clear explored path markers in maze
void resetMaze(SDL_Renderer* renderer, char grid[][n])
{
    for(int i = 1; i < n - 1; i++)
        for(int j = 1; j < n - 1; j++)
            if(grid[i][j] == '.' || grid[i][j] == 'f') 
                grid[i][j] = ' ';
    Display(renderer, grid); SDL_Delay(1000);
}

//Reset entire grid to walls
void resetAll(SDL_Renderer* renderer, char grid[][n])
{
    for(int i = 1; i < n - 1; i++)
        for(int j = 1; j < n - 1; j++)
            grid[i][j] = 'X';
    Display(renderer, grid); SDL_Delay(1000);
}

// RECURSIVE solve
void findRecursive(SDL_Renderer* renderer, char grid[][n], int r0, int c0, int r1, int c1)
{
    //if current equals end path found
    if(r0 == r1 && c0 == c1) return;
    //if current is a path mark it as explored
    if(grid[r0][c0] == ' ' || grid[r0][c0] == 's')
    {
        Track.push({r0, c0});
        if(grid[r0][c0] != 's')
        {
            grid[r0][c0] = 'p'; //(visualization)
            Display(renderer, grid); //SDL_Delay(10);
            grid[r0][c0] = '.';
        }
            
    }
    //search next cell recursively
    if(grid[r0 - 1][c0] == ' ' || grid[r0 - 1][c0] == 'e')
    {
        findRecursive(renderer, grid, r0 - 1, c0, r1, c1);
    }
    else if(grid[r0][c0 - 1] == ' ' || grid[r0][c0 - 1] == 'e')
    {
        findRecursive(renderer, grid, r0, c0 - 1, r1, c1);
    }
    else if(grid[r0 + 1][c0] == ' ' || grid[r0 + 1][c0] == 'e')
    {
        findRecursive(renderer, grid, r0 + 1, c0, r1, c1);
    }
    else if(grid[r0][c0 + 1] == ' ' || grid[r0][c0 + 1] == 'e')
    {
        findRecursive(renderer, grid, r0, c0 + 1, r1, c1);
    }
    else
    {   //if dead-end is reached
        //backtrack until unexplored neighbor cell is discovered
        //if there is nowhere to backtrack return;
        if(!Track.empty())
        {
            Track.pop();
            int r = Track.top().first, c = Track.top().second;
            findRecursive(renderer, grid, r, c, r1, c1);
        }
    }
    return;
}

void solveRecursive(SDL_Renderer* renderer, char grid[][n], int s, int e)
{
    findRecursive(renderer, grid, s, s, e, e);
    int t = Track.size();
    //While path from start to end exists: pop cell by cell and display 
    while(Track.size() != 1)
    {
        grid[Track.top().first][Track.top().second] = 'f';
        Track.pop();
        Display(renderer, grid); SDL_Delay(5);
    }
    grid[s][s] = 's';
    Display(renderer, grid); SDL_Delay(1000); resetMaze(renderer, grid);
}

// DEAD END solve
bool isDE(char grid[][n], int r, int c)
{
    //cell must be unexplored
    if(grid[r][c] != ' ') return false;
    //cell is a dead-end if 3 out of 4 adjacent cells are obstacle or previously explored
    int test = 0;
    if(grid[r - 1][c] == '.' || grid[r - 1][c] == 'X') test++;
    if(grid[r + 1][c] == '.' || grid[r + 1][c] == 'X') test++;
    if(grid[r][c - 1] == '.' || grid[r][c - 1] == 'X') test++;
    if(grid[r][c + 1] == '.' || grid[r][c + 1] == 'X') test++;
    return test == 3;
}

pair<int, int> nextPos(char grid[][n], int r, int c)
{
    if(isDE(grid, r - 1, c)) return {r - 1, c};
    if(isDE(grid, r + 1, c)) return {r + 1, c};
    if(isDE(grid, r, c - 1)) return {r, c - 1};
    if(isDE(grid, r, c + 1)) return {r, c + 1};
    return {-1, -1}; //junction has been reached
}

void solveDE(SDL_Renderer* renderer, char grid[][n], int s, int e)
{
    //mark all dead-ends in the grid and store into vector
    vector<pair<int, int>> de;
    for(int r = 1; r < n - 1; r++)
        for(int c = 1; c < n - 1; c++)
            if(isDE(grid, r, c))
                de.push_back({r, c});

    //while dead-ends exist
    while(!de.empty())
    {
        //for each dead-end:
        for(int i = 0; i < de.size(); i++)
        {
            //mark as explored
            int r = de.at(i).first, c = de.at(i).second;
            grid[r][c] = 'p'; //(visualization)
            Display(renderer, grid); //SDL_Delay(10);
            grid[r][c] = '.';
            
            //if junction has been reached remove from dead-end list
            //else push back continuing dead-end cell
            pair<int, int> next = nextPos(grid, r, c); 
            if(next.first == -1){
                de.erase(de.begin() + i); i--; continue;}
            de.at(i) = next;
        }
    }
    //display solution
    solveRecursive(renderer, grid, s, e); 
}

// Breadth First Search solve
class Node 
{
    //represents a cell
    //stores location on grid
    //pointers to parent cell & children cells
    private:
        pair<int, int> point; 
        Node* parent; vector<Node*> children;
        int g_score; int h_score; int f_score; //AStar
        //f, g, h score for path prediction
    public:
        //Constructing requires grid point
        Node(pair<int, int> a_point) : 
        point(a_point), f_score(9999), h_score(9999), g_score(9999) {}
        //mutators
        void setChild(Node* d_) {children.push_back(d_);}
        void setParent(Node* d_) {parent = d_;}
        void setG(int g) {g_score = g;}
        void setH(int h) {h_score = h;}
        void setF() {f_score = g_score + h_score;}
        void setF(int f) {f_score = f;};
        //accessors
        int R() {return point.first;}
        int C() {return point.second;}
        int getG() {return g_score;}
        int getH() {return h_score;}
        int getF() {return f_score;}
        Node* Parent() {return parent;}
        vector<Node*> Children() {return children;}
};

//Function to deallocate memory for a Node tree
void deMemory(Node* root) {
    for (auto child : root->Children()) {
        deMemory(child);
    }
    delete root;
}

void solveBFS(SDL_Renderer* renderer, char grid[][n], int s, int e)
{
    //queue to store valid cells
    queue<Node*> Order;
    //store intial cell
    Node* root = new Node({s, s});
    Order.push(root);
    Node* curr;
    //traverse the maze continuously as long as 
    //unexplored valid cells exist
    while(!Order.empty())
    {   
        curr = Order.front();
        Order.pop();
        int r = curr->R(); int c = curr->C();
        //if current cell equals end cell: break out
        //if current cell has been explored: next cell
        if(grid[r][c] == 'e') break;
        if(grid[r][c] == '.') continue;
        if(grid[r][c] != 's')
        {
            grid[r][c] = 'p'; //(visualization)
            Display(renderer, grid); //SDL_Delay(10);
            grid[r][c] = '.';
        }
        
        //if any unexplored valid neighbor cells exist: 
        //add it to search queue and set necessary links to current cell
        if(grid[r - 1][c] != 'X' && grid[r - 1][c] != '.')
        {
            Node* up = new Node({r - 1, c});
            up->setParent(curr);
            curr->setChild(up);
            Order.push(up);
        }
        if(grid[r + 1][c] != 'X' && grid[r + 1][c] != '.')
        {
            Node* down = new Node({r + 1, c});
            down->setParent(curr);
            curr->setChild(down);
            Order.push(down);
        }
        if(grid[r][c - 1] != 'X' && grid[r][c - 1] != '.')
        {
            Node* left = new Node({r, c - 1});
            left->setParent(curr);
            curr->setChild(left);
            Order.push(left);
        }
        if(grid[r][c + 1] != 'X' && grid[r][c + 1] != '.')
        {
            Node* right = new Node({r, c + 1});
            right->setParent(curr);
            curr->setChild(right);
            Order.push(right);
        }
    }
    //path found: 
    while(curr != root)
    {
        //backtrack solution path and display
        Display(renderer, grid); SDL_Delay(5);
        curr = curr->Parent();
        grid[curr->R()][curr->C()] = 'f';
    }
    //Deallocation
    deMemory(root); SDL_Delay(1000); resetMaze(renderer, grid);
}

// ASTAR solve
void solveAStar(SDL_Renderer* renderer, char grid[][n], int s, int e)
{
    //list to store valid cells
    list<Node*> OS;
    //store intial cell after setting score values
    Node* root = new Node({s, s});
    root->setG(0); 
    root->setH(abs(s - e) + abs(s - e));
    root->setF();
    OS.push_back(root);
    Node* curr;
    //traverse the maze continuously as long as 
    //unexplored valid cells exist
    while(!OS.empty())
    { 
        //select cell stored in list with the lowest h score
        //if scores are equivalent select any
        curr = OS.front();
        list<Node*>::iterator it;
        for(it = OS.begin(); it != OS.end(); it++)
            if((*it)->getH() < curr->getH())
                curr = *it;

        //if current cell equals end cell: break out
        //if current cell has been explored: next cell
        int r = curr->R(), c = curr->C();
        if(grid[r][c] == 'e') break;
            if(grid[r][c] == '.') continue;
        grid[r][c] = 'p'; //(visualization)
        Display(renderer, grid); //SDL_Delay(10);
        grid[r][c] = '.';

        //remove current cell from list as it has been searched
        OS.remove(curr); 

        //if any unexplored valid neighbor cells exist: 
        //set necessary links from current cell
        if(grid[r - 1][c] != 'X' && grid[r - 1][c] != '.')
        {
            Node* up = new Node({r - 1, c});
            curr->setChild(up);
        }
        if(grid[r + 1][c] != 'X' && grid[r + 1][c] != '.')
        {
            Node* down = new Node({r + 1, c});
            curr->setChild(down);
        }
        if(grid[r][c - 1] != 'X' && grid[r][c - 1] != '.')
        {
            Node* left = new Node({r, c - 1});
            curr->setChild(left);
        }
        if(grid[r][c + 1] != 'X' && grid[r][c + 1] != '.')
        {
            Node* right = new Node({r, c + 1});
            curr->setChild(right);
        }
        //for each valid neighbor cell:
        for(int i = 0; i < curr->Children().size(); i++)
        {
            //set link to current cell and set score values
            Node* child = curr->Children().at(i);
            child->setParent(curr);
            child->setG(curr->getG() + 1);
            child->setH(abs(child->R() - e) + abs(child->C() - e));
            child->setF();
            //add the cell to the list for next iteration
            OS.push_back(child); 
        }
    }
    //path found:
    grid[s][s] = 's'; 
    while(curr != root)
    {
        //backtrack solution path and display
        Display(renderer, grid); SDL_Delay(5);
        curr = curr->Parent();
        grid[curr->R()][curr->C()] = 'f';
    }
    
    //Deallocation
    deMemory(root); SDL_Delay(1000);
}   

///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////     MAIN     /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    //SDL Initialization
    TTF_Init(); 
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); 
    SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 
        SDL_RENDERER_ACCELERATED, &window, &renderer);
    SDL_GL_SetSwapInterval(0);
    SDL_RenderSetScale(renderer, 1, 1);

    // Time Seed Random
    srand(time(NULL));
    
    //Grid Initialization
    char grid[n][n];
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            grid[i][j] = 'X';

    // Text Initialization
    vector<bool> Clicked(11, false);
    vector<SDL_Rect> textRect(11);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray = {180, 180, 180, 255};
    SDL_Color yellow = {240, 210, 0, 255};
    SDL_Color blue = {0, 200, 255, 255};
    SDL_Color red = {255, 0, 0, 255};

    // Event Loop
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        // waiting for user input:
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                // on user clicks text:
                for (int i = 2; i < textRect.size(); i++) {
                    if (mouseX >= textRect[i].x && mouseX <= (textRect[i].x + textRect[i].w) &&
                        mouseY >= textRect[i].y && mouseY <= (textRect[i].y + textRect[i].h)) {
                            // set corresponding Clicked flags
                            if ((1 < i && i < 5) && 
                                (Clicked[2] || Clicked[3] || Clicked[4])) 
                                for (int j = 2; j < 5; j++) 
                                    Clicked[j] = false;
                            Clicked[i] = !Clicked[i]; 
                    }
                }
            }
        }
        // render texts 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
        textRect[0] = renderText(renderer, Clicked[0] ? yellow : white, 60, "Generation   (Choose 1)", centerX, 10);
        textRect[1] = renderText(renderer, Clicked[1] ? yellow : white, 60, "Pathfinding   (1    to    4)", centerX, screenHeight * 0.40);
        textRect[2] = renderText(renderer, Clicked[2] ? yellow : gray, 35, "Recursion", centerX, screenHeight * 0.15);
        textRect[3] = renderText(renderer, Clicked[3] ? yellow : gray, 35, "Prim", centerX, screenHeight * 0.23);
        textRect[4] = renderText(renderer, Clicked[4] ? yellow : gray, 35, "Kruskal", centerX, screenHeight * 0.31);
        textRect[5] = renderText(renderer, Clicked[5] ? yellow : gray, 35, "Recursion", centerX, screenHeight * 0.55);
        textRect[6] = renderText(renderer, Clicked[6] ? yellow : gray, 35, "Dead-End", centerX, screenHeight * 0.63);
        textRect[7] = renderText(renderer, Clicked[7] ? yellow : gray, 35, "BFS", centerX, screenHeight * 0.71);
        textRect[8] = renderText(renderer, Clicked[8] ? yellow : gray, 35, "A*", centerX, screenHeight * 0.79);
        textRect[9] = renderText(renderer, Clicked[9] ? blue : white, 25, "[Max   Speed]", screenWidth * 0.7, screenHeight * 0.9);
            renderText(renderer, white, 11, "Cannot Override FPS Limiter", screenWidth * 0.7, screenHeight * 0.95);
        textRect[10] = renderText(renderer, Clicked[10] ? red : white, 45, "[run]", screenWidth * 0.9, screenHeight * 0.88);
        SDL_RenderPresent(renderer);

        // User clicked [run] with valid inputs
        // begin visualization
        if (Clicked[10]) { 
            if ((Clicked[2] || Clicked[3] || Clicked[4])) {
                // reset grid
                resetAll(renderer, grid);
                // adjust scale 
                SDL_RenderSetScale(renderer, 15, 15); 
                // set speed
                if (Clicked[9]) 
                    maxSpeed = true;

                // Algorithms
                int start = 0, end = 0;
                if (Clicked[2]) generateRecursive(renderer, grid, n/2, n/2, start, end); 
                else if (Clicked[3]) generatePrims(renderer, grid, 1, 1, start, end); 
                else generateKruskal(renderer, grid, 1, 1, start, end); 

                if (Clicked[5]) solveRecursive(renderer, grid, start, end); 
                if (Clicked[6]) solveDE(renderer, grid, start, end); 
                if (Clicked[7]) solveBFS(renderer, grid, start, end); 
                if (Clicked[8]) solveAStar(renderer, grid, start, end); 
                
                // reset flags
                for (int i = 0; i < Clicked.size(); i++) 
                    Clicked[i] = false;
                maxSpeed = false;
                SDL_RenderSetScale(renderer, 1, 1);
            }
            else Clicked[10] = false;
        }
    } 

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    cout << "safely exited";
    return 0;
}