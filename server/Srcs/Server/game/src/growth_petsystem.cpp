// --------------------- Petsystem by Aslan v2.0 ----------------------//
//*********************************************************************//
//		socket 0 = IsActive() -- For Active Slot Effect
//		socket 1 = Realtime Socket (UNIX time to Dead)
//		socket 2 = Borntime (UNIX time from Borntime)
//		socket 3 = Bornduration (UNIX Duration Time)
//		socket 4 = DeadTime (UNIX)
//		socket 5 = 
//		
//		attrtype0 	= Pet Level
//		attrvalue0 	= Evolution
//		attrtype1 	= HP Value
//		attrvalue1 	= ATK Value
//		attrtype2 	= DEF Value
//		attrvalue2 	= 
//		attrtype3 	= Skill Lv Slot 1
//		attrvalue3 	= Skill ID Slot 1
//		attrtype4 	= Skill Lv Slot 2
//		attrvalue4 	= Skill ID Slot 2
//		attrtype5 	= Skill Lv Slot 3
//		attrvalue5 	= Skill ID Slot 3
//		attrtype6 	= 
//		attrvalue6 	= 
//
//
//************************************************//

#include "stdafx.h"

#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include "growth_petsystem.h"
#include "db.h"
#include "desc.h"

extern int passes_per_sec;

EVENTINFO(growth_petsystem_event_info)
{
	CGrowthPetSystem* pGrowthPetSystem;
};

EVENTFUNC(growth_petsystem_update_event)
{
	growth_petsystem_event_info* info = dynamic_cast<growth_petsystem_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	CGrowthPetSystem * pGrowthPetSystem = info->pGrowthPetSystem;

	if (NULL == pGrowthPetSystem)
		return 0;
	
	pGrowthPetSystem->Update(0);
	return PASSES_PER_SEC(1) / 4;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CGrowthPetSystem
///////////////////////////////////////////////////////////////////////////////////////

CGrowthPetSystem::CGrowthPetSystem(LPCHARACTER owner, DWORD options)
{
	m_pkOwner = owner;
	m_dwOptions = options;

	this->Reset();
}

CGrowthPetSystem::~CGrowthPetSystem()
{
	this->Destroy();
}

void CGrowthPetSystem::Destroy()
{
	this->Unsummon();
	event_cancel(&m_pkGrowthPetUpdateEvent);
	m_pkGrowthPetUpdateEvent = NULL;
	this->Reset();
}

void CGrowthPetSystem::Reset()
{
	m_dwVnum = 0;
	m_dwVID = 0;
	m_dwLevel = 0;
	m_dwLevelStep = 0;
	m_dwExpMob = 0;
	m_dwExpItem = 0;
	m_dwNextExpMob = 0;
	m_dwNextExpItem = 0;
	m_dwLastActionTime = 0;

	m_pkChar = NULL;

	m_originalMoveSpeed = 0;

	m_dwSummonItemVID = 0;
	m_dwSummonItemID = 0;
	m_dwSummonItemVnum = 0;

	m_dwEvo = 0;
	m_dwAge = 0;
	m_dwBirthday = 0;

	m_dwItemDuration = 0;
	m_dwBornDuration = 0;

	m_dwTimePet = 0;
	m_dwslotimm = 0;
	m_dwImmTime = 0;

	m_dwAgeBonus = 0;
	m_dwPetType = 0;

	for (int s = 0; s < FEED_SLOT_MAX_NUM; ++s) {
		m_dwFeedSlotItem[s] = -1;
	}
	for (int s = 0; s < REVIVE_SLOT_MAX_NUM; ++s) {
		m_dwReviveSlotItem[s] = -1;
	}

	for (int x = 0; x < 3; ++x) {
		m_dwTypeBonus[x] = 0;
		m_dwSkill[x][0] = -1;
		m_dwSkill[x][1] = 0;

		int btype[3] = { 1, 53, 54 };
		m_dwBonus[x][0] = btype[x];
		m_dwBonus[x][1] = 0;
	}
}

void CGrowthPetSystem::SetSummonItem(LPITEM pItem)
{
	if (NULL == pItem)
	{
		m_dwSummonItemVID = 0;
		m_dwSummonItemID = 0;
		m_dwSummonItemVnum = 0;
		return;
	}

	m_dwSummonItemVID = pItem->GetVID();
	m_dwSummonItemID = pItem->GetID();
	m_dwSummonItemVnum = pItem->GetVnum();
}

DWORD CGrowthPetSystem::Summon(LPITEM pSummonItem, bool bSpawnFar)
{
	std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery("SELECT name,level,evolution,exp,expi,bonus0,bonus1,bonus2,skill0,skill0lv,skill1,skill1lv,skill2,skill2lv,item_duration,born_duration,UNIX_TIMESTAMP(birthday),(UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(birthday))/60,age_bonus,pettype,typebonus0,typebonus1,typebonus2 FROM player_petsystem WHERE id = %lu ", pSummonItem->GetID()));
	if (pmsg2->Get()->uiNumRows > 0) {
		MYSQL_ROW row = mysql_fetch_row(pmsg2->Get()->pSQLResult);
		this->m_szName = row[0];
		this->m_dwLevel = atoi(row[1]);
		this->m_dwEvo = atoi(row[2]);
		this->m_dwExpMob = atoi(row[3]);
		this->m_dwExpItem = atoi(row[4]);
		this->m_dwBonus[0][1] = atoi(row[5]);
		this->m_dwBonus[1][1] = atoi(row[6]);
		this->m_dwBonus[2][1] = atoi(row[7]);
		this->m_dwSkill[0][0] = atoi(row[8]);
		this->m_dwSkill[0][1] = atoi(row[9]);
		this->m_dwSkill[1][0] = atoi(row[10]);
		this->m_dwSkill[1][1] = atoi(row[11]);
		this->m_dwSkill[2][0] = atoi(row[12]);
		this->m_dwSkill[2][1] = atoi(row[13]);
		this->m_dwItemDuration = atoi(row[14]);
		this->m_dwBornDuration = atoi(row[15]);
		this->m_dwBirthday = atoi(row[16]);
		this->m_dwAge = atoi(row[17]);
		this->m_dwAgeBonus = atoi(row[18]);
		this->m_dwPetType = atoi(row[19]);
		this->m_dwTypeBonus[0] = atoi(row[20]);
		this->m_dwTypeBonus[1] = atoi(row[21]);
		this->m_dwTypeBonus[2] = atoi(row[22]);
	}
	else {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Failed to summon the pet. ");
		return 0;
	}

	const char * szSpawnEffectFile = "";

	for (int i = 0; i < PET_MAX_NUM; ++i) {
		if (pSummonItem->GetVnum() == PetListTable[i].dwSealVnum) {
			if (GROWTH_PET_CHANGE_LOOK == 0)
				m_dwVnum = PetListTable[i].dwMobVnum[0];
			else if (GROWTH_PET_CHANGE_LOOK == 1) {
				if (GROWTH_PET_CHANGE_LOOK_AT_LEVEL <= m_dwLevel)
					m_dwVnum = PetListTable[i].dwMobVnum[1];
				else
					m_dwVnum = PetListTable[i].dwMobVnum[0];
			}
			else if (GROWTH_PET_CHANGE_LOOK == 2) {
				if (GROWTH_PET_CHANGE_LOOK_AT_EVO <= m_dwEvo)
					m_dwVnum = PetListTable[i].dwMobVnum[1];
				else
					m_dwVnum = PetListTable[i].dwMobVnum[0];
			}
			if (sizeof(PetListTable[i].szSpawnEffectFile) != 0) 
				szSpawnEffectFile = PetListTable[i].szSpawnEffectFile;
		}
	}

	long x = m_pkOwner->GetX();
	long y = m_pkOwner->GetY();
	long z = m_pkOwner->GetZ();

	if (true == bSpawnFar) {
		x += (number(0, 1) * 2 - 1) * number(2000, 2500);
		y += (number(0, 1) * 2 - 1) * number(2000, 2500);
	}
	else {
		x += number(-100, 100);
		y += number(-100, 100);
	}

	if (0 != m_pkChar) {
		m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y);
		m_dwVID = m_pkChar->GetVID();

		return m_dwVID;
	}

	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(m_dwVnum, m_pkOwner->GetMapIndex(), x, y, z, false, (int)(m_pkOwner->GetRotation() + 180), false);

	if (0 == m_pkChar) {
		sys_err("[CGrowthPetSystem::Summon] Failed to summon the pet. (vnum: %d)", m_dwVnum);
		return 0;
	}

	this->SetSummonItem(pSummonItem);

	m_pkChar->SetGrowthPet();
	m_pkChar->SetEmpire(m_pkOwner->GetEmpire());
	m_dwVID = m_pkChar->GetVID();

	this->SetName(m_szName.c_str());
	m_pkChar->SetLevel(static_cast<char>(m_dwLevel));
	this->SetNextExp();

	m_dwItemDuration = pSummonItem->GetSocket(1);
	
	pSummonItem->SetSocket(0, true);					// Is Pet Active
	pSummonItem->SetSocket(2, m_dwBirthday);			// Born Duration
	pSummonItem->SetSocket(3, m_dwBornDuration);		// Pet Birthday (Borntime)
	
	pSummonItem->SetForceAttribute(0, m_dwLevel, m_dwEvo);
	pSummonItem->SetForceAttribute(1, m_dwBonus[0][1], m_dwBonus[1][1]);
	pSummonItem->SetForceAttribute(2, m_dwBonus[2][1], 0);
	
	pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
	pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
	pSummonItem->SetForceAttribute(5, m_dwSkill[2][1], m_dwSkill[2][0]);
	pSummonItem->Lock(true);
	m_pkChar->Show(m_pkOwner->GetMapIndex(), x, y, z);
	
	size_t Size = strlen(szSpawnEffectFile);
	if (Size != 0)
		m_pkChar->SpecificEffectPacket(szSpawnEffectFile);

	if (GetExpMonster() >= GetNextExpFromMob() / 4)
		m_dwLevelStep = 1;
	if (GetExpMonster() >= GetNextExpFromMob() / 4 * 2)
		m_dwLevelStep = 2;
	if (GetExpMonster() >= GetNextExpFromMob() / 4 * 3)
		m_dwLevelStep = 3;

	SendPetInfoPacket();
	growth_petsystem_event_info* info = AllocEventInfo<growth_petsystem_event_info>();
	info->pGrowthPetSystem = this;
	m_pkGrowthPetUpdateEvent = event_create(growth_petsystem_update_event, info, PASSES_PER_SEC(1) / 4);

	this->GiveBuff();
	m_pkOwner->ComputePoints();

	m_pkChar->UpdatePacket();
	m_pkOwner->UpdatePacket();


	return m_dwVID;
}

