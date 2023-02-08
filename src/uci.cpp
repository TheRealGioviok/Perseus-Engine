#include "tt.h"
#include "types.h"
#include "movegen.h"
#include "tables.h"
#include "bench.h"
#include "evaluation.h"
#include "uci.h"
#include <fstream>
#include <vector>
#include <iostream>

bool quit = false;
int movesToGo = -1; // UCI parameter for the moves until the next time control.
int moveTime = -1;  // UCI parameter for the time per move.
int increment = -1; // UCI parameter for the time increment.
U64 startTime = -1; // The time when the engine started calculating.
U64 stopTime = -1;  // The time when the engine must stop calculating.
bool timeIsUp = false; // True if the time is up.
bool ponder = false;   // True if the engine is pondering.
U8 timeSet = -1;    // 0 for infinite, 1 for time per move, 2 for depth, 3 for normal play.
std::string hashDumpFile; // Additional opening hash file

void uciStr() {
    std::cout << "id name " << "Perseus" << std::endl;
    std::cout << "id author " << "G.M. Manduca" << std::endl;
    //std::cout << "option name stposHashDump type string default  " << std::endl;
    std::cout << "uciok" << std::endl;
}

int executeCommand(Game* game, char* command) {
    // Technically speaking, each possible command is present only once in the command, so we can look for the first character
    // Command supported:
    // - tt : Reads and reports the transposition table (if any) from the current position
    // - isready : returns "readyok" if the engine is ready to accept commands
    // - uci : returns engine info, followed by "uciok"
    // - setoption name <option_name> value <option_value> (To implement)
    // - ucinewgame : resets the engine to its initial state
    // - position fen <fen_string> : sets the position to the given fen string
    // - position startpos moves <move_list> : sets the position to the given move list
    // - go [depth / movetime / infinite] <num>: starts the engine to search for the best move using the parameters specified in the command
    // - stop : stops the engine from searching
    // - fen : print the currently loaded fen
    // - quit : exits the engine
    // - print : prints the current position
    // - eval : prints the static evaluation of the current position
    // - divide : prints the perft results for the current position, divided for each move
    // - movelist : wrapper for "divide 1"
    // - exec <command> : executes the given command ( no context is assumed, unless specified (this = game))
    // we will search for the first occurrence of a command
    char* ttt = strstr((char*)command, "tt");
    char* sanity = strstr((char*)command, "sanity");
	char* openingTT = strstr((char*)command, "openingTT");
    char* isReady = strstr((char*)command, "isready");
    char* uci = strstr((char*)command, "uci");
    char* setOption = strstr((char*)command, "setoption name");
    char* uciNewGame = strstr((char*)command, "ucinewgame");
    char* position = strstr((char*)command, "position");
    char* fen = strstr((char*)command, "fen");
    char* go = strstr((char*)command, "go");
    char* stop = strstr((char*)command, "stop");
    char* quit = strstr((char*)command, "quit");
    char* print = strstr((char*)command, "print");
    char* eval = strstr((char*)command, "eval");
    char* divide = strstr((char*)command, "divide");
    char* moveList = strstr((char*)command, "movelist");
    char* exec = strstr((char*)command, "exec");
    char* flip = strstr((char*)command, "flip");
    char* bench = strstr((char*)command, "bench");
    // The order of execution is the following:
    // - uci
    
    // - isready
    // - uci
    // - setoption
    // - ucinewgame
    // - position
    // - fen
    // - go
    // - stop
    // - quit
    // - print
    // - eval
    // - divide
    // - movelist
    // - tt
    // - bench

    if (uciNewGame){
        initTT();
        game->reset();
        if (hashDumpFile != "") {
            std::cout << "info string Loading opening hash dump...\n";
            std::ifstream f;
            f.open(hashDumpFile, std::ios::out | std::ios::binary);
            if (!f.good()) {
                std::cout << "info string Invalid file or path to file! Falling back to empty TT\n";
            }
            else {
                f.read((char*)tt, ttEntryCount * sizeof(ttEntry));
            }
            // Close
            f.close();
        }    
    }
    else if (uci) {
        uciStr();
        return 0;
    }

    if (isReady)
        std::cout << "readyok" << std::endl;

    if (flip) {
        game->pos.reflect();
        return 0;
    }
	
    if (sanity) {
        int cnt = 0;
        for (U64 i = 0; i < ttEntryCount; i++) {
            ttEntry entry = tt[i];
            if ((entry.eval != -infinity) && (entry.eval != entry.score)) {
                Depth depth;
                Score score, eval;
                HashKey hash;
                PackedMove bestMove;
                U8 flags;

                depth = entry.depth;
                score = entry.score;
                eval = entry.eval;
                hash = entry.hashKey;
                bestMove = entry.bestMove;
                flags = entry.flags;

                std::cout << "TT for hashKey " << std::hex << hash << std::dec << ":\n";
                std::cout << "Score: " << score << "\t\tStatic eval: " << eval << "\n";
                std::cout << "Depth: " << (int)depth << "\n";
                std::cout << "BestMove: " << getMoveString(bestMove) << "\n";
                std::cout << "Flags:\n" <<
                    (flags & hashUPPER ? "\thashUPPER\n" : "") <<
                    (flags & hashLOWER ? "\thashLOWER\n" : "") <<
                    (flags & hashEXACT ? "\thashEXACT\n" : "") <<
                    (flags & hashSINGULAR ? "\thashALPHA\n" : "") <<
                    (flags & hashINVALID ? "\thashALPHA\n" : "") << "\n\n";
                if (cnt++ >= 100) goto megabreak;
            }
        }
    megabreak:
        exit(-1);
    }

    
	

    if (setOption){
        // TODO: implement setoption
        setOptionCommand(game, command);
        return 0;
    }

    if (openingTT) {
        // the command should be "openingTT <depth>"
        // This will do a search at the given depth, then store the TT in a file
        // The file will be named "openingTT_<depth>.txt"
        // The file will contain the TT entries, one per line, in the following format:
        // <hashKey>|<depth>|<score>|<eval>|<bestMove>|<flags>
        // all the numbers will be in hex format
        // the flags will be a string of the following characters:
        // A = hashALPHA
        // B = hashLOWER
        // E = hashEXACT
        // S = hashSINGULAR
        // I = hashINVALID
        // O = hashOLD
        // The file will be overwritten if it already exists

        // first, we need to get the depth
        char* depthStr = strstr((char*)command, "openingTT");
        depthStr += 9;
        int depth = atoi(depthStr);
        if (depth <= 0) {
            std::cout << "Invalid depth!\n";
            return 0;
        }
        // now we can start the search
        // we will use the startpos
        game->reset();
        // we will use the given depth
        for (Depth d = 1; d <= depth; d++) {
            game->depth = d;
            game->searchMode = 1; // fixed depth search
            game->startSearch(false);
        }
        for (Depth d = 1; d <= depth; d++) {
            game->depth = depth;
            game->searchMode = 1; // fixed depth search
            game->startSearch(false);
        }
        // now we can save the TT
        // first, we need to create the file name
        std::string fileName = "openingTT_";
        fileName += std::to_string(depth);
        fileName += ".ttdump";
        // now we can open the file
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::binary );
        if (!file) {
            std::cout << "info string Cannot open file to dump!" << std::endl;
            return 1;
        }
        // Binary dump (faster and smaller than human readable dump)
        file.write((char*)tt, ttEntryCount * sizeof(ttEntry));
        // now we can close the file
        file.close();
        return 0;
    }

    if (ttt) {
        Position& pos = game->pos;
        ttEntry* entry = probeTT(pos.hashKey);
        if (!entry) {
            std::cout << "No entry!\n";
            return 0;
        }
        Depth depth;
        Score score, eval;
        HashKey hash;
        Move bestMove;
        U8 flags;

        depth = entry->depth;
        score = entry->score;
        eval = entry->eval;
        hash = entry->hashKey;
        bestMove = entry->bestMove;
        flags = entry->flags;

        std::cout << "TT for hashKey " << std::hex << pos.hashKey << " (" << hash << std::dec << ") :\n";
        std::cout << "Score: " << score << "\t\tStatic eval: " << eval << "\n";
        std::cout << "Depth: " << (int)depth << "\n";
        std::cout << "BestMove: " << getMoveString(bestMove) << "\n";
        std::cout << "Flags:\n" <<
            (flags & hashUPPER ? "\thashUPPER\n" : "") <<
            (flags & hashLOWER ? "\thashLOWER\n" : "") <<
            (flags & hashEXACT ? "\thashEXACT\n" : "") <<
            (flags & hashSINGULAR ? "\thashSingular\n" : "") <<
            (flags & hashINVALID ? "\thashInvalid\n" : "") << "\n";
    }


    

    if (position) positionCommand(game, command);
    
    if (fen) std::cout << game->pos.getFEN() << std::endl;

    if (go) goCommand(game, command);

    if (stop) game->stopped = true;

    if (quit) return 1;

    if (print) game->print();

    if (eval) std::cout << "Static evaluation for current position (from side to move perspective): " << pestoEval(&(game->pos)) << std::endl;

    if (divide) {
        std::cout << "Perft results for current position:" << std::endl;
        game->divide(atoi(divide + 7));
    }

    if (moveList) game->divide(1);

    if (exec) execCommand(game, command);

    if (bench){
        benchmark();
    }

    return 0;
}

