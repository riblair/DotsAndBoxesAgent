#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
#include<unistd.h> 
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

const int BOARD_WIDTH = 9; // boxes
const int BOARD_HEIGHT = 9; // boxes
const char* goFileName = "Clairvoyance.go";
const char* passFileName = "Clairvoyance.pass";
const char* endFileName = "end_file";
const char* moveFileName = "move_file";

const int DEPTH = 2;

int maxChain;

int test2 = 0;

// Delete approach not working, Try and make mailable board stuff. 

class Edge {
    
    bool filled = false;
    
    public:
        int coords[4];
        Edge* nextEdge; // this is for linkedList purposes.
        /*Constructor for Edge */
        Edge(int y1, int x1, int y2, int x2) {
            coords[0] = y1;
            coords[1] = x1;
            coords[2] = y2;
            coords[3] = x2;

            this->nextEdge = NULL;
        }

        void fill() {
            this->filled = true;
        }

        bool getFilled() {
            return this->filled;
        }

        void setFill(bool f) {
            this->filled = f;
        }

        int* getCoords() {
            return this->coords;
        }

        bool equals(Edge* otherEdge) {
            int* otherCoords = otherEdge->coords;
            return  coords[0 ]==otherCoords[0] && 
                    coords[1] == otherCoords[1] && 
                    coords[2] == otherCoords[2] && 
                    coords[3] == otherCoords[3];
        }
};

Edge* fakeMove = new Edge(0, 0, 0, 0);

class Box {  
    int filled = 0;
    int owned = 0;
    int boxX = 0;
    int boxY = 0;


    //going clockwise from top
    
    public:
        /*Constructor for Box */
        Edge *edges[4];

        Box(Edge *e0, Edge *e1, Edge *e2, Edge *e3, int bX, int bY) {
            edges[0] = e0;
            edges[1] = e1;
            edges[2] = e2;
            edges[3] = e3;
            boxX = bX;
            boxY = bY;
        }

        //helper functions

        // This function updates filled, owned, and edges. Returns true if filled 
        bool updateBox(int edgeNum, Edge** newEdge, int player) {
            //Edge* currentEdge = edges[edgeNum];

            edges[edgeNum] =  *newEdge;
            edges[edgeNum]->fill();
            int filledNum = 0;
            bool isFilled = false;

            for (int i = 0; i < 4; i++) {
                if (edges[i]->getFilled()) {
                    filledNum += 1;
                }
            }

            if(filledNum == 4) { //box is captured
                this->setOwned(player);
                isFilled = true;
            }

            this->filled = filledNum;
            
            return isFilled;
        }

        bool undoUpdate(int edgeNum, Edge** newEdge, int player) {
            edges[edgeNum] =  *newEdge;
            edges[edgeNum]->setFill(false);
            int filledNum = 0;
            bool unFilled = false;

            for (int i = 0; i < 4; i++) {
                if (edges[i]->getFilled()) {
                    filledNum += 1;
                }
            }

            if(this->filled == 4) { //box is no longer captured
                this->setOwned(0);
                unFilled = true;
            }

            this->filled = filledNum;
            
            return unFilled;
        }

        void setFilled(int f) {
            filled = f;
        }

        int getFilled() {
            return filled;
        }

        void setOwned(int o) {
            owned = o;
        }

        int getOwned() {
            return owned;
        }

        int getBoxX() {
            return boxX;
        }

        int getBoxY() {
            return boxY;
        }

};

