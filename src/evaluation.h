#pragma once

#include "BBmacros.h"
#include "Position.h"

const PScore materialValues[5] = {
    S(94,200), S(406,595), S(418,637), S(580,1167), S(1253,1398)
};

const PScore pawnTable[64] = {
    S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     ,
    S(90,100)  , S(51,101)  , S(87,57)   , S(103,0)   , S(88,1)    , S(49,29)   , S(-117,114), S(-85,117) ,
    S(1,107)   , S(31,92)   , S(48,50)   , S(44,2)    , S(59,0)    , S(98,34)   , S(53,67)   , S(5,80)    ,
    S(0,55)    , S(1,43)    , S(7,23)    , S(22,-6)   , S(31,2)    , S(30,14)   , S(-5,41)   , S(-6,40)   ,
    S(-6,27)   , S(-12,22)  , S(2,4)     , S(11,-5)   , S(15,-4)   , S(12,6)    , S(-7,14)   , S(-8,14)   ,
    S(-3,16)   , S(-6,9)    , S(-2,9)    , S(6,4)     , S(15,14)   , S(0,17)    , S(6,1)     , S(-2,1)    ,
    S(-6,24)   , S(-7,16)   , S(-9,24)   , S(-7,24)   , S(-8,33)   , S(12,33)   , S(14,13)   , S(-8,0)    ,
    S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     , S(0,0)     
};

const PScore knightTable[64] = {
    S(-161,-25), S(-76,57)  , S(-93,105) , S(19,75)   , S(58,69)   , S(-84,108) , S(-88,73)  , S(-115,-45),
    S(48,28)   , S(45,59)   , S(97,58)   , S(125,88)  , S(108,91)  , S(131,49)  , S(34,59)   , S(60,31)   ,
    S(29,50)   , S(64,71)   , S(78,111)  , S(87,111)  , S(116,103) , S(92,111)  , S(58,81)   , S(39,58)   ,
    S(62,68)   , S(67,83)   , S(78,113)  , S(75,133)  , S(76,142)  , S(80,122)  , S(62,112)  , S(63,87)   ,
    S(51,72)   , S(58,83)   , S(58,109)  , S(60,118)  , S(68,125)  , S(65,112)  , S(71,91)   , S(56,84)   ,
    S(25,29)   , S(38,60)   , S(37,82)   , S(47,106)  , S(47,104)  , S(35,79)   , S(36,63)   , S(27,38)   ,
    S(8,58)    , S(5,64)    , S(20,52)   , S(32,72)   , S(29,74)   , S(20,49)   , S(8,59)    , S(16,60)   ,
    S(-15,20)  , S(22,21)   , S(13,38)   , S(13,62)   , S(16,62)   , S(14,50)   , S(12,34)   , S(-16,10)  
};

const PScore bishopTable[64] = {
    S(-12,106) , S(-36,105) , S(-81,106) , S(-91,114) , S(-74,107) , S(-104,105), S(-2,92)   , S(-24,105) ,
    S(-14,97)  , S(42,92)   , S(43,87)   , S(15,90)   , S(39,83)   , S(37,89)   , S(14,100)  , S(3,92)    ,
    S(45,94)   , S(55,94)   , S(74,89)   , S(55,85)   , S(69,83)   , S(65,104)  , S(53,95)   , S(41,101)  ,
    S(35,89)   , S(69,85)   , S(53,89)   , S(74,103)  , S(78,105)  , S(67,88)   , S(73,91)   , S(29,99)   ,
    S(43,71)   , S(45,80)   , S(43,99)   , S(63,97)   , S(68,98)   , S(43,92)   , S(50,76)   , S(47,68)   ,
    S(46,70)   , S(56,77)   , S(49,89)   , S(52,88)   , S(46,88)   , S(49,83)   , S(51,70)   , S(51,73)   ,
    S(55,63)   , S(54,50)   , S(50,65)   , S(30,79)   , S(30,81)   , S(43,63)   , S(59,58)   , S(49,42)   ,
    S(62,59)   , S(46,73)   , S(29,74)   , S(21,68)   , S(17,75)   , S(27,78)   , S(37,73)   , S(52,55)   
};

const PScore rookTable[64] = {
    S(53,110), S(42,119), S(12,131), S(20,124), S(18,129), S(51,123), S(66,117), S(74,111),
    S(28,108), S(13,120), S(38,115), S(51,115), S(39,121), S(62,100), S(27,113), S(42,104),
    S(20,98) , S(73,79) , S(49,94) , S(83,72) , S(98,68) , S(84,85) , S(109,67), S(47,88) ,
    S(20,87) , S(43,83) , S(52,84) , S(78,72) , S(66,79) , S(69,79) , S(62,82) , S(45,82) ,
    S(-6,67) , S(1,83)  , S(1,83)  , S(14,75) , S(6,79)  , S(7,84)  , S(24,81) , S(9,64)  ,
    S(-12,44), S(7,60)  , S(-2,58) , S(11,50) , S(7,53)  , S(8,57)  , S(35,50) , S(7,34)  ,
    S(-38,46), S(3,38)  , S(4,46)  , S(13,41) , S(10,40) , S(22,32) , S(27,27) , S(-35,41),
    S(9,41)  , S(12,48) , S(17,50) , S(26,40) , S(23,41) , S(27,54) , S(28,42) , S(17,23) 
};