int setOptionCommand(Game* game, char* command) {
    // For now, we only have the stposHashDump command
    // Get option name
    char* optionNamec = command + 15;
    // get string after 
    std::string optionName(optionNamec);
    // find first space name
    size_t space = optionName.find(" value ");
    // split the arg
    std::string arg = optionName.substr(space + 1 + 5 + 1);
    // remove last characters from optionName
    optionName.erase(space);
    std::cout << "option name " << optionName << " value " << arg;
	// Depending on optionName
    if (optionName == "stposHashDump") {
        // Set hashDumpFile to arg
        hashDumpFile = arg;
    }
    
    return 0;
}

int positionCommand(Game *game, char* command){
    // the position command is composed by the following subcommands:
    // - fen <fen_string> : sets the position to the specified fen string
    // - startpos : sets the position to the starting position
    // - trickypos : sets the position to the tricky position (debugging purposes)
    // - moves <move_list> : sets the position to the specified move list

    // we will search for the first occurrence of a command

    game->pos.wipe();

    char* fen = strstr((char *)command, "fen");
    char* startposCMD = strstr((char *)command, "startpos");
    char* trickyposCMD = strstr((char *)command, "trickypos");
    char* moves = strstr((char *)command, "moves");

    if(startposCMD){
        game->parseFEN((char*)std::string(startPosition).c_str());
    }

    if(trickyposCMD){
        game->parseFEN((char*)std::string(trickyPosition).c_str());
    }

    if(fen){
        game->parseFEN(fen + 4);
    }

    if(moves){
        game->executeMoveList(moves + 6);
    }

    return 0;
}

