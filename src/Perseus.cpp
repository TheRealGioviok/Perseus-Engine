#include "uci.h"
#include "tables.h"
#include <cstdlib>
int main(){
    initAll();
    Game game;
    uciLoop(&game);
    return 0;
}