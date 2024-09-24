#pragma once

#include <DxLib.h>
#include <vector>
#include <string>

namespace CSVCharactorData
{
	// ファイル位置
	const char* const fileSmallPath  = "Data/CSV/CharactorSmallStrength.csv";
	const char* const fileMediumPath = "Data/CSV/CharactorMediumStrengthData.csv";
	const char* const fileLargePath  = "Data/CSV/CharactorLargeStrengthData.csv";

	// 書き込むデータ
	std::vector<std::vector<std::string>> firstSmallData =
	{
		{"No","Name","ValueNum","Parameter"},

		{"1","attackFrameMax","1","60"},
		{"2","attackFrameGapMax","1","15"},
		{"3","attackTotalFrame","1","75"},

		{"4","attackAfterStopFrameMax","1","60"},

		{"5","strongAttackFrameMax","1","5"},
		{"6","strongAttackFrameGapMax","1","120"},
		{"7","strongAttackTotalFrame","1","125"},

		{"8","guardFrameMax","1","30"},
		{"9","justGuardFrameMax","1","15"},

		{"10","stunFrameMax","1","240"},

		{"11","hpMax","1","3"},
		{"12","fightingMeterMax","1","100"},

		{"13","weaponRelativePos","3","-80","100","-50"},
		{"14","shieldRelativePos","3","100","100","-50"},

		{"15","weaponAttackRadius","1","50"},
		{"16","shieldRadius","1","50"},
		{"17","modelRadius","1","180"},

		{"18","weaponAttackPos","3","0","0","-210"},
		{"19","knockBackPos","3","0","0","-20"},

		{"20","weaponBackSpeed","1","30"},
		{"21","shieldBackSpeed","1","30"},		

		{"22","guradRate","1","0"},
	};

	// 書き込むデータ
	std::vector<std::vector<std::string>> firstMediumData =
	{
		{"No","Name","ValueNum","Parameter"},

		{"1","attackFrameMax","1","35"},
		{"2","attackFrameGapMax","1","15"},
		{"3","attackTotalFrame","1","50"},

		{"4","attackAfterStopFrameMax","1","60"},

		{"5","strongAttackFrameMax","1","5"},
		{"6","strongAttackFrameGapMax","1","60"},
		{"7","strongAttackTotalFrame","1","65"},

		{"8","guardFrameMax","1","20"},
		{"9","justGuardFrameMax","1","15"},

		{"10","stunFrameMax","1","180"},

		{"11","hpMax","1","6"},
		{"12","fightingMeterMax","1","100"},

		{"13","weaponRelativePos","3","-80","100","-50"},
		{"14","shieldRelativePos","3","100","100","-50"},

		{"15","weaponAttackRadius","1","50"},
		{"16","shieldRadius","1","50"},
		{"17","modelRadius","1","180"},

		{"18","weaponAttackPos","3","0","0","-210"},
		{"19","knockBackPos","3","0","0","-20"},

		{"20","weaponBackSpeed","1","30"},
		{"21","shieldBackSpeed","1","30"},

		{"22","guradRate","1","30"},
	};

	// 書き込むデータ
	std::vector<std::vector<std::string>> firstLargeData =
	{
		{"No","Name","ValueNum","Parameter"},

		{"1","attackFrameMax","1","20"},
		{"2","attackFrameGapMax","1","5"},
		{"3","attackTotalFrame","1","25"},

		{"4","attackAfterStopFrameMax","1","60"},

		{"5","strongAttackFrameMax","1","5"},
		{"6","strongAttackFrameGapMax","1","30"},
		{"7","strongAttackTotalFrame","1","35"},

		{"8","guardFrameMax","1","20"},
		{"9","justGuardFrameMax","1","15"},

		{"10","stunFrameMax","1","120"},

		{"11","hpMax","1","10"},
		{"12","fightingMeterMax","1","100"},

		{"13","weaponRelativePos","3","-80","100","-50"},
		{"14","shieldRelativePos","3","100","100","-50"},

		{"15","weaponAttackRadius","1","50"},
		{"16","shieldRadius","1","50"},
		{"17","modelRadius","1","180"},

		{"18","weaponAttackPos","3","0","0","-210"},
		{"19","knockBackPos","3","0","0","-20"},

		{"20","weaponBackSpeed","1","30"},
		{"21","shieldBackSpeed","1","30"},

		{"22","guradRate","1","5"},
	};
}