Edge* edgeCopy(Edge** original) {
    int moveCoords[4]; 
    moveCoords[0] = (*original)->getCoords()[0];
    moveCoords[1] = (*original)->getCoords()[1];
    moveCoords[2] = (*original)->getCoords()[2];
    moveCoords[3] = (*original)->getCoords()[3];

    Edge* copy = new Edge(moveCoords[0], moveCoords[1],moveCoords[2],moveCoords[3]);
    if((*original)->getFilled()) copy->fill();
    // NOT COPYING NEXTEDGE
    return copy;
}

 Box* boxCopy(Box** original) {
    
    Edge* copyTop = edgeCopy(&(*original)->edges[0]);
    Edge* copyRight = edgeCopy(&(*original)->edges[1]);
    Edge* copyBot = edgeCopy(&(*original)->edges[2]);
    Edge* copyLeft = edgeCopy(&(*original)->edges[3]);

    Box* boxCopy = new Box(copyTop, copyRight, copyBot, copyLeft, (*original)->getBoxX(),(*original)->getBoxY());
    boxCopy->setOwned((*original)->getOwned());
    boxCopy->setFilled((*original)->getFilled());

    return boxCopy;
 }

class Board {
    int myScore; // my boxes
    int enemyScore; //enemy boxes 
    bool nextPass; 
    Edge* mostRecentMove;
    

    public:
        Box* allBoxes[BOARD_HEIGHT][BOARD_WIDTH];

