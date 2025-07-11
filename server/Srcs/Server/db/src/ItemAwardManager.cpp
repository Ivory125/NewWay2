#include "stdafx.h"
#include "QID.h"
#include "DBManager.h"
#include "ItemAwardManager.h"
#include "Peer.h"
#include "ClientManager.h"

#ifdef ENABLE_EXTEND_ITEM_AWARD
#include <algorithm>
#include <cfloat>
#include <unordered_set>
#include <msl/random.h>

inline double uniform_random(const double a, const double b)
{
	return msl::random_real(a, b);
}

inline float gauss_random(const float fAverage, const float sigma)
{
	static bool bHasNextGaussian = false;
	static float fNextGaussian = 0.0f;

	if (bHasNextGaussian)
	{
		bHasNextGaussian = false;
		return (fNextGaussian * sigma) + fAverage;
	}
	else
	{
		double v1(0), v2(0), s(0);
		do
		{
			v1 = uniform_random(-1.f, 1.f);
			v2 = uniform_random(-1.f, 1.f);
			s = (v1 * v1) + (v2 * v2);
		} while (s >= 1.f || fabs(s) < FLT_EPSILON);

		const double multiplier = sqrtf(-2 * logf(s) / s);
		fNextGaussian = v2 * multiplier;
		bHasNextGaussian = true;
		return (v1 * multiplier) * sigma + fAverage;
	}
}
#endif

DWORD g_dwLastCachedItemAwardID = 0;
ItemAwardManager::ItemAwardManager()
{
}

ItemAwardManager::~ItemAwardManager()
{
}

void ItemAwardManager::RequestLoad()
{
	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT id,login,vnum,count,socket0,socket1,socket2,"
		#ifdef ENABLE_EXTEND_ITEM_AWARD
		"attrtype0, attrvalue0, "
		"attrtype1, attrvalue1, "
		"attrtype2, attrvalue2, "
		"attrtype3, attrvalue3, "
		"attrtype4, attrvalue4, "
		"attrtype5, attrvalue5, "
		"attrtype6, attrvalue6, "
		#endif
		"mall,why FROM item_award WHERE taken_time IS NULL and id > %d", g_dwLastCachedItemAwardID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_AWARD_LOAD, 0, NULL);
}

void ItemAwardManager::Load(SQLMsg * pMsg)
{
	MYSQL_RES * pRes = pMsg->Get()->pSQLResult;

	for (uint i = 0; i < pMsg->Get()->uiNumRows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes);
		int col = 0;

		DWORD dwID = 0;
		str_to_number(dwID, row[col++]);

		if (m_map_award.find(dwID) != m_map_award.end())
			continue;

		TItemAward * kData = new TItemAward{};

		kData->dwID	= dwID;
		trim_and_lower(row[col++], kData->szLogin, sizeof(kData->szLogin));
		str_to_number(kData->dwVnum, row[col++]);
		str_to_number(kData->dwCount, row[col++]);
		str_to_number(kData->dwSocket0, row[col++]);
		str_to_number(kData->dwSocket1, row[col++]);
		str_to_number(kData->dwSocket2, row[col++]);
		#ifdef ENABLE_EXTEND_ITEM_AWARD
		for (size_t j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
		{
			str_to_number(kData->aAttr[j].bType, row[col++]);
			str_to_number(kData->aAttr[j].sValue, row[col++]);
		}
		#endif
		str_to_number(kData->bMall, row[col++]);

		if (row[col])
		{
			strlcpy(kData->szWhy, row[col], sizeof(kData->szWhy));

			char* whyStr = kData->szWhy;
			char cmdStr[100] = "";
			strcpy(cmdStr,whyStr);
			char command[20] = "";
			// @fixme203 directly GetCommand instead of strcpy
			CClientManager::instance().GetCommand(cmdStr, command);
			//sys_err("%d,  %s",pItemAward->dwID,command);
			if( !(strcmp(command,"GIFT") ))
			{
				TPacketItemAwardInfromer giftData;
				strcpy(giftData.login, kData->szLogin);
				strcpy(giftData.command, command);
				giftData.vnum = kData->dwVnum;
				CClientManager::instance().ForwardPacket(HEADER_DG_ITEMAWARD_INFORMER,&giftData,sizeof(TPacketItemAwardInfromer));
			}
		}

		m_map_award.emplace(dwID, kData);

		printf("ITEM_AWARD load id %u bMall %d \n", kData->dwID, kData->bMall);
		sys_log(0, "ITEM_AWARD: load id %lu login %s vnum %lu count %u socket %lu", kData->dwID, kData->szLogin, kData->dwVnum, kData->dwCount, kData->dwSocket0);
		std::set<TItemAward *> & kSet = m_map_kSetAwardByLogin[kData->szLogin];
		kSet.emplace(kData);

		if (dwID > g_dwLastCachedItemAwardID)
			g_dwLastCachedItemAwardID = dwID;
	}
}

