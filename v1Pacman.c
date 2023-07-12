#include <stdio.h>

#define TAM 300
#define OVER 1 
#define WIN 2 
#define WALLCOLISION 13
#define GETFOOD 14
#define GHOST 15
#define OUTOFMOVES 17

typedef struct{
    int positionI;
    int positionJ;
} tPosition;

typedef struct{
    char moveInput;
    int moveResult;
    tPosition position;
}tMove;

typedef struct{
    int initialMoves;
    int remainingMoves;
    int moveCounter;
    tPosition playerPosition;
    tMove moves[3000];
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
    int trail[40][100];
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
    tPosition portals[2];
} tGame;

//CABEÇALHOS DAS FUNÇÕES

tGame createGame(char *path);
tGame initialDefinitions();
tMap readMapFile(tGame game, char *path);
tGame readMovesFile(tGame game, char *path);
tPosition locatePacman(tGame game);
int locateFood(tGame game, int i, int j);
int locatePortals(tGame game, int i, int j);
tGame initiateFood(tGame game);
tGame initiateGhosts(tGame game);
void printMap(tGame game, FILE *pFile);
//void printInitialGameStatus(tGame game);
tGame playGame(tGame game);
void printGameState(tGame game);
tGame movePacman(tGame game, char move);
tGame moveGhosts(tGame game);
tGame verifyGameResults(tGame game);
void writeExitFile(tGame game, FILE *pFile);
void writeExitFinalStatus(tGame game, FILE *pFile);
void generateInitializationFile(tGame game, char *path);
void generateResumeFile(tGame game, char path[]);
void generateStatisticsFile(tGame game, char path[]);
void generateTrailFile(tGame game, char path[]);
tGame initiatePortals(tGame game);
int locateGhost(tGame game, int i, int j);
int checksIfMoveIsInvalid(char move);
char changeMove(char move);

int main(int argc, char *argv[]){
    tGame game;
    char *path = argv[1];
    FILE *pExit;
    char exitPath[TAM];
    char move; 

    if(argc < 2){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }

    game = createGame(path);

    generateInitializationFile(game, path);

    //criar arquivo de saida
    sprintf(exitPath, "%s/saida/saida.txt", path);

    pExit = fopen(exitPath, "w");

    do {
    
        game.pacman.moves[game.pacman.moveCounter].moveResult = 0;
        game.pacman.remainingMoves--;
        game.pacman.moveCounter++;
        
        if(checksIfMoveIsInvalid(game.pacman.moves[game.pacman.moveCounter].moveInput)){
            game.over = OVER;
        }
        
        move = game.pacman.moves[game.pacman.moveCounter-1].moveInput;

        game = moveGhosts(game);

        game = movePacman(game, move);

        game = verifyGameResults(game);
        
        writeExitFile(game, pExit);

    } while(!game.over);

    writeExitFinalStatus(game, pExit);

    fclose(pExit);

    generateResumeFile(game, path);
    generateStatisticsFile(game, path);
    generateTrailFile(game, path);

    return 0;
}

tGame createGame(char *path){
    tGame game;
    char mapPath[TAM];
    FILE *pMap;

    game = initialDefinitions();

    sprintf(mapPath, "%s/mapa.txt", path);

    pMap = fopen(mapPath, "r");

    fscanf(pMap,"%d %d %d%*c", &game.map.sizeI, &game.map.sizeJ, &game.pacman.initialMoves);

    game.pacman.remainingMoves = game.pacman.initialMoves;

    game.map = readMapFile(game, path);

    game = readMovesFile(game, path);

    game.pacman.playerPosition = locatePacman(game);
    
    game.pacman.moves[0].position = game.pacman.playerPosition;

    //insert player inicial position in the trail matrix
    game.map.trail[game.pacman.playerPosition.positionI][game.pacman.playerPosition.positionJ] = game.pacman.moveCounter;

    game = initiateFood(game);

    game = initiateGhosts(game);

    game = initiatePortals(game);

    game.pacman.points = 0;

    fclose(pMap);

    //game = initiatePortals(game);

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

tMap readMapFile(tGame game, char *path){
    tMap map;
    int i, j;
    char mapPath[TAM];
    FILE *pMap;

    sprintf(mapPath, "%s/mapa.txt", path);

    pMap = fopen(mapPath, "r");

    fscanf(pMap, "%d %d %d%*c", &game.map.sizeI, &game.map.sizeJ, &game.pacman.initialMoves);

    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            fscanf(pMap, "%c", &map.board[i][j]);
        }
        fscanf(pMap, "%*c");
    }

    fclose(pMap);

    map.sizeI = game.map.sizeI;
    map.sizeJ = game.map.sizeJ;

    //inicializa a trilha
    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            map.trail[i][j] = -1;
        }
    }

    return map;
}