        Board() { // for beginning 
            myScore = 0;
            enemyScore = 0; 
            this->mostRecentMove = fakeMove;
            this->nextPass = false;
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

                    allBoxes[i][j] = new Box(topEdge, rightEdge, bottomEdge, leftEdge, i, j);
                }
            }
        }
        // this should be used to make a board from a previous board state
        Board(Board** prevBoard, Edge** currMove, int player, int ms, int es) { 

            // curMove should be a copy of a pointer, and not the actual move.
            this->mostRecentMove = *currMove;
            myScore = ms;
            enemyScore = es;
            //copy all boxes, update only relevant boxes

            Box* copyBox;

            // HEY, this now makes two equivalent edges for adjacent boxes. Might be an issue, will cross that bridge IF it arises. 
            for(int i = 0; i < BOARD_HEIGHT; i++) {
                for(int j = 0; j < BOARD_WIDTH; j++) {
                    copyBox = boxCopy(&(*prevBoard)->allBoxes[i][j]);
                    // copy boxes, then
                    allBoxes[i][j] = copyBox;
                }
            }
            /* relevant boxes are defined as boxes whose Edge needs to be updated
                cases:  left / right / top/ bottom MOST edges -> only update 1 box
                        vertical edge -> update boxes left and right
                        horizontal edge -> update boxes above and below
            */
           // all coords are in order of lowest Y, lowest X, highest Y, highest X
            int moveCoords[4]; 
            moveCoords[0] = (*currMove)->getCoords()[0];
            moveCoords[1] = (*currMove)->getCoords()[1];
            moveCoords[2] = (*currMove)->getCoords()[2];
            moveCoords[3] = (*currMove)->getCoords()[3];

            bool verticalMove = moveCoords[1] == moveCoords[3]; 
            bool horizontalMove = moveCoords[0] == moveCoords[2]; 

            if((!verticalMove && !horizontalMove) || (verticalMove && horizontalMove)) { //hopefully this never happens :)
                printf("Something went wrong with move coordinates\n");
                printf("Current coordinantes: %d,%d, %d,%d ", moveCoords[0],moveCoords[1],moveCoords[2],moveCoords[3]);
            }

            bool somethingFilled = false;

            bool topRowMove = horizontalMove && moveCoords[0] == 0;
            bool botRowMove = moveCoords[0] == BOARD_HEIGHT; 
            bool leftRowMove = verticalMove && moveCoords[1] == 0;
            bool rightRowMove = moveCoords[1] == BOARD_WIDTH;

            if(topRowMove) {
                somethingFilled = allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(0, currMove, player);
            }
            else if(rightRowMove) {
                somethingFilled = allBoxes[moveCoords[0]][moveCoords[1]-1]->updateBox(1, currMove, player);
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
                somethingFilled |= allBoxes[moveCoords[0]][moveCoords[1]-1]->updateBox(1, currMove, player);
                somethingFilled |= allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(3, currMove, player);
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
            this->enemyScore = enemyCurScore;
        }

        //Test this function
        Edge* getMoves() { 
            test2 = 0;

            Edge* moveList;
            moveList = fakeMove;
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
            //printf("list length: %d\n", test2);

            return moveList->nextEdge;
        }
        // DONE?
        Board* move(Board** curBoard, Edge** theMove, int player) { 

            if((*theMove)->equals(fakeMove)) {
                this->nextPass = false;
                return this;
            }
            else {
                Edge* copy = edgeCopy(theMove);
                copy->fill();
                Board* nextBoard = new Board(curBoard, &copy, player, myScore, enemyScore);
                return nextBoard;
            }
        }

        void undoMove(Edge** theMove, int player, Edge* newLastMove) {
            
            this->mostRecentMove = newLastMove;

            // all coords are in order of lowest Y, lowest X, highest Y, highest X
            int moveCoords[4]; 
            moveCoords[0] = (*theMove)->getCoords()[0];
            moveCoords[1] = (*theMove)->getCoords()[1];
            moveCoords[2] = (*theMove)->getCoords()[2];
            moveCoords[3] = (*theMove)->getCoords()[3];

            bool verticalMove = moveCoords[1] == moveCoords[3]; 
            bool horizontalMove = moveCoords[0] == moveCoords[2]; 

            bool somethingUnFilled = false;

            bool topRowMove = horizontalMove && moveCoords[0] == 0;
            bool botRowMove = moveCoords[0] == BOARD_HEIGHT; 
            bool leftRowMove = verticalMove && moveCoords[1] == 0;
            bool rightRowMove = moveCoords[1] == BOARD_WIDTH;

            if(topRowMove) {
                somethingUnFilled = allBoxes[moveCoords[0]][moveCoords[1]]->undoUpdate(0, theMove, player);
            }
            else if(rightRowMove) {
                somethingUnFilled = allBoxes[moveCoords[0]][moveCoords[1]-1]->undoUpdate(1, theMove, player);
            }
            else if(botRowMove) {
                somethingUnFilled = allBoxes[moveCoords[0]-1][moveCoords[1]]->undoUpdate(2, theMove, player);
            }
            else if(leftRowMove) { 
                somethingUnFilled = allBoxes[moveCoords[0]][moveCoords[1]]->undoUpdate(3, theMove, player);
            }
            else if(horizontalMove) { //this is a horizontal move that affects two cells 
                somethingUnFilled |= allBoxes[moveCoords[0]-1][moveCoords[1]]->undoUpdate(2, theMove, player);
                somethingUnFilled |= allBoxes[moveCoords[0]][moveCoords[1]]->undoUpdate(0, theMove, player);
            }
            else { //this is a vertical move and will affect two cells
                somethingUnFilled |= allBoxes[moveCoords[0]][moveCoords[1]-1]->undoUpdate(1, theMove, player);
                somethingUnFilled |= allBoxes[moveCoords[0]][moveCoords[1]]->undoUpdate(3, theMove, player);
            }

            if(somethingUnFilled) {
                this->updateScore();
            }
            this->nextPass = false;
        }

        void moveMalleable(Edge** theMove, int player) { 

            if((*theMove)->equals(fakeMove)) {
                this->nextPass = false;
            }
            else {
                this->mostRecentMove = *theMove;

                // all coords are in order of lowest Y, lowest X, highest Y, highest X
                int moveCoords[4]; 
                moveCoords[0] = (*theMove)->getCoords()[0];
                moveCoords[1] = (*theMove)->getCoords()[1];
                moveCoords[2] = (*theMove)->getCoords()[2];
                moveCoords[3] = (*theMove)->getCoords()[3];

                bool verticalMove = moveCoords[1] == moveCoords[3]; 
                bool horizontalMove = moveCoords[0] == moveCoords[2]; 

                bool somethingFilled = false;

                bool topRowMove = horizontalMove && moveCoords[0] == 0;
                bool botRowMove = moveCoords[0] == BOARD_HEIGHT; 
                bool leftRowMove = verticalMove && moveCoords[1] == 0;
                bool rightRowMove = moveCoords[1] == BOARD_WIDTH;

                if(topRowMove) {
                    somethingFilled = this->allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(0, theMove, player);
                }
                else if(rightRowMove) {
                    somethingFilled = this->allBoxes[moveCoords[0]][moveCoords[1]-1]->updateBox(1, theMove, player);
                }
                else if(botRowMove) {
                    somethingFilled = this->allBoxes[moveCoords[0]-1][moveCoords[1]]->updateBox(2, theMove, player);
                }
                else if(leftRowMove) { 
                    somethingFilled = this->allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(3, theMove, player);
                }
                else if(horizontalMove) { //this is a horizontal move that affects two cells 
                    somethingFilled |= this->allBoxes[moveCoords[0]-1][moveCoords[1]]->updateBox(2, theMove, player);
                    somethingFilled |= this->allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(0, theMove, player);
                }
                else { //this is a vertical move and will affect two cells
                    somethingFilled |= this->allBoxes[moveCoords[0]][moveCoords[1]-1]->updateBox(1, theMove, player);
                    somethingFilled |= this->allBoxes[moveCoords[0]][moveCoords[1]]->updateBox(3, theMove, player);
                }

                if(somethingFilled) {
                    this->updateScore();
                }
                this->nextPass = somethingFilled;
            
            }
        }

        void setPass(bool pass) {
            this->nextPass = pass;
        }

        Box* getNeighbor(int bx, int by, int side) {
            int neighborX = bx;
            int neighborY = by;

            if (side == 0) {
                neighborY -= 1;
            }
            else if (side == 1) {
                neighborX += 1;
            }
            else if (side == 2) {
                neighborY += 1;
            }
            else if (side == 3) {
                neighborX -= 1;
            }

            if ((neighborX >= 0) && (neighborY >= 0)) {
                return allBoxes[neighborY][neighborX];
            }
            else {
                return NULL;
            }
            
        }

        //return side with chainable neighbor (or 5 if no sides are chainable)
        int chainNeighbor(int bx, int by, int possibleSide) {
            Box* curBox = allBoxes[by][bx];
            bool newArr[4] = {(curBox->edges[0])->getFilled(), (curBox->edges[1])->getFilled(), (curBox->edges[2])->getFilled(), (curBox->edges[3])->getFilled()};
            newArr[possibleSide] = true;

            int emptySide = 0;
            for (int i = 0; i < 4; i++) {
                if (newArr[i] == false) {
                    emptySide = i;
                }
            }
            
            Box* neighbor = this->getNeighbor(bx,by,emptySide);

            if (neighbor == NULL) {
                return 5;
            }

            if (neighbor->getFilled() != 2) {
                emptySide = 5;
            }
            return emptySide;

        }
        
        //return number of boxes in chain (stop counting if chain is longer than another already found)
        int chainNum(int count, int bx, int by, int n) {
            
            int chainable = chainNeighbor(bx, by, n);
            printf("a chain at last\n");

            int empty = 0;
            if (chainable >= 2) {
                empty = chainable - 2;
            }
            else {
                empty = chainable + 2;
            }

            if ((chainable < 4) && (count <= maxChain)) {
                Box* neighbor = getNeighbor(bx,by,chainable);
                return chainNum(count + 1, neighbor->getBoxX(), neighbor->getBoxY(), empty);
            }
            else {
                if (count < maxChain) {
                    maxChain = count;
                }
                return count;
            }
            
            return 0;
        }

        int getScore() {
            return myScore - enemyScore;
        }
        

        int getEval() {
            int chain = 0;
            int eval = getScore();

            //printf("checkpoint score: %d\n", eval);

            // get box coords
            int bx = mostRecentMove->getCoords()[1];
            int by = mostRecentMove->getCoords()[0];

            bool noPass = false;
            //bool noPass = !(nextPass);

            //if opponents turn to move (not pass), count possible chain
            if ((noPass) && (mostRecentMove != NULL)) {
                printf("enemy does not pass\n");

                bool isHorizontal = true;
                if (mostRecentMove->getCoords()[0] != mostRecentMove->getCoords()[2]) {
                    isHorizontal = false;
                }


                
                if (isHorizontal) {
                    //if upper box exists
                    if ((allBoxes[by-1][bx] != NULL) && ((by - 1) >= 0)){
                        chain += chainNum(0, bx, by - 1, 2);
                    }

                    //if lower box exists
                    if ((allBoxes[by][bx] != NULL) && (by <= 9)){
                        chain += chainNum(0, bx, by, 0);
                    }
                }
                else {
                    
                    //if left box exists
                    if ((allBoxes[by][bx-1] != NULL) && (bx - 1 >= 0)){
                        chain += chainNum(0, bx - 1, by, 1);
                    }

                    //if right box exists
                    if ((allBoxes[by][bx] != NULL) && (bx <= 9)){
                        chain += chainNum(0, bx, by, 3);
                        
                    }
                    
                }
                
            }

            eval -= chain;

            return eval;
        }

        Edge* getMostRecentMove() {
            return mostRecentMove;
        }
    
};