void CGrowthPetSystem::Unsummon()
{
	if (true == this->IsSummoned())
	{
		SaveSQL();

		LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
		if (pSummonItem != NULL){
			pSummonItem->SetSocket(0, false);					// Is Pet Active

			pSummonItem->SetForceAttribute(1, m_dwBonus[0][1], m_dwBonus[1][1]);
			pSummonItem->SetForceAttribute(2, m_dwBonus[2][1], 0);

			pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
			pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
			pSummonItem->SetForceAttribute(5, m_dwSkill[2][1], m_dwSkill[2][0]);
			pSummonItem->Lock(false);
		}
		this->SetSummonItem(NULL);
		if (NULL != m_pkOwner)
			m_pkOwner->ComputePoints();

		if (NULL != m_pkChar) {
			M2_DESTROY_CHARACTER(m_pkChar);
			m_pkChar = NULL;
		}

		event_cancel(&m_pkGrowthPetUpdateEvent);
		m_pkGrowthPetUpdateEvent = NULL;

		this->ClearBuff();
		SendPetInfoPacket(true);
		this->Reset();
	}
}

void CGrowthPetSystem:: IncreasePetBonus() {
	
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_LEVELUP"));

	int age_bonus = m_dwAgeBonus;
	if (age_bonus > NEW_PET_AGE_MAX_BONUS_TIMES)
		age_bonus = NEW_PET_AGE_MAX_BONUS_TIMES;
	
	if (m_dwPetType == 1 || m_dwPetType == 3 || m_dwPetType == 5 || m_dwPetType == 7) {
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwATKJumpPerLv == 0) {
			int bonus = number(PetTypeAttrTable[m_dwPetType].dwATK[0], PetTypeAttrTable[m_dwPetType].dwATK[1]) + age_bonus;
			m_dwBonus[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_1"), bonus_info);
		}
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwHPJumpPerLv == 0) {
			int bonus = number(PetTypeAttrTable[m_dwPetType].dwHP[0], PetTypeAttrTable[m_dwPetType].dwHP[1]) + age_bonus;
			m_dwBonus[1][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_2"), bonus_info);
		}
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwDEFJumpPerLv == 0) {
			int bonus = number(PetTypeAttrTable[m_dwPetType].dwDEF[0], PetTypeAttrTable[m_dwPetType].dwDEF[1]) + age_bonus;
			m_dwBonus[2][1] += bonus;
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_3"), bonus_info);
		}
	}
	else {
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwATKJumpPerLv == 0) {
			int bonus = m_dwTypeBonus[0] + age_bonus;
			m_dwBonus[0][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_1"), bonus_info);
		}
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwHPJumpPerLv == 0) {
			int bonus = m_dwTypeBonus[1] + age_bonus;
			m_dwBonus[1][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_2"), bonus_info);
		}
		if (m_dwLevel % PetTypeAttrTable[m_dwPetType].dwDEFJumpPerLv == 0) {
			int bonus = m_dwTypeBonus[2] + age_bonus;
			m_dwBonus[2][1] += bonus; 
			float bonus_info = (float) bonus / 10;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_BONUS_3"), bonus_info);
		}
	}
	
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	SendPetInfoPacket();
	if (pSummonItem != NULL){
		pSummonItem->SetForceAttribute(1, m_dwBonus[0][1], m_dwBonus[1][1]);
		pSummonItem->SetForceAttribute(2, m_dwBonus[2][1], 0);
	}

	SaveSQL();
	GiveBuff();
	
	if (NULL != m_pkOwner)
		m_pkOwner->ComputePoints();
}

