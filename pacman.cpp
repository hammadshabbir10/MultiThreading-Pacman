#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <cmath>
#include <chrono>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>

std::chrono::steady_clock::time_point frightenedStartTime;
using namespace std;



const int WIDTH = 28;
const int HEIGHT = 28;
const int cellSize = 20;

const int PATH = 0;
const int WALL = 1;
const int PELLET = 2;
const int POWER_PELLET = 3;

int scoreA = 0;
int fd[2];
sem_t speedBoostSemaphore;
sem_t keySemaphore; 
sem_t permitSemaphore;
int maze[HEIGHT][WIDTH] = 
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 1},
    {1, 2, 1, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 2, 2, 1, 0, 1, 2, 1},
    {1, 2, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 1},  
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 1, 1, 1, 1, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1, 1, 1, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1},
    {1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1},
    {1, 1, 2, 2, 1, 2, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 1, 2, 2, 1, 1},
    {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1},
    {1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

enum GhostMode {
    Normal,
    Frightened
};

struct Ghost 
{
    int x, y;
    sf::ConvexShape shape;
    GhostMode mode = Normal;
    sf::Color originalColor;
    bool hasSpeedBoost = false;
    int speedMultiplier = 1;

    Ghost(int startX, int startY, sf::Color color, int numberOfSides) 
    {
        x = startX;
        y = startY;
        originalColor = color;
        shape.setPointCount(numberOfSides);

        float radius = 8; 
        float center_x = 10; 
        float center_y = 10; 

        for (int i = 0; i < numberOfSides; ++i) 
        {
            float angle = 2 * M_PI / numberOfSides * i;
            float x = center_x + radius * cos(angle);
            float y = center_y + radius * sin(angle);
            shape.setPoint(i, sf::Vector2f(x, y));
        }

        shape.setFillColor(color); 
    }

    void updateColor() 
    {
        if (mode == Normal) 
        {
            shape.setFillColor(originalColor);
        } 
        else if (mode == Frightened) 
        {
            shape.setFillColor(sf::Color::Cyan);
        }
    }
};

Ghost ghost1(13, 14, sf::Color::Magenta, 8); 
Ghost ghost2(14, 14, sf::Color::Green, 4);
Ghost ghost3(15, 15, sf::Color::Red, 6);
Ghost ghost4(15, 14, sf::Color::White, 3); 


struct Player 
{
    int x = 1;  
    int y = 1;  
    int lives = 4;
    sf::CircleShape shape;

    Player() 
    {
        shape = sf::CircleShape(cellSize / 2 - 1);
        shape.setFillColor(sf::Color::Yellow);
    }
} pacMan;

enum GameState 
{
    Menu,
    Playing,
    GameOver,
    Help,
};

pthread_mutex_t playerMutex;
pthread_mutex_t ghostMutex;
pthread_mutex_t gameMutex;
pthread_cond_t gameCond;
bool gameStarted = false;

void setupNonBlockingPipe();
void updateScore(int pacmanX, int pacmanY);
void readScore();
void drawMaze(sf::RenderWindow& window);
void* handlePlayerMovement(void* arg);
void* ghostMovement(void* arg);
bool checkCollision(const Player& pacman, const Ghost& ghost);
void updateGame(Player& player, Ghost& ghost1, Ghost& ghost2, Ghost& ghost3, Ghost& ghost4, GameState& state);
void drawLives(sf::RenderWindow& window, const Player& player);
void displayGameOver(sf::RenderWindow& window);
void displayMenu(sf::RenderWindow& window);
void displayInstructions(sf::RenderWindow& window);
void* gameLogic(void* arg);
void* powerPelletSpawner(void* arg);
void* applySpeedBoost(void* arg);
void* uiThread(void* arg);

void setupNonBlockingPipe() 
{
    if (pipe(fd) == -1) 
    {
        cout << "Failed to create pipe" << endl;
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(fd[0], F_GETFL, 0);
    fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);
}

void updateScore(int pacmanX, int pacmanY) 
{
    if (maze[pacmanY][pacmanX] == PELLET) 
    {
        maze[pacmanY][pacmanX] = PATH; 
        scoreA += 1;
        write(fd[1], &scoreA, sizeof(scoreA));
    }
    else if (maze[pacmanY][pacmanX] == POWER_PELLET) 
    {
        maze[pacmanY][pacmanX] = PATH; 
        scoreA += 5; 
        write(fd[1], &scoreA, sizeof(scoreA)); 
        ghost1.mode = Frightened;
        ghost2.mode = Frightened;
        ghost3.mode = Frightened;
        ghost4.mode = Frightened;
        ghost1.updateColor();
        ghost2.updateColor();
        ghost3.updateColor();
        ghost4.updateColor();
        frightenedStartTime = std::chrono::steady_clock::now();
    }
}

void readScore() 
{
    int bufferA;
    if (read(fd[0], &bufferA, sizeof(bufferA)) > 0) 
    {
       cout << "Score updated: " << bufferA << endl;
    }
}

void drawMaze(sf::RenderWindow& window) 
{
    for (int i = 0; i < HEIGHT; i++) 
    {
        for (int j = 0; j < WIDTH; j++) 
        {
            if (maze[i][j] == WALL) 
            {
                sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
                cell.setFillColor(sf::Color::Blue); 
                cell.setPosition(j * cellSize, i * cellSize);
                window.draw(cell);
            } 
            else if (maze[i][j] == PELLET) 
            {
                sf::CircleShape pellet(2);
                pellet.setFillColor(sf::Color::White); 
                pellet.setPosition(j * cellSize + cellSize / 2 - 3, i * cellSize + cellSize / 2 - 3);
                window.draw(pellet);
            } 
            else if (maze[i][j] == POWER_PELLET) 
            {
                sf::CircleShape powerPellet(7); 
                powerPellet.setFillColor(sf::Color::Red); 
                powerPellet.setPosition(j * cellSize + cellSize / 2 - 5, i * cellSize + cellSize / 2 - 5);
                window.draw(powerPellet);
            }
        }
    }
}



struct Cell {
    int x, y;
};


void* handlePlayerMovement(void* arg) 
{
    while (true) 
    {
        usleep(100000); 

        pthread_mutex_lock(&playerMutex);
        int newX = pacMan.x;
        int newY = pacMan.y;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) 
        {   
           newY--;
        }   
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
        {
          newY++;
        }  
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
          newX--;
        }  
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
        {
          newX++;
        }  
        if (newX < 0) 
        {
            newX = WIDTH - 1; 
        } 
        else if (newX >= WIDTH) 
        {
            newX = 0; 
        }
        if (maze[newY][newX] != WALL) 
        {
            pacMan.x = newX;
            pacMan.y = newY;
            updateScore(pacMan.x, pacMan.y);
        }

        pthread_mutex_unlock(&playerMutex);
    }

    return NULL;
}