void deleteBoard(Board** oldBoard) {
    for(int i = 0; i < BOARD_HEIGHT; i++) {
        for(int j = 0; j < BOARD_WIDTH; j++) {
            for(int k = 0; k < 4; k++) {
                delete((*oldBoard)->allBoxes[i][j]->edges[k]);
            }
            delete((*oldBoard)->allBoxes[i][j]);
        }
    }
    // delete((*oldBoard)->allBoxes);
    delete((*oldBoard));
}

Edge* bestMove;

Board* currentBoard = new Board();
Edge* m0 = new Edge(0,0,0,0);
Edge* m1 = new Edge(0,0,0,0);
Edge* m2 = new Edge(0,0,0,0);
Edge* m3 = new Edge(0,0,0,0);
Edge* m4 = new Edge(0,0,0,0);
Edge* m5 = new Edge(0,0,0,0);
Edge* moveDepths[] = {m0, m1, m2, m3, m4, m5};

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

int test = 0;
int minimax(Board* board, int depth, bool isMax, int alpha, int beta) {
    currentBoard = board; 

    int eval = board->getEval();

    //printf("checkpoint 1, depth %d\n", depth);
    if (depth == 0) {
        if (depth < DEPTH) {
        Edge* recent = moveDepths[depth];
        currentBoard->undoMove(&recent, 2, moveDepths[depth+1]);
        moveDepths[depth] = moveDepths[depth+1];
        }
        //printf("checkpoint depth\n");

        return eval; // should be same as getScore() at terminal node.
    }

    Edge* moves = board->getMoves();
    if (moves == NULL) {
        //printf("checkpoint terminal\n");
        return board->getScore();
    }

    Board* childBoard = currentBoard; 
    if (isMax) {
        int maxEval = -100000;
        
        //printf("checkpoint isMax\n");
        while(moves != nullptr) {
            //printf("checkpoint 4\n");
            moveDepths[depth-1] = moves;
            childBoard->moveMalleable(&moves, 1);
            
            int eval = minimax(childBoard, depth-1, false, alpha, beta);
            if (eval > maxEval) {
                maxEval = eval;
                alpha = max(maxEval, alpha);
                if (depth == DEPTH) {
                    bestMove = moves;
                }
            }
            if (beta <= alpha) {
                break;
            }
            moves = moves->nextEdge;
        }
        if (depth < DEPTH) {
            Edge* recent = moveDepths[depth];
            currentBoard->undoMove(&recent, 2, moveDepths[depth+1]);
            moveDepths[depth] = moveDepths[depth+1];
        }

        return maxEval;
    }
    else {
        int minEval = 100000;
        while(moves != NULL) {
            moveDepths[depth-1] = moves;
            childBoard->moveMalleable(&moves, 2);

            int eval = minimax(childBoard, depth-1, true, alpha, beta);
            minEval = min(minEval, eval);
            beta = min(minEval, beta);
            if (beta <= alpha) {
                break;
            }

            moves = moves->nextEdge;
        }
        if (depth < DEPTH) {
            Edge* recent = moveDepths[depth];
            currentBoard->undoMove(&recent, 2, moveDepths[depth+1]);
            moveDepths[depth] = moveDepths[depth+1];
        }
        return minEval;
    }
};


