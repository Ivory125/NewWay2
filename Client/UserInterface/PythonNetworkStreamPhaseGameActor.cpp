#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "NetworkActorManager.h"
#include "PythonBackground.h"

#include "PythonApplication.h"
#include "AbstractPlayer.h"
#include "../gamelib/ActorInstance.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "InstanceBase.h"
#endif

void CPythonNetworkStream::__GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.GlobalPositionToLocalPosition(rGlobalX, rGlobalY);
}

void CPythonNetworkStream::__LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	CPythonBackground&rkBgMgr=CPythonBackground::Instance();
	rkBgMgr.LocalPositionToGlobalPosition(rLocalX, rLocalY);
}

bool CPythonNetworkStream::__CanActMainInstance()
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain=rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanAct();
}

void CPythonNetworkStream::__ClearNetworkActorManager()
{
	m_rokNetActorMgr->Destroy();
}

void __SetWeaponPower(IAbstractPlayer& rkPlayer, DWORD dwWeaponID)
{
	DWORD minPower=0;
	DWORD maxPower=0;
	DWORD minMagicPower=0;
	DWORD maxMagicPower=0;
	DWORD addPower=0;

	CItemData* pkWeapon;
	if (CItemManager::Instance().GetItemDataPointer(dwWeaponID, &pkWeapon))
	{
		if (pkWeapon->GetType()==CItemData::ITEM_TYPE_WEAPON)
		{
			minPower=pkWeapon->GetValue(3);
			maxPower=pkWeapon->GetValue(4);
			minMagicPower=pkWeapon->GetValue(1);
			maxMagicPower=pkWeapon->GetValue(2);
			addPower=pkWeapon->GetValue(5);
		}
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		else if (pkWeapon->GetType()==CItemData::ITEM_TYPE_COSTUME && pkWeapon->GetSubType()==CItemData::COSTUME_WEAPON)
		{
			CItemData* pkRealWeapon;
			if (CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, c_Equipment_Weapon)), &pkRealWeapon))
			{
				minPower=pkRealWeapon->GetValue(3);
				maxPower=pkRealWeapon->GetValue(4);
				minMagicPower=pkRealWeapon->GetValue(1);
				maxMagicPower=pkRealWeapon->GetValue(2);
				addPower=pkRealWeapon->GetValue(5);
			}
		}
#endif
	}

	rkPlayer.SetWeaponPower(minPower, maxPower, minMagicPower, maxMagicPower, addPower);
}

bool IsInvisibleRace(WORD raceNum)
{
	switch(raceNum)
	{
	case 20025:
	case 20038:
	case 20039:
		return true;
	default:
		return false;
	}
}

static SNetworkActorData s_kNetActorData;

bool CPythonNetworkStream::RecvCharacterAppendPacket()
{
	TPacketGCCharacterAdd chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_bType=chrAddPacket.bType;
	kNetActorData.m_dwMovSpd=chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd=chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace=chrAddPacket.wRaceNum;

	kNetActorData.m_dwStateFlags=chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID=chrAddPacket.dwVID;
	kNetActorData.m_fRot=chrAddPacket.angle;

	kNetActorData.m_stName="";

	kNetActorData.m_stName="";
	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);

	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);

	kNetActorData.m_sAlignment=0;/*chrAddPacket.sAlignment*/;
	kNetActorData.m_byPKMode=0;/*chrAddPacket.bPKMode*/;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetActorData.m_bLanguage = 0;
#endif
	kNetActorData.m_dwGuildID=0;/*chrAddPacket.dwGuild*/;
	kNetActorData.m_dwEmpireID=0;/*chrAddPacket.bEmpire*/;
	kNetActorData.m_dwArmor=0;/*chrAddPacket.awPart[CHR_EQUIPPART_ARMOR]*/;
	kNetActorData.m_dwWeapon=0;/*chrAddPacket.awPart[CHR_EQUIPPART_WEAPON]*/;
	kNetActorData.m_dwHair=0;/*chrAddPacket.awPart[CHR_EQUIPPART_HAIR]*/;