const PScore queenTable[64] = {
    S(-18,56) , S(1,62)   , S(-21,102), S(-21,114), S(-37,141), S(-13,132), S(-6,91)  , S(-7,68)  ,
    S(-1,56)  , S(-62,105), S(-8,81)  , S(-74,149), S(-75,184), S(-10,146), S(-77,144), S(4,78)   ,
    S(1,65)   , S(6,59)   , S(-10,88) , S(-1,96)  , S(7,118)  , S(-2,135) , S(8,113)  , S(-4,128) ,
    S(14,52)  , S(14,91)  , S(5,80)   , S(-12,140), S(-12,152), S(17,134) , S(31,126) , S(30,95)  ,
    S(17,13)  , S(16,62)  , S(4,78)   , S(-1,122) , S(1,111)  , S(14,96)  , S(27,66)  , S(28,46)  ,
    S(19,-5)  , S(28,21)  , S(20,57)  , S(15,48)  , S(19,39)  , S(21,57)  , S(36,21)  , S(34,-14) ,
    S(21,-16) , S(30,-16) , S(38,-29) , S(33,7)   , S(34,-1)  , S(49,-58) , S(44,-59) , S(21,-27) ,
    S(23,-25) , S(31,-31) , S(35,-31) , S(41,0)   , S(42,-35) , S(32,-56) , S(41,-58) , S(25,-49) 
};

const PScore kingTable[64] = {
    S(-21,-204), S(34,-68)  , S(65,-53)  , S(83,-27)  , S(84,-47)  , S(63,-34)  , S(58,-35)  , S(-17,-200),
    S(-60,-32) , S(80,48)   , S(125,27)  , S(179,-7)  , S(149,3)   , S(160,25)  , S(85,60)   , S(-5,-34)  ,
    S(-4,18)   , S(163,50)  , S(188,43)  , S(159,34)  , S(169,29)  , S(236,39)  , S(173,52)  , S(39,0)    ,
    S(-38,16)  , S(118,32)  , S(141,49)  , S(110,53)  , S(127,46)  , S(146,44)  , S(114,30)  , S(-50,3)   ,
    S(-54,-20) , S(68,14)   , S(115,33)  , S(65,58)   , S(83,48)   , S(90,28)   , S(75,5)    , S(-63,-23) ,
    S(-71,-7)  , S(5,7)     , S(33,22)   , S(13,46)   , S(29,38)   , S(33,17)   , S(11,-2)   , S(-47,-22) ,
    S(-5,-23)  , S(-18,3)   , S(-29,22)  , S(-85,35)  , S(-64,32)  , S(-52,28)  , S(-7,-5)   , S(-2,-46)  ,
    S(-21,-102), S(-2,-43)  , S(-43,-2)  , S(-110,-13), S(-69,-35) , S(-98,3)   , S(-6,-42)  , S(2,-133)  
};

const PScore knightMob[9] = {
    S(-27,-117), S(-9,-43), S(4,3), S(10,28), S(16,38), S(21,53), S(29,51), S(38,45), S(48,21)
};

const PScore bishopMob[14] = {
    S(-45,-179), S(-14,-102), S(2,-47), S(10,-19), S(20,2), S(25,21), S(29,33), S(27,42), S(26,51), S(29,54), S(38,49), S(60,34), S(65,40), S(87,-1)
};

const PScore rookMob[15] = {
    S(-22,-148), S(-26,-89), S(-16,-53), S(-17,-36), S(-16,-19), S(-18,1), S(-15,10), S(-10,10), S(-6,15), S(2,19), S(2,26), S(6,31), S(9,33), S(26,12), S(88,-20)
};

const PScore queenMob[28] = {
    S(-134,-452), S(-92,-474), S(-54,-370), S(-35,-278), S(-31,-181), S(-25,-124), S(-20,-92), S(-17,-75), S(-14,-55), S(-8,-43), S(-4,-37), S(1,-34), S(5,-32), S(9,-29), S(10,-30), S(11,-32), S(11,-33), S(6,-29), S(9,-36), S(11,-44), S(24,-68), S(29,-86), S(28,-98), S(42,-121), S(17,-124), S(27,-153), S(0,-120), S(-17,-84)
};

extern const PScore* pestoTables[6];

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };
extern PScore PSQTs[12][64];
/**
* @brief The initTables function initializes the material - positional tables
* @note This function is called once at the beginning of the program
*/
void initTables();

/**
 * @brief The pestoEval function evaluates the material and positional scores
 * @param pos The position to evaluate
 * @return The material and positional scores
 */
Score pestoEval(Position* pos);

/** 
 * @brief The initTropism function initializes the tropism tables
 * @note This is called at the start of the program 
 */
void initTropism();

/**
 * @brief The convertToFeatures function converts a set of positions to a set of features
 * @param filename The filename of the epd file
 * @param output The output file to write the features to
 */
void convertToFeatures(std::string filename, std::string output);