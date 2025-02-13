#pragma once

#include "BBmacros.h"
#include "Position.h"

const PScore pawnTable[2][64] = {
	{
		S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,
		S(194, 272)   ,S(153, 271)   ,S(193, 216)   ,S(201, 172)   ,S(180, 178)   ,S(135, 226)   ,S(-78, 321)   ,S(-48, 332)   ,
		S(97, 302)    ,S(124, 281)   ,S(146, 236)   ,S(140, 192)   ,S(154, 202)   ,S(210, 238)   ,S(155, 276)   ,S(111, 286)   ,
		S(94, 254)    ,S(96, 236)    ,S(103, 217)   ,S(119, 187)   ,S(130, 202)   ,S(138, 216)   ,S(100, 243)   ,S(96, 240)    ,
		S(87, 227)    ,S(83, 219)    ,S(99, 201)    ,S(107, 192)   ,S(112, 199)   ,S(116, 207)   ,S(100, 212)   ,S(97, 210)    ,
		S(89, 216)    ,S(87, 209)    ,S(94, 208)    ,S(106, 205)   ,S(115, 220)   ,S(107, 222)   ,S(120, 198)   ,S(109, 195)   ,
		S(86, 226)    ,S(84, 218)    ,S(83, 217)    ,S(93, 220)    ,S(92, 243)    ,S(129, 234)   ,S(140, 207)   ,S(110, 185)   ,
		S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,
	},
	{
		S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,
		S(135, 330)   ,S(112, 337)   ,S(194, 297)   ,S(195, 224)   ,S(188, 209)   ,S(194, 191)   ,S(85, 266)    ,S(74, 273)    ,
		S(122, 321)   ,S(178, 313)   ,S(176, 282)   ,S(157, 234)   ,S(163, 208)   ,S(159, 236)   ,S(150, 257)   ,S(92, 280)    ,
		S(130, 250)   ,S(123, 248)   ,S(129, 231)   ,S(133, 213)   ,S(129, 210)   ,S(112, 223)   ,S(80, 248)    ,S(85, 250)    ,
		S(125, 216)   ,S(115, 220)   ,S(125, 209)   ,S(121, 208)   ,S(110, 203)   ,S(93, 215)    ,S(71, 226)    ,S(68, 231)    ,
		S(126, 204)   ,S(137, 197)   ,S(122, 217)   ,S(111, 219)   ,S(99, 216)    ,S(90, 215)    ,S(75, 221)    ,S(66, 223)    ,
		S(129, 203)   ,S(152, 200)   ,S(139, 226)   ,S(103, 241)   ,S(81, 232)    ,S(77, 231)    ,S(69, 231)    ,S(68, 233)    ,
		S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,S(103, 205)   ,
	},
};
const PScore knightTable[2][64] = {
	{
		S(248, 585)   ,S(344, 661)   ,S(330, 707)   ,S(447, 674)   ,S(488, 668)   ,S(331, 701)   ,S(336, 672)   ,S(315, 550)   ,
		S(477, 630)   ,S(478, 656)   ,S(526, 656)   ,S(552, 692)   ,S(547, 692)   ,S(559, 646)   ,S(471, 661)   ,S(494, 628)   ,
		S(463, 642)   ,S(490, 669)   ,S(510, 709)   ,S(519, 712)   ,S(551, 705)   ,S(535, 710)   ,S(486, 688)   ,S(469, 665)   ,
		S(492, 666)   ,S(497, 677)   ,S(509, 710)   ,S(504, 738)   ,S(505, 749)   ,S(514, 731)   ,S(494, 722)   ,S(494, 699)   ,
		S(479, 669)   ,S(488, 681)   ,S(487, 709)   ,S(489, 721)   ,S(496, 733)   ,S(495, 719)   ,S(504, 701)   ,S(485, 697)   ,
		S(457, 622)   ,S(468, 658)   ,S(466, 683)   ,S(477, 711)   ,S(476, 710)   ,S(464, 683)   ,S(466, 673)   ,S(457, 652)   ,
		S(439, 643)   ,S(447, 658)   ,S(454, 654)   ,S(460, 677)   ,S(456, 680)   ,S(450, 653)   ,S(439, 665)   ,S(445, 673)   ,
		S(425, 600)   ,S(452, 619)   ,S(446, 633)   ,S(450, 668)   ,S(445, 671)   ,S(442, 663)   ,S(441, 644)   ,S(404, 632)   ,
	},
	{
		S(279, 544)   ,S(353, 645)   ,S(350, 694)   ,S(472, 661)   ,S(464, 658)   ,S(315, 699)   ,S(333, 661)   ,S(277, 558)   ,
		S(465, 626)   ,S(466, 662)   ,S(528, 657)   ,S(585, 673)   ,S(544, 682)   ,S(552, 650)   ,S(475, 652)   ,S(483, 619)   ,
		S(444, 664)   ,S(506, 674)   ,S(508, 714)   ,S(532, 712)   ,S(550, 699)   ,S(526, 701)   ,S(515, 655)   ,S(469, 642)   ,
		S(480, 680)   ,S(501, 694)   ,S(509, 721)   ,S(519, 731)   ,S(516, 731)   ,S(513, 708)   ,S(499, 693)   ,S(492, 665)   ,
		S(485, 682)   ,S(488, 694)   ,S(497, 710)   ,S(498, 722)   ,S(501, 722)   ,S(492, 709)   ,S(488, 684)   ,S(489, 663)   ,
		S(435, 654)   ,S(459, 669)   ,S(471, 684)   ,S(472, 708)   ,S(474, 705)   ,S(463, 680)   ,S(461, 651)   ,S(442, 624)   ,
		S(430, 680)   ,S(418, 680)   ,S(442, 659)   ,S(451, 673)   ,S(456, 675)   ,S(446, 655)   ,S(429, 663)   ,S(431, 638)   ,
		S(394, 651)   ,S(420, 651)   ,S(431, 662)   ,S(425, 670)   ,S(443, 658)   ,S(446, 625)   ,S(439, 625)   ,S(417, 588)   ,
	},
};
const PScore bishopTable[2][64] = {
	{
		S(428, 752)   ,S(409, 750)   ,S(354, 752)   ,S(351, 761)   ,S(371, 749)   ,S(335, 748)   ,S(430, 731)   ,S(428, 740)   ,
		S(428, 741)   ,S(488, 733)   ,S(487, 729)   ,S(456, 738)   ,S(483, 729)   ,S(482, 733)   ,S(447, 743)   ,S(446, 729)   ,
		S(488, 732)   ,S(491, 738)   ,S(517, 732)   ,S(497, 730)   ,S(513, 729)   ,S(505, 755)   ,S(491, 745)   ,S(480, 751)   ,
		S(475, 732)   ,S(510, 729)   ,S(494, 731)   ,S(516, 750)   ,S(518, 756)   ,S(513, 739)   ,S(516, 740)   ,S(467, 749)   ,
		S(483, 713)   ,S(490, 721)   ,S(483, 746)   ,S(503, 745)   ,S(510, 747)   ,S(485, 741)   ,S(492, 727)   ,S(493, 716)   ,
		S(487, 713)   ,S(500, 720)   ,S(489, 734)   ,S(492, 738)   ,S(488, 737)   ,S(491, 734)   ,S(493, 718)   ,S(497, 724)   ,
		S(499, 704)   ,S(495, 691)   ,S(493, 709)   ,S(472, 727)   ,S(472, 729)   ,S(489, 711)   ,S(504, 709)   ,S(494, 692)   ,
		S(505, 699)   ,S(491, 716)   ,S(472, 720)   ,S(465, 716)   ,S(460, 725)   ,S(467, 732)   ,S(469, 716)   ,S(492, 698)   ,
	},
	{
		S(397, 747)   ,S(371, 744)   ,S(389, 740)   ,S(361, 747)   ,S(361, 751)   ,S(337, 748)   ,S(446, 736)   ,S(405, 759)   ,
		S(404, 735)   ,S(453, 748)   ,S(476, 736)   ,S(456, 729)   ,S(468, 730)   ,S(496, 728)   ,S(488, 739)   ,S(454, 727)   ,
		S(475, 747)   ,S(507, 742)   ,S(509, 740)   ,S(519, 732)   ,S(517, 721)   ,S(519, 735)   ,S(511, 729)   ,S(497, 733)   ,
		S(474, 734)   ,S(516, 733)   ,S(505, 742)   ,S(523, 747)   ,S(534, 744)   ,S(504, 726)   ,S(514, 730)   ,S(491, 733)   ,
		S(490, 719)   ,S(480, 731)   ,S(498, 738)   ,S(512, 744)   ,S(510, 742)   ,S(495, 734)   ,S(495, 715)   ,S(478, 715)   ,
		S(489, 720)   ,S(499, 722)   ,S(501, 735)   ,S(503, 728)   ,S(495, 732)   ,S(495, 724)   ,S(495, 711)   ,S(478, 713)   ,
		S(487, 719)   ,S(512, 700)   ,S(494, 715)   ,S(478, 723)   ,S(475, 722)   ,S(479, 709)   ,S(482, 697)   ,S(493, 683)   ,
		S(501, 703)   ,S(469, 715)   ,S(467, 725)   ,S(463, 715)   ,S(464, 712)   ,S(469, 714)   ,S(483, 713)   ,S(496, 697)   ,
	},
};
const PScore rookTable[2][64] = {
	{
		S(659, 1311)  ,S(655, 1314)  ,S(625, 1324)  ,S(635, 1318)  ,S(630, 1325)  ,S(688, 1318)  ,S(666, 1323)  ,S(669, 1318)  ,
		S(639, 1303)  ,S(624, 1315)  ,S(648, 1308)  ,S(665, 1309)  ,S(648, 1317)  ,S(685, 1294)  ,S(639, 1308)  ,S(648, 1297)  ,
		S(630, 1288)  ,S(682, 1271)  ,S(658, 1285)  ,S(693, 1264)  ,S(709, 1261)  ,S(711, 1277)  ,S(729, 1258)  ,S(659, 1279)  ,
		S(630, 1276)  ,S(651, 1272)  ,S(661, 1273)  ,S(691, 1261)  ,S(678, 1272)  ,S(687, 1271)  ,S(673, 1276)  ,S(654, 1274)  ,
		S(606, 1259)  ,S(607, 1278)  ,S(611, 1272)  ,S(626, 1264)  ,S(617, 1271)  ,S(620, 1277)  ,S(642, 1272)  ,S(620, 1254)  ,
		S(603, 1235)  ,S(614, 1254)  ,S(604, 1248)  ,S(623, 1238)  ,S(619, 1242)  ,S(620, 1249)  ,S(649, 1239)  ,S(619, 1217)  ,
		S(585, 1231)  ,S(616, 1229)  ,S(615, 1237)  ,S(625, 1228)  ,S(623, 1229)  ,S(635, 1224)  ,S(643, 1215)  ,S(568, 1233)  ,
		S(624, 1230)  ,S(623, 1236)  ,S(627, 1237)  ,S(635, 1228)  ,S(634, 1230)  ,S(638, 1249)  ,S(632, 1237)  ,S(622, 1209)  ,
	},
	{
		S(669, 1306)  ,S(636, 1327)  ,S(660, 1321)  ,S(651, 1310)  ,S(657, 1306)  ,S(662, 1299)  ,S(689, 1289)  ,S(689, 1291)  ,
		S(614, 1309)  ,S(637, 1311)  ,S(670, 1302)  ,S(661, 1303)  ,S(663, 1298)  ,S(676, 1282)  ,S(640, 1295)  ,S(655, 1288)  ,
		S(625, 1294)  ,S(685, 1273)  ,S(676, 1288)  ,S(697, 1262)  ,S(713, 1248)  ,S(683, 1267)  ,S(712, 1249)  ,S(656, 1272)  ,
		S(615, 1282)  ,S(660, 1276)  ,S(677, 1273)  ,S(683, 1263)  ,S(681, 1260)  ,S(669, 1263)  ,S(672, 1257)  ,S(654, 1263)  ,
		S(585, 1249)  ,S(619, 1265)  ,S(613, 1276)  ,S(615, 1266)  ,S(618, 1260)  ,S(612, 1268)  ,S(613, 1270)  ,S(613, 1253)  ,
		S(575, 1223)  ,S(618, 1239)  ,S(620, 1241)  ,S(613, 1235)  ,S(613, 1236)  ,S(609, 1241)  ,S(629, 1241)  ,S(608, 1223)  ,
		S(480, 1253)  ,S(597, 1220)  ,S(616, 1231)  ,S(616, 1229)  ,S(619, 1223)  ,S(622, 1219)  ,S(625, 1215)  ,S(580, 1215)  ,
		S(576, 1233)  ,S(601, 1246)  ,S(631, 1236)  ,S(632, 1229)  ,S(631, 1222)  ,S(630, 1225)  ,S(635, 1221)  ,S(623, 1214)  ,
	},
};
const PScore queenTable[2][64] = {
	{
		S(1278, 1533) ,S(1316, 1519) ,S(1296, 1547) ,S(1299, 1554) ,S(1283, 1582) ,S(1319, 1573) ,S(1316, 1537) ,S(1291, 1527) ,
		S(1310, 1514) ,S(1255, 1559) ,S(1311, 1521) ,S(1254, 1591) ,S(1261, 1621) ,S(1328, 1608) ,S(1246, 1596) ,S(1319, 1546) ,
		S(1313, 1515) ,S(1316, 1514) ,S(1305, 1538) ,S(1319, 1549) ,S(1327, 1578) ,S(1324, 1592) ,S(1323, 1579) ,S(1300, 1603) ,
		S(1326, 1498) ,S(1326, 1536) ,S(1317, 1526) ,S(1302, 1592) ,S(1305, 1608) ,S(1343, 1590) ,S(1351, 1588) ,S(1347, 1558) ,
		S(1330, 1471) ,S(1329, 1517) ,S(1318, 1530) ,S(1313, 1578) ,S(1314, 1576) ,S(1331, 1562) ,S(1342, 1533) ,S(1350, 1516) ,
		S(1336, 1450) ,S(1341, 1475) ,S(1333, 1510) ,S(1331, 1503) ,S(1331, 1498) ,S(1336, 1518) ,S(1352, 1478) ,S(1348, 1452) ,
		S(1340, 1436) ,S(1343, 1440) ,S(1351, 1427) ,S(1345, 1463) ,S(1347, 1453) ,S(1365, 1390) ,S(1352, 1393) ,S(1320, 1443) ,
		S(1342, 1427) ,S(1344, 1429) ,S(1350, 1429) ,S(1356, 1457) ,S(1358, 1419) ,S(1332, 1413) ,S(1327, 1388) ,S(1325, 1406) ,
	},
	{
		S(1309, 1467) ,S(1315, 1499) ,S(1325, 1512) ,S(1305, 1559) ,S(1281, 1565) ,S(1317, 1521) ,S(1309, 1506) ,S(1306, 1495) ,
		S(1304, 1522) ,S(1243, 1570) ,S(1317, 1559) ,S(1248, 1613) ,S(1237, 1626) ,S(1316, 1516) ,S(1260, 1556) ,S(1327, 1497) ,
		S(1304, 1545) ,S(1315, 1543) ,S(1298, 1579) ,S(1328, 1561) ,S(1330, 1545) ,S(1315, 1541) ,S(1343, 1489) ,S(1340, 1503) ,
		S(1328, 1517) ,S(1341, 1566) ,S(1332, 1556) ,S(1320, 1591) ,S(1317, 1579) ,S(1332, 1513) ,S(1347, 1525) ,S(1348, 1504) ,
		S(1336, 1474) ,S(1335, 1530) ,S(1328, 1540) ,S(1322, 1569) ,S(1327, 1548) ,S(1327, 1518) ,S(1339, 1494) ,S(1337, 1468) ,
		S(1312, 1473) ,S(1340, 1467) ,S(1337, 1505) ,S(1335, 1485) ,S(1336, 1486) ,S(1339, 1488) ,S(1347, 1462) ,S(1342, 1429) ,
		S(1293, 1434) ,S(1328, 1403) ,S(1361, 1385) ,S(1353, 1436) ,S(1348, 1449) ,S(1357, 1405) ,S(1355, 1413) ,S(1350, 1403) ,
		S(1299, 1409) ,S(1325, 1384) ,S(1331, 1383) ,S(1348, 1398) ,S(1355, 1416) ,S(1358, 1392) ,S(1357, 1407) ,S(1350, 1388) ,
	},
};
const PScore kingTable[2][64] = {
	{
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(105, -44)   ,S(61, -36)    ,S(63, -31)    ,S(-29, -189)  ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(151, 14)    ,S(159, 30)    ,S(80, 63)     ,S(-5, -31)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(170, 41)    ,S(229, 48)    ,S(177, 55)    ,S(40, 8)      ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(120, 63)    ,S(144, 55)    ,S(116, 37)    ,S(-49, 14)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(74, 64)     ,S(79, 43)     ,S(78, 13)     ,S(-63, -16)   ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(23, 52)     ,S(25, 29)     ,S(18, 0)      ,S(-47, -18)   ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-67, 40)    ,S(-53, 35)    ,S(-5, -3)     ,S(1, -44)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-70, -28)   ,S(-97, 8)     ,S(-5, -38)    ,S(4, -127)    ,
	},
	{
		S(-20, -196)  ,S(42, -74)    ,S(83, -65)    ,S(102, -37)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-38, -45)   ,S(90, 38)     ,S(145, 17)    ,S(191, -13)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(4, 12)      ,S(175, 42)    ,S(201, 37)    ,S(176, 29)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-42, 12)    ,S(128, 21)    ,S(150, 44)    ,S(120, 50)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-54, -30)   ,S(71, 4)      ,S(113, 26)    ,S(72, 52)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-73, -18)   ,S(9, -9)      ,S(36, 12)     ,S(16, 36)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-12, -36)   ,S(-15, -16)   ,S(-20, 10)    ,S(-70, 23)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-24, -110)  ,S(-11, -54)   ,S(-50, -13)   ,S(-94, -27)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
	},
};