#ifdef ELEMENT_SPELL_WORLDARD
	kNetActorData.m_dwElementsEffect = 0;/*chrAddPacket.dwElementsEffect*/;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetActorData.m_dwAcce = 0;
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	kNetActorData.m_dwArrow=0;/*chrAddPacket.m_dwArrow*/
#endif
	kNetActorData.m_dwMountVnum=0;/*chrAddPacket.dwMountVnum*/;
	kNetActorData.m_dwLevel = 0;

	if(kNetActorData.m_bType != CActorInstance::TYPE_PC &&
		kNetActorData.m_bType != CActorInstance::TYPE_NPC)
	{
		const char * c_szName;
		CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
		if (rkNonPlayer.GetName(kNetActorData.m_dwRace, &c_szName))
			kNetActorData.m_stName = c_szName;
		//else
		//	kNetActorData.m_stName=chrAddPacket.name;

		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		s_kNetActorData = kNetActorData;
	}

	return true;
}

bool CPythonNetworkStream::RecvCharacterAdditionalInfo()
{
	TPacketGCCharacterAdditionalInfo chrInfoPacket;
	if (!Recv(sizeof(chrInfoPacket), &chrInfoPacket))
		return false;

	SNetworkActorData kNetActorData = s_kNetActorData;
	if (IsInvisibleRace(kNetActorData.m_dwRace))
		return true;

	if (kNetActorData.m_dwVID == chrInfoPacket.dwVID)
	{
		const char* c_szNpcClientName = nullptr;

		// ASLAN Buff NPC System + Mehrsprachigkeit
		if (
			(20101 <= kNetActorData.m_dwRace && 20232 >= kNetActorData.m_dwRace) ||
			(34001 <= kNetActorData.m_dwRace && 40305 >= kNetActorData.m_dwRace) ||
			(30000 == kNetActorData.m_dwRace) ||
			(10 == kNetActorData.m_dwRace) ||
			(11 == kNetActorData.m_dwRace)
		)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			if (kNetActorData.m_bType == CActorInstance::TYPE_NPC
				&& kNetActorData.m_dwRace != 30000
			)
			{
				if (
					(kNetActorData.m_dwRace > 34000 && kNetActorData.m_dwRace < 35000) ||
					(kNetActorData.m_dwRace > 20100 && kNetActorData.m_dwRace < 20110)
				)
				{
					char szPetName[CHARACTER_NAME_MAX_LEN];
					if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
						sprintf(szPetName, "%s - %s", chrInfoPacket.name, c_szNpcClientName);
					else
						sprintf(szPetName, "%s", chrInfoPacket.name);

					kNetActorData.m_stName = szPetName;
				}
				else
				{
					if (CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
						kNetActorData.m_stName = c_szNpcClientName;
					else
						kNetActorData.m_stName = chrInfoPacket.name;
				}
			}
			else
			{
				kNetActorData.m_stName = chrInfoPacket.name;
			}
#else
			kNetActorData.m_stName = chrInfoPacket.name;
#endif
		}
		else
		{
			// Ursprüngliche NPC-Namenszuweisung außerhalb des ASLAN-Blocks
			if (kNetActorData.m_bType == CActorInstance::TYPE_NPC && CPythonNonPlayer::Instance().GetName(kNetActorData.m_dwRace, &c_szNpcClientName))
				kNetActorData.m_stName = c_szNpcClientName;
			else
				kNetActorData.m_stName = chrInfoPacket.name;
		}

		kNetActorData.m_dwGuildID	= chrInfoPacket.dwGuildID;
		kNetActorData.m_dwLevel		= chrInfoPacket.dwLevel;
		kNetActorData.m_sAlignment	= chrInfoPacket.sAlignment;
		kNetActorData.m_byPKMode	= chrInfoPacket.bPKMode;
		kNetActorData.m_dwGuildID	= chrInfoPacket.dwGuildID;
		kNetActorData.m_dwEmpireID	= chrInfoPacket.bEmpire;
		kNetActorData.m_dwArmor		= chrInfoPacket.awPart[CHR_EQUIPPART_ARMOR];
		kNetActorData.m_dwWeapon	= chrInfoPacket.awPart[CHR_EQUIPPART_WEAPON];
		kNetActorData.m_dwHair		= chrInfoPacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ELEMENT_SPELL_WORLDARD
		kNetActorData.m_dwElementsEffect = chrInfoPacket.dwElementsEffect;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		kNetActorData.m_dwAcce		= chrInfoPacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
		kNetActorData.m_dwArrow		= chrInfoPacket.dwArrow;
#endif
		kNetActorData.m_dwMountVnum	= chrInfoPacket.dwMountVnum;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		kNetActorData.m_bLanguage = chrInfoPacket.bLanguage;
#endif
		__RecvCharacterAppendPacket(&kNetActorData);
	}
	else
	{
		TraceError("TPacketGCCharacterAdditionalInfo name=%s vid=%d race=%d Error", chrInfoPacket.name, chrInfoPacket.dwVID, kNetActorData.m_dwRace);
	}
	return true;
}

