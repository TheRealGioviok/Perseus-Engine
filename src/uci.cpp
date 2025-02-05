#include "tt.h"
#include "types.h"
#include "movegen.h"
#include "tables.h"
#include "bench.h"
#include "evaluation.h"
#include "constants.h"
#include "uci.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>

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

U32 hashSize = 64; // The size of the hash table in MB.

void uciStr() {
    std::cout << "id name " << "Perseus" << std::endl;
    std::cout << "id author " << "G.M. Manduca" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
    std::cout << "option name Hash type spin default 64 min 8 max 1024" << std::endl;
    
    for(const TunableParam& param : tunableParams())
        std::cout << "option name " << param.name << " type spin default " << param.defaultValue << " min " << param.minValue << " max " << param.maxValue << std::endl;

    std::cout << "uciok" << std::endl;
}

int executeCommand(Game* game, const std::string& command) {
    static const std::unordered_map<std::string, std::function<void()>> commandMap = {
        {"tunestr", [&]() {
            for (const TunableParam& param : tunableParams()) {
                std::cout << param.name << ", int, " << param.defaultValue << ", "
                          << param.minValue << ", " << param.maxValue << ", "
                          << param.cEnd << ", " << param.rEnd << std::endl;
            }
        }},
        {"ucinewgame", [&]() { initTT(); game->reset(); }},
        {"uci", [&]() { uciStr(); }},
        {"isready", [&]() { std::cout << "readyok" << std::endl; }},
        {"flip", [&]() { game->pos.reflect(); }},
        {"setoption name", [&]() { setOptionCommand(command); }},
        {"tt", [&]() {
            Position& pos = game->pos;
            if (auto* entry = probeTT(pos.hashKey)) {
                std::cout << "TT for hashKey " << std::hex << pos.hashKey << " (" << entry->hashKey << std::dec << ") :\n"
                          << "Score: " << entry->score << "\t\tStatic eval: " << entry->eval << "\n"
                          << "Depth: " << static_cast<int>(entry->depth) << "\n"
                          << "BestMove: " << getMoveString(entry->bestMove) << "\n";
            } else {
                std::cout << "No entry!\n";
            }
        }},
        {"position", [&]() { positionCommand(game, command); }},
        {"fen", [&]() { std::cout << game->pos.getFEN() << std::endl; }},
        {"go", [&]() { goCommand(game, command); }},
        {"stop", [&]() { game->stopped = true; }},
        {"quit", [&]() { exit(1); }},
        {"print", [&]() { game->print(); }},
        {"eval", [&]() { std::cout << "Static evaluation: " << pestoEval(&(game->pos)) << std::endl; }},
        {"divide", [&]() { game->divide(std::stoi(command.substr(7))); }},
        {"movelist", [&]() { game->divide(1); }},
        {"exec", [&]() { execCommand(game, command.c_str()); }},
        {"bench", [&]() {
            std::string arg = command.substr(5);
            int depth = (arg.empty()) ? 12 : std::stoi(arg);
            benchmark(depth);
        }},
        {"extract", [&]() {
            std::string inputFilename = command.substr(8);
            std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + ".feat";
            convertToFeatures(inputFilename.c_str(), outputFilename.c_str());
        }}
    };
    
    for (const auto& [key, func] : commandMap) {
        if (command.find(key) != std::string::npos) {
            func();
            return 0;
        }
    }
    return 0;
}

