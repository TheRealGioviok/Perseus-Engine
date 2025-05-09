#pragma once
#include "Game.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <cstdint>

inline uint64_t getTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

class UCIHandler {
public:
    explicit UCIHandler(Game* game);
    ~UCIHandler();
    void start();
    void stop();
private:
    void ioLoop();
    void processCommand(const std::string& cmd);
    void uciStr() const;
    int setOption(const std::string& cmd);
    int position(const std::string& cmd);
    int go(const std::string& cmd);
    void communicate();
    // State
    Game* game;
    std::atomic<bool> running;
    std::thread searchThread;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> commandQueue;
};