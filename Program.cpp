#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
#include<unistd.h> 

const int BOARD_WIDTH = 9; // boxes
const int BOARD_HEIGHT = 9; // boxes

class Edge {
    int coords[4];
    bool filled = false;
    
    public:
        Edge* nextEdge; // this is for linkedList purposes.
        /*Constructor for Edge */
        Edge(int y1, int x1, int y2, int x2) {
            coords[0] = y1;
            coords[1] = x1;
            coords[2] = y2;
            coords[3] = x2;

            nextEdge = NULL;
        }

        void fill() {
            filled = true;
        }

        bool getFilled() {
            return filled;
        }

        int* getCoords() {
            return coords;
        }

        bool equals(Edge* otherEdge) {
            int* otherCoords = otherEdge->coords;
            return  coords[0 ]==otherCoords[0] && 
                    coords[1] == otherCoords[1] && 
                    coords[2] == otherCoords[2] && 
                    coords[3] == otherCoords[3];
        }
};

class Box {  
    int filled = 0;
    int owned = 0;

    //going clockwise from top
    
    bool chainable[4] = {false, false, false, false};

    public:
        /*Constructor for Box */
        Edge *edges[4];

        Box(Edge *e0, Edge *e1, Edge *e2, Edge *e3) {
            edges[0] = e0;
            edges[1] = e1;
            edges[2] = e2;
            edges[3] = e3;
        }

        //helper functions

        // This function updates filled, owned, and edges. Returns true if filled 
        bool updateBox(int edgeNum, Edge* newEdge, int player) {
            edges[edgeNum] =  newEdge;
            if(edges[0]->getFilled() && edges[1]->getFilled() && edges[2]->getFilled() && edges[3]->getFilled()) { //box is captured
                this->setOwned(player);
                this->filled = true;
            }
            
            return this->filled;
        }

        void setOwned(int o) {
            owned = o;
        }

        int getOwned() {
            return owned;
        }
        /* only doing this because errors are occuring
        int chainNeighbor(int possibleSide) {
            bool newArr[4] = {edges[0]->getFilled(), edges[1]->getFilled(), edges[2]->getFilled(), edges[3]->getFilled()};
            newArr[possibleSide] = true;

            int emptySide = 0;
            for (int i; i < 4; i++) {
                if (newArr[i] == false) {
                    emptySide = i;
                }
            }
            if (neighbor[i].filled != 2) {
                emptySide = 5;
            }
            return emptySide;

        }
        */

       /* only doing this because errors are occuring
        int chainNum(int max, int count, int n) {

            //for testing
            int chainable = chainneighbor;

            if ((chainable < 4) && (count <= max)) {
                return neighbor.chainNum(max, count + 1);
            }
            else {
                return count;
            }
        }
        */
};

class Board {
    int myScore; // my boxes
    int enemyScore; //enemy boxes 
    int heuristic;
    bool nextPass; 
    Edge* mostRecentMove;
    

    public:
        Box* allBoxes[BOARD_HEIGHT][BOARD_WIDTH];

