#pragma once

#include <vector>
#include <string>

namespace CSVFontData
{

	const char* const filePath = "Data/CSV/FontData.csv";

	// 書き込むデータ
	std::vector<std::vector<std::string>> data =
	{
		{"No","FileName","Name", "Size","SizeNo", "Extension"},

		{"0","NikkyouSans-mLKax","Nikkyou Sans", "12","0", ".ttf"},
		{"1","NikkyouSans-mLKax","Nikkyou Sans", "40","1", ".ttf"},
		{"2","NikkyouSans-mLKax","Nikkyou Sans", "82", "2",".ttf"},
		{"3","NikkyouSans-mLKax","Nikkyou Sans", "132", "3",".ttf"},

		{"4","GenEiLateMinP_v2","源暎テラミン 詰 v2","12","4", ".ttf"},
		{"5","GenEiLateMinP_v2","源暎テラミン 詰 v2","40","5",".ttf"},
		{"6","GenEiLateMinP_v2","源暎テラミン 詰 v2","82","6",".ttf"},
		{"7","GenEiLateMinP_v2","源暎テラミン 詰 v2","137","7",".ttf"},
	};
}