bool CGrowthPetSystem::ChangePetAttr(LPITEM SpecifiyItem)
{
	LPITEM pSummonItem;
	if (SpecifiyItem == NULL)
		pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	else
		pSummonItem = SpecifiyItem;

	int age_bonus = m_dwAgeBonus;
	if (age_bonus > NEW_PET_AGE_MAX_BONUS_TIMES)
		age_bonus = NEW_PET_AGE_MAX_BONUS_TIMES;
	
	int pettype = 0;
	int typebonus[3];
	int rand_typebonus[3];
	int rand_bonus[3];
	for (int i = 0; i < 3; i++) {
		typebonus[i] = 0;
		rand_typebonus[i] = 0;
		rand_bonus[i] = 0;
	}
	
	int rand = number(1, 95);

	for (int i = 0; i < PET_MAX_NUM; ++i) {
		if (pSummonItem->GetVnum() == PetListTable[i].dwSealVnum) {
			rand_bonus[0] += number(PetListTable[i].dwBornAttr[0][0], PetListTable[i].dwBornAttr[0][1]);
			rand_bonus[1] += number(PetListTable[i].dwBornAttr[1][0], PetListTable[i].dwBornAttr[1][1]);
			rand_bonus[2] += number(PetListTable[i].dwBornAttr[2][0], PetListTable[i].dwBornAttr[2][1]);
		}
	}

	for (int i = 0; i < 9; ++i) { 
		if (rand >= PetTypeAttrTable[i].dwChance[0] && rand <= PetTypeAttrTable[i].dwChance[1]) {
			pettype = i; 
		}
	}

	if (pettype == 2 || pettype == 4 || pettype == 6 || pettype == 8) {
		rand_typebonus[0] = number(PetTypeAttrTable[pettype].dwATK[0], PetTypeAttrTable[pettype].dwATK[1]);
		rand_typebonus[1] = number(PetTypeAttrTable[pettype].dwHP[0], PetTypeAttrTable[pettype].dwHP[1]);
		rand_typebonus[2] = number(PetTypeAttrTable[pettype].dwDEF[0], PetTypeAttrTable[pettype].dwDEF[1]);
	}
	
	for (int i = 1; i < m_dwLevel+1; ++i) 
	{
		if (pettype == 1 || pettype == 3 || pettype == 5 || pettype == 7) {
			if (i % PetTypeAttrTable[pettype].dwATKJumpPerLv == 0)  { rand_bonus[0] += number(PetTypeAttrTable[pettype].dwATK[0], PetTypeAttrTable[pettype].dwATK[1]) + age_bonus; }
			if (i % PetTypeAttrTable[pettype].dwHPJumpPerLv == 0) { rand_bonus[1] += number(PetTypeAttrTable[pettype].dwHP[0], PetTypeAttrTable[pettype].dwHP[1]) + age_bonus; }
			if (i % PetTypeAttrTable[pettype].dwDEFJumpPerLv == 0) { rand_bonus[2] += number(PetTypeAttrTable[pettype].dwDEF[0], PetTypeAttrTable[pettype].dwDEF[1]) + age_bonus; }
		}
		else {
			if (i % PetTypeAttrTable[pettype].dwATKJumpPerLv == 0) { rand_bonus[0] += rand_typebonus[0] + age_bonus; }
			if (i % PetTypeAttrTable[pettype].dwHPJumpPerLv == 0) { rand_bonus[1] += rand_typebonus[1] + age_bonus; }
			if (i % PetTypeAttrTable[pettype].dwDEFJumpPerLv == 0) { rand_bonus[2] += rand_typebonus[2] + age_bonus; }
		}
	}

	m_dwPetType = pettype;

	m_dwBonus[0][1] = rand_bonus[0];
	m_dwBonus[1][1] = rand_bonus[1];
	m_dwBonus[2][1] = rand_bonus[2];
		
	m_dwTypeBonus[0] = rand_typebonus[0];
	m_dwTypeBonus[1] = rand_typebonus[1];
	m_dwTypeBonus[2] = rand_typebonus[2];

	if (pSummonItem != NULL){
		pSummonItem->SetForceAttribute(1, m_dwBonus[0][1], m_dwBonus[1][1]);
		pSummonItem->SetForceAttribute(2, m_dwBonus[2][1], 0);
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "GrowthPetGetType %d %d", 0, m_dwPetType);
	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "ASLAN_GROWTH_PET_RENEW_ATTR"), m_dwPetType);
	SaveSQL(pSummonItem);

	if (SpecifiyItem == NULL) {
#ifdef NEW_PET_SYSTEM_CHANGE_ATTR_EFFECTS
		m_pkChar->EffectPacket(SE_EFFECT_PET_CHANGE_ATTR);
#endif
		SendPetInfoPacket();

		GiveBuff();
	}
	return true;
}

