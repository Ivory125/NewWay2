#include "StdAfx.h"
#include "NetPacketHeaderMap.h"

void CNetworkPacketHeaderMap::Set(int header, const TPacketType & rPacketType)
{
	m_headerMap[header] = rPacketType;
}
bool CNetworkPacketHeaderMap::Get(int header, TPacketType * pPacketType)
{
	std::map<int, TPacketType>::iterator f=m_headerMap.find(header);

	if (m_headerMap.end()==f)
		return false;

	*pPacketType = f->second;

	return true;
}

CNetworkPacketHeaderMap::CNetworkPacketHeaderMap()
{
}

CNetworkPacketHeaderMap::~CNetworkPacketHeaderMap()
{
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