int goCommand(Game* game, char* command){
    // the go command is composed by the following subcommands:
    // - depth <num> : sets the depth to the specified value
    // - movetime <num> : sets the movetime to the specified value
    // - infinite : sets the movetime to infinite
    // - wtime <num> : sets the white time to the specified value
    // - btime <num> : sets the black time to the specified value
    // - winc <num> : sets the white increment to the specified value
    // - binc <num> : sets the black increment to the specified value
    // - movetime <num> : sets the movetime to the specified value

    // we will search for the first occurrence of a command
    char* depth = strstr((char *)command, "depth");
    char* movetime = strstr((char *)command, "movetime");
    char* infinite = strstr((char *)command, "infinite");
    char* wtime = strstr((char *)command, "wtime");
    char* btime = strstr((char *)command, "btime");
    char* winc = strstr((char *)command, "winc");
    char* binc = strstr((char *)command, "binc");

    if(depth){
        game->depth = atoi(depth + 6);
        game->searchMode = 1;
    }
    else {
        if(movetime){
            game->moveTime = getTime64();
            game->moveTime += atoi(movetime + 9);
            game->searchMode = 3;
            game->depth = 127;
        }
        else {
            // if no movetime is specified, we set the movetime to infinite
            game->moveTime = 0xFFFFFFFFFFFFFFFF;
            game->searchMode = 0;
            game->depth = 127;
        }

        if(infinite){
            game->moveTime = 0xFFFFFFFFFFFFFFFF;
            game->searchMode = 0;
            game->depth = 127;
        }

        if(wtime){
            game->wtime = atoi(wtime + 6);
            game->searchMode = 2; // To mark that we are not in infinite mode
        }

        if(btime){
            game->btime = atoi(btime + 6);
            game->searchMode = 2; // To mark that we are not in infinite mode
        }

        if(winc){
            game->winc = atoi(winc + 5);
            game->searchMode = 2; // To mark that we are not in infinite mode
        }

        if(binc){
            game->binc = atoi(binc + 5);
            game->searchMode = 2; // To mark that we are not in infinite mode
        }
    }

    game->startSearch(true);

    return 0;
}