bool CGrowthPetSystem::IncreasePetSkill(int skill) {
	
	int itemvnum = 55009 + skill;
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	
	if (GetLevel() < 40 && m_dwEvo < 0){
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_LEARN_SKILLS"));
		return false;
	}

	for (int i = 0; i < 3; ++i) {
		if (m_dwSkill[i][0] == skill) {
			if (m_dwSkill[i][1] < 20) {
				m_dwSkill[i][1] += 1;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_UPGRADE_SKILL"), m_dwSkill[i][1]);
				SendPetInfoPacket();
				
				pSummonItem->SetForceAttribute(i+3, m_dwSkill[i][1], m_dwSkill[i][0]);
				GiveBuff();
	
				return true;
			}
			else {
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_SKILL_IS_MAX_LEVEL"));
				return false;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (m_dwSkill[i][0] == 0) {
			m_dwSkill[i][0] = skill;
			m_dwSkill[i][1] = 1;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_LEARN_NEW_SKILL"));
			SendPetInfoPacket();
			
			pSummonItem->SetForceAttribute(i+3, m_dwSkill[i][1], m_dwSkill[i][0]);
			GiveBuff();

			return true;
		}
	}

	m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_LEARN_NEW_SKILL"));
	return false;
}
 
void CGrowthPetSystem::RemovePetSkill(int skillslot, int itemvnum) {
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	
	if (itemvnum == 55030) // Pet Revertus
	{
		int skilldesc = skillslot+1;
		
		if (m_dwSkill[skillslot][1] > 0)
		{
			m_dwSkill[skillslot][0] = 0;
			m_dwSkill[skillslot][1] = 0;
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_REMOVE_SKILL"), skilldesc);
			SendPetInfoPacket();
			pSummonItem->SetForceAttribute(skillslot+3, m_dwSkill[skillslot][1], m_dwSkill[skillslot][0]);
		}
	} 
	else if (itemvnum == 55029) // Pet Reverti
	{

		if (m_dwEvo >= 1)
		{
			m_dwSkill[0][0] = 0;
			m_dwSkill[0][1] = 0;
			pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
		}
		if (m_dwEvo >= 2)
		{
			m_dwSkill[1][0] = 0;
			m_dwSkill[1][1] = 0;
			pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
		}
		if (m_dwEvo >= 3)
		{
			m_dwSkill[2][0] = 0;
			m_dwSkill[2][1] = 0;
			pSummonItem->SetForceAttribute(5, m_dwSkill[2][1], m_dwSkill[2][0]);
		}

		SendPetInfoPacket();

		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_REMOVE_ALL_SKILLS"));
	}
	
	SaveSQL();
	GiveBuff();
}

void CGrowthPetSystem::DoPetSkillAlone() 
{
	/* if (GetLevel() < 81 || m_dwEvo < 3)
		return; */
	
	/***************/
	// Restauration
	/***************/
	if (m_dwSkill[0][0] == 10 && get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= PetSkillTable[10].dwCooltime || m_dwSkill[1][0] == 10 && get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= PetSkillTable[10].dwCooltime || m_dwSkill[2][0] == 10 && get_global_time() - m_pkOwner->GetNewPetSkillCD(0) >= PetSkillTable[10].dwCooltime)
	{
		if (m_pkOwner->GetHPPct() <= 20 && m_pkOwner->GetHPPct() > 0) 
		{
			int SkillslotLevel = 0;
			if (m_dwSkill[0][0] == 10)
				SkillslotLevel = 0;
			else if (m_dwSkill[1][0] == 10)
				SkillslotLevel = 1;
			else if (m_dwSkill[2][0] == 10)
				SkillslotLevel = 2;
			
			m_pkOwner->SetNewPetSkillCD(0, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "GrowthPetSetSkillCooltime %d %d", SkillslotLevel, PetSkillTable[10].dwCooltime);
			
			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[1][m_dwSkill[SkillslotLevel][1]];
			
			if (random <= chance)
			{
				int HealHP = PetSkillTable[10].dwLevelValue[m_dwSkill[SkillslotLevel][1]];
				m_pkOwner->PointChange(POINT_HP, HealHP);
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_RESTAURATION);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_RESTAURATION);
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_RESTAURATION"), HealHP);
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_RESTAURATION_FAIL"));
		}
	}

	/***************/
	// Immortal
	/***************/
	if (m_dwSkill[0][0] == 17 && get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= PetSkillTable[17].dwCooltime || m_dwSkill[1][0] == 17 && get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= PetSkillTable[17].dwCooltime || m_dwSkill[2][0] == 17 && get_global_time() - m_pkOwner->GetNewPetSkillCD(1) >= PetSkillTable[17].dwCooltime)
	{
		if (m_pkOwner->GetHPPct() < 50 && m_pkOwner->IsPosition(POS_FIGHTING))
		{
			int SkillslotLevel = 0;
			if (m_dwSkill[0][0] == 17)
				SkillslotLevel = 0;
			else if (m_dwSkill[1][0] == 17)
				SkillslotLevel = 1;
			else if (m_dwSkill[2][0] == 17)
				SkillslotLevel = 2;

			m_pkOwner->SetNewPetSkillCD(1, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "GrowthPetSetSkillCooltime %d %d", SkillslotLevel, PetSkillTable[17].dwCooltime);

			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[2][m_dwSkill[SkillslotLevel][1]];
			
			if (random <= chance)
			{
				m_pkOwner->SetImmortal(1);
				m_dwImmTime = get_global_time();
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_IMMORTAL);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_IMMORTAL);
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_IMMORTAL"));
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_IMMORTAL_CANNOT_DEAD"));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_IMMORTAL_FAIL"));
		}
	}

	/***************/
	// Panacea
	/***************/
	if (m_dwSkill[0][0] == 18 && get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= PetSkillTable[18].dwCooltime || m_dwSkill[1][0] == 18 && get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= PetSkillTable[18].dwCooltime || m_dwSkill[2][0] == 18 && get_global_time() - m_pkOwner->GetNewPetSkillCD(2) >= PetSkillTable[18].dwCooltime)
	{
		if (m_pkOwner->FindAffect(AFFECT_STUN) || m_pkOwner->FindAffect(AFFECT_SLOW) || m_pkOwner->FindAffect(AFFECT_POISON))
		{
			int SkillslotLevel = 0;
			if (m_dwSkill[0][0] == 18)
				SkillslotLevel = 0;
			else if (m_dwSkill[1][0] == 18)
				SkillslotLevel = 1;
			else if (m_dwSkill[2][0] == 18)
				SkillslotLevel = 2;
			
			m_pkOwner->SetNewPetSkillCD(2, get_global_time());
			m_pkOwner->ChatPacket(CHAT_TYPE_COMMAND, "GrowthPetSetSkillCooltime %d %d", SkillslotLevel, PetSkillTable[18].dwCooltime);
			
			int random = number(1, 100);
			int chance = Pet_SKill_TablePerc[3][m_dwSkill[SkillslotLevel][1]];
			
			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, "Skill 18: Random %d <= %d Chance", random, chance);
			
			if (random <= chance)
			{
				m_pkOwner->RemoveBadAffect();
				m_pkOwner->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
				m_pkChar->EffectPacket(SE_EFFECT_PET_SKILL_PANACEA);
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_PANACEA"));
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_PANACEA_REMOVE_BAD_AFFECT"));
			}
			else
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_USE_SKILL_PANACEA_FAIL"));
		}
	}
	
	/***************/
	// Featherlight
	/***************/
	if (m_dwSkill[0][0] == 23 && get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= PetSkillTable[23].dwCooltime || m_dwSkill[1][0] == 23 && get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= PetSkillTable[23].dwCooltime || m_dwSkill[2][0] == 23 && get_global_time() - m_pkOwner->GetNewPetSkillCD(3) >= PetSkillTable[23].dwCooltime)
	{
	}
}

