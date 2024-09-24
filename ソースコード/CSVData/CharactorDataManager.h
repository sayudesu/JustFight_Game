#pragma once
#include <DxLib.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "CSVManagerBase.h"

#include "../Util/ParameterData.h"


class CharactorDataManager final : public CSVManagerBase
{
private:
	// �T�E���h�ۑ��`���f�[�^
	struct CharactorData
	{
		int no = 0;
		std::string name = {};
		int valueNum = 0;

		std::vector<float> element{};
	};

private:
	CharactorDataManager() = default;
	static CharactorDataManager* m_pInstance;
public:
	virtual~CharactorDataManager() = default;
	// �R�s�[�R���X�g���N�^�̋֎~
	CharactorDataManager(const CharactorDataManager&) = delete;
	CharactorDataManager& operator=(const CharactorDataManager&) = delete;
	CharactorDataManager(CharactorDataManager&&) = delete;
	CharactorDataManager& operator=(CharactorDataManager&&) = delete;

	// �C���X�^���X�̍쐬
	static CharactorDataManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CharactorDataManager();
		}
		return *m_pInstance;
	}

	// �������
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}	

	// ���[�h
	void Load(
		const char* fileNamePath1, std::vector<std::vector<std::string>> data1,
		const char* fileNamePath2, std::vector<std::vector<std::string>> data2,
		const char* fileNamePath3, std::vector<std::vector<std::string>> data3);
	// �A�����[�h
	void Unload();
public:
	// �L�����N�^�[�̃p�����[�^�[��n��
	CharactorData GetParamData(ParamData id,int type);
private:
	void CSVLoad(std::ifstream& ifs, std::string line, int count, int mapkey, int type);
	// �T�E���h�̃f�[�^���i�[
	std::map<int, CharactorData> m_data[3]{};
};

