#include "uci.h"
#include "tt.h"
#include "evaluation.h"
#include "constants.h"
#include <iostream>
#include "bench.h"
#include <climits>

UCIHandler::UCIHandler(Game* g)
    : game(g), running(false) {}

UCIHandler::~UCIHandler() { stop(); }

void UCIHandler::start() {
    running = true;
    // io on main thread
    ioLoop();
}

void UCIHandler::stop() {
    running = false;
    game->searchLimits.stopped = true;  // Signal search to stop
    if (searchThread.joinable()) {
        searchThread.join();  // Ensure the search thread finishes cleanly
    }
}

void UCIHandler::ioLoop() {
    const int BUF = 8192;
    char buf[BUF];
    while (running) {
        if (!std::cin.good()) break;
        std::cin.getline(buf, BUF);
        std::string cmd(buf);
        if (cmd.empty()) continue;

        // Handle quit immediately
        if (cmd == "quit") {
            stop();
            break;
        }

        // Handle stop command during search - set the flag and wait for search to complete
        if (cmd == "stop") {
            if (game->searching) {
                game->searchLimits.stopped = true;
                // Wait for search to complete if needed
                if (searchThread.joinable()) {
                    searchThread.join();
                }
            }
            continue;
        }

        if (cmd.rfind("go", 0) == 0) {
            // Make sure any previous search is finished
            if (searchThread.joinable()) {
                searchThread.join();
            }
            
            // Parse go parameters first
            go(cmd);
            
            // Start a new search thread
            searchThread = std::thread([this] {
                game->startSearch(true);
                // Check if any commands were received during search
                communicate();
            });
            continue;
        }

        // Handle all other commands
        if (game->searching) {
            // If we're searching, queue the command for later processing
            std::lock_guard<std::mutex> lk(mtx);
            commandQueue.push(cmd);
        } else {
            // Process command directly when not searching
            processCommand(cmd);
        }
    }
}

void UCIHandler::processCommand(const std::string& cmd) {
    if (cmd == "uci")            { uciStr(); return; }
    if (cmd == "isready")        { std::cout << "readyok" << std::endl; return; }
    if (cmd == "stop")           { game->searchLimits.stopped = true; return; }
    if (cmd == "quit")           { stop(); return; }
    if (cmd.rfind("setoption", 0) == 0) { setOption(cmd); return; }
    if (cmd.rfind("ucinewgame", 0) == 0) { initTT(); game->reset(); return; }
    if (cmd.rfind("position", 0) == 0)  { position(cmd); return; }
    
    // Handle feature extraction command
    if (cmd.rfind("extract ", 0) == 0) {
        // Get the filename (skip "extract " prefix which is 8 characters)
        std::string inputFilename = cmd.substr(8);
        std::string outputFilename;
        
        // Find the last '.' in the filename
        size_t dotPos = inputFilename.find_last_of('.');
        if (dotPos != std::string::npos) {
            // If there's an extension, replace it with ".feat"
            outputFilename = inputFilename.substr(0, dotPos) + ".feat";
        } else {
            // If no extension, just add ".feat"
            outputFilename = inputFilename + ".feat";
        }
        
        // Convert to C-style strings for the function call
        convertToFeatures(inputFilename.c_str(), outputFilename.c_str());
        return;
    }
    
    if (cmd.rfind("bench", 0) == 0) {
        int d = 12;
        std::istringstream iss(cmd);
        std::string t; iss >> t; // bench
        if (iss >> d) {}
        benchmark(d);
        return;
    }
    if (cmd == "print")    { game->print(); return; }
    if (cmd == "eval")     { std::cout << pestoEval(&game->pos) << std::endl; return; }
    if (cmd == "movelist"){ game->divide(1); return; }
}

void UCIHandler::uciStr() const {
    std::cout << "id name Perseus" << std::endl;
    std::cout << "id author G.M. Manduca" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
    std::cout << "option name Hash type spin default " << hashSize
              << " min 8 max 33554432" << std::endl;
    for (auto& p : tunableParams())
        std::cout << "option name " << p.name
                  << " type spin default " << p.defaultValue
                  << " min " << p.minValue
                  << " max " << p.maxValue << std::endl;
    std::cout << "uciok" << std::endl;
}

int UCIHandler::setOption(const std::string& cmd) {
    auto n = cmd.find("name "); auto v = cmd.find("value ");
    if (n == std::string::npos || v == std::string::npos) return 0;
    std::string name = cmd.substr(n + 5, v - (n + 5) - 1);
    std::string val  = cmd.substr(v + 6);
    if (name == "Hash") { hashSize = std::stoull(val); resizeTT(hashSize); }
    else if (name == "Threads") {}
    else
        for (auto& p : tunableParams())
            if (p.name == name) {
                int x = std::stoi(val);
                if (x < p.minValue || x > p.maxValue) return 0;
                p = x; break;
            }
    return 0;
}

int UCIHandler::position(const std::string& cmd) {
    game->pos.wipe();
    if (cmd.find("startpos") != std::string::npos)
        game->parseFEN(const_cast<char*>(startPosition));
    else if (auto f = cmd.find("fen "); f != std::string::npos)
        game->parseFEN(const_cast<char*>(cmd.c_str() + f + 4));
    if (auto m = cmd.find("moves "); m != std::string::npos)
        game->executeMoveList(const_cast<char*>(cmd.c_str() + m + 6));
    return 0;
}

int UCIHandler::go(const std::string& cmd) {
    // Reset flags
    game->startTime = getTimeMs();
    game->searchLimits.stopped = false;
    game->searchLimits.hardNodesLimit = ULLONG_MAX;
    game->searchLimits.timeToQuit = ULLONG_MAX;
    game->searchLimits.depthLimit = maxPly - 1;
    game->searchLimits.timeControl = false; //

    std::istringstream iss(cmd);
    std::string token;
    while (iss >> token) {
        if (token == "movetime") {
            U64 ms; iss >> ms;
            game->searchLimits.moveTimeSearch(getTimeMs() + ms);
        } else if (token == "wtime") {
            game->searchLimits.timeControlSearch();
            iss >> game->wtime;
        } else if (token == "btime") {
            game->searchLimits.timeControlSearch();
            iss >> game->btime;
        } else if (token == "winc") {
            game->searchLimits.timeControlSearch();
            iss >> game->winc;
        } else if (token == "binc") {
            game->searchLimits.timeControlSearch();
            iss >> game->binc;
        } else if (token == "depth") {
            Depth depth; iss >> depth;
            game->searchLimits.depthLimitedSearch(depth);
        } else if (token == "nodes") {
            U64 nodes; iss >> nodes;
            game->searchLimits.nodesSearch(nodes);
        }
    }
    return 0;
}

void UCIHandler::communicate() {
    // Process any commands that were queued during search
    std::lock_guard<std::mutex> lk(mtx);
    while (!commandQueue.empty()) {
        auto c = commandQueue.front(); commandQueue.pop();
        
        // just to be safe lol
        if (c == "stop") game->searchLimits.stopped = true;
        if (c == "quit") {
            running = false;
            game->searchLimits.stopped = true;
            break;
        }
        
        // Process other commands that were queued during search (excluding go commands)
        if (!(c.rfind("go", 0) == 0)) { 
            processCommand(c);
        }
    }
}