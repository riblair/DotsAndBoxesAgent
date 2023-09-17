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
        /*Constructor for Edge */
        Edge(int y1, int x1, int y2, int x2) {
            coords[0] = y1;
            coords[1] = x1;
            coords[2] = y2;
            coords[3] = x2;
        }

        void fill() {
            filled = true;
        }


        int* getCoords() {
            return coords;
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
        void updateBox(int fill) {
            filled += 1;
        }

        void updateChainable() {

        }

        void setOwned(int o) {
            owned = o;
        }

        int getOwned() {
            // int owned;
            return 0;
        }

};

class Board {
    int curScore; // my box - enemy box
    int heuristic;
    bool nextPass; 
    Edge* mostRecentMove;
    Box* allBoxes[BOARD_HEIGHT][BOARD_WIDTH];

    public:

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
        Board(Board* prevBoard, Edge* currMove) { // this should be used to make a board from a previous board state
            this->mostRecentMove = currMove;
            
            // for(int i = 0; i < BOARD_HEIGHT; i++) {
            //     for(int j = 0; j < BOARD_WIDTH; j++) {
            //         this->allBoxes[i][j] = prevBoard->allBoxes[i][j];
            //     }
            // }
        }

        Edge* getMoves() { //TODO WRITE

            return NULL;
        }

        Board* move(Board* curBoard, Edge* theMove, int player) { //TODO WRITE
            Board* nextBoard = new Board(curBoard, theMove);
            return nextBoard;
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
const char* goFileName = "Clairvoyance.go";
const char* passFileName = "Clairvoyance.go";
const char* endFileName = "end_file";
const char* moveFileName = "move_file";
Edge* fakeMove = new Edge(0, 0, 0, 0);

void handleEndGame(FILE* endFile) {
    char endTxt[200];
    fread(endTxt, 200, 1, endFile);
    printf("%s", endTxt);
    fclose(endFile);
}

// returns move with name
char* edgeToString(Edge* edge) { 
    char* edgeString;
    int* coords = edge->getCoords();
    int outputNum = sprintf(edgeString, "Clairvoyance %d,%d, %d,%d", coords[0], coords[1], coords[2], coords[3]); //MYBE
    printf("Sprintf output %d, %s\n", outputNum,edgeString);
    return edgeString;
}

Edge* stringToEdge(char* moveString) { 
    char* moveCopy = (char*)malloc(200*sizeof(char));
    strncpy(moveCopy, moveString, 200);
    strtok(moveCopy," "); //gets rid of the name
    int y1 = atoi(strtok(moveCopy,","));
    Edge* newEdge = new Edge(atoi(strtok(moveCopy,",")),
                            atoi(strtok(moveCopy," ")),
                            atoi(strtok(moveCopy,",")),
                            atoi(strtok(moveCopy,","))); //not sure last delim is correct
    return NULL;
}

bool findFile(FILE* fp, const char* fileName) {
    fp = fopen(fileName, "r");
    return fp == NULL; // uhh mybe?
}

Edge* extractMove(FILE* moveFP) {
    char moveString[200];
    fread(moveString, sizeof(moveString), 1, moveFP);
    fclose(moveFP);
    return stringToEdge(moveString);
}

void writeMove(Edge* moveEdge) {
    FILE* local_moveFP = fopen(moveFileName, "w"); 
    fprintf(local_moveFP, "%s", edgeToString(moveEdge));
    fclose(local_moveFP);
}

Edge* generateRandomMove(Board* currBoard) {
    return currBoard->getMoves();
}


// current todo. TEST board, legal move list, and random move maker.
int main(int argc, char** argv) {
    printf("Clairvoyance starting");

    Board* localBoard = new Board(); 
    Edge* legalMoves;

    bool gameOver = false;
    Edge* bestMove; 
    Edge* oppMove;
    FILE* endFP;
    FILE* moveFP;

    /* We Loop until the game is over */
    while (!gameOver) {
        if(findFile(NULL, goFileName)) {
            if(findFile(endFP, endFileName)) {
                handleEndGame(endFP);
                gameOver = true;
                break;
            }
            findFile(moveFP, moveFileName);
            oppMove = extractMove(moveFP);
            localBoard = localBoard->move(localBoard, oppMove, 2);
            localBoard->setPass(false); //when player updates board, never handle passing

            if(findFile(NULL,passFileName)) {
                writeMove(fakeMove);
            }
            else {
                //this is where we take our turn
                /* GENERATE BEST MOVE */
                Edge* myMove = generateRandomMove(localBoard);
                localBoard = localBoard->move(localBoard, myMove, 1);
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