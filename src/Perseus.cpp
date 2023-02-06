#include "uci.h"
#include "tables.h"
int main(){
    initAll();
    Game game;
    uciLoop(&game);
    return 0;
}