void CGrowthPetSystem::GiveBuff()
{
	this->ClearBuff();
	int cbonus[3] = { m_pkOwner->GetMaxHP(), m_pkOwner->GetPoint(POINT_ATT_GRADE), m_pkOwner->GetPoint(POINT_DEF_GRADE) };
	for (int i = 0; i < 3; ++i) {
		m_pkOwner->AddAffect(AFFECT_GROWTH_PET, aApplyInfo[m_dwBonus[i][0]].bPointType, (int)(cbonus[i]*m_dwBonus[i][1]/1000), 0,  60 * 60 * 24 * 365, 0, false);
	}

	for (int slot = 0; slot < 3; slot++) {
		if (PetSkillTable[m_dwSkill[slot][0]].bIsActiveSkill == false) {
			m_pkOwner->AddAffect(AFFECT_GROWTH_PET, PetSkillTable[m_dwSkill[slot][0]].dwAffect, PetSkillTable[m_dwSkill[slot][0]].dwLevelValue[m_dwSkill[slot][1] - 1], 0, 60 * 60 * 24 * 365, 0, false);
		}
	}
	m_pkOwner->UpdatePacket();
}

void CGrowthPetSystem::ClearBuff()
{
	m_pkOwner->RemoveAffect(AFFECT_GROWTH_PET);
	return ;
}

void CGrowthPetSystem::SetItemFeed(int pos, int invpos) 
{
	if (m_dwFeedSlotItem[pos] != -1)
		return;

	if (pos >= 0 && pos < FEED_SLOT_MAX_NUM)
		m_dwFeedSlotItem[pos] = invpos;
}

bool CGrowthPetSystem::CheckFeedItem(BYTE wndType, LPITEM item) 
{
	if (wndType == 0) {	// Feed Lifetime
		if (item->GetType() == ITEM_PET_ASLAN && item->GetSubType() >= PET_EGG_ASLAN && item->GetSubType() <= PET_SEAL_ASLAN ||
			item->GetType() == ITEM_PET_ASLAN && item->GetSubType() == PET_FEED_ASLAN ||
			item->GetVnum() == 55001)
			return true;
	}
	if (wndType == 2) { // Feed Item-EXP
		if (item->GetType() == ITEM_WEAPON || item->GetType() == ITEM_ARMOR || item->GetType() == ITEM_PET_ASLAN && item->GetSubType() == PET_FEED_ASLAN)
			return true;
	}
	if (wndType == 3) { // Revive Pet
		if (item->GetVnum() == 55100)
			return true;
	}
	return false;
}

void CGrowthPetSystem::ResetItemFeed() {
	for (int slot = 0; slot < FEED_SLOT_MAX_NUM; ++slot) {
		m_dwFeedSlotItem[slot] = -1;
	}
}

void CGrowthPetSystem::ItemFeed(int type)
{					
	if (type != 0 && type != 2)
		return;

	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	BOOL isFullLifeTime = false;

	for (int slot = 0; slot < FEED_SLOT_MAX_NUM; ++slot)
	{
		if (m_dwFeedSlotItem[slot] != -1) {
			LPITEM item = m_pkOwner->GetInventoryItem(m_dwFeedSlotItem[slot]);
			if (!item) {
				ResetItemFeed();
				return;
			}
			if (item->GetID() == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetID()) {
				ResetItemFeed();
				return;
			}
			if (m_pkOwner->GetExchange() || m_pkOwner->GetMyShop() || m_pkOwner->GetShopOwner() || m_pkOwner->IsOpenSafebox() || m_pkOwner->IsCubeOpen()) {
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "ASLAN_GROWTH_PET_CANNOT_FEED"));
				ResetItemFeed();
				return;
			}
			
			// Lifetime
			if(type == 0)
			{
				if (CheckFeedItem(0, item))
				{
					int give_lifetime = 0;

					if (item->GetVnum() == 55001)
						give_lifetime = m_dwBornDuration / 2;
					else if (item->GetType() == ITEM_PET_ASLAN && item->GetSubType() >= PET_EGG_ASLAN && item->GetSubType() <= PET_SEAL_ASLAN) {
						BYTE get_perc = number(3, 5);
						give_lifetime = m_dwBornDuration * get_perc / 100;
					}

					if ((m_dwItemDuration - time(0)) + give_lifetime > m_dwBornDuration) {
						m_dwItemDuration = time(0) + m_dwBornDuration;
						isFullLifeTime = true;
					}
					else
						m_dwItemDuration += give_lifetime;

					pSummonItem->SetSocket(1, m_dwItemDuration);
					SendPetInfoPacket();
					if (item->GetType() == ITEM_PET_ASLAN && item->GetSubType() == PET_SEAL_ASLAN)
						DBManager::instance().DirectQuery("DELETE from player_petsystem WHERE id='%d'", item->GetID());
					ITEM_MANAGER::instance().RemoveItem(item);
					SaveSQL();

					if (isFullLifeTime == true) {
						ResetItemFeed();
						m_pkOwner->FlushDelayedSaveItem();
						return;
					}
				}
			}
			
			// Item-EXP
			else if(type == 2)
			{
				if (GetExpItem() >= GetNextExpFromItem())
					return;

				if (CheckFeedItem(2, item))
				{
					if (m_dwLevel >= GROWTH_PET_MAX_LEVEL) {
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_FEED_MAX_LEVEL"));
						ResetItemFeed();
						return;
					}
					if(GetExpItem() >= GetNextExpFromItem()) {
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_FEED_FULL_IEXP"));
						ResetItemFeed();
						return;
					}
					if (item->GetType() == ITEM_PET_ASLAN && item->GetSubType() == PET_FEED_ASLAN) {
						for (int i = 0; i < (sizeof PetSpecialItemExpTable / sizeof PetSpecialItemExpTable[0]); ++i) {
							if (item->GetVnum() == PetSpecialItemExpTable[i].dwItemVnum && m_dwEvo == PetSpecialItemExpTable[i].dwEvolution) {
								UpdateExperience((GetNextExpFromItem() / 100) * PetSpecialItemExpTable[i].dwGivePercent, 1);
								break;
							}
						}
					}
					else
						UpdateExperience(item->GetShopBuyPrice() / 2, 1);

					m_pkOwner->CreateFly(FLY_SP_BIG, GetCharacter());
					ITEM_MANAGER::instance().RemoveItem(item);
				}
			}
		}
	}

	m_pkOwner->FlushDelayedSaveItem();
	ResetItemFeed();
}

