#ifndef _PASSGEN_LOGIC_H_
#define _PASSGEN_LOGIC_H_

#include "binDefs.h"
#include "PassGen.h"

class PassGenLogic
{
public:
	PassGenLogic();
	~PassGenLogic();

	//pass generator logic init
	void InitPassLogic(const std::wstring& wConf, const std::wstring& wAutosave, const std::wstring& wPhrase);

	//����� ������
	void Accumulate(const char* pBuff, int iSize);

	//process income data
	void Process();

	//��������� ��� �������� � ����
	const char* GetResult(int *pSize);

	//debug logic
	void TestLogic();

private:

	//read node name
	int DetectName(int pos);

	//read chain state
	int DetectChainState(int pos);

	//read result
	int DetectResult(int pos);

	//analayse data from network
	void Analyse();

	//first client connect
	void FirstClientConnect();

	//generate file name
	std::string GenFileName(char* pChain);

	//debug logic
	void TestFillInBuff();

	PassGen m_PassGen;

	std::string sNode;

	char curChain[MAX_LEN_PASS];
	int iCurResult;

	//����� ��� ������ ���������
	char inBuff[32768];
	int inSize;

	//���������
	char outBuff[32768];
	int outSize;
};

#endif	//_PASSGEN_LOGIC_H_