bool CPythonNetworkStream::RecvCharacterAppendPacketNew()
{
	TraceError("TPacketGCCharacterAdd2 is packet that doesn't write.");
	TPacketGCCharacterAdd2 chrAddPacket;
	if (!Recv(sizeof(chrAddPacket), &chrAddPacket))
		return false;
	if(IsInvisibleRace(chrAddPacket.wRaceNum))
		return true;

	__GlobalPositionToLocalPosition(chrAddPacket.x, chrAddPacket.y);

	SNetworkActorData kNetActorData;
	kNetActorData.m_dwLevel		= 0;
	kNetActorData.m_bType		= chrAddPacket.bType;
	kNetActorData.m_dwGuildID	= chrAddPacket.dwGuild;
	kNetActorData.m_dwEmpireID	= chrAddPacket.bEmpire;
	kNetActorData.m_dwMovSpd	= chrAddPacket.bMovingSpeed;
	kNetActorData.m_dwAtkSpd	= chrAddPacket.bAttackSpeed;
	kNetActorData.m_dwRace		= chrAddPacket.wRaceNum;
	kNetActorData.m_dwArmor		= chrAddPacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetActorData.m_dwWeapon	= chrAddPacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetActorData.m_dwHair		= chrAddPacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ELEMENT_SPELL_WORLDARD
	kNetActorData.m_dwElementsEffect = chrAddPacket.dwElementsEffect;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetActorData.m_dwAcce		= chrAddPacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	kNetActorData.m_dwArrow		= chrAddPacket.dwArrow;
#endif
	kNetActorData.m_dwStateFlags= chrAddPacket.bStateFlag;
	kNetActorData.m_dwVID		= chrAddPacket.dwVID;
	kNetActorData.m_dwMountVnum	= chrAddPacket.dwMountVnum;
	kNetActorData.m_fRot		= chrAddPacket.angle;

	kNetActorData.m_kAffectFlags.CopyData(0, sizeof(chrAddPacket.dwAffectFlag[0]), &chrAddPacket.dwAffectFlag[0]);
	kNetActorData.m_kAffectFlags.CopyData(32, sizeof(chrAddPacket.dwAffectFlag[1]), &chrAddPacket.dwAffectFlag[1]);

	kNetActorData.SetPosition(chrAddPacket.x, chrAddPacket.y);
	kNetActorData.m_sAlignment=chrAddPacket.sAlignment;
	kNetActorData.m_byPKMode=chrAddPacket.bPKMode;
	kNetActorData.m_stName=chrAddPacket.name;
	__RecvCharacterAppendPacket(&kNetActorData);

	return true;
}