        Board() { // for beginning  
            this->mostRecentMove = NULL;
            Edge *topEdge, *rightEdge, *bottomEdge, *leftEdge;
            for(int i = 0; i < BOARD_HEIGHT; i++) {
                for(int j = 0; j < BOARD_WIDTH; j++) {
                    if(j == 0) { //if in leftmost col
                        leftEdge = new Edge(i, j, i+1, j); // TL to BL
                    }
                    else {
                        leftEdge = allBoxes[i][j-1]->edges[3];
                    }
                    if(i == 0) { //if in top row
                        topEdge = new Edge(i, j, i, j+1); // TL to TR
                    }
                    else {
                        topEdge = allBoxes[i-1][j]->edges[0];
                    }
                    rightEdge = new Edge(i, j+1, i+1, j+1); // TR to BR
                    bottomEdge = new Edge(i+1, j, i+1, j+1); // BL to BR

                    allBoxes[i][j] = new Box(topEdge, rightEdge, bottomEdge, leftEdge);
                }
            }
        }
        // this should be used to make a board from a previous board state
        Board(Board* prevBoard, Edge* currMove, int player) { 
            this->mostRecentMove = currMove;
            //copy all boxes, update only relevant boxes
            for(int i = 0; i < BOARD_HEIGHT; i++) {
                for(int j = 0; j < BOARD_WIDTH; j++) {
                    allBoxes[i][j] = prevBoard->allBoxes[i][j];
                }
            }
            /* relevant boxes are defined as boxes whose Edge needs to be updated
                cases:  left / right / top/ bottom MOST edges -> only update 1 box
                        vertical edge -> update boxes left and right
                        horizontal edge -> update boxes above and below
            */
           // all coords are in order of lowest Y, lowest X, highest Y, highest X
            int* moveCoords = currMove->getCoords();

            bool verticalMove = moveCoords[0] == moveCoords[2]; 
            bool horizontalMove = moveCoords[1] == moveCoords[3]; 

            if(!verticalMove && !horizontalMove || verticalMove && horizontalMove) { //hopefully this never happens :)
                printf("Something went wrong with move coordinates\n");
                printf("Current coordinantes: %d,%d, %d,%d ", moveCoords[0],moveCoords[1],moveCoords[2],moveCoords[3]);
            }

            bool somethingFilled = false;

            bool topRowMove = horizontalMove && moveCoords[0] == 0;
            bool botRowMove = moveCoords[0] == BOARD_HEIGHT+1; 
            bool leftRowMove = verticalMove && moveCoords[1] == 0;
            bool rightRowMove = moveCoords[1] == BOARD_WIDTH+1;

            if(topRowMove) {
                somethingFilled = allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(0, currMove, player);
            }
            else if(rightRowMove) {
                somethingFilled = allBoxes[moveCoords[0]-1][moveCoords[1]-1]->updateBox(1, currMove, player);
            }
            else if(botRowMove) {
                somethingFilled = allBoxes[moveCoords[0]-1][moveCoords[1]]->updateBox(2, currMove, player);
            }
            else if(leftRowMove) { 
                somethingFilled = allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(3, currMove, player);
            }
            else if(horizontalMove) { //this is a horizontal move that affects two cells 
                somethingFilled |= allBoxes[moveCoords[0]-1][moveCoords[1]]->updateBox(2, currMove, player);
                somethingFilled |= allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(0, currMove, player);
            }
            else { //this is a vertical move and will affect two cells
                somethingFilled |= allBoxes[moveCoords[0]][moveCoords[1]-1]->updateBox(3, currMove, player);
                somethingFilled |= allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(1, currMove, player);
            }

            if(somethingFilled) {
                this->updateScore();
            }
            this->nextPass = somethingFilled;
        }

        void updateScore() {
            //for all boxes check who owns it and add that to score
            int myCurScore = 0;
            int enemyCurScore = 0;
            int ownedVal;
            for(int i = 0; i < BOARD_HEIGHT; i++) {
                for(int j = 0; j < BOARD_WIDTH; j++) {
                    ownedVal = allBoxes[i][j]->getOwned();
                    if(ownedVal == 1)  myCurScore++;
                    else if(ownedVal == 2) enemyCurScore++;
                }
            }
            this->myScore = myCurScore;
            this->enemyScore = enemyScore;
        }