int setOptionCommand(const std::string& command) {
    std::string optionName = command.substr(15);
    size_t space = optionName.find(" value ");
    if (space == std::string::npos) {
        std::cout << "Invalid command format!\n";
        return 0;
    }
    
    std::string arg = optionName.substr(space + 7);
    optionName.erase(space);
    
    static std::unordered_map<std::string, std::function<void(const std::string&)>> optionHandlers = {
        {"stposHashDump", [](const std::string& arg) { hashDumpFile = arg; }},
        {"Hash", [](const std::string& arg) {
            int size = std::stoi(arg);
            if (size < 1) {
                std::cout << "Invalid hash size!\n";
                return;
            }
            resizeTT(size);
        }},
        {"Threads", [](const std::string&) { /* Placeholder for future multithreading */ }},
        {"lmrDepthValue", [](const std::string&) { initLMRTable(); }},
        {"lmrMoveValue", [](const std::string&) { initLMRTable(); }},
        {"lmrA0", [](const std::string&) { initLMRTable(); }},
        {"lmrC0", [](const std::string&) { initLMRTable(); }},
        {"lmrA1", [](const std::string&) { initLMRTable(); }},
        {"lmrC1", [](const std::string&) { initLMRTable(); }},
        {"lmpA0", [](const std::string&) { initLMRTable(); }},
        {"lmpC0", [](const std::string&) { initLMRTable(); }},
        {"lmpA1", [](const std::string&) { initLMRTable(); }},
        {"lmpC1", [](const std::string&) { initLMRTable(); }}
    };

    for (TunableParam& param : tunableParams()) {
        if (param.name == optionName) {
            int value = std::stoi(arg);
            if (value < param.minValue || value > param.maxValue) {
                std::cout << "Invalid value for option " << optionName << "!\n";
                return 0;
            }
            param = value;
            return 0;
        }
    }
    
    auto it = optionHandlers.find(optionName);
    if (it != optionHandlers.end()) {
        it->second(arg);
    } else {
        std::cout << "Option " << optionName << " not recognized!\n";
    }
    
    return 0;
}


int positionCommand(Game* game, const std::string& command) {
    game->pos.wipe();
    
    static std::unordered_map<std::string, std::function<void(Game*)>> positionHandlers = {
        {"startpos", [](Game* game) { game->parseFEN((char *)startPosition.c_str()); }},
        {"perseuspos", [](Game* game) { game->parseFEN((char *)perseusPosition.c_str()); }},
        {"trickypos", [](Game* game) { game->parseFEN((char *)trickyPosition.c_str()); }}
    };
    
    for (const auto& [key, handler] : positionHandlers) {
        if (command.find(key) != std::string::npos) {
            handler(game);
            return 0;
        }
    }
    
    size_t fenPos = command.find("fen ");
    if (fenPos != std::string::npos) {
        game->parseFEN((char *)command.substr(fenPos + 4).c_str());
    }
    
    size_t movesPos = command.find("moves ");
    if (movesPos != std::string::npos) {
        game->executeMoveList((char *)command.substr(movesPos + 6).c_str());
    }
    
    return 0;
}

int goCommand(Game* game, const std::string& command) {
    static std::unordered_map<std::string, std::function<void(Game*, const std::string&)>> goHandlers = {
        {"depth", [](Game* game, const std::string& arg) { game->depth = std::stoi(arg); game->searchMode = 1; }},
        {"movetime", [](Game* game, const std::string& arg) { game->moveTime = getTime64() + std::stoll(arg); game->searchMode = 3; game->depth = 127; }},
        {"infinite", [](Game* game, const std::string&) { game->moveTime = 0xFFFFFFFFFFFFFFFF; game->searchMode = 0; game->depth = 127; }},
        {"wtime", [](Game* game, const std::string& arg) { game->wtime = std::stoi(arg); game->searchMode = 2; }},
        {"btime", [](Game* game, const std::string& arg) { game->btime = std::stoi(arg); game->searchMode = 2; }},
        {"winc", [](Game* game, const std::string& arg) { game->winc = std::stoi(arg); game->searchMode = 2; }},
        {"binc", [](Game* game, const std::string& arg) { game->binc = std::stoi(arg); game->searchMode = 2; }},
        {"nodes", [](Game* game, const std::string& arg) { game->hardNodesLimit = std::stoll(arg); }}
    };

    game->hardNodesLimit = 0xFFFFFFFFFFFFFFFF;

    for (const auto& [key, handler] : goHandlers) {
        size_t pos = command.find(key + " ");
        if (pos != std::string::npos) {
            handler(game, command.substr(pos + key.length() + 1));
        }
    }
    
    game->startSearch(true);
    return 0;
}

void execCommand(Game* game, const char* command){
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
            BitBoard whitePieces = game->pos.occupancies[WHITE];
            BitBoard blackPieces = game->pos.occupancies[BLACK];
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
            const char* square1 = command + 10;
            std::cout << square1 << "\n";
            const char* square2 = square1 + 3;
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

#define MAX_INPUT_LENGTH 8192
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
        if(executeCommand(game, std::string(userInput)) == 1 || strcmp(userInput, "quit") == 0){
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