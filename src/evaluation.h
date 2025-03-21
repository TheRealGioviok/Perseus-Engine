#pragma once

#include "BBmacros.h"
#include "Position.h"

const PScore pawnTable[2][64] = {
	{
		S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,
		S(195, 262)   ,S(150, 262)   ,S(194, 206)   ,S(198, 166)   ,S(172, 176)   ,S(116, 225)   ,S(-90, 317)   ,S(-52, 324)   ,
		S(96, 299)    ,S(122, 277)   ,S(144, 232)   ,S(135, 192)   ,S(146, 202)   ,S(196, 239)   ,S(143, 277)   ,S(106, 284)   ,
		S(92, 250)    ,S(95, 233)    ,S(102, 214)   ,S(115, 186)   ,S(124, 204)   ,S(129, 217)   ,S(93, 242)    ,S(92, 238)    ,
		S(85, 223)    ,S(82, 216)    ,S(97, 198)    ,S(104, 192)   ,S(108, 199)   ,S(111, 208)   ,S(93, 212)    ,S(93, 208)    ,
		S(88, 213)    ,S(86, 207)    ,S(93, 206)    ,S(100, 204)   ,S(113, 218)   ,S(104, 221)   ,S(115, 198)   ,S(105, 194)   ,
		S(85, 223)    ,S(83, 213)    ,S(82, 214)    ,S(91, 217)    ,S(91, 237)    ,S(128, 230)   ,S(137, 203)   ,S(107, 182)   ,
		S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,
	},
	{
		S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,
		S(127, 323)   ,S(96, 332)    ,S(167, 297)   ,S(181, 221)   ,S(178, 205)   ,S(198, 181)   ,S(86, 257)    ,S(77, 262)    ,
		S(114, 320)   ,S(165, 314)   ,S(161, 283)   ,S(148, 235)   ,S(157, 207)   ,S(156, 234)   ,S(147, 254)   ,S(90, 277)    ,
		S(123, 249)   ,S(116, 247)   ,S(121, 231)   ,S(126, 213)   ,S(125, 208)   ,S(111, 220)   ,S(79, 245)    ,S(83, 246)    ,
		S(120, 214)   ,S(106, 219)   ,S(118, 207)   ,S(116, 207)   ,S(107, 201)   ,S(92, 214)    ,S(71, 223)    ,S(67, 228)    ,
		S(122, 202)   ,S(132, 197)   ,S(118, 216)   ,S(108, 218)   ,S(96, 214)    ,S(90, 214)    ,S(73, 218)    ,S(65, 219)    ,
		S(126, 199)   ,S(148, 197)   ,S(136, 223)   ,S(102, 235)   ,S(80, 226)    ,S(77, 225)    ,S(69, 226)    ,S(67, 227)    ,
		S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,S(101, 206)   ,
	},
};
const PScore knightTable[2][64] = {
	{
		S(261, 569)   ,S(350, 647)   ,S(326, 696)   ,S(438, 666)   ,S(484, 661)   ,S(345, 697)   ,S(348, 662)   ,S(326, 540)   ,
		S(464, 622)   ,S(468, 645)   ,S(501, 652)   ,S(524, 686)   ,S(531, 685)   ,S(538, 642)   ,S(460, 651)   ,S(485, 621)   ,
		S(457, 633)   ,S(478, 660)   ,S(498, 702)   ,S(501, 706)   ,S(518, 702)   ,S(505, 707)   ,S(457, 683)   ,S(462, 654)   ,
		S(494, 653)   ,S(498, 667)   ,S(507, 702)   ,S(501, 727)   ,S(501, 740)   ,S(507, 717)   ,S(491, 706)   ,S(491, 683)   ,
		S(483, 658)   ,S(493, 670)   ,S(488, 702)   ,S(487, 714)   ,S(498, 723)   ,S(495, 714)   ,S(508, 690)   ,S(485, 689)   ,
		S(459, 611)   ,S(468, 646)   ,S(462, 676)   ,S(472, 705)   ,S(471, 702)   ,S(457, 680)   ,S(458, 667)   ,S(451, 643)   ,
		S(441, 629)   ,S(448, 646)   ,S(452, 643)   ,S(458, 668)   ,S(454, 669)   ,S(444, 646)   ,S(435, 656)   ,S(441, 660)   ,
		S(429, 587)   ,S(453, 606)   ,S(447, 620)   ,S(449, 655)   ,S(446, 658)   ,S(442, 650)   ,S(440, 631)   ,S(405, 618)   ,
	},
	{
		S(288, 533)   ,S(364, 629)   ,S(361, 681)   ,S(462, 653)   ,S(465, 649)   ,S(314, 688)   ,S(334, 653)   ,S(292, 545)   ,
		S(460, 617)   ,S(451, 653)   ,S(501, 655)   ,S(561, 668)   ,S(536, 670)   ,S(527, 645)   ,S(468, 639)   ,S(475, 610)   ,
		S(434, 655)   ,S(480, 670)   ,S(486, 711)   ,S(510, 708)   ,S(520, 698)   ,S(514, 694)   ,S(499, 648)   ,S(466, 631)   ,
		S(476, 669)   ,S(498, 682)   ,S(502, 713)   ,S(512, 722)   ,S(511, 720)   ,S(512, 696)   ,S(499, 679)   ,S(494, 651)   ,
		S(485, 672)   ,S(490, 684)   ,S(495, 705)   ,S(498, 713)   ,S(501, 711)   ,S(495, 700)   ,S(497, 674)   ,S(492, 654)   ,
		S(428, 646)   ,S(449, 662)   ,S(461, 680)   ,S(465, 699)   ,S(467, 698)   ,S(459, 674)   ,S(460, 641)   ,S(444, 613)   ,
		S(428, 667)   ,S(415, 669)   ,S(436, 653)   ,S(448, 663)   ,S(455, 665)   ,S(445, 645)   ,S(430, 650)   ,S(432, 624)   ,
		S(393, 636)   ,S(422, 636)   ,S(432, 648)   ,S(426, 656)   ,S(444, 645)   ,S(447, 614)   ,S(442, 613)   ,S(419, 577)   ,
	},
};
const PScore bishopTable[2][64] = {
	{
		S(433, 742)   ,S(409, 744)   ,S(353, 744)   ,S(343, 754)   ,S(367, 738)   ,S(338, 737)   ,S(442, 723)   ,S(434, 730)   ,
		S(427, 730)   ,S(466, 732)   ,S(479, 721)   ,S(442, 733)   ,S(454, 728)   ,S(471, 724)   ,S(431, 739)   ,S(441, 721)   ,
		S(487, 719)   ,S(492, 726)   ,S(512, 724)   ,S(493, 723)   ,S(503, 724)   ,S(490, 751)   ,S(475, 739)   ,S(473, 742)   ,
		S(478, 719)   ,S(516, 716)   ,S(498, 719)   ,S(519, 741)   ,S(520, 749)   ,S(505, 734)   ,S(514, 734)   ,S(466, 737)   ,
		S(491, 701)   ,S(500, 709)   ,S(494, 734)   ,S(509, 735)   ,S(516, 739)   ,S(491, 735)   ,S(494, 721)   ,S(496, 707)   ,
		S(493, 700)   ,S(506, 709)   ,S(493, 723)   ,S(493, 730)   ,S(489, 728)   ,S(489, 726)   ,S(492, 712)   ,S(494, 717)   ,
		S(503, 693)   ,S(500, 680)   ,S(493, 699)   ,S(473, 716)   ,S(474, 718)   ,S(490, 700)   ,S(504, 698)   ,S(491, 683)   ,
		S(510, 690)   ,S(493, 707)   ,S(473, 711)   ,S(466, 705)   ,S(464, 713)   ,S(468, 718)   ,S(469, 705)   ,S(493, 689)   ,
	},
	{
		S(404, 738)   ,S(372, 735)   ,S(384, 731)   ,S(360, 738)   ,S(350, 743)   ,S(327, 743)   ,S(444, 729)   ,S(411, 746)   ,
		S(399, 727)   ,S(439, 741)   ,S(465, 728)   ,S(438, 725)   ,S(445, 728)   ,S(483, 722)   ,S(475, 732)   ,S(451, 716)   ,
		S(472, 737)   ,S(493, 736)   ,S(496, 734)   ,S(508, 727)   ,S(511, 715)   ,S(516, 726)   ,S(505, 721)   ,S(494, 722)   ,
		S(468, 726)   ,S(512, 726)   ,S(498, 736)   ,S(518, 741)   ,S(534, 735)   ,S(510, 714)   ,S(516, 720)   ,S(494, 719)   ,
		S(490, 710)   ,S(479, 723)   ,S(502, 731)   ,S(517, 736)   ,S(514, 733)   ,S(506, 722)   ,S(507, 703)   ,S(484, 704)   ,
		S(485, 714)   ,S(495, 715)   ,S(498, 728)   ,S(503, 719)   ,S(495, 722)   ,S(498, 715)   ,S(501, 701)   ,S(485, 700)   ,
		S(486, 708)   ,S(511, 689)   ,S(493, 703)   ,S(478, 712)   ,S(476, 710)   ,S(482, 698)   ,S(487, 684)   ,S(498, 674)   ,
		S(501, 692)   ,S(470, 704)   ,S(471, 712)   ,S(466, 702)   ,S(466, 702)   ,S(472, 705)   ,S(485, 703)   ,S(500, 686)   ,
	},
};
const PScore rookTable[2][64] = {
	{
		S(663, 1296)  ,S(657, 1299)  ,S(624, 1312)  ,S(637, 1305)  ,S(608, 1322)  ,S(671, 1313)  ,S(674, 1310)  ,S(667, 1307)  ,
		S(641, 1286)  ,S(627, 1297)  ,S(650, 1293)  ,S(667, 1294)  ,S(635, 1310)  ,S(676, 1284)  ,S(644, 1295)  ,S(644, 1284)  ,
		S(629, 1274)  ,S(683, 1255)  ,S(657, 1271)  ,S(693, 1252)  ,S(696, 1255)  ,S(693, 1270)  ,S(718, 1249)  ,S(648, 1271)  ,
		S(628, 1261)  ,S(649, 1258)  ,S(657, 1261)  ,S(686, 1251)  ,S(665, 1266)  ,S(673, 1263)  ,S(660, 1268)  ,S(641, 1266)  ,
		S(605, 1246)  ,S(610, 1264)  ,S(610, 1261)  ,S(624, 1252)  ,S(612, 1265)  ,S(618, 1270)  ,S(634, 1267)  ,S(610, 1249)  ,
		S(602, 1218)  ,S(614, 1235)  ,S(604, 1232)  ,S(624, 1220)  ,S(616, 1230)  ,S(618, 1235)  ,S(644, 1225)  ,S(612, 1204)  ,
		S(584, 1214)  ,S(616, 1212)  ,S(614, 1219)  ,S(625, 1212)  ,S(620, 1216)  ,S(634, 1209)  ,S(643, 1198)  ,S(565, 1217)  ,
		S(622, 1210)  ,S(624, 1217)  ,S(627, 1219)  ,S(634, 1210)  ,S(630, 1214)  ,S(636, 1230)  ,S(629, 1221)  ,S(618, 1193)  ,
	},
	{
		S(663, 1296)  ,S(631, 1317)  ,S(644, 1317)  ,S(627, 1309)  ,S(656, 1296)  ,S(657, 1290)  ,S(692, 1278)  ,S(694, 1280)  ,
		S(604, 1298)  ,S(635, 1299)  ,S(663, 1292)  ,S(649, 1293)  ,S(665, 1284)  ,S(676, 1269)  ,S(645, 1280)  ,S(657, 1273)  ,
		S(613, 1285)  ,S(676, 1263)  ,S(661, 1281)  ,S(687, 1254)  ,S(712, 1238)  ,S(680, 1254)  ,S(712, 1236)  ,S(655, 1260)  ,
		S(601, 1273)  ,S(646, 1266)  ,S(659, 1268)  ,S(666, 1258)  ,S(675, 1249)  ,S(663, 1251)  ,S(666, 1244)  ,S(650, 1250)  ,
		S(573, 1243)  ,S(610, 1260)  ,S(600, 1272)  ,S(609, 1261)  ,S(615, 1251)  ,S(610, 1259)  ,S(617, 1257)  ,S(609, 1241)  ,
		S(569, 1208)  ,S(615, 1223)  ,S(617, 1226)  ,S(610, 1220)  ,S(614, 1219)  ,S(611, 1225)  ,S(630, 1222)  ,S(607, 1207)  ,
		S(481, 1232)  ,S(598, 1202)  ,S(614, 1212)  ,S(613, 1212)  ,S(619, 1205)  ,S(621, 1204)  ,S(624, 1197)  ,S(579, 1199)  ,
		S(572, 1217)  ,S(599, 1227)  ,S(629, 1219)  ,S(630, 1210)  ,S(631, 1204)  ,S(629, 1208)  ,S(634, 1202)  ,S(622, 1193)  ,
	},
};
const PScore queenTable[2][64] = {
	{
		S(1276, 1558) ,S(1310, 1551) ,S(1286, 1585) ,S(1295, 1592) ,S(1273, 1615) ,S(1318, 1605) ,S(1331, 1584) ,S(1311, 1567) ,
		S(1297, 1542) ,S(1243, 1590) ,S(1296, 1560) ,S(1240, 1621) ,S(1229, 1659) ,S(1321, 1606) ,S(1249, 1635) ,S(1307, 1582) ,
		S(1297, 1543) ,S(1303, 1539) ,S(1290, 1562) ,S(1299, 1574) ,S(1298, 1592) ,S(1287, 1622) ,S(1280, 1622) ,S(1274, 1641) ,
		S(1308, 1526) ,S(1306, 1565) ,S(1299, 1551) ,S(1276, 1623) ,S(1269, 1643) ,S(1298, 1635) ,S(1306, 1634) ,S(1310, 1600) ,
		S(1314, 1505) ,S(1317, 1547) ,S(1302, 1565) ,S(1291, 1622) ,S(1287, 1620) ,S(1301, 1607) ,S(1309, 1581) ,S(1317, 1563) ,
		S(1320, 1483) ,S(1326, 1512) ,S(1315, 1545) ,S(1307, 1546) ,S(1305, 1537) ,S(1307, 1565) ,S(1322, 1527) ,S(1318, 1502) ,
		S(1323, 1472) ,S(1325, 1478) ,S(1330, 1468) ,S(1324, 1503) ,S(1323, 1491) ,S(1342, 1432) ,S(1329, 1445) ,S(1295, 1487) ,
		S(1324, 1462) ,S(1323, 1468) ,S(1328, 1467) ,S(1335, 1492) ,S(1334, 1457) ,S(1311, 1449) ,S(1306, 1437) ,S(1304, 1442) ,
	},
	{
		S(1304, 1507) ,S(1315, 1543) ,S(1302, 1568) ,S(1292, 1595) ,S(1288, 1594) ,S(1316, 1547) ,S(1305, 1537) ,S(1304, 1529) ,
		S(1271, 1550) ,S(1223, 1604) ,S(1287, 1592) ,S(1220, 1645) ,S(1219, 1659) ,S(1302, 1541) ,S(1253, 1582) ,S(1316, 1521) ,
		S(1270, 1581) ,S(1260, 1585) ,S(1256, 1607) ,S(1292, 1587) ,S(1303, 1566) ,S(1299, 1559) ,S(1322, 1517) ,S(1322, 1533) ,
		S(1284, 1554) ,S(1289, 1606) ,S(1278, 1599) ,S(1276, 1621) ,S(1285, 1605) ,S(1305, 1537) ,S(1319, 1553) ,S(1325, 1528) ,
		S(1298, 1517) ,S(1295, 1572) ,S(1292, 1574) ,S(1291, 1604) ,S(1299, 1587) ,S(1305, 1553) ,S(1323, 1526) ,S(1313, 1506) ,
		S(1279, 1507) ,S(1306, 1503) ,S(1303, 1542) ,S(1306, 1519) ,S(1310, 1521) ,S(1315, 1526) ,S(1326, 1501) ,S(1319, 1467) ,
		S(1265, 1469) ,S(1300, 1435) ,S(1332, 1415) ,S(1326, 1468) ,S(1322, 1483) ,S(1332, 1442) ,S(1332, 1450) ,S(1328, 1440) ,
		S(1273, 1439) ,S(1298, 1416) ,S(1309, 1406) ,S(1323, 1429) ,S(1330, 1447) ,S(1332, 1426) ,S(1331, 1440) ,S(1325, 1427) ,
	},
};
const PScore kingTable[2][64] = {
	{
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(99, -46)    ,S(69, -35)    ,S(74, -41)    ,S(0, -184)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(144, 19)    ,S(161, 32)    ,S(75, 61)     ,S(10, -18)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(151, 52)    ,S(218, 55)    ,S(159, 60)    ,S(58, 23)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(81, 80)     ,S(111, 70)    ,S(84, 50)     ,S(-35, 31)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(26, 78)     ,S(39, 58)     ,S(33, 26)     ,S(-34, 0)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-17, 60)    ,S(-8, 39)     ,S(-18, 9)     ,S(-5, -6)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-90, 34)    ,S(-70, 32)    ,S(-18, -11)   ,S(48, -42)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-75, -42)   ,S(-95, -3)    ,S(-7, -52)    ,S(44, -124)   ,
	},
	{
		S(-2, -205)   ,S(69, -81)    ,S(103, -66)   ,S(117, -41)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-22, -53)   ,S(97, 36)     ,S(150, 21)    ,S(198, -9)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(7, 7)       ,S(157, 48)    ,S(182, 47)    ,S(160, 39)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-54, 14)    ,S(95, 35)     ,S(110, 61)    ,S(82, 67)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-69, -28)   ,S(28, 18)     ,S(70, 42)     ,S(25, 66)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-84, -20)   ,S(-21, -1)    ,S(0, 23)      ,S(-19, 44)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-8, -49)    ,S(-22, -21)   ,S(-32, 6)     ,S(-89, 17)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-13, -129)  ,S(-7, -69)    ,S(-44, -25)   ,S(-93, -39)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
	},
};