bool findFile(FILE** fp, const char* fileName) {
    *fp = fopen(fileName, "r");
    return *fp != NULL;
}
/* It occurs to me that I do not know how to get the move properly. This is currently wrong. DEBUG THIS*/
// [name][space(s)][numeral],[numeral][space(s)][numeral],[numeral][space(s)]
Edge* stringToEdge(char* moveString) { 
    char row1[3];
    char col1[3];
    char row2[3];
    char col2[3];

    row1[2] = '\0';
    col1[2] = '\0';
    row2[2] = '\0';
    col2[2] = '\0';

    int length = strlen(moveString);
    bool firstFound = false;
    char curChar = moveString[0];
    char searchChar = ',';

    for(int i = 0; i < length; i++) {
        curChar = moveString[i];
        if(curChar == searchChar) {
            if(!firstFound) {
                row1[0] = moveString[i-2];
                row1[1] = moveString[i-1];

                col1[0] = moveString[i+1];
                col1[1] = moveString[i+2];
                firstFound = true;
                i = i+3; //to skip numerals
            }
            else {
                row2[0] = moveString[i-2];
                row2[1] = moveString[i-1];

                col2[0] = moveString[i+1];
                col2[1] = moveString[i+2];
                break;
            }
        }
    }

    int y1 = atoi(row1);
    int x1 = atoi(col1);
    int y2 = atoi(row2);
    int x2 = atoi(col2); 

    int lowestY = min(y1,y2);
    int lowestX = min(x1,x2);
    int highestY = max(y1,y2);
    int highestX = max(x1,x2);

    Edge* newEdge = new Edge(lowestY, lowestX, highestY, highestX); 

    //free(moveString); // prob not neccesary
    newEdge->fill();
    return newEdge;
}