bool CPythonNetworkStream::RecvCharacterUpdatePacket()
{
	TPacketGCCharacterUpdate chrUpdatePacket;
	if (!Recv(sizeof(chrUpdatePacket), &chrUpdatePacket))
		return false;

	SNetworkUpdateActorData kNetUpdateActorData;
	kNetUpdateActorData.m_dwLevel = chrUpdatePacket.dwLevel;
	kNetUpdateActorData.m_dwGuildID	= chrUpdatePacket.dwGuildID;
	kNetUpdateActorData.m_dwMovSpd	= chrUpdatePacket.bMovingSpeed;
	kNetUpdateActorData.m_dwAtkSpd	= chrUpdatePacket.bAttackSpeed;
	kNetUpdateActorData.m_dwArmor	= chrUpdatePacket.awPart[CHR_EQUIPPART_ARMOR];
	kNetUpdateActorData.m_dwWeapon	= chrUpdatePacket.awPart[CHR_EQUIPPART_WEAPON];
	kNetUpdateActorData.m_dwHair	= chrUpdatePacket.awPart[CHR_EQUIPPART_HAIR];
#ifdef ELEMENT_SPELL_WORLDARD
	kNetUpdateActorData.m_dwElementsEffect = chrUpdatePacket.dwElementsEffect;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kNetUpdateActorData.m_dwAcce	= chrUpdatePacket.awPart[CHR_EQUIPPART_ACCE];
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	kNetUpdateActorData.m_dwArrow	= chrUpdatePacket.dwArrow;
#endif
	kNetUpdateActorData.m_dwVID		= chrUpdatePacket.dwVID;

	kNetUpdateActorData.m_kAffectFlags.CopyData(0, sizeof(chrUpdatePacket.dwAffectFlag[0]), &chrUpdatePacket.dwAffectFlag[0]);
	kNetUpdateActorData.m_kAffectFlags.CopyData(32, sizeof(chrUpdatePacket.dwAffectFlag[1]), &chrUpdatePacket.dwAffectFlag[1]);

	kNetUpdateActorData.m_sAlignment	= chrUpdatePacket.sAlignment;
	kNetUpdateActorData.m_byPKMode		= chrUpdatePacket.bPKMode;
	kNetUpdateActorData.m_dwStateFlags	= chrUpdatePacket.bStateFlag;
	kNetUpdateActorData.m_dwMountVnum	= chrUpdatePacket.dwMountVnum;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	kNetUpdateActorData.m_bLanguage = chrUpdatePacket.bLanguage;
#endif

	__RecvCharacterUpdatePacket(&kNetUpdateActorData);

	return true;
}

void CPythonNetworkStream::__RecvCharacterAppendPacket(SNetworkActorData * pkNetActorData)
{
	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetActorData->m_dwVID))
	{
		rkPlayer.SetRace(pkNetActorData->m_dwRace);

		__SetWeaponPower(rkPlayer, pkNetActorData->m_dwWeapon);

		if (rkPlayer.NEW_GetMainActorPtr())
		{
			CPythonBackground::Instance().Update(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY, 0.0f);
			CPythonCharacterManager::Instance().Update();

			{
				std::string strMapName = CPythonBackground::Instance().GetWarpMapName();
				if (strMapName == "metin2_map_deviltower1")
					__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
			}
		}
		else
		{
			__ShowMapName(pkNetActorData->m_lCurX, pkNetActorData->m_lCurY);
		}
	}

	m_rokNetActorMgr->AppendActor(*pkNetActorData);

	if (GetMainActorVID()==pkNetActorData->m_dwVID)
	{
		rkPlayer.SetTarget(0);
		if (m_bComboSkillFlag)
			rkPlayer.SetComboSkillFlag(m_bComboSkillFlag);

		__SetGuildID(pkNetActorData->m_dwGuildID);
		//CPythonApplication::Instance().SkipRenderBuffering(10000);
	}
}

void CPythonNetworkStream::__RecvCharacterUpdatePacket(SNetworkUpdateActorData * pkNetUpdateActorData)
{
	m_rokNetActorMgr->UpdateActor(*pkNetUpdateActorData);

	IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
	if (rkPlayer.IsMainCharacterIndex(pkNetUpdateActorData->m_dwVID))
	{
		__SetGuildID(pkNetUpdateActorData->m_dwGuildID);
		__SetWeaponPower(rkPlayer, pkNetUpdateActorData->m_dwWeapon);

		__RefreshStatus();
		__RefreshAlignmentWindow();
		__RefreshEquipmentWindow();
		__RefreshInventoryWindow();
	}
	else
	{
		rkPlayer.NotifyCharacterUpdate(pkNetUpdateActorData->m_dwVID);
	}
}

bool CPythonNetworkStream::RecvCharacterDeletePacket()
{
	TPacketGCCharacterDelete chrDelPacket;

	if (!Recv(sizeof(chrDelPacket), &chrDelPacket))
	{
		TraceError("CPythonNetworkStream::RecvCharacterDeletePacket - Recv Error");
		return false;
	}

	m_rokNetActorMgr->RemoveActor(chrDelPacket.dwVID);

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
		"BINARY_PrivateShop_Disappear",
		Py_BuildValue("(i)", chrDelPacket.dwVID)
	);

	return true;
}

