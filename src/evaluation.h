#pragma once

#include "BBmacros.h"
#include "Position.h"

const PScore pawnTable[2][64] = {
	{
		S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,
		S(194, 262)   ,S(150, 263)   ,S(192, 207)   ,S(195, 167)   ,S(171, 176)   ,S(113, 225)   ,S(-85, 316)   ,S(-54, 325)   ,
		S(95, 298)    ,S(121, 277)   ,S(143, 232)   ,S(135, 191)   ,S(146, 203)   ,S(197, 238)   ,S(144, 276)   ,S(105, 284)   ,
		S(91, 250)    ,S(94, 234)    ,S(100, 214)   ,S(115, 187)   ,S(123, 204)   ,S(128, 218)   ,S(93, 244)    ,S(90, 239)    ,
		S(84, 224)    ,S(80, 216)    ,S(95, 199)    ,S(102, 193)   ,S(107, 201)   ,S(108, 209)   ,S(92, 214)    ,S(91, 209)    ,
		S(87, 214)    ,S(85, 208)    ,S(92, 206)    ,S(100, 205)   ,S(112, 220)   ,S(102, 223)   ,S(113, 199)   ,S(104, 193)   ,
		S(84, 223)    ,S(82, 214)    ,S(81, 214)    ,S(90, 218)    ,S(90, 238)    ,S(126, 231)   ,S(135, 204)   ,S(107, 183)   ,
		S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,
	},
	{
		S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,
		S(126, 324)   ,S(94, 335)    ,S(167, 297)   ,S(178, 223)   ,S(176, 207)   ,S(194, 184)   ,S(87, 258)    ,S(74, 262)    ,
		S(114, 319)   ,S(166, 313)   ,S(164, 281)   ,S(148, 235)   ,S(157, 206)   ,S(155, 234)   ,S(148, 254)   ,S(89, 276)    ,
		S(122, 249)   ,S(116, 248)   ,S(120, 232)   ,S(125, 214)   ,S(124, 208)   ,S(110, 221)   ,S(77, 246)    ,S(82, 247)    ,
		S(119, 215)   ,S(105, 221)   ,S(117, 209)   ,S(116, 210)   ,S(106, 202)   ,S(91, 215)    ,S(69, 224)    ,S(65, 228)    ,
		S(121, 203)   ,S(131, 198)   ,S(116, 217)   ,S(106, 220)   ,S(95, 215)    ,S(88, 216)    ,S(72, 219)    ,S(64, 220)    ,
		S(126, 200)   ,S(147, 197)   ,S(134, 223)   ,S(101, 235)   ,S(79, 228)    ,S(76, 226)    ,S(68, 227)    ,S(67, 228)    ,
		S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,S(100, 207)   ,
	},
};
const PScore knightTable[2][64] = {
	{
		S(258, 575)   ,S(349, 651)   ,S(329, 702)   ,S(438, 673)   ,S(485, 668)   ,S(345, 705)   ,S(346, 669)   ,S(324, 545)   ,
		S(475, 622)   ,S(476, 647)   ,S(525, 651)   ,S(544, 689)   ,S(528, 694)   ,S(557, 643)   ,S(461, 656)   ,S(493, 625)   ,
		S(460, 635)   ,S(487, 661)   ,S(506, 705)   ,S(505, 712)   ,S(535, 703)   ,S(507, 714)   ,S(464, 688)   ,S(457, 663)   ,
		S(490, 657)   ,S(494, 670)   ,S(502, 706)   ,S(498, 734)   ,S(496, 747)   ,S(503, 727)   ,S(487, 716)   ,S(487, 692)   ,
		S(480, 663)   ,S(490, 675)   ,S(484, 704)   ,S(483, 719)   ,S(492, 728)   ,S(490, 719)   ,S(503, 698)   ,S(482, 695)   ,
		S(454, 614)   ,S(464, 650)   ,S(458, 678)   ,S(469, 707)   ,S(466, 705)   ,S(454, 681)   ,S(453, 670)   ,S(447, 646)   ,
		S(437, 633)   ,S(443, 649)   ,S(447, 646)   ,S(454, 671)   ,S(451, 671)   ,S(441, 649)   ,S(432, 658)   ,S(437, 663)   ,
		S(424, 591)   ,S(449, 610)   ,S(442, 624)   ,S(445, 659)   ,S(441, 661)   ,S(439, 654)   ,S(436, 635)   ,S(401, 621)   ,
	},
	{
		S(286, 538)   ,S(358, 637)   ,S(361, 686)   ,S(461, 661)   ,S(462, 657)   ,S(318, 693)   ,S(333, 658)   ,S(290, 549)   ,
		S(464, 621)   ,S(455, 657)   ,S(518, 658)   ,S(574, 671)   ,S(537, 678)   ,S(547, 646)   ,S(472, 644)   ,S(481, 613)   ,
		S(433, 659)   ,S(487, 673)   ,S(491, 713)   ,S(515, 713)   ,S(534, 699)   ,S(518, 699)   ,S(506, 651)   ,S(464, 636)   ,
		S(473, 675)   ,S(494, 688)   ,S(497, 720)   ,S(509, 729)   ,S(508, 726)   ,S(507, 703)   ,S(494, 686)   ,S(489, 658)   ,
		S(481, 676)   ,S(486, 688)   ,S(491, 709)   ,S(494, 719)   ,S(496, 717)   ,S(490, 705)   ,S(492, 679)   ,S(486, 657)   ,
		S(425, 649)   ,S(446, 664)   ,S(458, 681)   ,S(461, 702)   ,S(463, 701)   ,S(455, 675)   ,S(455, 644)   ,S(439, 616)   ,
		S(424, 670)   ,S(412, 673)   ,S(431, 654)   ,S(444, 666)   ,S(450, 668)   ,S(441, 648)   ,S(426, 654)   ,S(427, 628)   ,
		S(391, 637)   ,S(418, 639)   ,S(428, 651)   ,S(422, 659)   ,S(440, 648)   ,S(443, 616)   ,S(437, 616)   ,S(414, 579)   ,
	},
};
const PScore bishopTable[2][64] = {
	{
		S(429, 745)   ,S(407, 744)   ,S(352, 746)   ,S(344, 756)   ,S(369, 740)   ,S(338, 739)   ,S(441, 725)   ,S(429, 734)   ,
		S(431, 730)   ,S(490, 725)   ,S(486, 722)   ,S(451, 734)   ,S(478, 724)   ,S(473, 725)   ,S(449, 736)   ,S(436, 723)   ,
		S(490, 721)   ,S(494, 728)   ,S(519, 726)   ,S(497, 724)   ,S(502, 726)   ,S(493, 752)   ,S(474, 741)   ,S(475, 742)   ,
		S(478, 721)   ,S(513, 721)   ,S(499, 721)   ,S(516, 744)   ,S(515, 751)   ,S(504, 736)   ,S(512, 738)   ,S(463, 740)   ,
		S(488, 705)   ,S(497, 712)   ,S(490, 738)   ,S(505, 737)   ,S(512, 742)   ,S(488, 738)   ,S(491, 724)   ,S(493, 709)   ,
		S(490, 704)   ,S(503, 713)   ,S(490, 727)   ,S(488, 732)   ,S(486, 731)   ,S(486, 728)   ,S(489, 714)   ,S(492, 720)   ,
		S(499, 697)   ,S(496, 684)   ,S(490, 703)   ,S(469, 719)   ,S(471, 721)   ,S(487, 704)   ,S(500, 701)   ,S(488, 686)   ,
		S(505, 693)   ,S(489, 710)   ,S(469, 713)   ,S(463, 707)   ,S(461, 715)   ,S(466, 721)   ,S(467, 707)   ,S(490, 692)   ,
	},
	{
		S(403, 738)   ,S(371, 735)   ,S(384, 732)   ,S(362, 739)   ,S(350, 745)   ,S(327, 743)   ,S(440, 730)   ,S(410, 747)   ,
		S(399, 729)   ,S(452, 739)   ,S(471, 727)   ,S(447, 725)   ,S(465, 725)   ,S(488, 723)   ,S(488, 729)   ,S(453, 716)   ,
		S(470, 739)   ,S(495, 737)   ,S(499, 735)   ,S(510, 728)   ,S(510, 718)   ,S(519, 727)   ,S(506, 722)   ,S(496, 722)   ,
		S(467, 727)   ,S(510, 728)   ,S(497, 738)   ,S(517, 743)   ,S(533, 738)   ,S(507, 717)   ,S(514, 722)   ,S(493, 721)   ,
		S(489, 710)   ,S(477, 725)   ,S(498, 732)   ,S(514, 738)   ,S(511, 735)   ,S(502, 725)   ,S(505, 705)   ,S(483, 706)   ,
		S(483, 715)   ,S(492, 718)   ,S(495, 729)   ,S(499, 723)   ,S(492, 724)   ,S(494, 717)   ,S(497, 703)   ,S(481, 703)   ,
		S(482, 711)   ,S(508, 691)   ,S(490, 706)   ,S(475, 715)   ,S(472, 712)   ,S(478, 701)   ,S(483, 687)   ,S(495, 677)   ,
		S(498, 694)   ,S(467, 706)   ,S(468, 714)   ,S(464, 704)   ,S(462, 704)   ,S(468, 707)   ,S(481, 705)   ,S(496, 690)   ,
	},
};
const PScore rookTable[2][64] = {
	{
		S(653, 1301)  ,S(647, 1305)  ,S(613, 1318)  ,S(624, 1311)  ,S(596, 1328)  ,S(659, 1319)  ,S(661, 1316)  ,S(657, 1313)  ,
		S(630, 1291)  ,S(617, 1303)  ,S(640, 1298)  ,S(656, 1299)  ,S(625, 1315)  ,S(666, 1289)  ,S(631, 1301)  ,S(634, 1291)  ,
		S(619, 1278)  ,S(673, 1260)  ,S(647, 1277)  ,S(682, 1257)  ,S(683, 1262)  ,S(683, 1275)  ,S(705, 1253)  ,S(637, 1277)  ,
		S(617, 1267)  ,S(638, 1264)  ,S(646, 1266)  ,S(673, 1257)  ,S(652, 1273)  ,S(662, 1270)  ,S(649, 1273)  ,S(629, 1273)  ,
		S(595, 1251)  ,S(600, 1270)  ,S(599, 1267)  ,S(613, 1258)  ,S(598, 1272)  ,S(606, 1277)  ,S(623, 1272)  ,S(599, 1255)  ,
		S(593, 1223)  ,S(605, 1240)  ,S(594, 1236)  ,S(613, 1226)  ,S(604, 1235)  ,S(607, 1239)  ,S(634, 1229)  ,S(603, 1209)  ,
		S(576, 1218)  ,S(607, 1216)  ,S(604, 1224)  ,S(615, 1216)  ,S(609, 1220)  ,S(623, 1213)  ,S(633, 1201)  ,S(555, 1221)  ,
		S(613, 1215)  ,S(614, 1222)  ,S(617, 1223)  ,S(625, 1215)  ,S(620, 1219)  ,S(626, 1235)  ,S(620, 1225)  ,S(609, 1197)  ,
	},
	{
		S(656, 1300)  ,S(620, 1322)  ,S(634, 1321)  ,S(616, 1314)  ,S(645, 1302)  ,S(648, 1294)  ,S(683, 1282)  ,S(684, 1285)  ,
		S(595, 1303)  ,S(625, 1302)  ,S(651, 1298)  ,S(639, 1298)  ,S(656, 1289)  ,S(665, 1275)  ,S(635, 1285)  ,S(647, 1279)  ,
		S(603, 1289)  ,S(664, 1268)  ,S(652, 1284)  ,S(675, 1258)  ,S(700, 1244)  ,S(668, 1260)  ,S(701, 1242)  ,S(645, 1265)  ,
		S(591, 1279)  ,S(633, 1271)  ,S(647, 1274)  ,S(653, 1263)  ,S(662, 1256)  ,S(651, 1256)  ,S(655, 1250)  ,S(640, 1255)  ,
		S(563, 1247)  ,S(600, 1265)  ,S(590, 1276)  ,S(595, 1267)  ,S(603, 1256)  ,S(598, 1264)  ,S(606, 1261)  ,S(598, 1246)  ,
		S(559, 1211)  ,S(606, 1226)  ,S(607, 1228)  ,S(599, 1224)  ,S(603, 1224)  ,S(600, 1229)  ,S(619, 1226)  ,S(598, 1211)  ,
		S(472, 1235)  ,S(589, 1204)  ,S(603, 1216)  ,S(603, 1215)  ,S(610, 1209)  ,S(611, 1207)  ,S(614, 1201)  ,S(570, 1202)  ,
		S(562, 1219)  ,S(590, 1230)  ,S(619, 1221)  ,S(620, 1214)  ,S(621, 1207)  ,S(618, 1211)  ,S(625, 1206)  ,S(613, 1198)  ,
	},
};
const PScore queenTable[2][64] = {
	{
		S(1273, 1522) ,S(1306, 1518) ,S(1283, 1554) ,S(1289, 1562) ,S(1267, 1587) ,S(1305, 1585) ,S(1324, 1557) ,S(1302, 1537) ,
		S(1295, 1507) ,S(1240, 1556) ,S(1292, 1527) ,S(1234, 1592) ,S(1222, 1629) ,S(1310, 1587) ,S(1245, 1597) ,S(1303, 1547) ,
		S(1297, 1507) ,S(1301, 1505) ,S(1287, 1528) ,S(1294, 1542) ,S(1290, 1565) ,S(1283, 1585) ,S(1277, 1585) ,S(1275, 1598) ,
		S(1307, 1489) ,S(1304, 1528) ,S(1297, 1515) ,S(1274, 1585) ,S(1266, 1603) ,S(1298, 1594) ,S(1305, 1593) ,S(1310, 1557) ,
		S(1312, 1469) ,S(1315, 1512) ,S(1300, 1527) ,S(1288, 1583) ,S(1285, 1577) ,S(1299, 1565) ,S(1309, 1537) ,S(1315, 1520) ,
		S(1319, 1447) ,S(1325, 1474) ,S(1314, 1508) ,S(1305, 1505) ,S(1305, 1497) ,S(1307, 1520) ,S(1322, 1481) ,S(1318, 1458) ,
		S(1322, 1434) ,S(1324, 1439) ,S(1331, 1428) ,S(1322, 1462) ,S(1324, 1449) ,S(1342, 1389) ,S(1330, 1397) ,S(1295, 1441) ,
		S(1323, 1424) ,S(1324, 1427) ,S(1329, 1427) ,S(1335, 1450) ,S(1335, 1416) ,S(1311, 1409) ,S(1308, 1387) ,S(1304, 1401) ,
	},
	{
		S(1296, 1480) ,S(1316, 1506) ,S(1303, 1533) ,S(1285, 1565) ,S(1283, 1565) ,S(1300, 1530) ,S(1301, 1507) ,S(1299, 1498) ,
		S(1264, 1525) ,S(1216, 1577) ,S(1279, 1565) ,S(1211, 1617) ,S(1207, 1638) ,S(1293, 1520) ,S(1246, 1557) ,S(1310, 1494) ,
		S(1269, 1544) ,S(1256, 1550) ,S(1251, 1571) ,S(1285, 1556) ,S(1295, 1540) ,S(1290, 1535) ,S(1315, 1492) ,S(1316, 1503) ,
		S(1280, 1520) ,S(1287, 1566) ,S(1276, 1557) ,S(1272, 1584) ,S(1279, 1570) ,S(1300, 1508) ,S(1313, 1524) ,S(1321, 1496) ,
		S(1295, 1478) ,S(1291, 1531) ,S(1287, 1538) ,S(1286, 1566) ,S(1295, 1549) ,S(1300, 1521) ,S(1318, 1493) ,S(1310, 1473) ,
		S(1275, 1472) ,S(1302, 1465) ,S(1301, 1501) ,S(1302, 1480) ,S(1306, 1485) ,S(1311, 1490) ,S(1322, 1468) ,S(1316, 1435) ,
		S(1262, 1433) ,S(1297, 1401) ,S(1329, 1380) ,S(1324, 1431) ,S(1320, 1447) ,S(1329, 1405) ,S(1330, 1417) ,S(1325, 1405) ,
		S(1270, 1403) ,S(1295, 1381) ,S(1306, 1373) ,S(1321, 1391) ,S(1327, 1411) ,S(1329, 1390) ,S(1329, 1404) ,S(1322, 1391) ,
	},
};
const PScore kingTable[2][64] = {
	{
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(106, -46)   ,S(80, -36)    ,S(83, -40)    ,S(0, -180)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(148, 16)    ,S(165, 30)    ,S(83, 61)     ,S(16, -15)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(150, 49)    ,S(219, 52)    ,S(159, 59)    ,S(59, 26)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(82, 77)     ,S(111, 67)    ,S(83, 48)     ,S(-30, 30)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(23, 75)     ,S(37, 55)     ,S(31, 25)     ,S(-31, -2)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-19, 58)    ,S(-10, 38)    ,S(-20, 9)     ,S(-2, -6)     ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-91, 33)    ,S(-72, 32)    ,S(-20, -8)    ,S(51, -41)    ,
		S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(-76, -42)   ,S(-96, -2)    ,S(-10, -48)   ,S(47, -123)   ,
	},
	{
		S(10, -205)   ,S(75, -80)    ,S(113, -68)   ,S(123, -42)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-16, -52)   ,S(104, 35)    ,S(154, 19)    ,S(199, -10)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(9, 7)       ,S(160, 46)    ,S(183, 45)    ,S(160, 38)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-51, 13)    ,S(97, 32)     ,S(113, 57)    ,S(85, 63)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-69, -29)   ,S(28, 17)     ,S(69, 39)     ,S(24, 63)     ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-82, -20)   ,S(-24, -2)    ,S(-1, 21)     ,S(-21, 41)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-8, -46)    ,S(-23, -22)   ,S(-33, 5)     ,S(-89, 16)    ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
		S(-14, -124)  ,S(-7, -65)    ,S(-44, -24)   ,S(-93, -39)   ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,S(0, 0)       ,
	},
};