void CGrowthPetSystem::SetName(const char* name)
{
	std::string petName = name;

	if (true == IsSummoned())
		m_pkChar->SetName(petName);
}

bool CGrowthPetSystem::CanEvolution()
{
	BOOL isFullEXPMob = false;
	BOOL isFullEXPItem = false;

	if (GetExpMonster() >= GetNextExpFromMob())
		isFullEXPMob = true;

	if (GetExpItem() >= GetNextExpFromItem())
		isFullEXPItem = true;

	if (PetEvoTable[m_dwEvo].dwUpgradeType == 0 && m_dwLevel >= PetEvoTable[m_dwEvo].dwLevel && isFullEXPMob && isFullEXPItem && m_dwEvo < GROWTH_PET_MAX_EVO)
		return true;
	else if (PetEvoTable[m_dwEvo].dwUpgradeType == 1 && m_dwLevel >= PetEvoTable[m_dwEvo].dwLevel && m_dwEvo < GROWTH_PET_MAX_EVO)
		return true;

	return false;
}

void CGrowthPetSystem::SetLevel(DWORD level)
{
	m_pkChar->SetLevel(static_cast<char>(level));
	m_dwLevel = level;
	m_dwExpMob = 0;
	m_dwExpItem = 0;
	m_dwLevelStep = 0;
	m_pkChar->SetExp(0);
	m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 1, GetLevel(), 1);
	SendPetInfoPacket();
	SetNextExp();
	m_pkChar->UpdatePacket();
	m_pkOwner->UpdatePacket();

	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
	pSummonItem->SetForceAttribute(0, m_dwLevel, m_dwEvo);
}

void CGrowthPetSystem::SetEvolution(int evo) {
	LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

	if (evo == 0) {
		m_dwEvo = 0;
		m_dwSkill[0][0] = -1;
		m_dwSkill[1][0] = -1;
		m_dwSkill[2][0] = -1;
		pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
		pSummonItem->SetForceAttribute(3, m_dwSkill[1][1], m_dwSkill[1][0]);
		pSummonItem->SetForceAttribute(3, m_dwSkill[2][1], m_dwSkill[2][0]);
	}
	if (evo == 1) {
		m_dwEvo = 1;
		m_dwSkill[0][0] = 0;
		m_dwSkill[1][0] = -1;
		m_dwSkill[2][0] = -1;
		pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
		pSummonItem->SetForceAttribute(3, m_dwSkill[1][1], m_dwSkill[1][0]);
		pSummonItem->SetForceAttribute(3, m_dwSkill[2][1], m_dwSkill[2][0]);
	}
	if (evo == 2) {
		m_dwEvo = 2;
		m_dwSkill[0][0] = 0;
		m_dwSkill[1][0] = 0;
		m_dwSkill[2][0] = -1;
		pSummonItem->SetForceAttribute(4, m_dwSkill[0][1], m_dwSkill[0][0]);
		pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
		pSummonItem->SetForceAttribute(4, m_dwSkill[2][1], m_dwSkill[2][0]);
	}
	if (evo == 3) {
		m_dwEvo = 3;
		m_dwSkill[0][0] = 0;
		m_dwSkill[1][0] = 0;
		m_dwSkill[2][0] = 0;
		pSummonItem->SetForceAttribute(4, m_dwSkill[0][1], m_dwSkill[0][0]);
		pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
		pSummonItem->SetForceAttribute(4, m_dwSkill[2][1], m_dwSkill[2][0]);
	}
	SendPetInfoPacket();
}

bool CGrowthPetSystem::IncreasePetEvolution() {
	if (m_dwEvo < GROWTH_PET_MAX_EVO) {
		if (CanEvolution()) {

			for (int items = 0; items < GROWTH_PET_EVO_MAX_ITEMS; ++items) {
				if (PetEvoTable[m_dwEvo].dwItemVnum[items] != 0) {
					if (m_pkOwner->CountSpecifyItem(PetEvoTable[m_dwEvo].dwItemVnum[items]) < PetEvoTable[m_dwEvo].dwItemCount[items]) {
						m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "ASLAN_GROWTH_PET_EVO_NOT_ENOUGT_ITEMS"));
						return false;
					}
				}
			}

			for (int removeitems = 0; removeitems < GROWTH_PET_EVO_MAX_ITEMS; ++removeitems) {
				m_pkOwner->RemoveSpecifyItem(PetEvoTable[m_dwEvo].dwItemVnum[removeitems], PetEvoTable[m_dwEvo].dwItemCount[removeitems]);
			}
			if (PetEvoTable[m_dwEvo].dwUpgradeType == 0)
				SetLevel(GetLevel() + 1);

			m_dwEvo += 1;

			m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_INCREASE_EVO"), m_dwEvo);

			LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());
			pSummonItem->SetForceAttribute(5, m_dwLevel, m_dwEvo);
			
			if (m_dwEvo == 1) {
				m_dwSkill[0][0] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CAN_LEARN_NEW_SKILL"));
				pSummonItem->SetForceAttribute(3, m_dwSkill[0][1], m_dwSkill[0][0]);
			}
			if (m_dwEvo == 2) {
				m_dwSkill[1][0] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CAN_LEARN_NEW_SKILL"));
				pSummonItem->SetForceAttribute(4, m_dwSkill[1][1], m_dwSkill[1][0]);
			}
			if (m_dwEvo == 3) {
				m_dwSkill[2][0] = 0;
				m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CAN_LEARN_NEW_SKILL"));
				pSummonItem->SetForceAttribute(5, m_dwSkill[2][1], m_dwSkill[2][0]);
			}

			for (int i = 0; i < PET_MAX_NUM; ++i) {
				if (pSummonItem->GetVnum() == PetListTable[i].dwSealVnum) {
					if (GROWTH_PET_CHANGE_LOOK == 1) {
						if (GROWTH_PET_CHANGE_LOOK_AT_LEVEL <= this->GetLevel()) {
							LPITEM SummonItem = pSummonItem;
							Unsummon();
							Summon(SummonItem);
						}
					}
					else if (GROWTH_PET_CHANGE_LOOK == 2) {
						if (GROWTH_PET_CHANGE_LOOK_AT_EVO <= this->GetEvolution()) {
							LPITEM SummonItem = pSummonItem;
							Unsummon();
							Summon(SummonItem);
						}
					}
				}
			}
			SendPetInfoPacket();
			return true;
		}
		else
			return false;
	}
	else {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_MAX_EVOLUTION"));
		return false;
	}
	return true;
}