        //Test this function
        Edge* getMoves() { 
            Edge* moveList = new Edge(0, 0, 0, 0); // a fake move pointer (not THE fake move pointer)
            Edge* headNode = moveList;
            if(this->nextPass) {
                return moveList;
            }
            /* for each box,   
                add bottom and right edges if not filled, 
                add top if top row and not filled,
                add left if first column */
            // ive decided to implement the movelist as a singled linkedlist where the first node is always the fake move
            Edge** boxEdges;
            for(int i = 0; i < BOARD_HEIGHT; i++) {
                for(int j = 0; j < BOARD_WIDTH; j++) {
                    boxEdges = allBoxes[i][j]->edges;
                    if(i == 0 && !boxEdges[0]->getFilled()) { //only add top edge if top and not filled
                        headNode->nextEdge = boxEdges[0];
                        headNode = headNode->nextEdge;
                    }
                    if(!boxEdges[1]->getFilled()) { //we can always add right edge if not filled
                        headNode->nextEdge = boxEdges[1];
                        headNode = headNode->nextEdge;
                    }
                    if(!boxEdges[2]->getFilled()) { //we can always add bottom edge if not filled
                        headNode->nextEdge = boxEdges[2];
                        headNode = headNode->nextEdge;
                    }
                    if(j == 0 && !boxEdges[3]->getFilled()) {//only add left edge if top and not filled
                        headNode->nextEdge = boxEdges[3];
                        headNode = headNode->nextEdge;
                    }
                }
            }
            return moveList->nextEdge;
        }
        // DONE?
        Board* move(Board* curBoard, Edge* theMove, int player) { 
            if(theMove->equals(fakeMove)) {
                this->nextPass = false;
                return this;
            }
            else {
                Board* nextBoard = new Board(curBoard, theMove, player);
                return nextBoard;
            }
        }

        void setHeuristic(int h) {
            heuristic = h;
        }

        void getHeuristic(int h) {
            heuristic = h;
        }

        void setPass(bool pass) {
            this->nextPass = pass;
        }
    
};

//evaluation function
int eval(){
    return 0;
}

Edge* bestMove;

int max(int x, int y) {
    int m = x;
    if (y > x)  {
        m = y;
    }
    return m;
}

int min(int x, int y) {
    int m = x;
    if (y < x)  {
        m = y;
    }
    return m;
}
/*
//actual minimax func
int minimax(Board* board, int depth, bool isMax, int alpha, int beta) {
    if ((depth == 0) || (board.getMoves() == NULL)) {
        return board.getEval();
    }

    if (isMax) {
        int maxEval = -100000;
        Edge moves[] = board->getMoves();
        for (int i: moves) {
            Board childBoard = board.move(moves[i], 1);
            int eval = minimax(childBoard, depth-1, false, alpha, beta);
            if (eval > maxEval) {
                maxEval = eval;
                alpha = max(maxEval, alpha);
                bestMove = moves[i];
            }
            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    }
    else {
        int minEval = 100000;
        Edge moves[] = board.getMoves();
        for (int i: moves) {
            Board childBoard = board.move(moves[i], 2);
            int eval = minimax(childBoard, depth-1, true, alpha, beta);
            minEval = min(minEval, eval);
            beta = min(minEval, beta);
            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
};
*/
const char* goFileName = "Clairvoyance.go";
const char* passFileName = "Clairvoyance.go";
const char* endFileName = "end_file";
const char* moveFileName = "move_file";
Edge* fakeMove = new Edge(0, 0, 0, 0);

bool findFile(FILE* fp, const char* fileName) {
    fp = fopen(fileName, "r");
    return fp != NULL; // uhh mybe?
}

Edge* stringToEdge(char* moveString) { 
    char* moveCopy = (char*)malloc(200*sizeof(char));
    strncpy(moveCopy, moveString, 200);
    strtok(moveCopy," "); //gets rid of the name
    int y1 = atoi(strtok(moveCopy,","));
    int x1 = atoi(strtok(moveCopy," "));
    int y2 = atoi(strtok(moveCopy,","));
    int x2 = atoi(strtok(moveCopy,",")); //not sure last delim is correct

    int lowestY = min(y1,y2);
    int lowestX = min(x1,x2);
    int highestY = max(y1,y2);
    int highestX = max(x1,x2);

    Edge* newEdge = new Edge(lowestY, lowestX, highestY, highestX); 
    newEdge->fill();
    return newEdge;
}