tGame readMovesFile(tGame game, char *path){
    int i=0;
    char movesPath[TAM];
    FILE *pMoves;

    sprintf(movesPath, "%s/jogadas.txt", path);

    pMoves = fopen(movesPath, "r");

    while(!feof(pMoves)){
        fscanf(pMoves, "%c%*c", &game.pacman.moves[i].moveInput);
        i++;
    }

    fclose(pMoves);

    return game;
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

int locateGhost(tGame game, int i, int j){
    int k, thereIsAGhostInThisPosition = -1;
    
    for(k=0; k<game.ghostAmount; k++){
        if(game.ghosts[k].ghostPosition.positionI == i &&
           game.ghosts[k].ghostPosition.positionJ == j && 
           game.ghosts[k].status == 1){
            thereIsAGhostInThisPosition = k;
            break;
           }
    }

    return thereIsAGhostInThisPosition;
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

int locatePortals(tGame game, int i, int j){
    int k, thereIsAPortalInThisPosition = -1;
    
    for(k=0; k<2; k++){
        if(game.portals[k].positionI == i &&
           game.portals[k].positionJ == j ){
            thereIsAPortalInThisPosition = k;
            break;
           }
    }

    return thereIsAPortalInThisPosition;
}

tGame initiatePortals(tGame game){
    int i, j, k=0;
    int food=0;

    for(i=0;i<game.map.sizeI; i++){
        for(j=0;j<game.map.sizeJ; j++){
            if(game.map.board[i][j] == game.symbol.portal){
                game.portals[k].positionI = i;
                game.portals[k].positionJ = j;
                k++;
                game.map.board[i][j] = game.symbol.empty;
            }
        }
    }

    return game;
}

void printMap(tGame game, FILE *pFile){
    int i, j;

    for(i=0; i<game.map.sizeI; i++){
        for(j=0; j<game.map.sizeJ; j++){
            if(locateFood(game, i, j) != -1 && 
                game.map.board[i][j] == game.symbol.empty){

                fprintf(pFile, "%c", game.symbol.food);
            }
            else if(locatePortals(game, i, j) != -1 && game.map.board[i][j] == game.symbol.empty){
                fprintf(pFile, "%c", game.symbol.portal);
            }
            else{
                fprintf(pFile, "%c", game.map.board[i][j]);
            }
        }
        fprintf(pFile, "\n");
    }

    return;
}

void generateInitializationFile(tGame game, char *path){
    char fileName[TAM];
    FILE *pInit;
    
    sprintf(fileName, "%s/saida/inicializacao.txt", path);

    pInit = fopen(fileName, "w");

    printMap(game, pInit);

    fprintf(pInit, "Pac-Man comecara o jogo na linha %d e coluna %d\n",
            game.pacman.playerPosition.positionI + 1,
            game.pacman.playerPosition.positionJ + 1);

    fclose(pInit);
}

void writeExitFile(tGame game, FILE *pFile){
    int i; 

    i = game.pacman.moveCounter;

    fprintf(pFile, "Estado do jogo apos o movimento '%c':\n", game.pacman.moves[i-1].moveInput);
    printMap(game, pFile);
    fprintf(pFile, "Pontuacao: %d\n\n", game.pacman.points);

    return;
}

char changeMove(char move){
    char oppositeMove;
    
    if(move == 'w'){
        oppositeMove = 's';
    }
    else if(move == 's'){
        oppositeMove = 'w';
    }
    else if(move == 'd'){
        oppositeMove = 'a';
    }
    else if(move == 'a'){
        oppositeMove = 'd';
    }
    
    return oppositeMove;
}

int checksIfMoveIsInvalid(char move){
    if(move == 'a' || move == 'd' || move == 'w' || move == 's'){
        return 0;
    }
    else{
        return 1;
    }
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
    int i, pI, pJ, a, portal, ghost; 
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
    else if(locatePortals(game, next.positionI, next.positionJ) != -1 && game.map.board[next.positionI][next.positionJ] == game.symbol.empty){
        if(game.portals[0].positionI == next.positionI && game.portals[0].positionJ == next.positionJ){
            next.positionI = game.portals[1].positionI;
            next.positionJ = game.portals[1].positionJ;
        }
        else if(game.portals[1].positionI == next.positionI && game.portals[1].positionJ == next.positionJ){
            next.positionI = game.portals[0].positionI;
            next.positionJ = game.portals[0].positionJ;
        }
    }
    
    ghost = locateGhost(game, pI, pJ);
    
    if(locateGhost(game, pI, pJ) != -1){
        game.map.board[pI][pJ] = game.ghosts[ghost].type;
    }
    else{
        game.map.board[pI][pJ] = game.symbol.empty;
    }
    
    //após as avaliações, o pacman pode mover pra próxima posição 
    //mas primeiro tem que limpar a posição antiga
    
    //se a proxima posicao NAO for um fantasma,ele pode mover pra proxima posição
        
    if(!(game.map.board[next.positionI][next.positionJ] >= 'A' && game.map.board[next.positionI][next.positionJ] <= 'Z')){
        game.map.board[next.positionI][next.positionJ] = game.symbol.pacman;
        game.map.trail[next.positionI][next.positionJ] = game.pacman.moveCounter;

        portal = locatePortals(game, next.positionI, next.positionJ);

        if(portal != -1){
            if(portal == 0){
                game.map.trail[game.portals[1].positionI][game.portals[1].positionJ] = game.pacman.moveCounter;
            }
            else if(portal == 1){
                game.map.trail[game.portals[0].positionI][game.portals[0].positionJ] = game.pacman.moveCounter;
            }
        }          
    }     
   
    game.pacman.playerPosition.positionI = next.positionI;
    game.pacman.playerPosition.positionJ = next.positionJ;
    
    game.pacman.moves[game.pacman.moveCounter].position.positionI = next.positionI;
    game.pacman.moves[game.pacman.moveCounter].position.positionI = next.positionJ;
    
    return game;
}

tGame verifyGameResults(tGame game){
    int i, j;
    int pI, pJ;
    int food = -1;
    int ghost = -1;
    int flag = -7;

    pI = game.pacman.playerPosition.positionI;
    pJ = game.pacman.playerPosition.positionJ;

    //verifica se foi pra mesma posiçao de um fantasma
    for(i=0;i<game.ghostAmount;i++){
        if(((game.ghosts[i].ghostPosition.positionI == pI) && (game.ghosts[i].ghostPosition.positionJ ==  pJ))){
            game.over = OVER;
            game.pacman.moves[game.pacman.moveCounter-1].moveResult = GHOST;
            return game;
        }
    }
    
    //CASO ESPECIFICO EM QUE O FANTASMA CRUZA COM O PACMAN:
    
    if(game.pacman.moveCounter > 1){
        ghost = locateGhost(game, game.pacman.moves[game.pacman.moveCounter-2].position.positionI, game.pacman.moves[game.pacman.moveCounter-2].position.positionJ);
        if(ghost != -1){
            if(game.ghosts[ghost].movePattern == changeMove(game.pacman.moves[game.pacman.moveCounter-2].moveInput)){
                game.over = OVER;
                game.pacman.moves[game.pacman.moveCounter-1].moveResult = GHOST;
                return game;
            }
        }
    }
    ///////////////////////////////////////////////////////////////

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

    if(game.pacman.remainingMoves == 0){
        game.over = OVER;
        game.pacman.moves[game.pacman.moveCounter].moveResult = OUTOFMOVES;
        return game;
    }

    //se não tiver mais nenhuma comida 
    if(flag == -7){
        game.over = WIN;
    }

    return game;
}

void writeExitFinalStatus(tGame game, FILE *pExit){
    if(game.over == WIN){
        fprintf(pExit, "Voce venceu!\n");
    }
    else if(game.over == OVER){
        fprintf(pExit,"Game over!\n");
    }

    fprintf(pExit,"Pontuacao final: %d\n", game.pacman.points);

    return;
}

void generateResumeFile(tGame game, char path[]){
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
            fprintf(pResume, "Movimento %d (%c) colidiu na parede\n", i+1, game.pacman.moves[i].moveInput);
        }
        else if(game.pacman.moves[i].moveResult == 15){
            fprintf(pResume, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", i+1, game.pacman.moves[i].moveInput);
        }
    }

    fclose(pResume);

    return;
}

void generateStatisticsFile(tGame game, char path[]){
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

void generateTrailFile(tGame game, char path[]){
    char fileName[TAM];
    int i, j;
    FILE *pTrail = NULL;

    sprintf(fileName, "%s/saida/trilha.txt", path);

    pTrail = fopen(fileName, "w");
    
    for(i=0; i<game.map.sizeI;i++){
        for(j=0; j<game.map.sizeJ;j++){
            if(game.map.trail[i][j] == -1){
                fprintf(pTrail, "%c ", game.symbol.wall);
            }
            else{
                fprintf(pTrail, "%d ", game.map.trail[i][j]);
            }
        }
        fprintf(pTrail, "\n");
    }

    fclose(pTrail);

    return;
}