bool findPath(const Cell& start, const Cell& end, Cell* path, int& pathLength)
{
    bool visited[HEIGHT][WIDTH] = {false};
    int parent[HEIGHT][WIDTH][2];
    int queue[HEIGHT * WIDTH][2];
    int front = 0, rear = -1;

    queue[++rear][0] = start.x;
    queue[rear][1] = start.y;
    visited[start.y][start.x] = true;

    while (front <= rear) 
    {
        int currentX = queue[front][0];
        int currentY = queue[front++][1];

        if (currentX == end.x && currentY == end.y)
        {
            int idx = 0;
            while (!(currentX == start.x && currentY == start.y)) 
            {
                path[idx++] = { currentX, currentY };
                int tempX = currentX;
                currentX = parent[currentY][currentX][0];
                currentY = parent[currentY][tempX][1];
            }
            path[idx++] = start;
            pathLength = idx;
            return true;
        }

        int dx[] = { 0, 0, 1, -1 };
        int dy[] = { -1, 1, 0, 0 };
        for (int dir = 0; dir < 4; ++dir) 
        {
            int newX = currentX + dx[dir];
            int newY = currentY + dy[dir];

            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT && maze[newY][newX] != WALL && !visited[newY][newX]) 
            {
                queue[++rear][0] = newX;
                queue[rear][1] = newY;
                visited[newY][newX] = true;
                parent[newY][newX][0] = currentX;
                parent[newY][newX][1] = currentY;
            }
        }
    }
    return false;
}