std::set<TItemAward *> * ItemAwardManager::GetByLogin(const char * c_pszLogin)
{
	itertype(m_map_kSetAwardByLogin) it = m_map_kSetAwardByLogin.find(c_pszLogin);

	if (it == m_map_kSetAwardByLogin.end())
		return NULL;

	return &it->second;
}

void ItemAwardManager::Taken(DWORD dwAwardID, DWORD dwItemID)
{
	itertype(m_map_award) it = m_map_award.find(dwAwardID);

	if (it == m_map_award.end())
	{
		sys_log(0, "ITEM_AWARD: Taken ID not exist %lu", dwAwardID);
		return;
	}

	TItemAward * k = it->second;
	k->bTaken = true;

	// Update taken_time in database to prevent not to give him again.

	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery),
			"UPDATE item_award SET taken_time=NOW(),item_id=%u WHERE id=%u AND taken_time IS NULL",
			dwItemID, dwAwardID);

	CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_AWARD_TAKEN, 0, NULL);
}

std::map<DWORD, TItemAward *>& ItemAwardManager::GetMapAward()
{
	return m_map_award;
}

std::map<std::string, std::set<TItemAward *> >& ItemAwardManager::GetMapkSetAwardByLogin()
{
	return m_map_kSetAwardByLogin;
}