Edge* extractMove(FILE** moveFP) {
    char moveString[200];
    fread(moveString, sizeof(moveString), 1, *moveFP);
    printf("after fread \n");
    fclose(*moveFP);
    return stringToEdge(moveString);
}


// returns move with name
char* edgeToString(Edge* edge) { 
    char* edgeString = (char*)malloc(20*sizeof(char)); // we have no way to close this, may cause memory leak :)
    int* coords = edge->coords;
    int outputNum = snprintf(edgeString, 24, "Clairvoyance %d,%d %d,%d", *coords, *(coords+1), *(coords+2), *(coords+3));
    printf("Sprintf output %d, %s\n", outputNum,edgeString);
    return edgeString;
}

void writeMove(Edge* moveEdge) {
    FILE* local_moveFP = fopen(moveFileName, "w"); 
    fprintf(local_moveFP, "%s", edgeToString(moveEdge));
    fclose(local_moveFP);
}

void findError(Edge* move) {
    int* moveCoords = move->getCoords();
    bool verticalMove = moveCoords[1] == moveCoords[3]; 
    bool horizontalMove = moveCoords[0] == moveCoords[2]; 

    bool check1 = !verticalMove && !horizontalMove;
    bool check2 = verticalMove && horizontalMove;
    bool check3 = moveCoords[0] < 0 || moveCoords[1] < 0 || moveCoords[2] < 0 || moveCoords[3] < 0;
    bool check4 = moveCoords[0] > BOARD_HEIGHT || moveCoords[2] > BOARD_HEIGHT || moveCoords[1] > BOARD_WIDTH ||moveCoords[3] > BOARD_WIDTH;  

    if(check1 || check2) { 
        printf("INVALID Opponenet Move \n");
    }
    if(check3 || check4) {
        printf("Opponenet's Move OUT OF BOUNDS\n");
    }
    if(!check1 && !check2 && !check3 && !check4) { // if it a valid edge AND the ref has reported that the game is over, the move must be a a filled edge
        printf("Opponents move is an ALREADY FILLED EDGE");
    }
    printf("Opponent's coordinantes: %d,%d, %d,%d ", moveCoords[0],moveCoords[1],moveCoords[2],moveCoords[3]);
}