void* ghostMovement(void* arg) 
{
    Ghost* ghost = static_cast<Ghost*>(arg);
    while (true) 
    {
        usleep(200000 / ghost->speedMultiplier); // Adjust speed based on boost

        pthread_mutex_lock(&ghostMutex);

        int deltaX = pacMan.x - ghost->x;
        int deltaY = pacMan.y - ghost->y;

        int stepX = (deltaX != 0) ? (deltaX / abs(deltaX)) : 0; 
        int stepY = (deltaY != 0) ? (deltaY / abs(deltaY)) : 0;

        int proposedX = ghost->x + stepX;
        int proposedY = ghost->y + stepY;

        bool canMoveX = (maze[ghost->y][proposedX] != WALL);
        bool canMoveY = (maze[proposedY][ghost->x] != WALL);

        if (canMoveX && canMoveY) 
        {
            if (abs(deltaY) > abs(deltaX)) 
            {
                ghost->y = proposedY;
            } 
            else 
            {
                ghost->x = proposedX;
            }
        } 
        else if (canMoveX) 
        {
            ghost->x = proposedX;
        } 
        else if (canMoveY) 
        {
            ghost->y = proposedY;
        } 

        pthread_mutex_unlock(&ghostMutex);
    }
    return NULL;
}

void* ghostfindingMovement1(void* arg) 
{
    Ghost* ghost = static_cast<Ghost*>(arg);
    while (true) {
        usleep(200000);

        pthread_mutex_lock(&ghostMutex);

        Cell start = { ghost->x, ghost->y };
        Cell end = { pacMan.x, pacMan.y };
        Cell path[HEIGHT * WIDTH];
        int pathLength = 0;

        if (findPath(start, end, path, pathLength) && pathLength > 1) 
        {
            ghost->x = path[pathLength - 2].x;
            ghost->y = path[pathLength - 2].y;
        }

        pthread_mutex_unlock(&ghostMutex);
    }
    return NULL;
}


void* ghostfindingMovement2(void* arg) 
{
    Ghost* ghost = static_cast<Ghost*>(arg);
    while (true) {
        usleep(200000);

        pthread_mutex_lock(&ghostMutex);

        Cell start = { ghost->x, ghost->y };
        Cell end = { pacMan.x, pacMan.y };
        Cell path[HEIGHT * WIDTH];
        int pathLength = 0;

        if (findPath(start, end, path, pathLength) && pathLength > 1) {
            ghost->x = path[pathLength - 2].x;
            ghost->y = path[pathLength - 2].y;
        }

        pthread_mutex_unlock(&ghostMutex);
    }
    return NULL;
}

void* ghostfindingMovement3(void* arg) 
{
    Ghost* ghost = static_cast<Ghost*>(arg);
    while (true) 
    {
        usleep(200000);

        pthread_mutex_lock(&ghostMutex);

        Cell start = { ghost->x, ghost->y };
        Cell end = { pacMan.x, pacMan.y };
        Cell path[HEIGHT * WIDTH];
        int pathLength = 0;

        if (findPath(start, end, path, pathLength) && pathLength > 1) {
            ghost->x = path[pathLength-2].x;
            ghost->y = path[pathLength-2].y;
        }

        pthread_mutex_unlock(&ghostMutex);
    }
    return NULL;
}




GameState gameState = Menu;

bool checkCollision(const Player& pacman, const Ghost& ghost) 
{
    float pacmanLeft = pacman.x * cellSize;
    float pacmanTop = pacman.y * cellSize;
    float pacmanRight = pacmanLeft + cellSize;
    float pacmanBottom = pacmanTop + cellSize;

    float ghostLeft = ghost.x * cellSize;
    float ghostTop = ghost.y * cellSize;
    float ghostRight = ghostLeft + cellSize;
    float ghostBottom = ghostTop + cellSize;

    return !(pacmanRight < ghostLeft || pacmanLeft > ghostRight || pacmanBottom < ghostTop || pacmanTop > ghostBottom);
}