constexpr PScore materialValues[5] = {S(100, 207), S(435, 626), S(453, 670), S(605, 1208), S(1296, 1469), };
constexpr PScore knightMob[9] = {S(-33, -128), S(-14, -50), S(-2, -3), S(5, 22), S(10, 32), S(15, 46), S(24, 45), S(33, 39), S(46, 12), };
constexpr PScore bishopMob[14] = {S(-55, -183), S(-27, -105), S(-10, -49), S(-4, -21), S(6, -1), S(11, 18), S(14, 30), S(12, 38), S(12, 47), S(16, 49), S(22, 42), S(44, 26), S(50, 31), S(71, -12), };
constexpr PScore rookMob[15] = {S(-24, -145), S(-31, -97), S(-23, -62), S(-22, -45), S(-21, -27), S(-23, -8), S(-20, 1), S(-15, 3), S(-12, 9), S(-4, 14), S(-5, 20), S(1, 24), S(4, 25), S(20, 5), S(84, -28), };
constexpr PScore queenMob[28] = {S(-131, -462), S(-91, -483), S(-55, -392), S(-38, -304), S(-35, -209), S(-33, -152), S(-31, -116), S(-27, -98), S(-25, -77), S(-21, -66), S(-16, -57), S(-13, -54), S(-9, -50), S(-6, -48), S(-4, -48), S(-2, -48), S(-3, -52), S(-7, -50), S(-6, -56), S(-3, -66), S(9, -92), S(15, -114), S(12, -126), S(21, -149), S(-8, -153), S(0, -184), S(-42, -145), S(-40, -131), };

constexpr Score gamephaseInc[12] = { 0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0 };

//constexpr Score KSAMG = -2;
constexpr double KSCALEMG = 881.30126953125;
constexpr double KSBMG = 1; 
constexpr double KSCMG = -2.6634058952331543;
// constexpr Score KSAEG = -71;
constexpr double KSCALEEG = 1095.6646728515625;
constexpr double KSBEG = 1;
constexpr double KSCEG = 2.485602378845215;

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