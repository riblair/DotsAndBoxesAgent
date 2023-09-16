#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>


const int BOARD_WIDTH = 9;
const int BOARD_Height = 9;
const char* goFileName = "Clairvoyance.go";
const char* passFileName = "Clairvoyance.go";
const char* endFileName = "end_file";
const char* moveFileName = "move_file";
char fakeMove[21] = "Clairvoyance 0,0 0,0";

void handleEndGame(FILE* endFile) {
    char endTxt[200];
    fread(endTxt, 200, 1, endFile);
    printf("%s", endTxt);
    fclose(endFile);
}

// TODO Write this function
char* edgeToString(/* Edge* edge */) { 
    return NULL;
}

// TODO Write this function
void /* Edge* */ stringToEdge(char* moveString) { 

}

bool findFile(FILE* fp, const char* fileName) {
    fp = fopen(fileName, "r");
    return fp == NULL; // uhh mybe?
}

void /* Edge* */ extractMove(FILE* moveFP) {
    // CLOSE FILE AFTER THIS!
}

void updateLegalMoveList(/* Edge* moveList, Edge* move */) {
    
}

void writeMove(char* move) {
    FILE* local_moveFP = fopen(moveFileName, "w"); 
    fprintf(local_moveFP, "%s", move);
    fclose(local_moveFP);
}


int main(int argc, char** argv) {
    printf("Clairvoyance starting");

    //Board localBoard = new Board( /* parameters */ ); 
    //Edge* legalMoves = new Edge[BOARD_WIDTH*BOARD_Height]
    //Agent myAgent = new Agent( /* parameters */ );
    bool gameOver = false;
    /* Edge* bestMove; */
    /* Edge* oppMove */
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
            /* oppMove = extractMove(moveFP) */
            updateLegalMoveList(/* legalMoves, oppMove*/ );

            if(findFile(NULL,passFileName)) {
                writeMove(fakeMove);
            }
            else {
                //this is where we take our turn
                /* Edge* bestMove = myAgent.generateBestMove() */
                updateLegalMoveList(/* legalMoves, bestMove*/);
                writeMove(fakeMove /* bestMove */); //replace with bestMove
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
