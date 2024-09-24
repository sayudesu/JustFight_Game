#pragma once
#include <vector>
#include <string>
#include <map>

#include "CSVManagerBase.h"

#include "../Util/SoundName.h"
#include "../Util/ModelName.h"

// �T�E���h�}�l�[�W���[�̃V���O���g�[���N���X

class ModelManager final : public CSVManagerBase
{
private:
	ModelManager() = default;
	static ModelManager* m_pInstance;
public:
	virtual~ModelManager() = default;
	// �R�s�[�R���X�g���N�^�̋֎~
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator=(ModelManager&&) = delete;

	// �C���X�^���X�̍쐬
	static ModelManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new ModelManager();
		}
		return *m_pInstance;
	}

	// �������
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

public:
	// ���[�h
	void Load(const char* fileNamePath, std::vector<std::vector<std::string>> data);
	// �A�����[�h
	void Unload();
public:
	std::string ModelType(ModelName type);
private:
	// �T�E���h�ۑ��`���f�[�^
	struct ModelData
	{
		int no;		
		std::string name;		
		std::string extension;
	};

	// �T�E���h�̃f�[�^���i�[
	std::map<int, ModelData> m_data;
	// �n���h��
	std::vector<int> m_handle;
	std::vector<std::string> m_filePath;
};