void CGrowthPetSystem::UpdateExperience(DWORD exp, int mode)
{
	if (GetLevel() >= GROWTH_PET_MAX_LEVEL) {
		SendPetInfoPacket();
		return;
	}

	if (exp < 0)
		return;

	BOOL isFullEXPMob = false;
	BOOL isFullEXPItem = false;

	if (mode == 0) {
		if (GetExpMonster() + exp >= GetNextExpFromMob())
			m_dwExpMob = GetNextExpFromMob();
		else 
		{
			m_dwExpMob += exp;
			if (GetLevelStep() < 4)
			{
				if (GetExpMonster() >= GetNextExpFromMob() && m_dwLevelStep == 3) {
					m_dwLevelStep = 4;
					if (GetExpItem() < GetNextExpFromItem())
						m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
				}
				else if (GetExpMonster() >= GetNextExpFromMob() / 4 * 3 && m_dwLevelStep == 2) {
					m_dwLevelStep = 3;
					m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
				}
				else if (GetExpMonster() >= GetNextExpFromMob() / 4 * 2 && m_dwLevelStep == 1) {
					m_dwLevelStep = 2;
					m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
				}
				else if (GetExpMonster() >= GetNextExpFromMob() / 4 && m_dwLevelStep == 0) {
					m_dwLevelStep = 1;
					m_pkChar->SendPetLevelUpEffect(m_pkChar->GetVID(), 25, GetLevel(), 1);
				}
			}
		}
	}
	if (mode == 1) {
		if (GetExpItem() + exp >= GetNextExpFromItem()) 
			m_dwExpItem = GetNextExpFromItem();
		else
			m_dwExpItem += exp;
	}

	if (GetExpMonster() >= GetNextExpFromMob())
		isFullEXPMob = true;
	if (GetExpItem() >= GetNextExpFromItem())
		isFullEXPItem = true;

	SendPetInfoPacket();

	if (m_dwEvo < GROWTH_PET_MAX_EVO && PetEvoTable[m_dwEvo].dwLevel <= m_dwLevel && PetEvoTable[m_dwEvo].dwUpgradeType == 0 && isFullEXPMob && isFullEXPItem)
		return;

	if (GetExpMonster() >= GetNextExpFromMob() && GetExpItem() >= GetNextExpFromItem()) {
		SetLevel(GetLevel() + 1);
		IncreasePetBonus();
	}
}

void CGrowthPetSystem::SetNextExp()
{
	m_dwNextExpMob = m_pkChar->PetGetNextExp(0);
	m_dwNextExpItem = m_pkChar->PetGetNextExp(1);
}

void CGrowthPetSystem::SetReviveItem(int pos, int invpos)
{
	if (m_dwReviveSlotItem[pos] != -1)
		return;

	if (pos >= 0 && pos < REVIVE_SLOT_MAX_NUM)
		m_dwReviveSlotItem[pos] = invpos;
}

void CGrowthPetSystem::ResetReviveItem() {
	for (int slot = 0; slot < REVIVE_SLOT_MAX_NUM; ++slot) {
		m_dwReviveSlotItem[slot] = -1;
	}
}

void CGrowthPetSystem::RevivePet(LPITEM SealItem, bool bIsSpecialItem)
{
	if (m_pkOwner->GetExchange() || m_pkOwner->GetMyShop() || m_pkOwner->GetShopOwner() || m_pkOwner->IsOpenSafebox() || m_pkOwner->IsCubeOpen()) {
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT( "ASLAN_GROWTH_PET_CANNOT_REVIVE"));
		ResetReviveItem();
		return;
	}
	if (SealItem->GetType() == ITEM_PET_ASLAN && SealItem->GetSubType() == PET_SEAL_ASLAN) {
		if (SealItem->GetSocket(4) == 0 || SealItem->GetSocket(1) > time(0)) {
			ResetReviveItem();
			return;
		}

		int AgeSeconds = SealItem->GetSocket(4) - SealItem->GetSocket(2);
		int AgeDays = 0;
		if (AgeSeconds < 86400)
			AgeDays = 1;
		else {
			while (AgeSeconds > 86400) {
				AgeSeconds -= 86400; 
				AgeDays += 1; 
			}
		}
		int ReviveItemCount = 0;
		for (int slot = 0; slot < REVIVE_SLOT_MAX_NUM; ++slot) {
			if (m_dwReviveSlotItem[slot] != -1) {
				LPITEM item = m_pkOwner->GetInventoryItem(m_dwReviveSlotItem[slot]);
				if (!CheckFeedItem(3, item))
					return;

				int iCount = MIN(AgeDays, item->GetCount());
				item->SetCount(item->GetCount() - iCount);
				ReviveItemCount += iCount;
				AgeDays -= iCount;
				if (AgeDays < 0)
					break;
			}
		}
		if (ReviveItemCount == 0)
			return;

		SealItem->SetSocket(1, time(0) + SealItem->GetSocket(3));
		SealItem->SetSocket(2, time(0) - (86400 * ReviveItemCount));
		SealItem->SetSocket(4, 0);
		SealItem->StartCheckPetDeadEvent();

		DBManager::instance().DirectQuery (
			"UPDATE player_petsystem SET item_duration=%d, birthday=FROM_UNIXTIME(%d) WHERE id = %lu",
			SealItem->GetSocket(1),
			SealItem->GetSocket(2),
			ITEM_MANAGER::instance().FindByVID(SealItem->GetVID())->GetID()
		);
	}

	ResetReviveItem();
}

void CGrowthPetSystem::UpdateTime() 
{
	DoPetSkillAlone();
	m_dwTimePet += 1;

	if (m_dwTimePet >= 60) {
		m_dwTimePet = 0;
		
		m_dwAge += 1;
		SendPetInfoPacket();
	}
}

bool CGrowthPetSystem::Update(DWORD deltaTime)
{

	bool bResult = true;

	if (IsSummoned()) {
		DoPetSkillAlone();

		if (m_pkOwner->IsImmortal() && PetSkillTable[17].dwLevelValue[m_dwSkill[m_dwslotimm][1]] <= (get_global_time() - m_dwImmTime) * 10) {
			m_dwImmTime = 0;
			m_pkOwner->SetImmortal(0);
		}
	}
	if ((IsSummoned() && (ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetSocket(1) - time(0) <= 0)) || NULL == ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID()) || ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetOwner() != this->GetOwner())
	{
		this->Unsummon();
		m_pkOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_IS_DEAD"));
		return true;
	}

	if (this->IsSummoned() && HasOption(EPetOption_Followable))
		bResult = bResult && this->_UpdateFollowAI();

	return bResult;
}

