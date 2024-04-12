#include <lcom/lcf.h>

typedef struct BoardStruct{
    int matrix[4][4];
} Board;

typedef struct PlayerStruct{
    char *name;
    unsigned int lives; 
    bool isAlive;
    float bestTime;
} Player;

typedef enum {
    Ace = 1,Two,Three,Four,Five,Six,Seven,Eight,Nine,Ten,Jack,Queen,King
} cardNumber;

typedef enum {
    Spades, Clubs, Hearts, Diamonds
} cardSuit;

typedef enum {
    Menu, Game, Pause, HighScores,Instructions, Exit, Wait
} gameState;

typedef struct Cards{
    int x, y;
    bool isFlipped;
    bool isFlipping;
    bool flipSuccess;
    cardNumber num;
    cardSuit suit;

} card;

typedef struct Mouse{
    int x, y;
} Mouse;

typedef struct Date{
    unsigned int day, month, year;
    unsigned int hour, minute, second;
} Date;
