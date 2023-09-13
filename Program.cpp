#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Edge {
    int coords[4];
    bool filled = false;
    int heuristic;

    public:
        //constructor
        Edge(int x1, int y1, int x2, int y2) {
            coords[0] = x1;
            coords[1] = y1;
            coords[2] = x2;
            coords[3] = y2;
        }

        void fill() {
            filled = true;
        }

        void setHeuristic(int h) {
            heuristic = h;
        }

        void getHeuristic(int h) {
            heuristic = h;
        }
};

class Box {  
    int filled = 0;
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

        void updateChainable() {

        }

};


int main(int argc, char** argv) {
    printf("Hello World");
    return 0;
}