bool CPythonNetworkStream::RecvCharacterMovePacket()
{
	TPacketGCMove kMovePacket;
	if (!Recv(sizeof(TPacketGCMove), &kMovePacket))
	{
		Tracen("CPythonNetworkStream::RecvCharacterMovePacket - PACKET READ ERROR");
		return false;
	}

	__GlobalPositionToLocalPosition(kMovePacket.lX, kMovePacket.lY);

	SNetworkMoveActorData kNetMoveActorData;
	kNetMoveActorData.m_dwArg=kMovePacket.bArg;
	kNetMoveActorData.m_dwFunc=kMovePacket.bFunc;
	kNetMoveActorData.m_dwTime=kMovePacket.dwTime;
	kNetMoveActorData.m_dwVID=kMovePacket.dwVID;
	kNetMoveActorData.m_fRot=kMovePacket.bRot*5.0f;
	kNetMoveActorData.m_lPosX=kMovePacket.lX;
	kNetMoveActorData.m_lPosY=kMovePacket.lY;
	kNetMoveActorData.m_dwDuration=kMovePacket.dwDuration;

	m_rokNetActorMgr->MoveActor(kNetMoveActorData);

	return true;
}

bool CPythonNetworkStream::RecvOwnerShipPacket()
{
	TPacketGCOwnership kPacketOwnership;

	if (!Recv(sizeof(kPacketOwnership), &kPacketOwnership))
		return false;

	m_rokNetActorMgr->SetActorOwner(kPacketOwnership.dwOwnerVID, kPacketOwnership.dwVictimVID);

	return true;
}

bool CPythonNetworkStream::RecvSyncPositionPacket()
{
	TPacketGCSyncPosition kPacketSyncPos;
	if (!Recv(sizeof(kPacketSyncPos), &kPacketSyncPos))
		return false;

	TPacketGCSyncPositionElement kSyncPos;

	UINT uSyncPosCount=(kPacketSyncPos.wSize-sizeof(kPacketSyncPos))/sizeof(kSyncPos);
	for (UINT iSyncPos=0; iSyncPos<uSyncPosCount; ++iSyncPos)
	{
		if (!Recv(sizeof(TPacketGCSyncPositionElement), &kSyncPos))
			return false;

#ifdef __MOVIE_MODE__
		return true;
#endif __MOVIE_MODE__

		//Tracenf("CPythonNetworkStream::RecvSyncPositionPacket %d (%d, %d)", kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);

		__GlobalPositionToLocalPosition(kSyncPos.lX, kSyncPos.lY);
		m_rokNetActorMgr->SyncActor(kSyncPos.dwVID, kSyncPos.lX, kSyncPos.lY);
	}

	return true;
}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
bool CPythonNetworkStream::RecvBuffNPCUseSkill()
{
	TPacketGCBuffNPCUseSkill packet;
	if (!Recv(sizeof(TPacketGCBuffNPCUseSkill), &packet)) {
		Tracen("CPythonNetworkStream::RecvBuffNPCUseSkill - PACKET READ ERROR");
		return false;
	}
	CInstanceBase* pkInstClone = CPythonCharacterManager::Instance().GetInstancePtr(packet.dVid);
	DWORD dwSkillIndex = packet.dSkillVnum;
	CPythonSkill::TSkillData* pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillIndex, &pSkillData))
		return false;

	DWORD dSkillGrade;
	if (packet.dSkillLevel < 20)
		dSkillGrade = 0;
	else if (packet.dSkillLevel >= 20 && packet.dSkillLevel < 30)
		dSkillGrade = 1;
	else if (packet.dSkillLevel >= 30 && packet.dSkillLevel < 40)
		dSkillGrade = 2;
	else if (packet.dSkillLevel >= 40)
		dSkillGrade = 3;

	DWORD dwMotionIndex = pSkillData->GetSkillMotionIndex(dSkillGrade);
	if (!pkInstClone->NEW_UseSkill(dwSkillIndex, dwMotionIndex, 1, false))
	{
		Tracenf("CPythonPlayer::UseBuffNPCSkill(%d) - ERROR", dwSkillIndex);
	}

	return true;
}
#endif

//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
