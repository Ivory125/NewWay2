#pragma once

class CReferenceObject
{
	public:
		CReferenceObject();
		virtual ~CReferenceObject();

		void AddReference();
		void AddReferenceOnly();
		void Release();

		int GetReferenceCount();

		bool canDestroy();

	protected:
		virtual void OnConstruct();
		virtual void OnSelfDestruct();

	private:
		int m_refCount;
		bool m_destructed;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
