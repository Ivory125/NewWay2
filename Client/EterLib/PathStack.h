#pragma once

#include <deque>
#include <string>

class CPathStack
{
	public:
		CPathStack();
		virtual ~CPathStack();

		void SetBase();

		void MoveBase();

		void Push();

		bool Pop();

		void Move(const char* c_szPathName);
		void GetCurrentPathName(std::string* pstCurPathName);

	protected:
		std::string				m_stBasePathName;
		std::deque<std::string>	m_stPathNameDeque;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