void CGrowthPetSystem::SendPetInfoPacket(bool isUnsummon)
{
	if (isUnsummon) {
		TGrowthPetInfoPacket pack;
		pack.bHeader = HEADER_GC_GROWTH_PET_INFO;
		pack.dwSealItemVnum = 0;
		m_pkOwner->GetDesc()->Packet(&pack, sizeof(pack));
		return;
	}
	if (IsSummoned()) {
		LPITEM pSummonItem = ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID());

		TGrowthPetInfoPacket pack;
		pack.bHeader = HEADER_GC_GROWTH_PET_INFO;
		pack.dwSealItemVnum = m_dwSummonItemVnum;
		pack.dwSealSlot = pSummonItem->GetCell();
		memcpy(pack.szPetName, m_szName.c_str(), 13);
		pack.dwAge = m_dwAge;
		pack.dwBirthday = m_dwBirthday;
		pack.dwItemDuration = m_dwItemDuration;
		pack.dwBornDuration = m_dwBornDuration;
		pack.dwLevel = m_dwLevel;
		pack.dwEvo = m_dwEvo;
		pack.bCanEvolution = CanEvolution();
		pack.dwExpMob = m_dwExpMob;
		pack.dwExpItem = m_dwExpItem;
		pack.dwNextExp[0] = m_pkChar->PetGetNextExp(0);
		pack.dwNextExp[1] = m_pkChar->PetGetNextExp(1);
		for (int i = 0; i < 3; ++i) {
			pack.dwBonus[i] = m_dwBonus[i][1];
			pack.dwSkillInfo[i][0] = m_dwSkill[i][0];
			pack.dwSkillInfo[i][1] = m_dwSkill[i][1];
		}
		m_pkOwner->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CGrowthPetSystem::SaveSQL(LPITEM SpecifiyItem) {
	if (SpecifiyItem == NULL) {
		DBManager::instance().DirectQuery
		(
			"UPDATE player_petsystem SET level = %d, evolution=%d, exp=%d, expi=%d, bonus0=%d, bonus1=%d, bonus2=%d, skill0=%d, skill0lv= %d, skill1=%d, skill1lv= %d, skill2=%d, skill2lv= %d, item_duration=%d, born_duration=%d, age = (UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(birthday))/60, age_bonus = FLOOR(age / 60 / 24 / %d), pettype=%d, typebonus0=%d, typebonus1=%d, typebonus2=%d WHERE id = %lu ",
			this->GetLevel(),
			this->m_dwEvo,
			this->GetExpMonster(),
			this->GetExpItem(),
			this->m_dwBonus[0][1], this->m_dwBonus[1][1], this->m_dwBonus[2][1],
			this->m_dwSkill[0][0], this->m_dwSkill[0][1], this->m_dwSkill[1][0], this->m_dwSkill[1][1], this->m_dwSkill[2][0], this->m_dwSkill[2][1],
			this->m_dwItemDuration, this->m_dwBornDuration,
			NEW_PET_AGE_GIVE_BONUS_EVERY_DAYS,
			this->m_dwPetType,
			this->m_dwTypeBonus[0], this->m_dwTypeBonus[1], this->m_dwTypeBonus[2],
			ITEM_MANAGER::instance().FindByVID(this->GetSummonItemVID())->GetID()
		);
	}
	else {
		DBManager::instance().DirectQuery
		(
			"UPDATE player_petsystem SET pettype=%d, bonus0=%d, bonus1=%d, bonus2=%d, typebonus0=%d, typebonus1=%d, typebonus2=%d WHERE id = %lu",
			this->m_dwPetType,
			this->m_dwBonus[0][1],
			this->m_dwBonus[1][1],
			this->m_dwBonus[2][1],
			this->m_dwTypeBonus[0],
			this->m_dwTypeBonus[1],
			this->m_dwTypeBonus[2],
			ITEM_MANAGER::instance().FindByVID(SpecifiyItem->GetVID())->GetID()
		);
	}
}

bool CGrowthPetSystem::Follow(float fMinDistance)
{
	if( !m_pkOwner || !m_pkChar) 
		return false;

	float fOwnerX = m_pkOwner->GetX();
	float fOwnerY = m_pkOwner->GetY();

	float fPetX = m_pkChar->GetX();
	float fPetY = m_pkChar->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
		return false;

	m_pkChar->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pkChar->GetRotation(), fDistToGo, &fx, &fy);
	
	if (!m_pkChar->Goto((int)(fPetX+fx+0.5f), (int)(fPetY+fy+0.5f)) )
		return false;

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

// char_state.cpp StateHorse
bool CGrowthPetSystem::_UpdateFollowAI()
{
	if (!m_pkOwner || !m_pkChar)
		return false;

	if (0 == m_pkChar->m_pkMobData)
	{
		return false;
	}

	if (0 == m_originalMoveSpeed)
	{
		const CMob* mobData = CMobManager::Instance().Get(m_dwVnum);

		if (0 != mobData)
			m_originalMoveSpeed = mobData->m_table.sMovingSpeed;
	}
	float	START_FOLLOW_DISTANCE = 500.0f;
	float	START_RUN_DISTANCE = 1500.0f;	

	float	RESPAWN_DISTANCE = 4500.f;		
	int	APPROACH = 300;

	bool bDoMoveAlone = true;
	bool bRun = false;	

	DWORD currentTime = get_dword_time();

	long ownerX = m_pkOwner->GetX();		long ownerY = m_pkOwner->GetY();
	long charX = m_pkChar->GetX();			long charY = m_pkChar->GetY();

	float fDist = DISTANCE_APPROX(charX - ownerX, charY - ownerY);

	if (fDist >= RESPAWN_DISTANCE)
	{
		float fOwnerRot = m_pkOwner->GetRotation() * 3.141592f / 180.f;
		float fx = -APPROACH * cos(fOwnerRot);
		float fy = -APPROACH * sin(fOwnerRot);
		if (m_pkChar->Show(m_pkOwner->GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}
	
	if (fDist >= START_FOLLOW_DISTANCE)
	{
		if( fDist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pkChar->SetNowWalking(!bRun);
		
		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}
	else if (currentTime - m_dwLastActionTime > number(5000, 12000))
	{
		this->_UpdatAloneActionAI(START_FOLLOW_DISTANCE, START_FOLLOW_DISTANCE);
	}
	else
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	return true;
}

bool CGrowthPetSystem::_UpdatAloneActionAI(float fMinDist, float fMaxDist)
{
	if (!m_pkOwner || !m_pkChar)
		return false;
	
	float fDist = number(fMinDist, fMaxDist);
	float r = (float)number (0, 359);
	float dest_x = GetOwner()->GetX() + fDist * cos(r);
	float dest_y = GetOwner()->GetY() + fDist * sin(r);

	m_pkChar->SetNowWalking(true);

	if (!m_pkChar->IsStateMove() && m_pkChar->Goto(dest_x, dest_y))
		m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	m_dwLastActionTime = get_dword_time();

	return true;
}

#endif