constexpr PScore materialValues[5] = {S(101, 206), S(437, 623), S(455, 669), S(613, 1200), S(1293, 1490), };
constexpr PScore knightMob[9] = {S(-32, -127), S(-13, -49), S(-2, -3), S(5, 21), S(11, 31), S(18, 45), S(25, 43), S(36, 35), S(48, 7), };
constexpr PScore bishopMob[14] = {S(-55, -182), S(-26, -104), S(-10, -48), S(-4, -20), S(6, 0), S(11, 19), S(14, 31), S(12, 39), S(11, 49), S(14, 51), S(21, 43), S(42, 28), S(47, 33), S(70, -10), };
constexpr PScore rookMob[15] = {S(-27, -144), S(-34, -96), S(-24, -62), S(-23, -45), S(-24, -26), S(-25, -7), S(-23, 2), S(-18, 3), S(-14, 9), S(-7, 14), S(-6, 21), S(-1, 25), S(2, 26), S(18, 7), S(81, -29), };
constexpr PScore queenMob[28] = {S(-141, -449), S(-102, -469), S(-64, -381), S(-46, -295), S(-44, -199), S(-42, -141), S(-38, -105), S(-35, -86), S(-33, -66), S(-28, -54), S(-24, -46), S(-20, -41), S(-16, -39), S(-12, -36), S(-11, -36), S(-9, -37), S(-9, -41), S(-12, -41), S(-9, -47), S(-8, -58), S(5, -84), S(12, -106), S(11, -125), S(19, -147), S(-12, -150), S(-9, -176), S(-57, -131), S(-44, -127), };

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };

//constexpr Score KSAMG = -2;
constexpr double KSCALEMG = 761.5427856445312;
constexpr double KSBMG = 1; 
constexpr double KSCMG = -2.782630681991577;
// constexpr Score KSAEG = -71;
constexpr double KSCALEEG = 912.7113647460938;
constexpr double KSBEG = 1;
constexpr double KSCEG = -2.383039712905884;

constexpr PScore DAMPEN = S(static_cast<S16>(0.09197794646024704 * RESOLUTION), static_cast<S16>(0.844261884689331 * RESOLUTION));

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