void updateGame(Player& player, Ghost& ghost1, Ghost& ghost2, Ghost& ghost3, Ghost& ghost4, GameState& state) 
{
    if (checkCollision(player, ghost1) || checkCollision(player, ghost2) || checkCollision(player, ghost3) || checkCollision(player, ghost4)) 
    {
        if (ghost1.mode == Frightened || ghost2.mode == Frightened || ghost3.mode == Frightened || ghost4.mode == Frightened) 
        {
            if (ghost1.mode == Frightened && checkCollision(player, ghost1)) {
                ghost1.x = 13;
                ghost1.y = 14;
                ghost1.mode = Normal;
                ghost1.updateColor();
            } else if (ghost2.mode == Frightened && checkCollision(player, ghost2)) {
                ghost2.x = 14;
                ghost2.y = 14;
                ghost2.mode = Normal;
                ghost2.updateColor();
            } else if (ghost3.mode == Frightened && checkCollision(player, ghost3)) {
                ghost3.x = 15;
                ghost3.y = 15;
                ghost3.mode = Normal;
                ghost3.updateColor();
            } else if (ghost4.mode == Frightened && checkCollision(player, ghost4)) {
                ghost4.x = 15;
                ghost4.y = 14;
                ghost4.mode = Normal;
                ghost4.updateColor();
            }
        } 
        else 
        {
            player.lives--;
            player.x = 1;
            player.y = 1;
        }
    }

    if (ghost1.mode == Frightened || ghost2.mode == Frightened || ghost3.mode == Frightened || ghost4.mode == Frightened) 
    {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - frightenedStartTime).count() > 10) 
        {
            ghost1.mode = Normal;
            ghost2.mode = Normal;
            ghost3.mode = Normal;
            ghost4.mode = Normal;
            ghost1.updateColor();
            ghost2.updateColor();
            ghost3.updateColor();
            ghost4.updateColor();
        }
    }

    if (player.lives <= 0) 
    {
        state = GameOver;
    }
}

void drawLives(sf::RenderWindow& window, const Player& player) 
{
    float lifeRadius = 7;
    float spacing = 4;
    sf::CircleShape lifeShape(lifeRadius);
    lifeShape.setFillColor(sf::Color::Yellow);

    for (int i = 1; i < player.lives; ++i) 
    {
        lifeShape.setPosition(14 + i * (2 * lifeRadius + spacing), 580);
        window.draw(lifeShape);
    }
}

void displayGameOver(sf::RenderWindow& window) 
{
    sf::Sprite background; //Game background sprite
    sf::Texture bg_texture;
    bg_texture.loadFromFile("background3.jpg");
    background.setTexture(bg_texture);
    background.setScale(1.5, 1.5);                                  
    window.clear(sf::Color::Black);
    window.draw(background);
    sf::Font font;
    font.loadFromFile("img/ShortBaby-Mg2w.ttf");
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(40); 
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setPosition(200, 250);  

    window.draw(gameOverText);
    window.display();

    sf::sleep(sf::seconds(5));
}


void* gameEngine(void* arg) 
{
    while (gameStarted == true) 
    {
        pthread_mutex_lock(&playerMutex);
        pthread_mutex_lock(&ghostMutex);
        updateGame(pacMan, ghost1, ghost2, ghost3, ghost4, gameState);
        pthread_mutex_unlock(&ghostMutex);
        pthread_mutex_unlock(&playerMutex);
        usleep(100000);
    }
    return NULL;
}

void* powerPelletSpawner(void* arg)
{
    while (gameStarted == true)
    {
        usleep(4000000);
        int randX, randY;
        do 
        {
            randX = rand() % WIDTH;
            randY = rand() % HEIGHT;
        } 
        while (maze[randY][randX] != PATH);
        
        pthread_mutex_lock(&playerMutex);
        pthread_mutex_lock(&ghostMutex);
        
        maze[randY][randX] = POWER_PELLET;
        
        pthread_mutex_unlock(&ghostMutex);
        pthread_mutex_unlock(&playerMutex);

        usleep(40000000); 
        
        pthread_mutex_lock(&playerMutex);
        pthread_mutex_lock(&ghostMutex);
        
        if (maze[randY][randX] == POWER_PELLET) 
        {
            maze[randY][randX] = PATH;
        }
        
        pthread_mutex_unlock(&ghostMutex);
        pthread_mutex_unlock(&playerMutex);
    }
    return NULL;
}

void* applySpeedBoost(void* arg)
{
    Ghost* ghost = static_cast<Ghost*>(arg);
    sem_wait(&speedBoostSemaphore); 

    pthread_mutex_lock(&ghostMutex);
    ghost->speedMultiplier = 2;
    ghost->hasSpeedBoost = true;
    pthread_mutex_unlock(&ghostMutex);

    usleep(20000000); 

    pthread_mutex_lock(&ghostMutex);
    ghost->speedMultiplier = 1; 
    ghost->hasSpeedBoost = false;
    pthread_mutex_unlock(&ghostMutex);

    sem_post(&speedBoostSemaphore);
    return NULL;
}

