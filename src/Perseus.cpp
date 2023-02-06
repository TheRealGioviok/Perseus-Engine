#include "uci.h"
#include "tables.h"
#include <cstdlib>
int main(){
    setvbuf(stdout, NULL, _IONBF, 0);
    initAll();
    Game game;
    uciLoop(&game);
    return 0;
}