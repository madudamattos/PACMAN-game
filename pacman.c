#include <stdio.h>

typedef struct{
    int positionI;
    int positionJ;
} tPosition;

typedef struct{
    int initialMoves;
    int remainingMoves;
    tPosition playerPosition;
    tMove moves[50];
    int points; 
} tPlayer;

typedef struct{
    tPosition moveInput;
    int moveResult;
}tMove;


typedef struct{
    char movePattern;
    tPosition ghostPosition;
    int status;
    //'status' indicate if the type of ghost
} tGhost;

typedef struct{
    int sizeI;
    int sizeJ;
    char board[40][100];    
} tMap;

typedef struct{
    tPosition foodPosition;
    int number;
    int status;
} tFood;

typedef struct{
    char food;
    char pacman;
    char ghostB;
    char ghostP;
    char ghostI;
    char ghostC;
    char portal;
    char wall;
    char empty;
}tSymbols;

typedef struct{
    tMap map;
    tSymbols symbol;
    tPlayer pacman;
    tGhost ghostB;
    tGhost ghostP;
    tGhost ghostI;
    tGhost ghostC;
    tFood foods[30];
    int foodAmount;
    //tPosition portals[30];
    //int portalAmount; 
} tGame;

//CABEÇALHOS DAS FUNÇÕES

tGame createGame();
tGame initialDefinitions();
tMap readMap (tGame game);
tPosition locatePacman(tGame game);
int locateFood(tGame game, int i, int j);
tGame initiateFood(tGame game);
tGame initiateGhosts(tGame game);
void printMap(tGame game);
void printInitialGameStatus(tGame game);
void playGame(tGame game);

int main(){
    tGame game;
    int continueGame = 1;

    game = createGame();

    printInitialGameStatus(game);

    do{
        playGame();
        //continueGame = gameOver(game);
        printGameState(game);

    }while(continueGame);

    return 0;
}

tGame createGame(){
    tGame game;
    
    game = initialDefinitions();

    scanf("%d %d %d\n", &game.map.sizeI, &game.map.sizeJ, &game.pacman.initialMoves);

    game.pacman.remainingMoves = game.pacman.initialMoves;

    game.map = readMap(game);

    game.pacman.playerPosition = locatePacman(game);

    game = initiateFood(game);

    game = initiateGhosts(game);

    //initiatePortals(game);

    return game;
}

tGame initialDefinitions(){
    tGame game;

    game.symbol.food  = '*';
    game.symbol.pacman = '>';
    game.symbol.ghostB = 'B';
    game.symbol.ghostP = 'P';
    game.symbol.ghostI = 'I';
    game.symbol.ghostC = 'C';
    game.symbol.portal = '@';
    game.symbol.wall = '#';
    game.symbol.empty = ' ';

    game.ghostB.movePattern = 'a';
    game.ghostP.movePattern = 'w';
    game.ghostI.movePattern = 's';
    game.ghostC.movePattern = 'd';

    game.pacman.points = 0;
    game.ghostB.status = 0;
    game.ghostP.status = 0;
    game.ghostI.status = 0;
    game.ghostC.status = 0;

    return game;
}

tMap readMap (tGame game){
    tMap map;
    int i, j;

    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            scanf("%c", &map.board[i][j]);
        }
        scanf("%*c");
    }

    map.sizeI = game.map.sizeI;
    map.sizeJ = game.map.sizeJ;
    
    return map;
}

tPosition locatePacman(tGame game){
    tPosition pacman;
    int i, j;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            if(game.map.board[i][j] == game.symbol.pacman){
                pacman.positionI = i;
                pacman.positionJ = j;
            }
        }
    }

    return pacman;
}

int locateFood(tGame game, int i, int j){
    int k, thereIsAFoodInThisPosition = 0;
    
    for(k=0; k<game.foodAmount; k++){
        if(game.foods[k].foodPosition.positionI == i &&
           game.foods[k].foodPosition.positionJ == j && 
           game.foods[k].status == 1){
            thereIsAFoodInThisPosition = 1;
           }
    }

    return thereIsAFoodInThisPosition;
}

tGame initiateFood(tGame game){
    int i, j, k=0;
    int food=0;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            if(game.map.board[i][j] == game.symbol.food){
                game.foods[k].foodPosition.positionI = i;
                game.foods[k].foodPosition.positionJ = j;
                game.foods[k].status = 1;
                game.foods[k].number = k+1;
                food++;
                k++;
                game.map.board[i][j] = game.symbol.empty;
            }
        }
    }

    game.foodAmount = food;

    return game;
}

tGame initiateGhosts(tGame game){
    int i, j;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            
            if (game.map.board[i][j] == game.symbol.ghostB){
                game.ghostB.status = 1;
                game.ghostB.ghostPosition.positionI=i;
                game.ghostB.ghostPosition.positionJ=j;
            }
            else if(game.map.board[i][j] == game.symbol.ghostP){
                game.ghostP.status = 1;
                game.ghostP.ghostPosition.positionI=i;
                game.ghostP.ghostPosition.positionJ=j;
            }
            else if(game.map.board[i][j] == game.symbol.ghostI){
                game.ghostI.status = 1;
                game.ghostI.ghostPosition.positionI=i;
                game.ghostI.ghostPosition.positionJ=j;
            }
            else if(game.map.board[i][j] == game.symbol.ghostC){
                game.ghostC.status = 1;
                game.ghostC.ghostPosition.positionI=i;
                game.ghostC.ghostPosition.positionJ=j;
            }
        
        }
    }

    return game;
}

void printMap(tGame game){
    int i, j;

    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            if(locateFood(game, i, j) == 1 && 
                game.map.board[i][j] == game.symbol.empty){

                printf("%c", game.symbol.food);
            }
            else{
                printf("%c", game.map.board[i][j]);
            }
        }
        printf("\n");
    }
}

void printInitialGameStatus(tGame game){
    int i, j;
    
    printMap(game);

    printf("Pac-Man comecara o jogo na linha %d e coluna %d\n",
            game.pacman.playerPosition.positionI+1,
            game.pacman.playerPosition.positionJ+1);


    return;
}

// void printGameState(game){
//     printf("Estado do jogo apos o movimento '%c':\n", );

// }

void playGame(tGame game){
    int moveCounter;

    moveCounter = game.pacman.initialMoves - game.pacman.remainingMoves;

    scanf("%c", &game.pacman.moves[moveCounter]);

    game.pacman.remainingMoves--;
    
    
    return;
}

tGame moveGhosts(tGame game){
    int i, j;

    

    return game2;
}

tPosition locateGhost(tGame game, char ghostSymbol){
    tPosition ghost;
    int i, j;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            if(game.map.board[i][j] == ghostSymbol){
                ghost.positionI = i;
                ghost.positionJ = j;
            }
        }
    }

    return ghost;
}