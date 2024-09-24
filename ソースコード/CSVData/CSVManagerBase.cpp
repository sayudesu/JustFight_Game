#include <DxLib.h>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <vector>

#include "CSVManagerBase.h"

std::ifstream CSVManagerBase::CreateOrOpen(const char* fileNamePath, std::vector<std::vector<std::string>> data)
{
	// フォルダを作成するディレクトリのパス
	const wchar_t* folderPath = L"Data/CSV";

	// フォルダを作成
	if (CreateDirectoryW(folderPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		// フォルダの作成に失敗した場合の処理		
	}

	// ファイルを開く
	std::ifstream ifs(fileNamePath);

	// ファイルが存在しない場合は作成する
	if (!ifs.is_open())
	{
		std::ofstream ofs(fileNamePath);

		// ファイルを即座に閉じる
		ofs.close();

		// 作成したファイルを再度開く
		ifs.open(fileNamePath);

		// CSVファイルにデータを書き込む
		WriteCSV(fileNamePath, data);
	}

	return ifs;
}

void CSVManagerBase::WriteCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data)
{
	std::ofstream ofs(filename);

	// ファイルが正しく開かれたか確認
	if (!ofs.is_open())
	{
		return;
	}

	// データをCSV形式でファイルに書き込む
	for (const auto& row : data)
	{
		for (size_t i = 0; i < row.size(); ++i)
		{
			ofs << row[i];
			if (i != row.size() - 1)
			{
				// カンマでセルを区切る
				ofs << ",";
			}
		}

		// 改行で行を区切る
		ofs << "\n";
	}

	// ファイルを閉じる
	ofs.close();
}

std::vector<std::string> CSVManagerBase::Split(std::string& input, char delimiter)
{
	// 入力文字列をストリームに変換
	std::istringstream stream(input);

	// 分割された各フィールドを格納するための変数
	std::string field;

	// 分割された文字列を格納するベクター
	std::vector<std::string> result;

	// ストリームから1つずつフィールドを取り出し、ベクターに追加
	while (getline(stream, field, delimiter))
	{
		result.push_back(field);
	}

	// 分割された文字列のベクターを返す
	return result;
}
