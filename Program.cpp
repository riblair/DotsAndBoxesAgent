#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Edge {
    int coords[4];
    bool filled = false;

    public:
        //constructor
        Edge(int y1, int x1, int y2, int x2) {
            coords[0] = y1;
            coords[1] = x1;
            coords[2] = y2;
            coords[3] = x2;
        }

        void fill() {
            filled = true;
        }

        bool getFilled() {
            return filled;
        }
};

class Box {  
    int filled = 0;
    int owned = 0;

    //going clockwise from top
    Edge *edges[4];

    bool chainable[4] = {false, false, false, false};

    public:
        //constructor
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

        void setOwned(int o) {
            owned = o;
        }

        int getOwned() {
            int owned;
        }

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

};

//evaluation function
int eval(){
    return 0;
}

Edge bestMove;

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

//actual minimax func
int minimax(Board board, int depth, bool isMax, int alpha, int beta) {
    if ((depth == 0) || (board.getMoves() == NULL)) {
        return board.getEval();
    }

    if (isMax) {
        int maxEval = -100000;
        Edge moves[] = board.getMoves();
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
}


int main(int argc, char** argv) {
    printf("Hello World");
    return 0;
}