constexpr PScore materialValues[5] = {S(103, 205), S(436, 622), S(449, 666), S(606, 1192), S(1310, 1456), };
constexpr PScore knightMob[9] = {S(-25, -129), S(-6, -51), S(5, -3), S(12, 23), S(18, 33), S(23, 47), S(30, 47), S(41, 41), S(52, 15), };
constexpr PScore bishopMob[14] = {S(-45, -182), S(-14, -105), S(4, -50), S(10, -22), S(21, -1), S(27, 17), S(30, 28), S(27, 37), S(29, 48), S(31, 48), S(40, 42), S(63, 26), S(67, 34), S(92, -11), };
constexpr PScore rookMob[15] = {S(-19, -140), S(-25, -89), S(-16, -55), S(-14, -39), S(-14, -22), S(-16, -3), S(-13, 7), S(-8, 7), S(-4, 11), S(5, 16), S(5, 22), S(10, 26), S(12, 27), S(30, 7), S(96, -29), };
constexpr PScore queenMob[28] = {S(-127, -456), S(-85, -471), S(-46, -385), S(-29, -291), S(-25, -194), S(-23, -136), S(-19, -102), S(-15, -82), S(-14, -64), S(-9, -51), S(-4, -43), S(1, -38), S(4, -35), S(8, -33), S(11, -33), S(11, -34), S(11, -37), S(8, -37), S(12, -44), S(17, -55), S(31, -81), S(38, -101), S(37, -119), S(52, -145), S(31, -152), S(33, -176), S(-14, -136), S(-10, -117), };

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };

//constexpr Score KSAMG = -2;
constexpr double KSCALEMG = 927.0235595703125;
constexpr double KSBMG = 1; // 1.6044918298721313;
constexpr double KSCMG = -3.219916343688965;
// constexpr Score KSAEG = -71;
constexpr double KSCALEEG = 1044.626953125;
constexpr double KSBEG = 1; // 2.070645570755005;
constexpr double KSCEG = 2.8153092861175537;

#define PAWNHASHSIZE 262144ULL

struct PawnEvalHashEntry {
    U64 hash;
    PScore score;
    BitBoard passers; // This can be shrinked to add more stuff and keep the alignement
};

extern PawnEvalHashEntry pawnEvalHash[PAWNHASHSIZE];

extern PScore PSQTs[2][12][64];
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