void execCommand(Game* game, char* command){
    // the exec command is composed by the following subcommands:
    // - exec <command> : executes the specified command

    char* exec = strstr((char *)command, "exec");

    if(exec){
        // We will add more commands as we need them, for now we just have:
        // "mayBeZugswang" : returns game->pos.mayBeZugswang()
        // "printBB <bb>" : calls printBitBoard((BitBoard)bb);
        if(strstr((char *)command, "zugswang")){
            std::cout << "game->pos.mayBeZugswang() returned " << game->pos.mayBeZugzwang() << std::endl;
        }
        else if (strstr((char *)command, "pinned")){
            std::cout << "pinned pieces: \n";
            bool side = game->pos.side;
            BitBoard whitePieces = game->pos.bitboards[0] | game->pos.bitboards[1] | game->pos.bitboards[2] | game->pos.bitboards[3] | game->pos.bitboards[4] | game->pos.bitboards[5];
            BitBoard blackPieces = game->pos.bitboards[6] | game->pos.bitboards[7] | game->pos.bitboards[8] | game->pos.bitboards[9] | game->pos.bitboards[10] | game->pos.bitboards[11];
            BitBoard pieces = whitePieces | blackPieces;
            BitBoard toPrint = 0;
            if (side == WHITE){
                BitBoard blackBQ = game->pos.bitboards[8] | game->pos.bitboards[10];
                BitBoard blackRQ = game->pos.bitboards[9] | game->pos.bitboards[10];
                BitBoard ourKing = game->pos.bitboards[5];
                Square ourKingSquare = popLsb(ourKing);
                toPrint = getPinnedPieces(pieces, whitePieces, ourKingSquare, blackRQ, blackBQ);
            }
            else{
                BitBoard whiteBQ = game->pos.bitboards[2] | game->pos.bitboards[4];
                BitBoard whiteRQ = game->pos.bitboards[3] | game->pos.bitboards[4];
                BitBoard ourKing = game->pos.bitboards[11];
                Square ourKingSquare = popLsb(ourKing);
                toPrint = getPinnedPieces(pieces, blackPieces, ourKingSquare, whiteRQ, whiteBQ);
            }
            printBitBoard(toPrint);
        }
        else if (strstr((char* )command, "btwn")){
            // read the next two squares. Each square is two characters long, so we need to read two characters for each square
            char* square1 = command + 10;
            std::cout << square1 << "\n";
            char* square2 = square1 + 3;
            Square sq1 = squareFromName(square1);
            Square sq2 = squareFromName(square2);
            BitBoard btwn = squaresBetween[sq1][sq2];
            printBitBoard(btwn);
        }
        else if(strstr((char *)command, "printBB")){
            char* bb = strstr((char *)command, "printBB");
            // bb is unsigned long long
            printBitBoard((BitBoard)std::stoull(bb + 8));
        }
    }
}

#define MAX_INPUT_LENGTH 2000
void uciLoop(Game* game){
    // The uciLoop is implemented in a way that it can be interrupted by the user, even if the engine is searching
    // The user can type "quit" to exit the engine
    
    std::cin.clear();
    std::cin.sync();
    std::cout.flush();

    // define user / GUI input buffer
    char userInput[MAX_INPUT_LENGTH];

    std::string input = "";

    // loop
    while(1){

        // reset the input string
        input = "";

        // read the user input
        std::cin.getline(userInput, MAX_INPUT_LENGTH);

        //print received input
        //std::cout << "Received input: " << userInput << std::endl;

        // if recived quit during search
        if (quit){
            return;
        }


        // make sure the user input is not new line
        if (userInput[0] == '\n' || strcmp(userInput, "") == 0){
            continue;
        }

        // execute the command
        if(executeCommand(game, userInput) == 1 || strcmp(userInput, "quit") == 0){
            break;
        }

        // flush the output
        std::cout.flush();

        std::cout << std::flush;

        // clear the input buffer
        std::cin.clear();

        // sync the input buffer
        std::cin.sync();

        // clear the user input buffer
        memset(userInput, 0, MAX_INPUT_LENGTH);
    }
}

int inputWaiting(){
#ifndef WIN32
    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));
#else
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }

    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
        return dw;
    }

    else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
#endif
}

void readInput(Game *game){
    // bytes to read holder
    int bytes;

    // GUI/user input
    char input[256] = "";
    char* endc;

    // "listen" to STDIN
    if (inputWaiting()) {
        // tell engine to stop calculating
        game->stopped = 1;

        // loop to read bytes from STDIN
        do {
            // read bytes from STDIN
#ifndef WIN32
            bytes = read(fileno(stdin), input, 256);
#else
            bytes = _read(_fileno(stdin), input, 256);
#endif
        }
        // until bytes available
        while (bytes < 0);

        // searches for the first occurrence of '\n'
        endc = strchr(input, '\n');

        // if found new line set value at pointer to 0
        if (endc) *endc = 0;

        // if input is available
        if (strlen(input) > 0) {
            // match UCI "quit" command
            if (!strcmp(input, "quit"))
            {
                //std::cout << "morte allo spaghetto volante"<<std::endl;
                // tell engine to terminate exacution    
                quit = 1;
            }
        }
    }
}

void UCICommunicate(Game *game){
    // if time is up, stop the search
    if (getTime64() > game->moveTime){
        game->stopped = true;
    }
    readInput(game);
}

/**
 * @brief The getTimeMs function returns the current time in milliseconds.
 * @return The current time in milliseconds.
 */
U64 getTime64() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}