bool handleEndGame() {
    FILE* endFP = (FILE*)malloc(sizeof(FILE*));
    if(findFile(&endFP, endFileName)) {
        //print referee error
        char endTxt[200];
        fread(endTxt, 200, 1, endFP);
        printf("%s", endTxt);
        fclose(endFP);
        free(endFP);
        //find own error 
        FILE* moveFile = (FILE*)malloc(sizeof(FILE*));
        findFile(&moveFile, moveFileName);
        findError(extractMove(&moveFile));
        free(moveFile);

        return true;
    }   
    free(endFP);
    return false;
}

Board* handleOppTurn(Board* currentBoard) {
    Edge* oppMove;
    Board* nextBoard = currentBoard;
    FILE* moveFP = (FILE*)malloc(sizeof(FILE*));
    findFile(&moveFP, moveFileName); // should always find file
    struct stat* fileStat = (struct stat*)malloc(sizeof(struct stat));
    stat(moveFileName,fileStat);
    if(!(fileStat->st_size > 0)) { //first move with an empty move file
        fclose(moveFP);
        return currentBoard; 
    }
    oppMove = extractMove(&moveFP); // this fcn also closes moveFP

    printf("extracted move\n");
    if(!oppMove->equals(fakeMove)) { // dont update board if opp passes 
        currentBoard->moveMalleable(&oppMove, 2);
    }
    currentBoard->setPass(false); //when player updates board, never handle passing
    return nextBoard; 
}

Edge* generateRandomMove(Board* currBoard) {
    return currBoard->getMoves();
}

// current issues, move file not closed properly in some place, board no update correctly
// Testable program now. Will make first legal move
int main(int argc, char** argv) {
    
    printf("Clairvoyance starting\n");

    Board* localBoard = new Board(); 
    printf("no crash\n");
    bool pass;
    clock_t t;
    FILE* goFile = (FILE*)malloc(sizeof(FILE*));
    FILE* passFile = (FILE*)malloc(sizeof(FILE*));
    
    /* We Loop until the game is over */
    while (true) {
        bool go = findFile(&goFile, goFileName);
        fclose(goFile);
        pass = findFile(&passFile, passFileName);
        fclose(passFile);
        if(go || pass) {
            printf("found go file!\n");
            //fclose(goFile);
            // start timer
            t = clock();
            if(handleEndGame()) break; //handles ending game if endgame file exists
            printf("done endgame!\n");
            localBoard = handleOppTurn(localBoard); // extracts opp move and updates current board
            printf("done opp turn!\n");
            for (int i = 0; i <= DEPTH; i++) {
                moveDepths[i] = localBoard->getMostRecentMove(); 
            }
            if(pass){ writeMove(fakeMove);} // handles passing if pass file exists

            if(!pass) { //take our turn if not passed
                maxChain = 10000;

                /* GENERATE BEST MOVE */
                bestMove = generateRandomMove(localBoard);

                minimax(localBoard, DEPTH, true, -100000, 100000); // 3 produced a move that gave the enemy a box. No Bueno
                printf("my move %d, %d, %d, %d\n", bestMove->coords[0], bestMove->coords[1], bestMove->coords[2], bestMove->coords[3]);
                bestMove->fill();
                //localBoard->moveMalleable(&moveDepths[DEPTH], 1);
                localBoard->moveMalleable(&bestMove, 1);
                for (int i = 0; i <= DEPTH; i++) {
                    moveDepths[i] = localBoard->getMostRecentMove(); 
                }
                //localBoard->setPass(false); //when player updates board, never handle passing
                writeMove(bestMove); //replace with bestMove
            }
            t = clock() - t;
            printf ("I took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
            sleep(1);
        }
        else {
            sleep(1);
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