void* uiThread(void* arg) 
{
    sf::RenderWindow window(sf::VideoMode(560, 600), "OS Project Pac-Man");
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    pthread_mutex_lock(&gameMutex);
                    gameStarted = true;
                    pthread_cond_signal(&gameCond);
                    pthread_mutex_unlock(&gameMutex);
                } else if (event.key.code == sf::Keyboard::H) {
                    displayInstructions(window);
                } else if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        if (!gameStarted) {
            displayMenu(window);
        } else {
            
            window.clear();
            drawMaze(window);

            float playerPosX = pacMan.x * cellSize;
            float playerPosY = pacMan.y * cellSize;
            pacMan.shape.setPosition(playerPosX, playerPosY);
            window.draw(pacMan.shape);

            float ghost1PosX = ghost1.x * cellSize;
            float ghost1PosY = ghost1.y * cellSize;
            ghost1.shape.setPosition(ghost1PosX, ghost1PosY);
            window.draw(ghost1.shape);

            float ghost2PosX = ghost2.x * cellSize;
            float ghost2PosY = ghost2.y * cellSize;
            ghost2.shape.setPosition(ghost2PosX, ghost2PosY);
            window.draw(ghost2.shape);

            float ghost3PosX = ghost3.x * cellSize;
            float ghost3PosY = ghost3.y * cellSize;
            ghost3.shape.setPosition(ghost3PosX, ghost3PosY);
            window.draw(ghost3.shape);

            float ghost4PosX = ghost4.x * cellSize;
            float ghost4PosY = ghost4.y * cellSize;
            ghost4.shape.setPosition(ghost4PosX, ghost4PosY);
            window.draw(ghost4.shape);

            drawLives(window, pacMan);

            int bufferA;
            if (read(fd[0], &bufferA, sizeof(bufferA)) > 0) {
                scoreA = bufferA;
            }

            sf::Font font;
            if (!font.loadFromFile("img/ShortBaby-Mg2w.ttf")) {
                cout << "Failed to load font!" << endl;
                exit(EXIT_FAILURE);
            }

            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setCharacterSize(15);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(14.f, 560.f);
            scoreText.setString("Score: " + std::to_string(scoreA));
            window.draw(scoreText);

            sf::Text hammadText;
            hammadText.setFont(font);
            hammadText.setString("Hammad & ");
            hammadText.setCharacterSize(15);
            hammadText.setFillColor(sf::Color::White);
            hammadText.setPosition(430.f, 560.f);
            window.draw(hammadText);

            sf::Text iqrashText;
            iqrashText.setFont(font);
            iqrashText.setString("Iqrash");
            iqrashText.setCharacterSize(15);
            iqrashText.setFillColor(sf::Color::White);
            iqrashText.setPosition(500.f, 560.f);
            window.draw(iqrashText);

            window.display();

            if (gameState == GameOver) {
                displayGameOver(window);
                break;
            }
        }
    }

    return NULL;
}