Edge* extractMove(FILE* moveFP) {
    char moveString[200];
    fread(moveString, sizeof(moveString), 1, moveFP);
    fclose(moveFP);
    return stringToEdge(moveString);
}


// returns move with name
char* edgeToString(Edge* edge) { 
    char* edgeString;
    int* coords = edge->getCoords();
    int outputNum = sprintf(edgeString, "Clairvoyance %d,%d, %d,%d", coords[0], coords[1], coords[2], coords[3]); //MYBE
    printf("Sprintf output %d, %s\n", outputNum,edgeString);
    return edgeString;
}

void writeMove(Edge* moveEdge) {
    FILE* local_moveFP = fopen(moveFileName, "w"); 
    fprintf(local_moveFP, "%s", edgeToString(moveEdge));
    fclose(local_moveFP);
}

bool handleEndGame() {
    FILE* endFP;
    if(findFile(endFP, endFileName)) {
        char endTxt[200];
        fread(endTxt, 200, 1, endFP);
        printf("%s", endTxt);
        fclose(endFP);
        return true;
    }   
    return false;
}

Board* handleOppTurn(Board* currentBoard) {
    Edge* oppMove;
    Board* nextBoard = currentBoard;
    FILE* moveFP;
    findFile(moveFP, moveFileName); // should always find file
    oppMove = extractMove(moveFP); // this fcn also closes moveFP
    if(!oppMove->equals(fakeMove)) { // dont update board if opp passes 
        nextBoard = currentBoard->move(currentBoard, oppMove, 2);
    }
    nextBoard->setPass(false); //when player updates board, never handle passing
    return nextBoard; 
}

bool handlePass() {
    FILE* passFP;
    bool needtoPass = findFile(passFP,passFileName);
    if(needtoPass) writeMove(fakeMove);
    fclose(passFP);
    return needtoPass;
}

Edge* generateRandomMove(Board* currBoard) {
    return currBoard->getMoves();
}

// current todo. TEST board, TEST legal move list, and random move maker.
// Testable program now. Will make first legal move
int main(int argc, char** argv) {
    printf("Clairvoyance starting");

    Board* localBoard = new Board(); 
    Edge* bestMove; 
    bool pass;
    /* We Loop until the game is over */
    while (true) {
        if(findFile(NULL, goFileName)) {

            if(handleEndGame()) break; //handles ending game if endgame file exists

            localBoard =  handleOppTurn(localBoard); // extracts opp move and updates current board
            
            pass = handlePass(); // handles passing if pass file exists

            if(!pass) { //take our turn if not passed
                /* GENERATE BEST MOVE */
                bestMove = generateRandomMove(localBoard);
                localBoard = localBoard->move(localBoard, bestMove, 1);
                localBoard->setPass(false); //when player updates board, never handle passing
                writeMove(bestMove); //replace with bestMove
            }
            sleep(105); //wait for referee to update. 
        }
        else {
            sleep(10); //not our turn, keep waiting.
        }
    }
    return 0;

    /*  On Start:
        Make board object 
        Create list of legal moves (all edges)
        Make AI object 
        Wait for Turn
    */
   /*   Turn:
        scan directory every 10ms for "groupname.go"
        Once found, scan directory for "end_game"
            If found, handleEndGame(FILE* endFile) 
        scan directory for "move_file"
            If NOT found, panic!
        extract move from "move_file" using stringToEdge(char* moveString)
        updateLegalMoveList(Edge* edge) 
        scan directory for "groupname.pass"
            if found, writeMove(FILE* move_file, char* fakeMoveString), END TURN
        Edge* bestEdge = AIAgent.generateBestmove() - HARD PART
        updateLegalMoveList(Edge* bestEdge) 
        char* bestMoveString = edgeToString(Edge* bestEdge) 
        writeMove(FILE* move_file, char* bestMoveString), END TURN

   */
}