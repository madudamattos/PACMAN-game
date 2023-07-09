#include <stdio.h>

#define TAM 1000
#define OVER 1 
#define WIN 2 
#define WALLCOLISION 13
#define GETFOOD 14
#define GHOST 15

typedef struct{
    int positionI;
    int positionJ;
} tPosition;

typedef struct{
    char moveInput;
    int moveResult;
}tMove;

typedef struct{
    int initialMoves;
    int remainingMoves;
    int moveCounter;
    tPosition playerPosition;
    tMove moves[TAM];
    int points; 
} tPlayer;

typedef struct{
    char type;
    char movePattern;
    int status;
    tPosition ghostPosition;
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
    char ghostP;
    char ghostB;
    char ghostC;
    char ghostI;
    char portal;
    char wall;
    char empty;
}tSymbols;

typedef struct{
    tMap map;
    tSymbols symbol;
    tPlayer pacman;
    tGhost ghosts[TAM];
    tFood foods[TAM];
    int foodAmount;
    int ghostAmount;
    int over;
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
tGame playGame(tGame game);
void printGameState(tGame game);
tGame movePacman(tGame game, char move);
tGame moveGhosts(tGame game);
tPlayer receiveMove(tGame game);
tGame verifyGameResults(tGame game);
void printGameFinalStatus(tGame game);
void GenerateResumeFile(tGame game, char path[]);
void GenerateStatisticsFile(tGame game, char path[]);

int main(int argc, char *argv[]){
    tGame game;
    char *path = argv[1];

    if(argc < 2){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }

    game = createGame();

    printInitialGameStatus(game);

    do{
        game = playGame(game);
        printGameState(game);

    }while(!(game.over));

    printGameFinalStatus(game);

    GenerateResumeFile(game, path);
    GenerateStatisticsFile(game, path);

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
    int i;

    game.over = 0; 

    game.symbol.food  = '*';
    game.symbol.pacman = '>';
    game.symbol.ghostB = 'B';
    game.symbol.ghostP = 'P';
    game.symbol.ghostI = 'I';
    game.symbol.ghostC = 'C';
    game.symbol.portal = '@';
    game.symbol.wall = '#';
    game.symbol.empty = ' ';

    game.pacman.points = 0;
    game.pacman.moveCounter = 0;
    
    for(i=0; i<TAM; i++){
        game.ghosts[i].status = 0;
    }

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
    int k, thereIsAFoodInThisPosition = -1;
    
    for(k=0; k<game.foodAmount; k++){
        if(game.foods[k].foodPosition.positionI == i &&
           game.foods[k].foodPosition.positionJ == j && 
           game.foods[k].status == 1){
            thereIsAFoodInThisPosition = k;
            break;
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
    int i, j, k=0;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            
            if (game.map.board[i][j] >= 'A' && game.map.board[i][j] <= 'Z'){
                game.ghosts[k].status = 1;
                game.ghosts[k].type = game.map.board[i][j];
                game.ghosts[k].ghostPosition.positionI=i;
                game.ghosts[k].ghostPosition.positionJ=j;

                if(game.ghosts[k].type == game.symbol.ghostB){
                    game.ghosts[k].movePattern = 'a';
                }
                else if(game.ghosts[k].type == game.symbol.ghostP){
                    game.ghosts[k].movePattern = 'w';
                }
                else if(game.ghosts[k].type == game.symbol.ghostI){
                    game.ghosts[k].movePattern = 's';
                }
                else if(game.ghosts[k].type == game.symbol.ghostC){
                    game.ghosts[k].movePattern = 'd';
                }
                
                k++;
            }
        }
    }

    game.ghostAmount = k;

    return game;
}

void printMap(tGame game){
    int i, j;

    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            if(locateFood(game, i, j) != -1 && 
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

    printf("Pac-Man comecara o jogo na linha %d e coluna %d\n\n",
            game.pacman.playerPosition.positionI+1,
            game.pacman.playerPosition.positionJ+1);


    return;
}


void printGameState(tGame game){
    int i; 

    i = game.pacman.moveCounter;

    printf("Estado do jogo apos o movimento '%c':\n", game.pacman.moves[i-1].moveInput);
    printMap(game);
    printf("Pontuacao: %d\n\n", game.pacman.points);

    return;
}

tGame playGame(tGame game){
    int mc; 
    char move; 

    game.pacman = receiveMove(game);

    mc = game.pacman.moveCounter;

    move = game.pacman.moves[mc-1].moveInput;

    game = moveGhosts(game);

    game = movePacman(game, move);

    game = verifyGameResults(game);

    return game;
}

tPlayer receiveMove(tGame game){
    tPlayer player;

    player = game.pacman;

    int mc; 

    mc = player.moveCounter;

    scanf("%c%*c", &player.moves[mc].moveInput);

    player.moves[mc].moveResult = 0;
    player.remainingMoves--;
    player.moveCounter++;
    
    return player;
}

tGame moveGhosts(tGame game){
    int i, pI, pJ; 
    tPosition next;

    for(i=0; i<game.ghostAmount; i++){

        pI = game.ghosts[i].ghostPosition.positionI;
        pJ = game.ghosts[i].ghostPosition.positionJ;

        next.positionI = pI;
        next.positionJ = pJ;

        //infere a próxima posição
        
        if(game.ghosts[i].movePattern == 'w'){
            next.positionI = pI - 1;
        }
        else if(game.ghosts[i].movePattern == 'a'){
            next.positionJ = pJ - 1;
        }
        else if(game.ghosts[i].movePattern == 's'){
            next.positionI = pI + 1;
        }
        else if(game.ghosts[i].movePattern == 'd'){
            next.positionJ = pJ + 1;
        }

        //se a próxima posição for parede, inverte o padrão de movimento e muda a localização da próxima posição

        if(game.map.board[next.positionI][next.positionJ] == 
           game.symbol.wall){

            if(game.ghosts[i].movePattern == 'w'){
                game.ghosts[i].movePattern = 's';
                next.positionI = pI + 1; 
            }
            else if(game.ghosts[i].movePattern == 'a'){
                game.ghosts[i].movePattern = 'd';
                next.positionJ = pJ + 1; 
            }
            else if(game.ghosts[i].movePattern == 's'){
                game.ghosts[i].movePattern = 'w';
                next.positionI = pI - 1; 
            }
            else if(game.ghosts[i].movePattern == 'd'){
                game.ghosts[i].movePattern = 'a';
                next.positionJ = pJ - 1; 
            }
        }

        //após as avaliações, o fantasma pode mover pra próxima posição 

        //primeiro limpa a posição antiga no mapa
        
        game.map.board[pI][pJ] = game.symbol.empty;
        game.map.board[next.positionI][next.positionJ] = game.ghosts[i].type;        
        game.ghosts[i].ghostPosition.positionI = next.positionI;
        game.ghosts[i].ghostPosition.positionJ = next.positionJ;
    }

    return game;
}

tGame movePacman(tGame game, char move){
    int i, pI, pJ, a; 
    tPosition next;

    pI = game.pacman.playerPosition.positionI;
    pJ = game.pacman.playerPosition.positionJ;

    next.positionI = pI;
    next.positionJ = pJ;

    //infere a próxima posição
        
    if(move == 'w'){
         next.positionI = pI - 1;
    }
    else if(move == 'a'){
        next.positionJ = pJ - 1;
    }
    else if(move == 's'){
        next.positionI = pI + 1;
    }
    else if(move == 'd'){
        next.positionJ = pJ + 1;
    }


    //se a próxima posição for parede, inverte o padrão de movimento e muda a localização da próxima posição

    if(game.map.board[next.positionI][next.positionJ] == 
        game.symbol.wall){

        next.positionI = pI;
        next.positionJ = pJ;
        game.pacman.moves[game.pacman.moveCounter-1].moveResult = WALLCOLISION;
    }

    game.map.board[pI][pJ] = game.symbol.empty;

    //após as avaliações, o pacman pode mover pra próxima posição 
    //mas primeiro tem que limpar a posição antiga
    
    //se a proxima posicao NAO for um fantasma,ele pode mover pra proxima posição
        
    if(!(game.map.board[next.positionI][next.positionJ] >= 'A' && game.map.board[next.positionI][next.positionJ] <= 'Z')){
        game.map.board[next.positionI][next.positionJ] = game.symbol.pacman;      
    }     

    game.pacman.playerPosition.positionI = next.positionI;
    game.pacman.playerPosition.positionJ = next.positionJ;

    return game;
}

tGame verifyGameResults(tGame game){
    int i, j;
    int pI, pJ;
    int food = -1;
    int flag = -7;

    pI = game.pacman.playerPosition.positionI;
    pJ = game.pacman.playerPosition.positionJ;

    //verifica se o jogo acabou
    for(i=0;i<game.ghostAmount;i++){
        if(((game.ghosts[i].ghostPosition.positionI == pI) && (game.ghosts[i].ghostPosition.positionJ ==  pJ))){
            game.over = OVER;
            game.pacman.moves[game.pacman.moveCounter-1].moveResult = GHOST;
            return game;
        }
    }

    if(game.pacman.remainingMoves == 0){
        game.over = OVER;
        return game;
    }

    //verifica se tem uma comida naquela posição
    food = locateFood(game, pI, pJ);

    //se tiver comida vai pontuar
    if(food != -1){
        game.pacman.points++;
        game.foods[food].status = 0;
        game.pacman.moves[game.pacman.moveCounter-1].moveResult = GETFOOD;
    }
    
    //avalia se tem comida no mapa depois da ultima jogada
    for(i=0;i<game.foodAmount;i++){
        if(game.foods[i].status == 1){
            flag = 1;
        }
    }

    //se não tiver mais nenhuma comida 
    if(flag == -7){
        game.over = WIN;
    }

    return game;
}

void printGameFinalStatus(tGame game){
    if(game.over == WIN){
        printf("Voce venceu!\n");
    }
    else if(game.over == OVER){
        printf("Game over!\n");
    }

    printf("Pontuacao final: %d\n", game.pacman.points);

    return;
}

void GenerateResumeFile(tGame game, char path[]){
    char fileName[TAM];
    int i;
    FILE *pResume = NULL;

    sprintf(fileName, "%s/saida/resumo.txt", path);

    pResume = fopen(fileName, "w");
    
    for(i=0; i<game.pacman.moveCounter;i++){
        if(game.pacman.moves[i].moveResult == 14){
            fprintf(pResume, "Movimento %d (%c) pegou comida\n", i+1, game.pacman.moves[i].moveInput);
        }
        else if(game.pacman.moves[i].moveResult == 13){
            fprintf(pResume, "Movimento %d (%c) colidiu com a parede\n", i+1, game.pacman.moves[i].moveInput);
        }
        else if(game.pacman.moves[i].moveResult == 15){
            fprintf(pResume, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", i+1, game.pacman.moves[i].moveInput);
        }
    }

    fclose(pResume);

    return;
}

void GenerateStatisticsFile(tGame game, char path[]){
    char fileName[TAM];
    int i=0, m=0, n=0, o=0, a=0, w=0, d=0, s=0;

    m = game.pacman.moveCounter;
    n = m - game.pacman.points;

    for(i=0; i<m; i++){

        if(game.pacman.moves[i].moveResult == 13){
            o++;
        }
        if(game.pacman.moves[i].moveInput == 'w'){
            w++;
        }
        else if(game.pacman.moves[i].moveInput == 'd'){
            d++;
        }
        else if(game.pacman.moves[i].moveInput == 'a'){
            a++;
        }
        else if(game.pacman.moves[i].moveInput == 's'){
            s++;
        }
    }

    FILE *pStatistics = NULL;

    sprintf(fileName, "%s/saida/estatisticas.txt", path);

    pStatistics = fopen(fileName, "w");

    fprintf(pStatistics, "Numero de movimentos: %d\n", m);
    fprintf(pStatistics, "Numero de movimentos sem pontuar: %d\n", n);
    fprintf(pStatistics, "Numero de colisoes com parede: %d\n", o);
    fprintf(pStatistics, "Numero de movimentos para baixo: %d\n", s);
    fprintf(pStatistics, "Numero de movimentos para cima: %d\n", w);
    fprintf(pStatistics, "Numero de movimentos para esquerda: %d\n", a);
    fprintf(pStatistics, "Numero de movimentos para direita: %d\n", d);
    
    fclose(pStatistics);

    return;
}