void displayMenu(sf::RenderWindow& window) 
{
    sleep(1);
    sf::Sprite background; //Game background sprite
    sf::Texture bg_texture;
    bg_texture.loadFromFile("background3.jpg");
    background.setTexture(bg_texture);
    background.setScale(1.5, 1.5);                                  
    window.clear(sf::Color::Black);
    window.draw(background);
    sf::Font font;
    font.loadFromFile("img/ShortBaby-Mg2w.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(28);
    text.setString("\n\n\n\t\t\t\t\t\tPACMAN\n\n\n\n\t\t\tPress S to Start a new game \n\n\t\t\tPress H for help \n\n\t\t\tPress Esc to Exit");
    text.setFillColor(sf::Color::White);
    window.draw(text);
    window.display();
}

void displayInstructions(sf::RenderWindow& window) 
{
    sleep(1);
    sf::Sprite background;
    sf::Texture bg_texture;
    bg_texture.loadFromFile("background3.jpg");
    background.setTexture(bg_texture);
    background.setScale(1.5, 1.5);                                  
    window.clear(sf::Color::Black);
    window.draw(background);
    sf::Font font;
    font.loadFromFile("img/ShortBaby-Mg2w.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(23);
    text.setString("\n\n\n\t\t\t\t\t\t\t\t\Instructions: \n\n\n->Press Right, Left, Up and Down for movement.\n\n->The maze is wrapped around.\n\n->Stay away from enemies' attack.\n\n->Every coin increases your score by 1.\n\n->Power Pellets give you 5 points and let you eat ghosts.\n\n Press M back to the Menu\n\n->Press Esc to Exit.");
    text.setFillColor(sf::Color::White);
    window.draw(text);
    window.display();
    
}


void* handleGhostResources(void* arg)
{
    Ghost* ghost = static_cast<Ghost*>(arg);

    while (true) 
    {
        sem_wait(&keySemaphore);
        sem_wait(&permitSemaphore);

        usleep(rand() % 3000000); 

        sem_post(&keySemaphore);
        sem_post(&permitSemaphore);

        usleep(rand() % 500000);
    }

    return NULL;
}

int main() {
    srand(time(0));

    if (XInitThreads() == 0) {
        cout << "X11 threading initialization failed." << endl;
        return -1;
    }

    setupNonBlockingPipe();

    sem_init(&speedBoostSemaphore, 0, 2);
    sem_init(&keySemaphore, 0, 2);
    sem_init(&permitSemaphore, 0, 2);
    
    pthread_mutex_init(&playerMutex, NULL);
    pthread_mutex_init(&ghostMutex, NULL);
    pthread_mutex_init(&gameMutex, NULL);
    pthread_cond_init(&gameCond, NULL);

    pthread_t uiThreadHandle;
    pthread_create(&uiThreadHandle, NULL, uiThread, NULL);

    pthread_mutex_lock(&gameMutex);
    while (!gameStarted) 
    {
        pthread_cond_wait(&gameCond, &gameMutex);
    }
    pthread_mutex_unlock(&gameMutex);

    pthread_t playerThread, ghost1Thread, ghost2Thread, ghost3Thread, ghost4Thread, gameLogicThread, powerPelletThread;

    pthread_create(&powerPelletThread, NULL, powerPelletSpawner, NULL);
    pthread_create(&gameLogicThread, NULL, gameEngine, NULL);
    
    pthread_create(&playerThread, NULL, handlePlayerMovement, NULL);
    pthread_create(&ghost1Thread, NULL, ghostfindingMovement1, &ghost1);
    pthread_create(&ghost2Thread, NULL, ghostfindingMovement2, &ghost2);
    pthread_create(&ghost3Thread, NULL, ghostfindingMovement3, &ghost3);
    pthread_create(&ghost4Thread, NULL, ghostMovement, &ghost4);
 

    pthread_t ghost1BoostThread, ghost3BoostThread, ghost1ResourceThread, ghost2ResourceThread, ghost3ResourceThread,ghost4ResourceThread;
     
    pthread_create(&ghost1BoostThread, NULL, applySpeedBoost, &ghost1);
    pthread_create(&ghost3BoostThread, NULL, applySpeedBoost, &ghost3);

    pthread_create(&ghost1ResourceThread, NULL, handleGhostResources, &ghost1);
    pthread_create(&ghost2ResourceThread, NULL, handleGhostResources, &ghost2);
    pthread_create(&ghost3ResourceThread, NULL, handleGhostResources, &ghost3);
    pthread_create(&ghost4ResourceThread, NULL, handleGhostResources, &ghost4);


    pthread_join(playerThread, NULL);
    pthread_join(ghost1Thread, NULL);
    pthread_join(ghost2Thread, NULL);
    pthread_join(ghost3Thread, NULL);
    pthread_join(ghost4Thread, NULL);
    pthread_join(gameLogicThread, NULL);
    pthread_join(powerPelletThread, NULL);
    pthread_join(ghost1BoostThread, NULL);
    pthread_join(ghost3BoostThread, NULL);
    pthread_join(uiThreadHandle, NULL);
    
    pthread_join(ghost1ResourceThread, NULL);
    pthread_join(ghost2ResourceThread, NULL);
    pthread_join(ghost3ResourceThread, NULL);
    pthread_join(ghost4ResourceThread, NULL);
    
    pthread_mutex_destroy(&playerMutex);
    pthread_mutex_destroy(&ghostMutex);
    pthread_mutex_destroy(&gameMutex);
    pthread_cond_destroy(&gameCond);
    sem_destroy(&speedBoostSemaphore);

    return 0;
}