#ifdef USE_ITEM_AWARD_CHECK_ATTRIBUTES
/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| Attribute set index, return a specific index by item type & item sub type.
||| List initialization (since C++11)
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
int8_t ItemAwardManager::GetItemAttributeSetIndex(const uint8_t bItemType, const uint8_t bItemSubType) const
{
	using TAttributeMapT = std::map<uint8_t, std::map<uint8_t, int8_t>>;
	static const TAttributeMapT mapAttrSetFields =
	{
		{ ITEM_WEAPON,
			{
				{WEAPON_ARROW, ITEM_ATTRIBUTE_NONE},
				#ifdef ENABLE_QUIVER_SYSTEM
				{WEAPON_QUIVER, ITEM_ATTRIBUTE_NONE},
				#endif
			}
		},

		{ ITEM_ARMOR,
			{
				{ARMOR_BODY, ATTRIBUTE_SET_BODY},
				{ARMOR_WRIST, ATTRIBUTE_SET_WRIST},
				{ARMOR_FOOTS, ATTRIBUTE_SET_FOOTS},
				{ARMOR_NECK, ATTRIBUTE_SET_NECK},
				{ARMOR_HEAD, ATTRIBUTE_SET_HEAD},
				{ARMOR_SHIELD, ATTRIBUTE_SET_SHIELD},
				{ARMOR_EAR, ATTRIBUTE_SET_EAR},
				#ifdef ENABLE_PENDANT_SYSTEM
				{ARMOR_PENDANT, ATTRIBUTE_SET_PENDANT},
				#endif
				#ifdef ENABLE_GLOVE_SYSTEM
				{ARMOR_GLOVE, ATTRIBUTE_SET_GLOVE},
				#endif
			}
		},

		{ ITEM_COSTUME,
			{
				{COSTUME_BODY, ATTRIBUTE_SET_BODY},
				{COSTUME_HAIR, ATTRIBUTE_SET_HEAD},
				#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				{COSTUME_MOUNT, ITEM_ATTRIBUTE_NONE},
				#endif
				#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				{COSTUME_WEAPON, ATTRIBUTE_SET_WEAPON},
				#endif
			}
		},
	};

	const auto c_iter_type = mapAttrSetFields.find(bItemType);
	if (c_iter_type != mapAttrSetFields.end())
	{
		const auto c_iter_sub_type = c_iter_type->second.find(bItemSubType);
		if (c_iter_sub_type != c_iter_type->second.end())
			return c_iter_sub_type->second;
		else if (c_iter_type->first == ITEM_WEAPON)
			return ATTRIBUTE_SET_WEAPON;
	}

	return ITEM_ATTRIBUTE_NONE;
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| 15.04.2018 - Added a check for attr types and values min - max.
||| You can't insert wrong bonuses into a specific item.
||| Eg. Add 2000M MAX_HP on your Sword+9, was possible, now not.
||| Eg. Add +500 INT to your shield, now there's a check for min-max value of player.item_attr Lv.1 - Lv.5
||| and your 500 INT value will be replaced with max value from lvl5 of bonus, like 12 (lv5), that happen with all the bonuses,
|||	same thing with the values lower than lvl1, like 5 HP_REGEN on your neck, when the minimum (lv1) is 10, the value will be replaced with 10.
||| If the bonus type can't be added into a specific item, the bonus will be ignored > deleted. (example: critical pct to armor)
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemAttributes(TItemAward & rkItemAward, const TItemTable & rkItemTable, const std::vector<TItemAttrTable> & vec_itemAttrTable, const std::vector<TItemAttrTable> & vec_itemRareTable)
{
	const auto bItemType = rkItemTable.bType;
	const auto bItemSubType = rkItemTable.bSubType;
	const auto iAttributeSet = GetItemAttributeSetIndex(bItemType, bItemSubType);
	if (iAttributeSet != ITEM_ATTRIBUTE_NONE)
	{
		// remove duplicates
		std::unordered_set<int> duplicates;
		for (size_t i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			// clear up duplicates after reached the rare ones
			if (i == ITEM_ATTRIBUTE_RARE_START)
				duplicates.clear();

			auto bApplyType = rkItemAward.aAttr[i].bType;
			// check if already present
			if (duplicates.find(bApplyType) != duplicates.end())
			{
				rkItemAward.aAttr[i].bType = 0;
				rkItemAward.aAttr[i].sValue = 0;
				continue;
			}
			duplicates.emplace(bApplyType);
		}

		for (size_t i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			const auto bApplyType = rkItemAward.aAttr[i].bType;
			const TItemAttrTable * pkAttrTable{nullptr};

			// skip special bonus
			if (bApplyType == APPLY_SKILL_DAMAGE_BONUS || bApplyType == APPLY_NORMAL_HIT_DAMAGE_BONUS)
				continue;

			// check normal bonus with item_attr
			if (i < ITEM_ATTRIBUTE_NORM_END)
			{
				for (size_t j = 0; j < vec_itemAttrTable.size() && !pkAttrTable; ++j)
				{
					const TItemAttrTable & rkAttrTable = vec_itemAttrTable.at(j);
					if (rkAttrTable.dwApplyIndex == bApplyType)
					{
						const auto bAttrLevel = rkAttrTable.bMaxLevelBySet[iAttributeSet];
						if (bAttrLevel > 0)
							pkAttrTable = &rkAttrTable;
					}
				}
			}
			// check rare bonus with item_attr_rare
			else if (i >= ITEM_ATTRIBUTE_RARE_START)
			{
				for (size_t j = 0; j < vec_itemRareTable.size() && !pkAttrTable; ++j)
				{
					const TItemAttrTable & rkAttrTable = vec_itemRareTable.at(j);
					if (rkAttrTable.dwApplyIndex == bApplyType)
					{
						const auto bAttrLevel = rkAttrTable.bMaxLevelBySet[iAttributeSet];
						if (bAttrLevel > 0)
							pkAttrTable = &rkAttrTable;
					}
				}
			}

			if (pkAttrTable)
			{
				const auto sMinValue = static_cast<int16_t>(pkAttrTable->lValues[0]);
				const auto sMaxValue = static_cast<int16_t>(pkAttrTable->lValues[(sizeof(pkAttrTable->lValues) / sizeof(pkAttrTable->lValues[0])) - 1]);
				rkItemAward.aAttr[i].sValue = MINMAX(sMinValue, rkItemAward.aAttr[i].sValue, sMaxValue);
			}
			else
			{
				rkItemAward.aAttr[i].bType = 0;
				rkItemAward.aAttr[i].sValue = 0;
			}
		}
	}
}
#endif

#ifdef ENABLE_EXTEND_ITEM_AWARD
/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| 02.04.2018 - Fixed unknown average/skill damage bonus value.
||| player.item_proto.addon_type = -1 (Eg. 189, 199, 299, 1139, 1179, 2159, 2179, 3169, 3219, 5119, 5129, 6019, 6069, 6079, 7169)[+0 - +9]
||| That's for the items which have addon type (-1) and you added them in item shop without bonuses like skill damage or hit damage,
||| value x, y as default, so they'll will be without bonuses and get 'bugged'.
||| Now when the item will be inserted there'll be a check if item doesn't have those bonuses (from query) add a random average/skill damage bonus value.
||| INSERT INTO player.item_award(`login`, `vnum`, `count`, `mall`) VALUES ('account', 189, 1, 1);
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemAddonType(TItemAward & rkItemAward, const TItemTable & rkItemTable)
{
	const bool bIsAddonTypeItem = (rkItemTable.sAddonType == -1);
	if (!bIsAddonTypeItem)
		return;

	bool bHasBonus = false;
	for (size_t i = 0; i < ITEM_ATTRIBUTE_MAX_NUM && !bHasBonus; ++i)
	{
		const auto bType = rkItemAward.aAttr[i].bType;
		const auto sValue = rkItemAward.aAttr[i].sValue;

		if ((bType == APPLY_SKILL_DAMAGE_BONUS || bType == APPLY_NORMAL_HIT_DAMAGE_BONUS) && sValue)
			bHasBonus = true;
	}

	if (!bHasBonus)
	{
		const auto sApplySkillDamageValue = MINMAX(-30, static_cast<int16_t>((gauss_random(0, 5) + 0.5f)), 30);
		const auto sApplyNormalHitValue = std::abs(sApplySkillDamageValue) <= 20 ?
												(-2 * sApplySkillDamageValue) + std::abs(number(-8, 8) + number(-8, 8)) + number(1, 4) :
												(-2 * sApplySkillDamageValue) + number(1, 5);

		rkItemAward.aAttr[0].bType = APPLY_NORMAL_HIT_DAMAGE_BONUS;
		rkItemAward.aAttr[0].sValue = sApplyNormalHitValue;
		rkItemAward.aAttr[1].bType = APPLY_SKILL_DAMAGE_BONUS;
		rkItemAward.aAttr[1].sValue = sApplySkillDamageValue;

		// clear up possible duplicates
		for (size_t i = 2; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			const auto bType = rkItemAward.aAttr[i].bType;
			if (bType == APPLY_SKILL_DAMAGE_BONUS || bType == APPLY_NORMAL_HIT_DAMAGE_BONUS)
			{
				rkItemAward.aAttr[i].bType = 0;
				rkItemAward.aAttr[i].sValue = 0;
			}
		}
	}
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| 12.04.2019 - Added support for books.
||| Check skill types, unknown skill, skill vnum need to be saved into socket0,
||| (4=Aura of the Sword < player.skill_proto), if the skill vnum is unknown, there will be a random book based on pc races,
||| excluded skills PASSIVE, GUILD, SUPPORT.
||| INSERT INTO player.item_award(`login`, `vnum`, `count`, `mall`) VALUES ('account', 50300, 1, 1); # Random book
||| INSERT INTO player.item_award(`login`, `vnum`, `count`, `socket0`, `mall`) VALUES ('account', 50300, 1, 4, 1); # Specific book by skill vnum
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemSkillBook(TItemAward & rkItemAward, const std::vector<TSkillTable> vec_skillTable)
{
	const bool bIsBookItem = (rkItemAward.dwVnum == ITEM_SKILL_VNUM || rkItemAward.dwVnum == ITEM_SKILLFORGET_VNUM);
	if (!bIsBookItem)
		return;

	auto dwSocket0SkillVnum = rkItemAward.dwSocket0;
	bool bHasSkill = false;
	for (size_t i = 0; i < vec_skillTable.size(); ++i)
	{
		if (vec_skillTable[i].dwVnum == dwSocket0SkillVnum)
		{
			bHasSkill = true;
			break;
		}
	}

	if (!bHasSkill)
	{
		do
		{
			const TSkillTable * pkSkillTable = &vec_skillTable.at(number(0, vec_skillTable.size() - 1));
			if (!pkSkillTable)
				continue;

			const auto dwSkillVnum = pkSkillTable->dwVnum;
			const auto bSkillType = pkSkillTable->bType;

			if ((JOB_WARRIOR + 1 <= bSkillType && bSkillType <= JOB_SHAMAN + 1)
				#ifdef ENABLE_WOLFMAN_CHARACTER
				|| bSkillType == 7
				#endif
			)
				continue;

			dwSocket0SkillVnum = dwSkillVnum;
			break;
		}
		while (true);

		rkItemAward.dwSocket0 = dwSocket0SkillVnum;
	}
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| 11.04.2019 - Fixed unstackable items.
||| Check if item count overflow occured, then set it to maximum.
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemCount(TItemAward & rkItemAward, const TItemTable & rkItemTable)
{
	const bool bIsStackableItem = (rkItemTable.dwFlags & ITEM_FLAG_STACKABLE);
	if (rkItemAward.dwCount > 1 && !bIsStackableItem)
		rkItemAward.dwCount = 1;
	else
		rkItemAward.dwCount = MINMAX(1, rkItemAward.dwCount, ITEM_MAX_COUNT);
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| 20.08.2021 - Fixed not slottable items.
||| New created equipments had no available slots.
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemSocket(TItemAward & rkItemAward, const TItemTable & rkItemTable)
{
	// check for limited time items
	for (const auto & limit : rkItemTable.aLimits)
	{
		if (LIMIT_REAL_TIME == limit.bType || LIMIT_REAL_TIME_START_FIRST_USE == limit.bType)
			return;
	}

	// check slottable sockets for non limited time items
	const auto maxSockets = std::min<int>(rkItemTable.bGainSocketPct, ITEM_SOCKET_MAX_NUM);
	if (maxSockets <= 0)
		return;

	if (maxSockets >= 1)
		rkItemAward.dwSocket0 = 1;
	if (maxSockets >= 2)
		rkItemAward.dwSocket1 = 1;
	if (maxSockets >= 3)
		rkItemAward.dwSocket2 = 1;
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
||| TODO-UNFINISHED: Check if apply_type exists in bonuses.
||| Check if apply_value, apply_duration is equal with grades (1/2/3/4/5) from settings, blend.txt
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/
void ItemAwardManager::CheckItemBlend(TItemAward & rkItemAward, const TItemTable & rkItemTable)
{
	const bool bIsBlendItem = (rkItemTable.bType == ITEM_BLEND);
	if (!bIsBlendItem)
		return;

	const auto bApplyType = rkItemAward.dwSocket0;
	const auto bApplyValue = rkItemAward.dwSocket1;
	const auto dwApplyDuration = rkItemAward.dwSocket2;

	if (bApplyType == 0 || bApplyValue == 0 || dwApplyDuration == 0)
		sys_err("ItemAwardManager: Unknown sockets for ITEM_BLEND.");
}
#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
