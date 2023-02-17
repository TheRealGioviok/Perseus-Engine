#pragma once

#pragma once
#include "Game.h"
#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#include <io.h>
#endif
#include <cstring>
#include <chrono>

extern bool quit;
extern int movesToGo; // UCI parameter for the moves until the next time control.
extern int moveTime;  // UCI parameter for the time per move.
extern int increment; // UCI parameter for the time increment.
extern U64 startTime; // The time when the engine started calculating.
extern U64 stopTime;  // The time when the engine must stop calculating.
extern bool timeIsUp; // True if the time is up.
extern bool ponder;   // True if the engine is pondering.
extern U8 timeSet; // 0 for infinite, 1 for time per move, 2 for depth, 3 for normal play.
extern std::string hashDumpFile; // Additional opening hash file

/**
 * @brief The getTimeMs function returns the current time in milliseconds.
 * @return The current time in milliseconds.
 */
U64 getTime64();

/**
 * @brief The uciLoop function is the main function of the engine.
 * It is called by the main function of the engine.
 * It is responsible for interfacing the engine with the GUI.
 * @param game The game object.
 * @note This function writes to the standard output, which is then redirected to the GUI.
 */
void uciLoop(Game *game);

/**
 * @brief The executeCommand function reads a string and executes the command(s) contained in the string.
 * @param game The game object.
 * @param command The command string.
 * @return The return value is the return value of the command.
 */
int executeCommand(Game *game, char *command);

/**
 * @brief The positionCommand function handles the position command.
 * @param game The game object.
 * @param command The command string.
 * @return The return value is the return value of the command.
 */
int positionCommand(Game *game, char *command);

/**
 * @brief The goCommand function handles the go command.
 * @param game The game object.
 * @param command The command string.
 * @return The return value is the return value of the command.
 * @note This function is responsible for the main search loop.
 */
int goCommand(Game *game, char *command);

/**
 * @brief The setOptionCommand function handles the setOption command
 * @param game The game object.
 * @param command The command string.
 * @return The return value is the return value of the command.
 */
int setOptionCommand(Game* game, char* command);

/**
 * @brief The inputWaiting function waits for input from the GUI. The code is based on the code from Code Monkey King's BBC chess engine which is based on the code from VICE by BlueFeverSoftware.
 * @return The return value is the length of the input string (0 means no input).
 * @note This function is responsible for async communication with the GUI.
 */
int inputWaiting();

/**
 * @brief The readInput function reads the input from the GUI. The code is based on the code from Code Monkey King's BBC chess engine which is based on the code from VICE by BlueFeverSoftware.
 * It is responsible for async communication with the GUI during the main search loop. It is not responsible for command execution (except for the stop command).
 * @param game
 */
void readInput(Game *game);

/**
 * @brief The UCICommunicate function is responsible for mid-search communication with the GUI.
 * @param game The game object.
 */
void UCICommunicate(Game *game);

/**
 * @brief The execCommand function directly calls a method. It assumes no context, unless specified, for example, "execCommand game.print()" is equivalent to the print method.
 * @param game The game object.
 * @param command The command string.
 */
void execCommand(Game *game, char *command);
