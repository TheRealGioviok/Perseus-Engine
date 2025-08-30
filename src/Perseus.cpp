#include "uci.h"
#include "tables.h"
#include "bench.h"
#include "evaluation.h"
#include "history.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

void split(std::string s, std::string delim, std::vector<std::string> & v){
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos) {
        token = s.substr(0, pos);
        v.push_back(token);
        s.erase(0, pos + delim.length());
    }
    v.push_back(s);
}

void perftSuite(){
    Game g;
    // Open the perft suite file
    std::ifstream perftFile("../perft.txt");
    std::vector<std::string> failed; // The perft not passed
    std::string line;
    int cnt = 0;
    U64 totNodes = 0;
    while (std::getline(perftFile, line)) {
        // Read the fen
        std::string fen = line.substr(0, line.find(","));
        // Read the perft results
        std::string perftResults = line.substr(line.find(",") + 1);
        // Parse the perft results
        std::vector<std::string> perftResultsVector;
        split(perftResults, ",", perftResultsVector);
        // Parse the fen
        g.parseFEN((char*)fen.c_str());
        // Run the perft tests (ONLY if target perft is less than 100000000)
        for (size_t i = 0; i < perftResultsVector.size(); i++) {
            if (i > 5) {
                break;
            }
            U64 target = U64(atoll(perftResultsVector[i].c_str()));
            if (target < 100000000LL) {
                std::cout << "\rPerft " << (int)cnt << " depth " << (int)i + 1 << ": " << (int)target << " nodes";
                U64 result = g.perft(i + 1);
                totNodes += result;
                if (result != target) {
                    std::cout << " Failed.";
                    failed.push_back(fen + "," + perftResultsVector[i]);
                }
            }
        }
        cnt++;
        std::cout << "\n\n" << std::endl;
        std::cout << "=== Summary ===\n";
        std::cout << "Passed : " << (cnt - failed.size()) << " / " << cnt<< "\n";
        std::cout << "Failed : " << failed.size() << " / " << cnt << "\n";
        std::cout << "Total nodes : " << totNodes << std::endl;
    }

    std::ofstream failedFile("failed.txt");
    for (size_t i = 0; i < failed.size(); i++) {
        failedFile << failed[i] << "\n";
    }
    failedFile.close();
    perftFile.close();
    std::cout << "Perft suite completed.\n";
}

int main(int argc, char* argv[]){
    initAll();
    //perftSuite();
    Game game;

    // Check if the first argument is "bench"
    if (argc > 1){
        if (std::string(argv[1]) == "bench") {
            // If the second argument is provided, parse it; otherwise, use the default value 12
            int depth = (argc > 2) ? std::atoi(argv[2]) : 12;
            benchmark(depth);
            return 0;
        }
        else if (std::string(argv[1]) == "extract") {
            // If the second argument is provided, parse it; otherwise, return error
            if (argc < 3) {
                std::cerr << "Usage " << argv[0] << " extract <filename>\n";
                return 1;
            }
            std::string inputFilename(argv[2]);
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
            convertToFeatures(inputFilename, outputFilename);
            return 0;
        }
    }
    uciLoop(&game);
    return 0;
}
