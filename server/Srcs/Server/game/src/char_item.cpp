#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "arena.h"
#include "threeway_war.h"

#include "safebox.h"
#include "shop.h"

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif
#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
#include "growth_petsystem.h"
#endif

#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#include "../../common/PulseManager.h"
#endif

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"
#include "PetSystem.h"

#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX
#define ENABLE_ITEM_RARE_ATTR_LEVEL_PCT

enum {ITEM_BROKEN_METIN_VNUM = 28960};

// CHANGE_ITEM_ATTRIBUTES
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
		case 5:
		case 25:
		case 45:
		case 108:
		case 109:
			return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;
	if (IS_CASTLE_MAP(map_index))
		return false;

	switch (map_index)
	{
		case 66 :
		case 71 :
		case 72 :
		case 73 :
		case 193 :
#if 0
		case 184 :
		case 185 :
		case 186 :
		case 187 :
		case 188 :
		case 189 :
#endif

		case 216 :
		case 217 :
		case 208 :

		case 113 :
			return false;
	}

	if (map_index > 10000) return false;

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) return true;

	switch (nMapIndex)
	{
		case 1 :
		case 3 :
		case 21 :
		case 23 :
		case 41 :
		case 43 :
			return true;
	}

	return false;
}

static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE==GET_SEX(ch))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

#ifdef ELEMENT_SPELL_WORLDARD
	if (IsOpenElementsSpell())
		return false;
#endif

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (!IsValidItemPosition(Cell))
		return NULL;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pDSItems[wCell];

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		if(wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid Buff-EQ item cell %d >= %d", wCell, BUFF_WINDOW_SLOT_MAX_NUM);
			return NULL;
		}
		return m_PlayerSlots->pBuffEquipmentItem[wCell];
#endif

	default:
		return NULL;
	}
	return NULL;
}

void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem
	#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	, bool bWereMine
	#endif
)
{
	if (!m_PlayerSlots)
		return;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch(window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_PlayerSlots->pItems[wCell];

			if (pOld)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						if (m_PlayerSlots->pItems[p] && m_PlayerSlots->pItems[p] != pOld)
							continue;

						m_PlayerSlots->bItemGrid[p] = 0;
					}
				}
				else
					m_PlayerSlots->bItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						m_PlayerSlots->bItemGrid[p] = wCell + 1;
					}
				}
				else
					m_PlayerSlots->bItemGrid[wCell] = wCell + 1;
			}

			m_PlayerSlots->pItems[wCell] = pItem;
		}
		break;

	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_PlayerSlots->pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (m_PlayerSlots->pDSItems[p] && m_PlayerSlots->pDSItems[p] != pOld)
							continue;

						m_PlayerSlots->wDSItemGrid[p] = 0;
					}
				}
				else
					m_PlayerSlots->wDSItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						m_PlayerSlots->wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					m_PlayerSlots->wDSItemGrid[wCell] = wCell + 1;
			}

			m_PlayerSlots->pDSItems[wCell] = pItem;
		}
		break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		{
			if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM) {
				sys_err("CHARACTER::SetItem: invalid BUFF_EQUIPMENT item cell %d", wCell);
				return;
			}
			LPITEM pOld = m_PlayerSlots->pBuffEquipmentItem[wCell];

			if (pOld && pItem) {
				return;
			}

			m_PlayerSlots->pBuffEquipmentItem[wCell] = pItem;
			if (GetBuffNPCSystem() != NULL) {
				if (GetBuffNPCSystem()->IsSummoned()) {
					GetBuffNPCSystem()->UpdateBuffEquipment();
				}
			}
		}
		break;	
#endif
	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

#ifdef ELEMENT_SPELL_WORLDARD
			// Setze das Elementen-Grad-Attribut
			pack.grade_element = pItem->GetElementGrade();
			// Kopiere die Elementenangriffe vom pItem-Objekt in das pack.attack_element-Array
			thecore_memcpy(pack.attack_element, pItem->GetElementAttacks(), sizeof(pack.attack_element));
			// Setze den Elementen-Typ-Bonus
			pack.element_type_bonus = pItem->GetElementsType();
			// Kopiere die Elementen-Wert-Boni vom pItem-Objekt in das pack.elements_value_bonus-Array
			thecore_memcpy(pack.elements_value_bonus, pItem->GetElementsValues(), sizeof(pack.elements_value_bonus));
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
#ifdef ELEMENT_SPELL_WORLDARD
			// Setze das Elementen-Grad-Attribut auf 0
			pack.grade_element = 0;
			// Setze alle Elementenangriffe im pack.attack_element-Array auf 0
			memset(pack.attack_element, 0, sizeof(pack.attack_element));
			// Setze den Elementen-Typ-Bonus auf 0
			pack.element_type_bonus = 0;
			// Setze alle Elementen-Wert-Boni im pack.elements_value_bonus-Array auf 0
			memset(pack.elements_value_bonus, 0, sizeof(pack.elements_value_bonus));
#endif
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			pItem->SetWindow(BUFF_EQUIPMENT);
			break;
#endif
		}
	}
}

LPITEM CHARACTER::GetWear(BYTE bCell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_PlayerSlots->pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(BYTE bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);

	if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	for (i = 0; i < BUFF_WINDOW_SLOT_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BUFF_EQUIPMENT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (!m_PlayerSlots)
		return false;

	switch (Cell.window_type)
	{
	case INVENTORY:
		{
			WORD bCell = Cell.cell;

			++iExceptionCell;

			if (Cell.IsBeltInventoryPosition())
			{
				LPITEM beltItem = GetWear(WEAR_BELT);

				if (NULL == beltItem)
					return false;

				if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
					return false;

				if (m_PlayerSlots->bItemGrid[bCell])
				{
					if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
						return true;

					return false;
				}

				if (bSize == 1)
					return true;

			}
			else if (bCell >= INVENTORY_MAX_NUM)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell])
			{
				if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= INVENTORY_MAX_NUM)
							return false;

						if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
							return false;

						if (m_PlayerSlots->bItemGrid[p])
							if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
						return false;

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
		break;
	case DRAGON_SOUL_INVENTORY:
		{
			WORD wCell = Cell.cell;
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				return false;

			iExceptionCell++;

			if (m_PlayerSlots->wDSItemGrid[wCell])
			{
				if (m_PlayerSlots->wDSItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
						return true;

					int j = 1;

					do
					{
						int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							return false;

						if (m_PlayerSlots->wDSItemGrid[p])
							if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
				else
					return false;
			}

			if (1 == bSize)
				return true;
			else
			{
				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_PlayerSlots->wDSItemGrid[p]) // @fixme159 (from bItemGrid)
						if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
							return false;
				}
				while (++j < bSize);

				return true;
			}
		}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		{
			UINT wCell = Cell.cell;
			if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
			{
				return false;
			}

			if (m_PlayerSlots->pBuffEquipmentItem[wCell])
			{
				return false;
			}

			return true;
		}
#endif
	}
	return false;
}

int CHARACTER::GetEmptyInventory(BYTE size) const
{
	for ( int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), size))
			return i;
	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_PlayerSlots->wDSItemGrid, m_PlayerSlots->wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무료 개량 기회는 20 이하의 무기만 가능합니다"));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 개량할 수 없습니다."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 돈이 부족합니다."));
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 재료가 부족합니다."));
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number(1, 100);

	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			// DETAIL_REFINE_LOG

			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
		PayRefineFee(cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN   = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL   = 3,
	YAGONG_SCROLL  = 4,
	MEMO_SCROLL	   = 5,
	BDRAGON_SCROLL	= 6,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 개량할 수 없습니다."));
		return false;
	}

	// MUSIN_SCROLL
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 개량서로 더 이상 개량할 수 없습니다."));
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 개량서로 개량할 수 없습니다."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템으로 개량할 수 없습니다."));
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 돈이 부족합니다."));
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개량을 하기 위한 재료가 부족합니다."));
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN ||
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL ||
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) {} // @fixme121
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();

			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);

		PayRefineFee(prt->cost);
	}

	return true;
}

#ifdef ELEMENT_SPELL_WORLDARD

bool CHARACTER::ElementsSpellItem(LPITEM pkItem, LPITEM pkTarget)
{
	// ?erpr?e, ob der Charakter mit dem Gegenstand umgehen kann
	if (!CanHandleItem())
		return false;

	// Hole die Zelle des Zielgegenstands
	BYTE bCell = pkTarget->GetCell();

	// ?erpr?e, ob die Zelle g?tig ist
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	// Hole den Gegenstand aus dem Inventar an der gegebenen Zelle
	LPITEM item = GetInventoryItem(bCell);

	// ?erpr?e, ob ein Gegenstand gefunden wurde
	if (!item)
		return false;

	// Initialisiere das Paket f? die Elementenzauber-Nachricht
	TPacketGCElementsSpell p;
	p.header = HEADER_GC_ELEMENTS_SPELL;
	p.pos = bCell;
	
// Je nach Untertyp des Gegenstands f?re verschiedene Aktionen aus
	if(pkItem->GetSubType() == USE_ELEMENT_DOWNGRADE)
	{
		p.subheader = ELEMENT_SPELL_SUB_HEADER_OPEN;
		p.cost = GOLD_DOWNGRADE_BONUS_ELEMENTS_SPELL;
		p.func = false;
		p.grade_add = 0;
	}
	else if(pkItem->GetSubType() == USE_ELEMENT_UPGRADE)
	{
		p.subheader = ELEMENT_SPELL_SUB_HEADER_OPEN;
		p.cost = GOLD_ADD_BONUS_ELEMENTS_SPELL;
		p.func = true;
		// Wenn der Zielgegenstand kein Elementen-Grad hat, setze den Grad gem癌 des Gegenstands
		if (pkTarget->GetElementGrade() == 0){
			p.grade_add = pkItem->GetValue(0);
		}else{
			p.grade_add = 0;
		}

	}
	else if(pkItem->GetSubType() == USE_ELEMENT_CHANGE)
	{
		p.subheader = ELEMENT_SPELL_SUB_HEADER_OPEN_CHANGE;
		p.cost = GOLD_CHANGE_BONUS_ELEMENTS_SPELL;
	}
// Sende das Elementenzauber-Paket an den Client
	GetDesc()->Packet(&p, sizeof(TPacketGCElementsSpell));
// Setze den Elementenzauber als ge?fnet
	SetOpenElementsSpell(true,pkItem->GetCell());
	return true;

}

void CHARACTER::ElementsSpellItemFunc(int pos, BYTE type_select)
{
// ?erpr?e, ob der Charakter im Austausch ist oder einen sicheren Bereich ?fnet
	if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen())
	{
		//ChatPacket(CHAT_TYPE_INFO, "Error 100.");
		SetOpenElementsSpell(false);
		return;
	}
// ?erpr?e, ob die Elementen-Zusatz-Zelle und die Position g?tig sind
	if (m_iElementsAdditionalCell < 0 || pos < 0){
		//ChatPacket(CHAT_TYPE_INFO, "Error 200.");
		return;
	}
// Hole die Gegenst?de
	LPITEM itemWeapon = GetInventoryItem(pos);
	LPITEM itemElements = GetInventoryItem(m_iElementsAdditionalCell);
// ?erpr?e, ob beide Gegenst?de existieren
	if (!itemWeapon || !itemElements)
	{
		//ChatPacket(CHAT_TYPE_INFO, "Error 300.");
		return;
	}
// ?erpr?e, ob der Elementen-Gegenstand eine g?tige Aktion ausf?rt
	if (itemElements->GetSubType() != USE_ELEMENT_UPGRADE && itemElements->GetSubType() != USE_ELEMENT_DOWNGRADE && itemElements->GetSubType() != USE_ELEMENT_CHANGE){
		//ChatPacket(CHAT_TYPE_INFO, "Error 400.");
		return;
	}
// ?erpr?e, ob der Waffengegenstand g?tig ist
	if (itemWeapon->GetType() != ITEM_WEAPON){
		//ChatPacket(CHAT_TYPE_INFO, "Error 500.");
		return;
	}
// F?re Aktionen je nach Untertyp des Elementen-Gegenstands aus
	if (itemElements->GetSubType() == USE_ELEMENT_UPGRADE)
	{
		ChatPacket(CHAT_TYPE_INFO, "USE_ELEMENT_UPGRADE.");
// ?erpr?e, ob genug Yang vorhanden ist
		if (GetGold() < GOLD_ADD_BONUS_ELEMENTS_SPELL)
		{
			ChatPacket(CHAT_TYPE_INFO, "Tut mir leid, du hast nicht genug Yang.");
			return;
		}
// Berechne die Erfolgschance
		int percent = number(1,100);
// F?re die Elementenzauber-Aktion aus, wenn die Erfolgschance erreicht ist
		if (percent <= PERCENT_ADD_BONUS_ELEMENTS_SPELL){
// F?e dem Waffengegenstand ein neues Element hinzu
			if(itemWeapon->GetElementGrade() == 0)
			{
// Erh?e den Grad des Waffengegenstands und aktualisiere die Elementenwerte
				DWORD attack_element = number(ATTACK_RANGE_BONUS_ELEMENTS_SPELL_MIN,ATTACK_RANGE_BONUS_ELEMENTS_SPELL_MAX);
				short sValue = number(VALUES_RANGE_BONUS_ELEMENTS_SPELL_MIN,VALUES_RANGE_BONUS_ELEMENTS_SPELL_MAX);
				itemWeapon->SetElementNew(1,attack_element,itemElements->GetValue(0),sValue);
				ChatPacket(CHAT_TYPE_COMMAND, "ElementsSpellSuceeded");
			}

			else if(itemWeapon->GetElementGrade() > 0)
			{
				if(itemWeapon->GetElementsType() != itemElements->GetValue(0)){
					return;
				}

				DWORD attack_element = itemWeapon->GetElementAttack(itemWeapon->GetElementGrade()-1)+ATTACK_RANGE_BONUS_ELEMENTS_SPELL_MIN;
				short sValue = itemWeapon->GetElementsValue(itemWeapon->GetElementGrade()-1)+VALUES_RANGE_BONUS_ELEMENTS_SPELL_MIN;
				itemWeapon->SetElementNew(itemWeapon->GetElementGrade()+1,number(attack_element,attack_element+(ATTACK_RANGE_BONUS_ELEMENTS_SPELL_MAX-ATTACK_RANGE_BONUS_ELEMENTS_SPELL_MIN)),itemWeapon->GetElementsType(),number(sValue,sValue+(VALUES_RANGE_BONUS_ELEMENTS_SPELL_MAX-VALUES_RANGE_BONUS_ELEMENTS_SPELL_MIN)));
				ChatPacket(CHAT_TYPE_COMMAND, "ElementsSpellSuceeded");
			}
		}else{
// Der Elementenzauber schl?t fehl
			ChatPacket(CHAT_TYPE_COMMAND, "ElementsSpellFailed");
		}
// Ziehe den Yang-Betrag ab
		PointChange(POINT_GOLD, -GOLD_ADD_BONUS_ELEMENTS_SPELL);
	}
	else if(itemElements->GetSubType() == USE_ELEMENT_DOWNGRADE)
	{
		ChatPacket(CHAT_TYPE_INFO, "USE_ELEMENT_DOWNGRADE.");
// ?erpr?e, ob genug Yang vorhanden ist
		if (GetGold() < GOLD_DOWNGRADE_BONUS_ELEMENTS_SPELL)
		{
			ChatPacket(CHAT_TYPE_INFO, "Tut mir leid, du hast nicht genug Yang.");
			return;
		}
 // Setze das Elementen-Grad des Waffengegenstands zur?k
		if((itemWeapon->GetElementGrade())-1 <= 0){
			itemWeapon->DeleteAllElement(0);
		}else{
			itemWeapon->DeleteAllElement((itemWeapon->GetElementGrade())-1);
		}
// Benachrichtige ?er die erfolgreich durchgef?rte Aktion
		ChatPacket(CHAT_TYPE_COMMAND, "ElementsSpellDownGradeSuceeded");

		PointChange(POINT_GOLD, -GOLD_DOWNGRADE_BONUS_ELEMENTS_SPELL);
	}
	else if(itemElements->GetSubType() == USE_ELEMENT_CHANGE)
	{
		//ChatPacket(CHAT_TYPE_INFO, "USE_ELEMENT_CHANGE.");
		if (GetGold() < GOLD_CHANGE_BONUS_ELEMENTS_SPELL)
		{
			ChatPacket(CHAT_TYPE_INFO, "Tut mir leid, du hast nicht genug Yang.");
			return;
		}
// ?erpr?e, ob der Elemententyp des Gegenstands ge?dert werden kann
		if(itemWeapon->GetElementGrade() <= 0)
		{
			//ChatPacket(CHAT_TYPE_INFO, "Error 1.");
			return;
		}
// ?dere den Elemententyp des Waffengegenstands
		if (type_select == itemWeapon->GetElementsType())
		{
			//ChatPacket(CHAT_TYPE_INFO, "Error 2.");
			return;
		}
// F?re die ?derung des Elemententyps aus
		itemWeapon->ChangeElement(type_select);
		ChatPacket(CHAT_TYPE_COMMAND, "ElementsSpellChangeSuceeded %d %d",itemWeapon->GetVnum(),type_select);
// Ziehe den Yang-Betrag ab
		PointChange(POINT_GOLD, -GOLD_CHANGE_BONUS_ELEMENTS_SPELL);
	}
// Verringere die Anzahl des Elementen-Gegenstands um eins
	itemElements->SetCount(itemElements->GetCount()-1);
}

void CHARACTER::SetOpenElementsSpell(bool b, int iAdditionalCell)
{
// Setze den Zustand des Elementenzaubers und die Zusatzzelle
	m_OpenElementsSpell = b;
	m_iElementsAdditionalCell = iAdditionalCell;
}

#endif

bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	LPITEM item = GetInventoryItem(bCell);

	if (!item)
		return false;

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사귀 타워 완료 보상은 한번까지 사용가능합니다."));
		return false;
	}
	// END_OF_REFINE_COST

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
	p.type = bType;

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 이 방식으로는 개량할 수 없습니다."));
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 개량서를 합칠 수는 없습니다."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("축복의 서와 현철을 합칠 수 있습니다."));
				return false;
			}
		}
	}

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무료 개량 기회는 20 이하의 무기만 가능합니다"));
			return false;
		}
		p.cost = 0;
	}
	else
		p.cost = ComputeRefineFee(prt->cost);

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// MUSIN_SCROLL
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		// END_OF_MUSIN_SCROLL
		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

#ifdef ENABLE_EXTENDED_SOCKETS
		for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
#ifdef ENABLE_EXTENDED_SOCKETS
			for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);

					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 수 있는 메틴석이 없습니다."));
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("기억해 둘 수 없는 위치 입니다."));
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다."));
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;

		case 301:
		case 302:
		case 303:
		case 304:
			if( GetLevel() < 90 )
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템의 레벨 제한보다 레벨이 낮습니다."));
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("기억된 위치가 타제국에 속해 있어서 귀환할 수 없습니다."));
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2:
			ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
			break;
		default:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("갑옷을 벗어야 개인 상점을 열 수 있습니다."));
			break;
	}
#endif
}

// MYSHOP_PRICE_LIST
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}

//

//
void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)

		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
	}

	__OpenPrivateShop();
}

//

//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	} else {
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST

int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("남은 생명력 양이 모자라 사용할 수 없습니다."));
			return -1;
		}

		consumeLife = needLife;

		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("남은 정신력 양이 모자라 사용할 수 없습니다."));
		return -1;
	}

	return needSP;
}

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템의 레벨 제한보다 레벨이 낮습니다."));
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				//iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %d %s, Inven %d, Cell %d, DestInven %d, DestCell %d, ItemType %d, SubType %d", item->GetVnum(), item->GetName(), item->GetWindow(), item->GetCell(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if ( CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true )
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, "Affects are not loaded yet!");
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you have no equipped belt.");
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you don't upgrade your belt.");
			return false;
		}
	}
	// @fixme141 END

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

	switch (item->GetType())
	{
		case ITEM_HAIR:
			return ItemProcess_Hair(item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph(item);

		case ITEM_QUEST:
#ifdef ENABLE_QUEST_DND_EVENT
			if (IS_SET(item->GetFlag(), ITEM_FLAG_APPLICABLE))
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				quest::CQuestManager::instance().DND(GetPlayerID(), item, item2, false);
				return true;
			}
#endif

			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
			}

			if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
					quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
				else
					quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
			}
			break;

		case ITEM_CAMPFIRE:
			{
				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("모닥불을 피울 수 없는 지점입니다."));
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물 속에 모닥불을 피울 수 없습니다."));
					return false;
				}

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);
			}
			break;

		case ITEM_UNIQUE:
			{
				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_RESIST_MAGIC:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					default:
						{
							if (item->GetSubType() == USE_SPECIAL)
							{
								sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

								switch (item->GetVnum())
								{
									case 71049:
										if (g_bEnableBootaryCheck)
										{
											if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
											{
												UseSilkBotary();
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인 상점을 열 수 없는 지역입니다"));
											}
										}
										else
										{
											UseSilkBotary();
										}
										break;
								}
							}
							else
							{
								if (!item->IsEquipped())
									EquipItem(item);
								else
									UnequipItem(item);
							}
						}
						break;
				}
			}
			break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:
		case ITEM_BELT:
			// MINING
		case ITEM_PICK:
			// END_OF_MINING
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		#ifdef ENABLE_PET_SYSTEM_EX
		case ITEM_PET:
			switch (item->GetSubType())
			{
				case PET_UPBRINGING: //1
				case PET_PAY: //12
				{
					auto mobVnum = item->GetValue(0);
					if (!mobVnum)
					{
						ChatPacket(CHAT_TYPE_INFO, "Item Pet %d with no mob in value0", item->GetVnum());
						return false;
					}
					auto * petSystem = GetPetSystem();
					if (!petSystem)
						return false;

					if (petSystem->CountSummoned())
						petSystem->UnsummonAll();
					else
					{
						// summon
						constexpr bool bFromFar = false;
						const auto * pkMob = CMobManager::instance().Get(mobVnum);
						const auto * petName = (pkMob) ? pkMob->GetLocaleName() : "";
						petSystem->Summon(mobVnum, item, petName, bFromFar);
						// spawn effect
						constexpr const char * spawn_effect_file_name = "d:\\ymir work\\effect\\etc\\appear_die\\npc2_appear.mse";
						auto * petActor = petSystem->GetByVnum(mobVnum);
						if (petActor && petActor->GetCharacter() && item->GetSubType() == PET_PAY)
							petActor->GetCharacter()->SpecificEffectPacket(spawn_effect_file_name);
					}
				}
				break;
		#endif

				case PET_EGG: //0
				case PET_BAG: //2
				case PET_FEEDSTUFF: //3
				case PET_SKILL: //4
				case PET_SKILL_DEL_BOOK: //5
				case PET_NAME_CHANGE: //6
				case PET_EXPFOOD: //7
				case PET_SKILL_ALL_DEL_BOOK: //8
				case PET_EXPFOOD_PER: //9
				case PET_ATTR_DETERMINE: //10
				case PET_ATTR_CHANGE: //11
				case PET_PRIMIUM_FEEDSTUFF: //13
					break;
			}
			break;

		case ITEM_DS:
			{
				if (!item->IsEquipped())
					return false;
				return DSManager::instance().PullOut(this, NPOS, item);
			break;
			}
		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_FISH:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
					return false;
				}
#endif

				if (item->GetSubType() == FISH_ALIVE)
					fishing::UseFish(this, item);
			}
			break;

		case ITEM_TREASURE_BOX:
			{
				return false;

			}
			break;

		case ITEM_TREASURE_KEY:
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() != ITEM_TREASURE_BOX)
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠로 여는 물건이 아닌것 같다."));
					return false;
				}

				if (item->GetValue(0) == item2->GetValue(0))
				{
					DWORD dwBoxVnum = item2->GetVnum();
					std::vector <DWORD> dwVnums;
					std::vector <DWORD> dwCounts;
					std::vector <LPITEM> item_gets(0);
					int count = 0;

					if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					{
						ITEM_MANAGER::instance().RemoveItem(item);
						ITEM_MANAGER::instance().RemoveItem(item2);

						for (int i = 0; i < count; i++){
							switch (dwVnums[i])
							{
								case CSpecialItemGroup::GOLD:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
									break;
								case CSpecialItemGroup::EXP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
									break;
								case CSpecialItemGroup::MOB:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
									break;
								case CSpecialItemGroup::SLOW:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
									break;
								case CSpecialItemGroup::DRAIN_HP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
									break;
								case CSpecialItemGroup::POISON:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
									break;
#ifdef ENABLE_WOLFMAN_CHARACTER
								case CSpecialItemGroup::BLEEDING:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
									break;
#endif
								case CSpecialItemGroup::MOB_GROUP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
									break;
								default:
									if (item_gets[i])
									{
										if (dwCounts[i] > 1)
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetName(), dwCounts[i]);
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetName());

									}
							}
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠가 맞지 않는 것 같다."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("열쇠가 맞지 않는 것 같다."));
					return false;
				}
			}
			break;

		case ITEM_GIFTBOX:
			{
#ifdef ENABLE_NEWSTUFF
				if (g_BoxUseTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_BoxUseTimeLimitValue)))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
					return false;
				}
#endif
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if( dwBoxVnum > 51500 && dwBoxVnum < 52000 )
				{
					if( !(this->DragonSoul_IsQualified()) )
					{
						ChatPacket(CHAT_TYPE_INFO,LC_TEXT("먼저 용혼석 퀘스트를 완료하셔야 합니다."));
						return false;
					}
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount()-1);

					for (int i = 0; i < count; i++){
						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
							break;
						case CSpecialItemGroup::EXP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
							break;
						case CSpecialItemGroup::MOB:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
							break;
						case CSpecialItemGroup::SLOW:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
							break;
						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
							break;
						case CSpecialItemGroup::POISON:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
							break;
#ifdef ENABLE_WOLFMAN_CHARACTER
						case CSpecialItemGroup::BLEEDING:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
							break;
						default:
							if (item_gets[i])
							{
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetName());
							}
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
					return false;
				}
			}
			break;

		case ITEM_SKILLFORGET:
			{
				if (!item->GetSocket(0))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					return false;
				}

				DWORD dwVnum = item->GetSocket(0);

				if (SkillLevelDown(dwVnum))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("스킬 레벨을 내리는데 성공하였습니다."));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("스킬 레벨을 내릴 수 없습니다."));
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
					return false;
				}

				DWORD dwVnum = 0;

				if (item->GetVnum() == 50300)
				{
					dwVnum = item->GetSocket(0);
				}
				else
				{
					dwVnum = item->GetValue(0);
				}

				if (0 == dwVnum)
				{
					ITEM_MANAGER::instance().RemoveItem(item);

					return false;
				}

				if (true == LearnSkillByBook(dwVnum))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

					if (distribution_test_server)
						iReadDelay /= 3;

					SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
				}
			}
			break;

		case ITEM_USE:
			{
				if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
				{
					if (test_server)
						sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

					int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
					int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
					int apply_value = item->GetValue(2);
					int apply_duration = item->GetValue(1);

					switch (item->GetSubType())
					{
						case USE_ABILITY_UP:
							if (FindAffect(affect_type, apply_type))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
								return false;
							}

							{
								switch (item->GetValue(0))
								{
									case APPLY_MOV_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_ATT_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
										break;

									case APPLY_STR:
									case APPLY_DEX:
									case APPLY_CON:
									case APPLY_INT:
									case APPLY_CAST_SPEED:
									case APPLY_RESIST_MAGIC:
									case APPLY_ATT_GRADE_BONUS:
									case APPLY_DEF_GRADE_BONUS:
										AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
										break;
								}
							}

							if (GetDungeon())
								GetDungeon()->UsePotion(this);

							if (GetWarMap())
								GetWarMap()->UsePotion(this, item);

							item->SetCount(item->GetCount() - 1);
							break;

					case USE_AFFECT :
						{
							if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
							}
							else
							{
								AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
										break;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("월병 또는 종자 를 사용하였습니다"));
									SetUseSeedOrMoonBottleTime();
								}
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break;
					}

					return true;
				}

				if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
				{
					if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
						return false;
					}
#ifdef ENABLE_NEWSTUFF
					else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
						return false;
					}
#endif
				}

				if (test_server)
				{
					 sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
				}

				switch (item->GetSubType())
				{
					case USE_TIME_CHARGE_PER:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret;
								char buf[128];
								if (item->GetVnum() == DRAGON_HEART_VNUM)
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
								}
								else
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								}
								if (ret > 0)
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%ld}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d초 만큼 충전되었습니다."), ret);
									item->SetCount(item->GetCount() - 1);
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									return true;
								}
								else
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
									{
										sprintf(buf, "No change by item{VN:%d SOC%d:%ld}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									}
									else
									{
										sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("충전할 수 없습니다."));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_TIME_CHARGE_FIX:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								char buf[128];
								if (ret)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d초 만큼 충전되었습니다."), ret);
									sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("충전할 수 없습니다."));
									sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_SPECIAL:

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
						 if (item->GetVnum() == ASLAN_BUFF_NPC_SEAL_VNUM)
						{
							if(GetBuffNPCSystem()->IsActive()) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
								return false;
							}
							
							if (item->GetSocket(0) == 0) {
								SetBuffSealVID(item->GetCell());
								
								TPacketGCBuffNPCAction packet;
								packet.bHeader = HEADER_GC_BUFF_NPC_ACTION;
								packet.bAction = 0;
								packet.dValue0 = 0;
								packet.dValue1 = 0;
								packet.dValue2 = 0;
								packet.dValue3 = 0;
								GetDesc()->Packet(&packet, sizeof(packet));
							}
							else
							{
								DBManager::instance().DirectQuery("UPDATE player.player_buff_npc SET player_name='%s', player_id=%d WHERE id=%d ", GetName(), GetPlayerID(), item->GetSocket(0));
								item->RemoveFromCharacter();
								GetBuffNPCSystem()->LoadBuffNPC();
								SetQuestFlag("buff_npc.is_summon", 1);
								GetBuffNPCSystem()->Summon();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_ACTIVATE_SEAL"));
							}
						}
#endif
						switch (item->GetVnum())
						{
							case ITEM_NOG_POCKET:
								{
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
										return false;
									}
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_RAMADAN_CANDY:
								{
									// @fixme147 BEGIN
									if (FindAffect(AFFECT_RAMADAN_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
										return false;
									}
									// @fixme147 END
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;
							case ITEM_MARRIAGE_RING:
								{
									marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
									if (pMarriage)
									{
										if (pMarriage->ch1 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
												break;
											}
										}

										if (pMarriage->ch2 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
												break;
											}
										}

										int consumeSP = CalculateConsumeSP(this);

										if (consumeSP < 0)
											return false;

										PointChange(POINT_SP, -consumeSP, false);

										WarpToPID(pMarriage->GetOther(GetPlayerID()));
									}
									else
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("결혼 상태가 아니면 결혼반지를 사용할 수 없습니다."));
								}
								break;

							case UNIQUE_ITEM_CAPE_OF_COURAGE:
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
								AggregateMonster();
								item->SetCount(item->GetCount()-1);
								break;
								
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
							case PRIVATE_SHOP_SEARCH_LOOKING_GLASS:
							case PRIVATE_SHOP_SEARCH_TRADING_GLASS:
								OpenPrivateShopSearch(item->GetVnum());
								break;
#endif								

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							case ACCE_REVERSAL_VNUM_1:
							case ACCE_REVERSAL_VNUM_2:
							{
								LPITEM item2;
								if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
									return false;

								if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
									return false;

								if (!CleanAcceAttr(item, item2))
									return false;
								item->SetCount(item->GetCount()-1);
								break;
							}
#endif

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_TREASURE_BOX:
								break;

							case 30093:
							case 30094:
							case 30095:
							case 30096:

								{
									const int MAX_BAG_INFO = 26;
									static struct LuckyBagInfo
									{
										DWORD count;
										int prob;
										DWORD vnum;
									} b1[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27003 },
										{ 10,	100,	27005 },
										{ 10,	50,	27006 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};

									LuckyBagInfo * bi = NULL;
									bi = b1;

									int pct = number(1, 1000);

									int i;
									for (i=0;i<MAX_BAG_INFO;i++)
									{
										if (pct <= bi[i].prob)
											break;
										pct -= bi[i].prob;
									}
									if (i>=MAX_BAG_INFO)
										return false;

									if (bi[i].vnum == 50300)
									{
										GiveRandomSkillBook();
									}
									else if (bi[i].vnum == 1)
									{
										PointChange(POINT_GOLD, 1000, true);
									}
									else
									{
										AutoGiveItem(bi[i].vnum, bi[i].count);
									}
									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							case 50004:
								{
									if (item->GetSocket(0))
									{
										item->SetSocket(0, item->GetSocket(0) + 1);
									}
									else
									{
										int iMapIndex = GetMapIndex();

										PIXEL_POSITION pos;

										if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
										{
											item->SetSocket(0, 1);
											item->SetSocket(1, pos.x);
											item->SetSocket(2, pos.y);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 곳에선 이벤트용 감지기가 동작하지 않는것 같습니다."));
											return false;
										}
									}

									int dist = 0;
									float distance = (DISTANCE_SQRT(GetX()-item->GetSocket(1), GetY()-item->GetSocket(2)));

									if (distance < 1000.0f)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이벤트용 감지기가 신비로운 빛을 내며 사라집니다."));

										struct TEventStoneInfo
										{
											DWORD dwVnum;
											int count;
											int prob;
										};
										const int EVENT_STONE_MAX_INFO = 15;
										TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  8 },
											{ 27004, 10,  6 },
											{ 27002, 10, 12 },
											{ 27005, 10, 12 },
											{ 27100,  1,  9 },
											{ 27103,  1,  9 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 12 },

											{ 25040,  1,  4 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1,  0 },

											{ 50001,  1,  0 },
											{ 50003,  1,  1 },
										};
										TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  1 },
											{ 27004, 10,  1 },
											{ 27004, 10,  9 },
											{ 27005, 10,  9 },
											{ 27100,  1,  5 },
											{ 27103,  1,  5 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 14 },

											{ 25040,  1,  5 },

											{ 27410,  1,  5 },
											{ 27600,  1,  5 },
											{ 25100,  1,  5 },

											{ 50001,  1,  0 },
											{ 50003,  1,  5 },

										};
										TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  0 },
											{ 27004, 10,  0 },
											{ 27002, 10,  0 },
											{ 27005, 10,  0 },
											{ 27100,  1,  0 },
											{ 27103,  1,  0 },
											{ 27101,  1,  0 },
											{ 27104,  1,  0 },
											{ 27999,  1, 25 },

											{ 25040,  1,  0 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1, 15 },

											{ 50001,  1, 10 },
											{ 50003,  1, 50 },

										};

										{
											TEventStoneInfo* info;
											if (item->GetSocket(0) <= 4)
												info = info_4;
											else if (item->GetSocket(0) <= 7)
												info = info_7;
											else
												info = info_10;

											int prob = number(1, 100);

											for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
											{
												if (!info[i].prob)
													continue;

												if (prob <= info[i].prob)
												{
													AutoGiveItem(info[i].dwVnum, info[i].count);
													break;
												}
												prob -= info[i].prob;
											}
										}

										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header	= HEADER_GC_CHAT;
										pack_chat.size		= sizeof(TPacketGCChat) + len;
										pack_chat.type		= CHAT_TYPE_COMMAND;
										pack_chat.id		= 0;
										pack_chat.bEmpire	= GetDesc()->GetEmpire();
										//pack_chat.id	= vid;

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());

										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
										return true;
									}
									else if (distance < 20000)
										dist = 1;
									else if (distance < 70000)
										dist = 2;
									else
										dist = 3;

									const int STONE_DETECT_MAX_TRY = 10;
									if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이벤트용 감지기가 흔적도 없이 사라집니다."));
										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
										AutoGiveItem(27002);
										return true;
									}

									if (dist)
									{
										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf),
												"StoneDetect %u %d %d",
											   	(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header	= HEADER_GC_CHAT;
										pack_chat.size		= sizeof(TPacketGCChat) + len;
										pack_chat.type		= CHAT_TYPE_COMMAND;
										pack_chat.id		= 0;
										pack_chat.bEmpire	= GetDesc()->GetEmpire();
										//pack_chat.id		= vid;

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());
									}

								}
								break;

							case 27989:
							case 76006:
								{
									LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

									if (pMap != NULL)
									{
										item->SetSocket(0, item->GetSocket(0) + 1);

										FFindStone f;

										// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
										pMap->for_each(f);

										if (f.m_mapStone.size() > 0)
										{
											std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

											DWORD max = UINT_MAX;
											LPCHARACTER pTarget = stone->second;

											while (stone != f.m_mapStone.end())
											{
												DWORD dist = (DWORD)DISTANCE_SQRT(GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

												if (dist != 0 && max > dist)
												{
													max = dist;
													pTarget = stone->second;
												}
												stone++;
											}

											if (pTarget != NULL)
											{
												int val = 3;

												if (max < 10000) val = 2;
												else if (max < 70000) val = 1;

												ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
														(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("감지기를 작용하였으나 감지되는 영석이 없습니다."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("감지기를 작용하였으나 감지되는 영석이 없습니다."));
										}

										if (item->GetSocket(0) >= 6)
										{
											ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
											ITEM_MANAGER::instance().RemoveItem(item);
										}
									}
									break;
								}
								break;

							case 27996:
								item->SetCount(item->GetCount() - 1);
								AttackedByPoison(NULL); // @warme008
								break;

							case 27987:

								{
									item->SetCount(item->GetCount() - 1);

									int r = number(1, 100);

									if (r <= 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 돌조각이 나왔습니다."));
										AutoGiveItem(27990);
									}
									else
									{
										const int prob_table_gb2312[] =
										{
											95, 97, 99
										};

										const int * prob_table = prob_table_gb2312;

										if (r <= prob_table[0])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개가 흔적도 없이 사라집니다."));
										}
										else if (r <= prob_table[1])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 백진주가 나왔습니다."));
											AutoGiveItem(27992);
										}
										else if (r <= prob_table[2])
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 청진주가 나왔습니다."));
											AutoGiveItem(27993);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("조개에서 피진주가 나왔습니다."));
											AutoGiveItem(27994);
										}
									}
								}
								break;

							case 71013:
								CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50100:
							case 50101:
							case 50102:
							case 50103:
							case 50104:
							case 50105:
							case 50106:
								CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50200:
								if (g_bEnableBootaryCheck)
								{
									if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
									{
										__OpenPrivateShop();
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인 상점을 열 수 없는 지역입니다"));
									}
								}
								else
								{
									__OpenPrivateShop();
								}
								break;

							case fishing::FISH_MIND_PILL_VNUM:
								AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50301:
							case 50302:
							case 50303:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
										return false;
									}

									int lv = GetSkillLevel(SKILL_LEADERSHIP);

									if (lv < item->GetValue(0))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 너무 어려워 이해하기가 힘듭니다."));
										return false;
									}

									if (lv >= item->GetValue(1))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 아무리 봐도 도움이 될 것 같지 않습니다."));
										return false;
									}

									if (LearnSkillByBook(SKILL_LEADERSHIP))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50304:
							case 50305:
							case 50306:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨 30이 되기 전에는 습득할 수 있을 것 같지 않습니다."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨 50이 되기 전에는 습득할 수 있을 것 같지 않습니다."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("연계기는 더이상 수련할 수 없습니다."));
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
									}
								}
								break;
							case 50311:
							case 50312:
							case 50313:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);
									if (GetSkillLevel(dwSkillVnum)>=20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 완벽하게 알아들을 수 있는 언어이다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50061 :
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50314: case 50315: case 50316:
							case 50323: case 50324:
							case 50325: case 50326:
								{
									if (IsPolymorphed() == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 능력을 올릴 수 없습니다."));
										return false;
									}

									int iSkillLevelLowLimit = item->GetValue(0);
									int iSkillLevelHighLimit = item->GetValue(1);
									int iPct = MINMAX(0, item->GetValue(2), 100);
									int iLevelLimit = item->GetValue(3);
									DWORD dwSkillVnum = 0;

									switch (item->GetVnum())
									{
										case 50314: case 50315: case 50316:
											dwSkillVnum = SKILL_POLYMORPH;
											break;

										case 50323: case 50324:
											dwSkillVnum = SKILL_ADD_HP;
											break;

										case 50325: case 50326:
											dwSkillVnum = SKILL_RESIST_PENETRATE;
											break;

										default:
											return false;
									}

									if (0 == dwSkillVnum)
										return false;

									if (GetLevel() < iLevelLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책을 읽으려면 레벨을 더 올려야 합니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책은 너무 어려워 이해하기가 힘듭니다."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 책으로는 더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50902:
							case 50903:
							case 50904:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

										if (test_server)
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
										}
									}
									else
									{
										if (test_server)
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
										}
									}
								}
								break;

								// MINING
							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum)>=40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 수련할 수 없습니다."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
#ifdef ENABLE_BOOKS_STACKFIX
										item->SetCount(item->GetCount() - 1);
#else
										ITEM_MANAGER::instance().RemoveItem(item);
#endif

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;
								// END_OF_MINING

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변신중에는 책을 읽을수 없습니다."));
										return false;

									}
									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 승마 스킬을 수련할 수 있는 레벨이 아닙니다."));
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("주안술서를 통해 주화입마에서 빠져나왔습니다."));
										}
										else
										{
											SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
											return false;
										}
									}

									if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 승마 수련서를 읽을 수 없습니다."));
										return false;
									}

									if (number(1, 100) <= iPct)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("승마 수련서를 읽어 승마 스킬 포인트를 얻었습니다."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("얻은 포인트로는 승마 스킬의 레벨을 올릴 수 있습니다."));
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("승마 수련서 이해에 실패하였습니다."));
									}
#ifdef ENABLE_BOOKS_STACKFIX
									item->SetCount(item->GetCount() - 1);
#else
									ITEM_MANAGER::instance().RemoveItem(item);
#endif
								}
								break;

							case 70102:
							case 70103:
								{
									if (GetAlignment() >= 0)
										return false;

									int delta = MIN(-GetAlignment(), item->GetValue(0));

									sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

									UpdateAlignment(delta);
									item->SetCount(item->GetCount() - 1);

									if (delta / 10 > 0)
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("마음이 맑아지는군. 가슴을 짓누르던 무언가가 좀 가벼워진 느낌이야."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치가 %d 증가하였습니다."), delta/10);
									}
								}
								break;

							case 71107:
							case 39032: // @fixme169 mythical peach alternative vnum
								{
									int val = item->GetValue(0);
									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									if (!pPC) // @fixme169 missing check
										return false;
									int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (get_global_time() - last_use_time < interval * 60 * 60)
									{
										if (test_server == false)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 사용할 수 없습니다."));
											return false;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("테스트 서버 시간제한 통과"));
										}
									}

									if (GetAlignment() == 200000)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치를 더 이상 올릴 수 없습니다."));
										return false;
									}

									if (200000 - GetAlignment() < val * 10)
									{
										val = (200000 - GetAlignment()) / 10;
									}

									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);

									item->SetCount(item->GetCount()-1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

									ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("마음이 맑아지는군. 가슴을 짓누르던 무언가가 좀 가벼워진 느낌이야."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("선악치가 %d 증가하였습니다."), val);

									char buf[256 + 1];
									snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
									LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
								}
								break;

							case 71109:
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetSocketCount() == 0)
										return false;

									switch( item2->GetType() )
									{
										case ITEM_WEAPON:
											break;
										case ITEM_ARMOR:
											switch (item2->GetSubType())
											{
											case ARMOR_EAR:
											case ARMOR_WRIST:
											case ARMOR_NECK:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 영석이 없습니다"));
												return false;
											}
											break;

										default:
											return false;
									}

									std::stack<long> socket;

									for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										socket.push(item2->GetSocket(i));

									int idx = ITEM_SOCKET_MAX_NUM - 1;

									while (socket.size() > 0)
									{
										if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
											break;

										idx--;
										socket.pop();
									}

									if (socket.size() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빼낼 영석이 없습니다"));
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);

										char buf[256+1];
										snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
												item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
										LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case 70201:
							case 70202:
							case 70203:
							case 70204:
							case 70205:
							case 70206:
								{
									// NEW_HAIR_STYLE_ADD
									if (GetPart(PART_HAIR) >= 1001)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("현재 헤어스타일에서는 염색과 탈색이 불가능합니다."));
									}
									// END_NEW_HAIR_STYLE_ADD
									else
									{
										quest::CQuestManager& q = quest::CQuestManager::instance();
										quest::PC* pPC = q.GetPC(GetPlayerID());

										if (pPC)
										{
											int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

											if (last_dye_level == 0 ||
													last_dye_level+3 <= GetLevel() ||
													item->GetVnum() == 70201)
											{
												SetPart(PART_HAIR, item->GetVnum() - 70201);

												if (item->GetVnum() == 70201)
													pPC->SetFlag("dyeing_hair.last_dye_level", 0);
												else
													pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

												item->SetCount(item->GetCount() - 1);
												UpdatePacket();
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d 레벨이 되어야 다시 염색하실 수 있습니다."), last_dye_level+3);
											}
										}
									}
								}
								break;

							case ITEM_NEW_YEAR_GREETING_VNUM:
								{
									DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											if (dwVnums[i] == CSpecialItemGroup::GOLD)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
										}

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case ITEM_VALENTINE_ROSE:
							case ITEM_VALENTINE_CHOCOLATE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 열 수 없습니다."));
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WHITEDAY_CANDY:
							case ITEM_WHITEDAY_ROSE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 열 수 없습니다."));
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case 50011:
								{
									DWORD dwBoxVnum = 50011;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets(0);
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											char buf[50 + 1];
											snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
											LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

											//ITEM_MANAGER::instance().RemoveItem(item);
											item->SetCount(item->GetCount() - 1);

											switch (dwVnums[i])
											{
											case CSpecialItemGroup::GOLD:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), dwCounts[i]);
												break;

											case CSpecialItemGroup::EXP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 부터 신비한 빛이 나옵니다."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d의 경험치를 획득했습니다."), dwCounts[i]);
												break;

											case CSpecialItemGroup::MOB:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
												break;

											case CSpecialItemGroup::SLOW:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 빨간 연기를 들이마시자 움직이는 속도가 느려졌습니다!"));
												break;

											case CSpecialItemGroup::DRAIN_HP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자가 갑자기 폭발하였습니다! 생명력이 감소했습니다."));
												break;

											case CSpecialItemGroup::POISON:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
												break;
#ifdef ENABLE_WOLFMAN_CHARACTER
											case CSpecialItemGroup::BLEEDING:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 나온 녹색 연기를 들이마시자 독이 온몸으로 퍼집니다!"));
												break;
#endif
											case CSpecialItemGroup::MOB_GROUP:
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 몬스터가 나타났습니다!"));
												break;

											default:
												if (item_gets[i])
												{
													if (dwCounts[i] > 1)
														ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 %d 개 나왔습니다."), item_gets[i]->GetName(), dwCounts[i]);
													else
														ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상자에서 %s 가 나왔습니다."), item_gets[i]->GetName());
												}
												break;
											}
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("아무것도 얻을 수 없었습니다."));
										return false;
									}
								}
								break;

							case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
								{
									//PointChange(POINT_GOLD, -iCost);
									PointChange(POINT_STAT_RESET_COUNT, 1);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50107:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN

									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50108:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN

									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
#endif
									item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WONSO_BEAN_VNUM:
								PointChange(POINT_HP, GetMaxHP() - GetHP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_SUGAR_VNUM:
								PointChange(POINT_SP, GetMaxSP() - GetSP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_FRUIT_VNUM:
								PointChange(POINT_STAMINA, GetMaxStamina()-GetStamina());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_ELK_VNUM:
								{
									int iGold = item->GetSocket(0);
									ITEM_MANAGER::instance().RemoveItem(item);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("돈 %d 냥을 획득했습니다."), iGold);
									PointChange(POINT_GOLD, iGold);
								}
								break;

							case 70021:
								{
									int HealPrice = quest::CQuestManager::instance().GetEventFlag("MonarchHealGold");
									if (HealPrice == 0)
										HealPrice = 2000000;

									if (CMonarch::instance().HealMyEmpire(this, HealPrice))
									{
										char szNotice[256];
										snprintf(szNotice, sizeof(szNotice), LC_TEXT("군주의 축복으로 이지역 %s 유저는 HP,SP가 모두 채워집니다."), EMPIRE_NAME(GetEmpire()));
										SendNoticeMap(szNotice, GetMapIndex(), false);

										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군주의 축복을 사용하였습니다."));
									}
								}
								break;

							case 27995:
								{
								}
								break;

							case 71092 :
								{
									if (m_pkChrTarget != NULL)
									{
										if (m_pkChrTarget->IsPolymorphed())
										{
											m_pkChrTarget->SetPolymorph(0);
											m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
										}
									}
									else
									{
										if (IsPolymorphed())
										{
											SetPolymorph(0);
											RemoveAffect(AFFECT_POLYMORPH);
										}
									}
								}
								break;

							case 71051 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType()) // @fixme124
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
									if (item2->AddRareAttribute2())
#else
									if (item2->AddRareAttribute())
#endif
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성공적으로 속성이 추가 되었습니다"));

										int iAddedIdx = item2->GetRareAttrCount() + 4;
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 이 아이템으로 속성을 추가할 수 없습니다"));
									}
								}
								break;

							case 71052 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (ITEM_COSTUME == item2->GetType()) // @fixme124
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
									if (item2->ChangeRareAttribute2())
#else
									if (item2->ChangeRareAttribute())
#endif
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경 시킬 속성이 없습니다"));
									}
								}
								break;

							case ITEM_AUTO_HP_RECOVERY_S:
							case ITEM_AUTO_HP_RECOVERY_M:
							case ITEM_AUTO_HP_RECOVERY_L:
							case ITEM_AUTO_HP_RECOVERY_X:
							case ITEM_AUTO_SP_RECOVERY_S:
							case ITEM_AUTO_SP_RECOVERY_M:
							case ITEM_AUTO_SP_RECOVERY_L:
							case ITEM_AUTO_SP_RECOVERY_X:

							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
							case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
									}
#ifdef ENABLE_NEWSTUFF
									else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
										return false;
									}
#endif

									EAffectTypes type = AFFECT_NONE;
									bool isSpecialPotion = false;

									switch (item->GetVnum())
									{
										case ITEM_AUTO_HP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_HP_RECOVERY_S:
										case ITEM_AUTO_HP_RECOVERY_M:
										case ITEM_AUTO_HP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
											type = AFFECT_AUTO_HP_RECOVERY;
											break;

										case ITEM_AUTO_SP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_SP_RECOVERY_S:
										case ITEM_AUTO_SP_RECOVERY_M:
										case ITEM_AUTO_SP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
										case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
											type = AFFECT_AUTO_SP_RECOVERY;
											break;
									}

									if (AFFECT_NONE == type)
										break;

									if (item->GetCount() > 1)
									{
										int pos = GetEmptyInventory(item->GetSize());

										if (-1 == pos)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지품에 빈 공간이 없습니다."));
											break;
										}

										item->SetCount( item->GetCount() - 1 );

										LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
										item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

										if (item->GetSocket(1) != 0)
										{
											item2->SetSocket(1, item->GetSocket(1));
										}

										item = item2;
									}

									CAffect* pAffect = FindAffect( type );

									if (NULL == pAffect)
									{
										EPointTypes bonus = POINT_NONE;

										if (true == isSpecialPotion)
										{
											if (type == AFFECT_AUTO_HP_RECOVERY)
											{
												bonus = POINT_MAX_HP_PCT;
											}
											else if (type == AFFECT_AUTO_SP_RECOVERY)
											{
												bonus = POINT_MAX_SP_PCT;
											}
										}

										AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

										item->Lock(true);
										item->SetSocket(0, true);

										AutoRecoveryItemProcess( type );
									}
									else
									{
										if (item->GetID() == pAffect->dwFlag)
										{
											RemoveAffect( pAffect );

											item->Lock(false);
											item->SetSocket(0, false);
										}
										else
										{
											LPITEM old = FindItemByID( pAffect->dwFlag );

											if (NULL != old)
											{
												old->Lock(false);
												old->SetSocket(0, false);
											}

											RemoveAffect( pAffect );

											EPointTypes bonus = POINT_NONE;

											if (true == isSpecialPotion)
											{
												if (type == AFFECT_AUTO_HP_RECOVERY)
												{
													bonus = POINT_MAX_HP_PCT;
												}
												else if (type == AFFECT_AUTO_SP_RECOVERY)
												{
													bonus = POINT_MAX_SP_PCT;
												}
											}

											AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

											item->Lock(true);
											item->SetSocket(0, true);

											AutoRecoveryItemProcess( type );
										}
									}
								}
								break;
						}
						break;

					case USE_CLEAR:
						{
							switch (item->GetVnum())
							{
#ifdef ENABLE_WOLFMAN_CHARACTER
								case 27124: // Bandage
									RemoveBleeding();
									break;
#endif
								case 27874: // Grilled Perch
								default:
									RemoveBadAffect();
									break;
							}
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_INVISIBILITY:
						{
							if (item->GetVnum() == 70026)
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC(GetPlayerID());

								if (pPC != NULL)
								{
									int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

									if (get_global_time() - last_use_time < 10*60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 사용할 수 없습니다."));
										return false;
									}

									pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
								}
							}

							AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
										return false;
								}
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("월병 또는 종자 를 사용하였습니다"));
									SetUseSeedOrMoonBottleTime();
								}
								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								//RESTRICT_USE_SEED_OR_MOONBOTTLE
								item->SetCount(item->GetCount() - 1);
								//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
							}
						}
						break;

					case USE_POTION:
						if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
								return false;
							}

							switch (item->GetVnum())
							{
								case 27001 :
								case 27002 :
								case 27003 :
								case 27004 :
								case 27005 :
								case 27006 :
									if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
									{
										if (m_nPotionLimit <= 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용 제한량을 초과하였습니다."));
											return false;
										}
									}
									break;

								default :
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련장에서 사용하실 수 없습니다."));
									return false;
							}
						}
#ifdef ENABLE_NEWSTUFF
						else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
							return false;
						}
#endif

						if (item->GetValue(1) != 0)
						{
							if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
							{
								return false;
							}

							PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_SPUP_BLUE);
						}

						if (item->GetValue(0) != 0)
						{
							if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
							{
								return false;
							}

							PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_HPUP_RED);
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						m_nPotionLimit--;
						break;

					case USE_POTION_CONTINUE:
						{
							if (item->GetValue(0) != 0)
							{
								AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
							}
							else if (item->GetValue(1) != 0)
							{
								AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
							}
							else
								return false;
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_DXUP_PURPLE);
#endif
									break;

								case APPLY_ATT_SPEED:
									AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
									EffectPacket(SE_SPEEDUP_GREEN);
#endif
									break;

								case APPLY_STR:
									AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_TALISMAN:
						{
							const int TOWN_PORTAL	= 1;
							const int MEMORY_PORTAL = 2;

							if (GetMapIndex() == 200 || GetMapIndex() == 113)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("현재 위치에서 사용할 수 없습니다."));
								return false;
							}

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#ifdef ENABLE_NEWSTUFF
							else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("대련 중에는 이용할 수 없는 물품입니다."));
								return false;
							}
#endif

							if (m_pkWarpEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이동할 준비가 되어있음으로 귀환부를 사용할수 없습니다"));
								return false;
							}

							// CONSUME_LIFE_WHEN_USE_WARP_ITEM
							int consumeLife = CalculateConsume(this);

							if (consumeLife < 0)
								return false;
							// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

							if (item->GetValue(0) == TOWN_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (!GetDungeon())
										if (!GiveRecallItem(item))
											return false;

									PIXEL_POSITION posWarp;

									if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
									{
										// CONSUME_LIFE_WHEN_USE_WARP_ITEM
										PointChange(POINT_HP, -consumeLife, false);
										// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

										WarpSet(posWarp.x, posWarp.y);
									}
									else
									{
										sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
									}
								}
								else
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("원래 위치로 복귀"));

									ProcessRecallItem(item);
								}
							}
							else if (item->GetValue(0) == MEMORY_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (GetDungeon())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("던전 안에서는 %s%s 사용할 수 없습니다."),
												item->GetName(),
												"");
										return false;
									}

									if (!GiveRecallItem(item))
										return false;
								}
								else
								{
									// CONSUME_LIFE_WHEN_USE_WARP_ITEM
									PointChange(POINT_HP, -consumeLife, false);
									// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

									ProcessRecallItem(item);
								}
							}
						}
						break;

					case USE_TUNING:
					case USE_DETACHMENT:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 영석은 이 아이템으로 개량할 수 없습니다"));
								return false;
							}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0
								|| item->GetVnum() == ACCE_REVERSAL_VNUM_1
								|| item->GetVnum() == ACCE_REVERSAL_VNUM_2
							)
							{
								if (!CleanAcceAttr(item, item2))
									return false;
								item->SetCount(item->GetCount()-1);
								return true;
							}
#endif

							if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
							{
								if (item->GetVnum() == 71056)
								{
									RefineItem(item, item2);
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("영석은 이 아이템으로 개량할 수 없습니다"));
								}
							}
							else
							{
								RefineItem(item, item2);
							}
						}
						break;

					case USE_CHANGE_COSTUME_ATTR:
					case USE_RESET_COSTUME_ATTR:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME != item2->GetType())
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetAttributeSetIndex() == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->GetAttributeCount() == 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경할 속성이 없습니다."));
								return false;
							}

							switch (item->GetSubType())
							{
								case USE_CHANGE_COSTUME_ATTR:
									item2->ChangeAttribute();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
									}
									break;
								case USE_RESET_COSTUME_ATTR:
									item2->ClearAttribute();
									item2->AlterToMagicItem();
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
									}
									break;
							}

							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));

							item->SetCount(item->GetCount() - 1);
							break;
						}

						//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
#ifdef ELEMENT_SPELL_WORLDARD
					case USE_ELEMENT_DOWNGRADE:
					case USE_ELEMENT_UPGRADE:
					case USE_ELEMENT_CHANGE:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging() || item2->IsEquipped())
								return false;
							//ChatPacket(CHAT_TYPE_INFO, "useitem");
							ElementsSpellItem(item, item2);
						}
						break;
#endif
					case USE_PUT_INTO_BELT_SOCKET:
					case USE_PUT_INTO_RING_SOCKET:
					case USE_PUT_INTO_ACCESSORY_SOCKET:
					case USE_ADD_ACCESSORY_SOCKET:
					case USE_CLEAN_SOCKET:
					case USE_CHANGE_ATTRIBUTE:
					case USE_CHANGE_ATTRIBUTE2 :
					case USE_ADD_ATTRIBUTE:
					case USE_ADD_ATTRIBUTE2:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME == item2->GetType())
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
								return false;
							}

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							switch (item->GetSubType())
							{
								case USE_CLEAN_SOCKET:
									{
										int i;
#ifdef ENABLE_EXTENDED_SOCKETS
										for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
										{
											if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
												break;
										}

#ifdef ENABLE_EXTENDED_SOCKETS
										if (i == ITEM_STONES_MAX_NUM)
#else
										if (i == ITEM_SOCKET_MAX_NUM)
#endif
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("청소할 석이 박혀있지 않습니다."));
											return false;
										}

										int j = 0;

#ifdef ENABLE_EXTENDED_SOCKETS
										for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
										{
											if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
												item2->SetSocket(j++, item2->GetSocket(i));
										}

#ifdef ENABLE_EXTENDED_SOCKETS
										for (; j < ITEM_STONES_MAX_NUM; ++j)
#else
										for (; j < ITEM_SOCKET_MAX_NUM; ++j)
#endif
										{
											if (item2->GetSocket(j) > 0)
												item2->SetSocket(j, 1);
										}

										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
										}

										item->SetCount(item->GetCount() - 1);

									}
									break;

								case USE_CHANGE_ATTRIBUTE :
								case USE_CHANGE_ATTRIBUTE2 : // @fixme123
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->GetAttributeCount() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("변경할 속성이 없습니다."));
										return false;
									}

									if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
									{
										DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

										quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

										if (pPC)
										{
											DWORD dwNowSec = get_global_time();

											DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

											if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 바꾼지 %d분 이내에는 다시 변경할 수 없습니다.(%d 분 남음)"),
														dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
												return false;
											}

											pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
										}
									}

									if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 0, 30, 40, 3
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else if (item->GetVnum() == 76014)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
										{
											0, 10, 50, 39, 1
										};

										item2->ChangeAttribute(aiChangeProb);
									}

									else
									{
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("적용 레벨보다 높아 사용이 불가능합니다."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무기와 갑옷에만 사용 가능합니다."));
												break;
											}
										}
										item2->ChangeAttribute();
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경하였습니다."));
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
									}

									item->SetCount(item->GetCount() - 1);
									break;

								case USE_ADD_ATTRIBUTE :
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->GetAttributeCount() < 4)
									{
										if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("적용 레벨보다 높아 사용이 불가능합니다."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("무기와 갑옷에만 사용 가능합니다."));
												break;
											}
										}
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 실패하였습니다."));
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
										}

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더이상 이 아이템을 이용하여 속성을 추가할 수 없습니다."));
									}
									break;

								case USE_ADD_ATTRIBUTE2 :

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성을 변경할 수 없는 아이템입니다."));
										return false;
									}

									if (item2->GetAttributeCount() == 4)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 성공하였습니다."));

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE2_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("속성 추가에 실패하였습니다."));
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE2_FAIL", buf);
										}

										item->SetCount(item->GetCount() - 1);
									}
									else if (item2->GetAttributeCount() == 5)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("더 이상 이 아이템을 이용하여 속성을 추가할 수 없습니다."));
									}
									else if (item2->GetAttributeCount() < 4)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 재가비서를 이용하여 속성을 추가시켜 주세요."));
									}
									else
									{
										// wtf ?!
										sys_err("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
									}
									break;

								case USE_ADD_ACCESSORY_SOCKET:
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->IsAccessoryForSocket())
										{
											if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
#ifdef ENABLE_ADDSTONE_FAILURE
												if (number(1, 100) <= 50)
#else
												if (1)
#endif
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소켓이 성공적으로 추가되었습니다."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
												}
												else
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소켓 추가에 실패하였습니다."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 소켓을 추가할 공간이 없습니다."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템으로 소켓을 추가할 수 없는 아이템입니다."));
										}
									}
									break;

								case USE_PUT_INTO_BELT_SOCKET:
								case USE_PUT_INTO_ACCESSORY_SOCKET:
									if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
										{
											if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
											{
												item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("장착에 성공하였습니다."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("장착에 실패하였습니다."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											if (item2->GetAccessorySocketMaxGrade() == 0)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("먼저 다이아몬드로 악세서리에 소켓을 추가해야합니다."));
											else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 장착할 소켓이 없습니다."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다이아몬드로 소켓을 추가해야합니다."));
											}
											else
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 액세서리에는 더이상 보석을 장착할 수 없습니다."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템을 장착할 수 없습니다."));
									}
									break;
							}
							if (item2->IsEquipped())
							{
								BuffOnAttr_AddBuffsFromItem(item2);
							}
						}
						break;
						//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

					case USE_BAIT:
						{
							if (m_pkFishingEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시 중에 미끼를 갈아끼울 수 없습니다."));
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

							if (weapon->GetSocket(2))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 꽂혀있던 미끼를 빼고 %s를 끼웁니다."), item->GetName());
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("낚시대에 %s를 미끼로 끼웁니다."), item->GetName());
							}

							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

					case USE_AFFECT :
						{
							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
							}
							else
							{
								AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_CREATE_STONE:
						AutoGiveItem(number(28000, 28013));
						item->SetCount(item->GetCount() - 1);
						break;

					case USE_RECIPE :
						{
							LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
							DWORD dwSourceCount1 = item->GetValue(2);

							LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
							DWORD dwSourceCount2 = item->GetValue(4);

							if (dwSourceCount1 != 0)
							{
								if (pSource1 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 조합을 위한 재료가 부족합니다."));
									return false;
								}
							}

							if (dwSourceCount2 != 0)
							{
								if (pSource2 == NULL)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 조합을 위한 재료가 부족합니다."));
									return false;
								}
							}

							if (pSource1 != NULL)
							{
								if (pSource1->GetCount() < dwSourceCount1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("재료(%s)가 부족합니다."), pSource1->GetName());
									return false;
								}

								pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
							}

							if (pSource2 != NULL)
							{
								if (pSource2->GetCount() < dwSourceCount2)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("재료(%s)가 부족합니다."), pSource2->GetName());
									return false;
								}

								pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
							}

							LPITEM pBottle = FindSpecifyItem(50901);

							if (!pBottle || pBottle->GetCount() < 1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("빈 병이 모자릅니다."));
								return false;
							}

							pBottle->SetCount(pBottle->GetCount() - 1);

							if (number(1, 100) > item->GetValue(5))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("물약 제조에 실패했습니다."));
								return false;
							}

							AutoGiveItem(item->GetValue(0));
						}
						break;
				}
			}
			break;

		case ITEM_METIN:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetType() == ITEM_PICK) return false;
				if (item2->GetType() == ITEM_ROD) return false;

				int i;

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					DWORD dwVnum;

					if ((dwVnum = item2->GetSocket(i)) <= 2)
						continue;

					TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

					if (!p)
						continue;

					if (item->GetValue(5) == p->alValues[5])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 종류의 메틴석은 여러개 부착할 수 없습니다."));
						return false;
					}
				}

				if (item2->GetType() == ITEM_ARMOR)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 메틴석은 장비에 부착할 수 없습니다."));
						return false;
					}
				}
				else if (item2->GetType() == ITEM_WEAPON)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 메틴석은 무기에 부착할 수 없습니다."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("부착할 수 있는 슬롯이 없습니다."));
					return false;
				}

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= 30)
#else
						if (1)
#endif
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 성공하였습니다."));
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("메틴석 부착에 실패하였습니다."));
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}

						LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
						ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
						break;
					}

				if (i == ITEM_SOCKET_MAX_NUM)
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("부착할 수 있는 슬롯이 없습니다."));
			}
			break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
			{
				if (!item->IsEquipped())
					EquipItem(item);
			}
			break;

		case ITEM_BLEND:

			sys_log(0,"ITEM_BLEND!!");
			if (Blend_Item_find(item->GetVnum()))
			{
				int		affect_type		= AFFECT_BLEND;
				int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
				int		apply_value		= item->GetSocket(1);
				int		apply_duration	= item->GetSocket(2);

				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 효과가 걸려 있습니다."));
					}
					else
					{
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						item->SetCount(item->GetCount() - 1);
					}
				}
			}
			break;
		case ITEM_EXTRACT:
			{
				LPITEM pDestItem = GetItem(DestCell);
				if (NULL == pDestItem)
				{
					return false;
				}
				switch (item->GetSubType())
				{
				case EXTRACT_DRAGON_SOUL:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
					}
					return false;
				case EXTRACT_DRAGON_HEART:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
					}
					return false;
				default:
					return false;
				}
			}
			break;

#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
		case ITEM_PET_ASLAN:
			{
				CGrowthPetSystem* petSystem = GetGrowthPetSystem();

				switch (item->GetSubType())
				{
					case PET_EGG_ASLAN:
						SetEggVid(item->GetVnum());
						ChatPacket(CHAT_TYPE_COMMAND, "GrowthPetOpenIncubator %d %d %d", 0, item->GetVnum(), GROWTH_PET_BORN_COST);
						return false;

					case PET_SEAL_ASLAN:
						{
							if (LPITEM item2 = GetItem(DestCell)) 
							{
								if (item2->GetVnum() == 55002) 
								{
									if (item2->GetSocket(0) > 0) 
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_TRANSPORTBOX_IS_FULL"));
									else if (item->GetVnum() == 55002 && item2->GetVnum() == 55002 && item->GetSocket(0) == 0) {
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_TRANSPORTBOX_IS_EMPTY"));
										return false;
									}
									else
									{
										if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
											return false;

										if (item->GetSocket(1) < time(0) || item2->GetSocket(4) != 0) {
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_IS_DEAD"));
											return false;
										}

										if (item2->IsExchanging())
											return false;

										if (petSystem->IsSummoned() && item->GetID() == petSystem->GetSummonItemID()) {
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_UNSUMMON_FIRST"));
											return false;
										}

										item2->SetSocket(0, item->GetVnum());					// Item Vnum
										item2->SetSocket(1, item->GetSocket(1) - time(0));		// Restlaufzeit
										item2->SetSocket(2, time(0) - item->GetSocket(2));	 	// Differenz Jetzt - Borntime UNIX

										int box_duration = time(0) + 60 * 60 * 24 * GROWTH_PET_TRANSPORTBOX_DURATION_DAYS;

										item2->SetSocket(3, item->GetSocket(3)); // Max Duration UNIX
										item2->SetSocket(4, 0);
										item2->SetSocket(5, box_duration);

										item2->SetForceAttribute(0, item->GetAttributeType(0), item->GetAttributeValue(0));
										item2->SetForceAttribute(1, item->GetAttributeType(1), item->GetAttributeValue(1));

										item2->SetForceAttribute(2, item->GetAttributeType(2), item->GetAttributeValue(2));
										item2->SetForceAttribute(3, item->GetAttributeType(3), item->GetAttributeValue(3));
										item2->SetForceAttribute(4, item->GetAttributeType(4), item->GetAttributeValue(4));
										item2->SetForceAttribute(5, item->GetAttributeType(5), item->GetAttributeValue(5));

										DBManager::instance().DirectQuery("UPDATE player_petsystem SET id =%d WHERE id = %d", item2->GetID(), item->GetID());
										item->StopCheckPetDeadEvent();
										ITEM_MANAGER::instance().RemoveItem(item);
										return true;
									}
								}
								return false;
							}
							else {
								if (petSystem->IsSummoned()) {
									if (item->GetVID() == petSystem->GetSummonItemVID()) {
										petSystem->Unsummon();
										SetQuestFlag("growth_pet.seal_id", 0);
									}
									else {
										petSystem->Unsummon();
										if (item->GetSocket(1) - time(0) > 0) {
											petSystem->Summon(item);
											SetQuestFlag("growth_pet.seal_id", item->GetID());
										}
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_IS_DEAD"));
									}
								}
								else {
									if (item->GetSocket(1) - time(0) > 0) {
										petSystem->Summon(item);
										SetQuestFlag("growth_pet.seal_id", item->GetID());
									}
									else
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_IS_DEAD"));
								}
							}
						}
						return false;

					case PET_BOOK_ASLAN:
					{
						if (!petSystem->IsSummoned()){
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_SHOULD_SUMMONED"));
							return false;
						}

						int SkillID;

						for (int i = 0; i < PET_SKILL_MAX_NUM + 1; ++i) {
							if (item->GetVnum() == PetSkillTable[i].dwBook) {
								SkillID = i;
								break;
							}
						}

						if (SkillID == petSystem->GetPetSkillType(0) && petSystem->GetPetSkillLevel(0) < 20 || SkillID == petSystem->GetPetSkillType(1) && petSystem->GetPetSkillLevel(1) < 20 || SkillID == petSystem->GetPetSkillType(2) && petSystem->GetPetSkillLevel(2) < 20)
						{
							int chance = 50;

							item->SetCount(item->GetCount() - 1);

							if (chance >= number(1, 100)) {
								petSystem->IncreasePetSkill(SkillID);
								return true;
							}
							else {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_FAIL_LEARN_SKILL"));
								return false;
							}
						}
						else if (SkillID == petSystem->GetPetSkillType(0) && petSystem->GetPetSkillLevel(0) >= 20 || SkillID == petSystem->GetPetSkillType(1) && petSystem->GetPetSkillLevel(1) >= 20 || SkillID == petSystem->GetPetSkillType(2) && petSystem->GetPetSkillLevel(2) >= 20) {
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_SKILL_MAX_POINT"));
							return false;
						}
						else if (petSystem->GetPetSkillType(0) == 0 || petSystem->GetPetSkillType(1) == 0 || petSystem->GetPetSkillType(2) == 0) {
							petSystem->IncreasePetSkill(SkillID);
							item->SetCount(item->GetCount() - 1);
							return true;
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_LEARN_NEW_SKILL"));
							return false;
						}
						return false;
					}
					case PET_REVIVE_ASLAN:
					{
						LPITEM item2;

						if (item->GetVnum() != 55001) {
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_FEED_ITEM_IS_FALSE"));
							return false;
						}

						if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
							return false;

						if (item2->IsExchanging())
							return false;

						if (item2->GetType() == ITEM_PET_ASLAN && item2->GetSubType() == PET_SEAL_ASLAN)
						{
							if (petSystem->IsSummoned()) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_UNSUMMON_FIRST"));
								return false;
							}

							if (item2->GetSocket(1) < time(0) && item2->GetSocket(4) != 0)
							{
								item2->SetSocket(1, time(0) + (item2->GetSocket(3) / 2));
								item2->SetSocket(2, time(0));
								item2->SetSocket(4, 0);

								DBManager::instance().DirectQuery("UPDATE player_petsystem SET item_duration=%d, birthday=NOW(), age=0 WHERE id = %d", item2->GetSocket(1), item2->GetID());
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_REFILL_LIFETIME_HALF"));
								item->SetCount(item->GetCount() - 1);
								return true;
							}
							else
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_REFILL_LIFETIME_WITH_UI"));

							return false;

						}
						else
							return false;
					}
					case PET_FEED_ASLAN:
						return false;

					case PET_BOX_ASLAN:
					{
						if (LPITEM item2 = GetItem(DestCell))
						{
							if (item2->GetSocket(0) > 0)
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_TRANSPORTBOX_IS_FULL"));
							else if (item->GetVnum() == 55002 && item2->GetVnum() == 55002 && item->GetSocket(0) == 0) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_TRANSPORTBOX_IS_EMPTY"));
								return false;
							}
							else
							{
								if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
									return false;

								if (item2->IsExchanging())
									return false;

								item2->SetSocket(0, item->GetSocket(0));				// Item Vnum
								item2->SetSocket(1, item->GetSocket(1));				// Restlaufzeit
								item2->SetSocket(2, item->GetSocket(2)); 				// Borntime UNIX
								
								int box_duration = time(0) + 60 * 60 * 24 * GROWTH_PET_TRANSPORTBOX_DURATION_DAYS;

								item2->SetSocket(3, item->GetSocket(3)); // Max Duration UNIX
								item2->SetSocket(4, 0);
								item2->SetSocket(5, box_duration);

								item2->SetForceAttribute(0, item->GetAttributeType(0), item->GetAttributeValue(0));
								item2->SetForceAttribute(1, item->GetAttributeType(1), item->GetAttributeValue(1));
								item2->SetForceAttribute(2, item->GetAttributeType(2), item->GetAttributeValue(2));
								item2->SetForceAttribute(3, item->GetAttributeType(3), item->GetAttributeValue(3));
								item2->SetForceAttribute(4, item->GetAttributeType(4), item->GetAttributeValue(4));
								item2->SetForceAttribute(5, item->GetAttributeType(5), item->GetAttributeValue(5));

								DBManager::instance().DirectQuery("UPDATE player_petsystem SET id =%d WHERE id = %d", item2->GetID(), item->GetID());
								item->StopCheckPetDeadEvent();
								ITEM_MANAGER::instance().RemoveItem(item);
								return true;
							}
						}
						else {
#ifdef ENABLE_SPECIAL_INVENTORY
							int pos = GetEmptyInventory(item);
#else
							int pos = GetEmptyInventory(item->GetSize());
#endif
							if (item->GetSocket(5) < time(0)) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_MUST_TRANSFER_IN_NEW_BOX"));
								return false;
							}

							if (pos == -1) {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PET_SYSTEM_NOT_ENOUGHT_INVENTORYSPACE"));
								return false;
							}

							if (item->IsExchanging())
								return false;

							LPITEM itemseal = AutoGiveItem(item->GetSocket(0), 1);

							itemseal->SetSocket(1, item->GetSocket(1) + time(0));
							itemseal->SetSocket(2, item->GetSocket(2) + time(0));
							itemseal->SetSocket(3, item->GetSocket(3));
							itemseal->SetSocket(4, 0);
							itemseal->SetSocket(5, 0);
							itemseal->SetForceAttribute(0, item->GetAttributeType(0), item->GetAttributeValue(0));
							itemseal->SetForceAttribute(1, item->GetAttributeType(1), item->GetAttributeValue(1));
							itemseal->SetForceAttribute(2, item->GetAttributeType(2), item->GetAttributeValue(2));
							itemseal->SetForceAttribute(3, item->GetAttributeType(3), item->GetAttributeValue(3));
							itemseal->SetForceAttribute(4, item->GetAttributeType(4), item->GetAttributeValue(4));
							itemseal->SetForceAttribute(5, item->GetAttributeType(5), item->GetAttributeValue(5));
							itemseal->StartCheckPetDeadEvent();

							DBManager::instance().DirectQuery("UPDATE player_petsystem SET id = %d, birthday = (NOW() - INTERVAL %d SECOND) WHERE id = %d", itemseal->GetID(), item->GetSocket(2), item->GetID());
							ITEM_MANAGER::instance().RemoveItem(item);
							return true;
						}
					}
					default:
						return false;
				}
			}
			break;
#endif
		case ITEM_NONE:
			sys_err("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

	if (!item->CanUsedBy(this))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("군직이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if(Cell.IsBuffEquipmentPosition())
	{
#ifdef ENABLE_SPECIAL_INVENTORY
		int iEmptyCell = GetEmptyInventory(item);
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#endif

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("사용할수 없습니다."));
			return false;
		}

		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("삼거리 전투 참가중에는 귀환부,귀환기억부를 사용할수 없습니다."));
			return false;
		}
		int iPulse = thecore_pulse();

		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("창고를 연후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);

			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			return false;
		}

		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 귀환부,귀환기억부 를 사용할수 없습니다."));
			return false;
		}

		//PREVENT_REFINE_HACK

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 개량후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK

		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("개인상점 사용후 %d초 이내에는 귀환부,귀환기억부를 사용할 수 없습니다."), g_nPortalLimitTime);
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}

			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x,2) + pow((float)y,2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이동 되어질 위치와 너무 가까워 귀환부를 사용할수 없습니다."));
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant,nDist);
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE

		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래 후 %d초 이내에는 귀환부,귀환기억부등을 사용할 수 없습니다."), g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049))
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("거래창,창고 등을 연 상태에서는 보따리,비단보따리를 사용할수 없습니다."));
			return false;
		}

	}
	//END_PREVENT_TRADE_WINDOW

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

bool CHARACTER::DestroyItem(TItemPos Cell)
{
	LPITEM item = NULL;
	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("??창을 연 상태에?는 아이템을 옮길 수 없습니다."));
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("강화창을 연 상태에서는 아이템을 옮길 수 없습니다."));
		return false;
	}
	if (IsDead())
		return false;
	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;
	if (item->IsExchanging())
		return false;
	if (true == item->isLocked())
		return false;
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;
	if (item->GetCount() <= 0)
		return false;
	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
	ITEM_MANAGER::instance().RemoveItem(item);
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Du hast %s zerstoert."), item->GetName());
#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
	if (item->GetType() == ITEM_PET_ASLAN && item->GetSubType() == PET_SEAL_ASLAN)
		DBManager::instance().DirectQuery("DELETE from player_petsystem WHERE id='%d'", item->GetID());
#endif
	return true;
}

#ifdef ENABLE_SELL_ITEM
bool CHARACTER::SellItem(TItemPos Cell)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("#4 <Unknown translate> Report GM"));
		return false;
	}

#ifdef __INVENTORY_PROTECT_SYSTEM__
	if (GetQuestFlag("abs.INVENTORY_LOCK") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your inventory is locked."));
		return false;
	}
#endif

#ifdef __ATTR_TRANSFER_SYSTEM__
	if (IsAttrTransferOpen())
		return false;
#endif

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging() || item->IsEquipped())
		return false;

	if (true == item->isLocked())
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (item->GetCount() <= 0)
		return false;

	long long dwPrice = item->GetShopBuyPrice();
	dwPrice *= item->GetCount();

	const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(dwPrice);

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), dwPrice, GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have reached the maximum limit of yang."));
		return false;
	}

	item->SetCount(item->GetCount() - item->GetCount());
	PointChange(POINT_GOLD, dwPrice, false);

	char buf[1024];
	char itemlink[256];
	int len;
	len = snprintf(itemlink, sizeof(itemlink), "item:%x:%x", item->GetVnum(), item->GetFlag());

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", item->GetSocket(i));

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		if (item->GetAttributeType(i) != 0)
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d", item->GetAttributeType(i), item->GetAttributeValue(i));

	snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, item->GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You succesfully sell %s."), buf);
	return true;
}
#endif

bool CHARACTER::DropItem(TItemPos Cell, BYTE bCount)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("강화창을 연 상태에서는 아이템을 옮길 수 없습니다."));
		return false;
	}

#ifdef ENABLE_NEWSTUFF
	if (g_ItemDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::ItemDrop, std::chrono::milliseconds(g_ItemDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
		return false;
	}
#endif
	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("버릴 수 없는 아이템입니다."));
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("떨어진 아이템은 3분 후 사라집니다."));
#ifdef ENABLE_NEWSTUFF
		pkItemToDrop->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPITEM]);
#else
		pkItemToDrop->StartDestroyEvent();
#endif

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}

bool CHARACTER::DropGold(int gold)
{
	if (gold <= 0 || gold > GetGold())
		return false;

	if (!CanHandleItem())
		return false;

#ifdef ENABLE_NEWSTUFF
	if (g_GoldDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_GoldDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
		return false;
	}
#endif

	LPITEM item = ITEM_MANAGER::instance().CreateItem(1, gold);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			//Motion(MOTION_PICKUP);
			PointChange(POINT_GOLD, -gold, true);

			if (gold > 1000)
				LogManager::instance().CharLog(this, gold, "DROP_GOLD", "");

#ifdef ENABLE_NEWSTUFF
			item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPGOLD]);
#else
			item->StartDestroyEvent();
#endif
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("떨어진 아이템은 %d분 후 사라집니다."), 150/60);
		}

		Save();
		return true;
	}

	return false;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CHARACTER::DropCheque(int cheque)
{
#ifdef DISABLE_CHEQUE_DROP
	return false;
#else
	if (cheque <= 0 || cheque > GetCheque())
		return false;

	if (cheque >= CHEQUE_MAX)
		return false;

	if (!CanHandleItem())
		return false;

#ifdef ENABLE_NEWSTUFF
	if (g_GoldDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_GoldDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
		return false;
	}
#endif

	LPITEM item = ITEM_MANAGER::instance().CreateItem(CHEQUE_VNUM, cheque);
	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();
		if (item->AddToGround(GetMapIndex(), pos))
		{
			PointChange(POINT_CHEQUE, -cheque, true);
			LogManager::instance().CharLog(this, cheque, "DROP_CHEQUE", "");
			#ifdef ENABLE_NEWSTUFF
			item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPGOLD]);
			#else
			item->StartDestroyEvent();
			#endif
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("떨어진 아이템은 %d분 후 사라집니다."), 150/60);
		}
		Save();
		return true;
	}
	return false;
#endif
}
#endif

bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, BYTE count)
{
	if (Cell.IsSamePosition(DestCell)) // @fixme196 (check same slot n same window aliases)
		return false;

	if (!IsValidItemPosition(Cell))
		return false;

	LPITEM item = NULL;
	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (true == item->isLocked())
		return false;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if(DestCell.IsBuffEquipmentPosition())
	{
		if(DestCell.IsBuffEquipmentPosition() && !Cell.IsEquipPosition()) {
			if(!EquipBuffItem(DestCell.cell,item)) {
				return false;
			}
		}
		else if (Cell.IsBuffEquipmentPosition() && !DestCell.IsEquipPosition()) {
			if(!UnequipBuffItem(Cell.cell,item)) {
				return false;
			}
		}
	}
#endif

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("강화창을 연 상태에서는 아이템을 옮길 수 없습니다."));
		return false;
	}

	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 벨트 인벤토리로 옮길 수 없습니다."));
		return false;
	}

	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 장비를 착용하고 있습니다."));

			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
				count = item->GetCount();

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif
			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);

			char szBuf[51+1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item)
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center )
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, iMoney, true);

					if (iMoney > 1000)
					{
						LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", ""));
					}
				}
		}
	};
}

void CHARACTER::GiveGold(int iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, dwMyAmount, true);

		if (dwMyAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", ""));
		}
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);

		if (iAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, iAmount, "GET_GOLD", ""));
		}
	}
}

#ifdef ENABLE_CHEQUE_SYSTEM
void CHARACTER::GiveCheque(int iAmount)
{
	if (iAmount <= 0)
		return;

	PointChange(POINT_CHEQUE, iAmount, true);
}
#endif

bool CHARACTER::PickupItem(DWORD dwVID)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this))
	{
		// @fixme150 BEGIN
		if (item->GetType() == ITEM_QUEST)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
				return false;
			}
		}
		// @fixme150 END

		if (item->IsOwnership(this))
		{
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM(item);

				Save();
			}
#if defined(ENABLE_CHEQUE_SYSTEM) && !defined(DISABLE_CHEQUE_DROP)
			else if (item->GetVnum() == CHEQUE_VNUM)
			{
				if (item->GetCount() + GetCheque() > CHEQUE_MAX - 1)
					return false;
				GiveCheque(item->GetCount());
				item->RemoveFromGround();
				M2_DESTROY_ITEM(item);
				Save();
			}
#endif

			else
			{
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
					BYTE bCount = item->GetCount();

					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;

							BYTE bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s"), item2->GetName());
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								return true;
							}
						}
					}

					item->SetCount(bCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
				else
				{
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}

				item->RemoveFromGround();

				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

				char szHint[32+1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s"), item->GetName());

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);

			GetParty()->ForEachOnlineMember(funcFindOwnership);

			LPCHARACTER owner = funcFindOwnership.owner;
			// @fixme115
			if (!owner)
				return false;

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
			}
			else
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("소지하고 있는 아이템이 너무 많습니다."));
						return false;
					}
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

			char szHint[32+1];
			snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog(owner, item, "GET", szHint);

			if (owner == this)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s"), item->GetName());
			else
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s 님으로부터 %s"), GetName(), item->GetName());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 전달: %s 님에게 %s"), owner->GetName(), item->GetName());
			}

			if (item->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem (owner->GetPlayerID(), item);

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(BYTE bCell, BYTE bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);

	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (bCell == bDestCell)
		return false;

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;

	if (item1 == item2)
	{
	    sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
	    return false;
	}

	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
				return false;
		}

		if (bEquipCell != item1->FindEquipCell(this))
			return false;

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
		else
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
			return false;
		}
	}
#endif

	if (false == CanUnequipNow(item))
		return false;

	int pos;
	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
	else
		pos = GetEmptyInventory(item->GetSize());

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
	{
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
	}
	else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));

	CheckMaximumPoints();

	return true;
}

//

//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말을 탄 상태에서 예복을 입을 수 없습니다."));
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑 중에는 착용중인 장비를 변경할 수 없습니다."));
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("성별이 맞지않아 이 아이템을 사용할 수 없습니다."));
		return false;
	}

	if (item->IsRideItem())
	{
		#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
			#ifdef ENABLE_NEWSTUFF
			// block mount spawn
			if (g_NoMountAtGuildWar && GetWarMap())
			{
				if (IsRiding())
					StopRiding();
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
				return false;
			}
			#endif

			// unsummon horse first
			if (GetHorse() || IsHorseRiding()) {
				StopRiding();
				HorseSummon(false);
			}

			if (GetHorse() || IsHorseRiding())
				return false;
		#else
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 탈것을 이용중입니다."));
		return false;
		#endif
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW
		&& (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("가만히 있을 때만 착용할 수 있습니다."));
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip the costume weapon. Wrong equipped weapon."));
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "이미 같은 종류의 용혼석을 착용하고 있습니다.");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}

	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			if (0 == item->GetSocket(1))
			{
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}
		#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
		#endif

		if (item->IsOldMountItem()) // @fixme152
			quest::CQuestManager::instance().SIGUse(GetPlayerID(), quest::QUEST_NO_NPC, item, false);
		#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
		else if (item->IsNewMountItem()) {
			const auto mountVnum = GetPoint(POINT_MOUNT);
			MountVnum(mountVnum);
		}
		#endif

	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if(0 == bOldValue)
	{
		CBuffOnAttributes* pBuff = NULL;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
				{
					static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] = {
						WEAR_COSTUME_BODY,
						WEAR_COSTUME_HAIR,
						#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
						WEAR_COSTUME_MOUNT,
						#endif
						#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						WEAR_COSTUME_WEAPON,
						#endif
					};
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			default:
				break;
			}
			m_map_buff_on_attrs.emplace(bType, pBuff);

		}
		else
			pBuff = it->second;
		if (pBuff != NULL)
			pBuff->On(bNewValue);
	}
	else
	{
		assert (m_map_buff_on_attrs.end() != it);
		it->second->ChangeBuffValue(bNewValue);
	}
}

LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
	for (int i=0 ; i < INVENTORY_MAX_NUM ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	return NULL;
}

int CHARACTER::CountSpecifyItem(DWORD vnum) const
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!",item->GetID());
		return;
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
	else
	{
		cell = GetEmptyInventory (item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
			item->SetOwnership (this, 300);
		else
			item->SetOwnership (this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
{
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

				auto bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s"), item->GetName());

					return item;
				}
			}
		}
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i=0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
							inv_item->GetSocket(1) == item->GetSocket(1) &&
							inv_item->GetSocket(2) == item->GetSocket(2) &&
							inv_item->GetCount() < g_bItemCountLimit)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
	else
		iEmptyCell = GetEmptyInventory(item->GetSize());

	if (iEmptyCell != -1)
	{
		if (bMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아이템 획득: %s"), item->GetName());

		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH &&
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				return true;
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
				return true;
			break;

			// BUILDING_NPC
		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;
			// END_OF_BUILDING_NPC

		case BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case BLACKSMITH2_MOB:
			if (item->GetRefineSet() >= 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
				return true;
			break;

		case 20101:
		case 20102:
		case 20103:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:

			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽지 않은 말에게 선초를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("죽은 말에게 사료를 먹일 수 없습니다."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				fishing::Grill(from, item);
			else
			{
				// TAKE_ITEM_BUG_FIX
				from->SetQuestNPCID(GetVID());
				// END_OF_TAKE_ITEM_BUG_FIX
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

			// DEVILTOWER_NPC
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
			}
			break;
			// END_OF_DEVILTOWER_NPC

		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
			}
			else
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이 아이템은 개량할 수 없습니다."));
			}
			break;

		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 선초를 주었습니다."));
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("말에게 사료를 주었습니다."));
				item->SetCount(item->GetCount()-1);
				EffectPacket(SE_HPUP_RED);
			}
			break;

		default:
			sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
											std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP()*dwCount/100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP()-1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					bSuccess = true;
				}
				break;
#endif
			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct);

					if (item_get)
					{
						bSuccess = true;
					}
				}
				break;
		}

		if (bSuccess)
		{
			dwItemVnums.emplace_back(dwVnum);
			dwItemCounts.emplace_back(dwCount);
			item_gets.emplace_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 이 머리를 사용할 수 없는 레벨입니다."));
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000;
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			break;
#endif
		default :
			return false;
			break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("동일한 머리 스타일로는 교체할 수 없습니다."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("이미 둔갑중인 상태입니다."));
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("둔갑할 수 없는 상태입니다."));
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("잘못된 둔갑 아이템입니다."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("잘못된 둔갑 아이템입니다."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
			{
				sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

				int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
				if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("나보다 너무 높은 레벨의 몬스터로는 변신 할 수 없습니다."));
					return false;
				}

				int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID)/40 * 25));
				iDuration *= 60;

				DWORD dwBonus = 0;

				dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID)/40) * 100;

				AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

				item->SetCount(item->GetCount()-1);
			}
			break;

		case 50322:
			{
				sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

				if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
				{
					CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
				}
				else
				{
				}
			}
			break;

		default :
			sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;
#ifdef ELEMENT_SPELL_WORLDARD
	if (IsOpenElementsSpell()) return false;
#endif

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM MountCostume = GetWear(WEAR_COSTUME_MOUNT);
#endif

	if( NULL != Unique1 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if( NULL != Unique2 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (MountCostume)
		return UnequipItem(MountCostume);
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i=0 ; i < sizeof(stunSkills)/sizeof(DWORD) ; ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef ENABLE_NEWSTUFF
				&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
			)
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;

						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
							bLog = true;
						pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
					}
					else
					{
						amount = avail;

						ITEM_MANAGER::instance().RemoveItem( pItem );
					}

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange( POINT_HP_RECOVERY, amount );
						EffectPacket( SE_AUTO_HPUP );
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange( POINT_SP_RECOVERY, amount );
						EffectPacket( SE_AUTO_SPUP );
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		return cell < (BUFF_WINDOW_SLOT_MAX_NUM);
#endif
	default:
		return false;
	}
}

#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}

bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
				return false;
			break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("레벨이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("근력이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("지능이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("민첩이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("체력이 낮아 착용할 수 없습니다."));
					return false;
				}
				break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("같은 종류의 유니크 아이템 두 개를 동시에 장착할 수 없습니다."));
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("결혼하지 않은 상태에서 예물을 착용할 수 없습니다."));
			return false;
		}

	}

	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "벨트 인벤토리에 아이템이 존재하면 해제할 수 없습니다.");

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
		else
			pos = GetEmptyInventory(item->GetSize());

		VERIFY_MSG( -1 == pos, "소지품에 빈 공간이 없습니다." );
	}

	return true;
}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
static bool FN_check_item_sex_buff(DWORD dwSex, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) {
		if (0==dwSex)
			return false;
	}
	
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) {
		if (1==dwSex)
			return false;
	}

	return true;
}

static bool FN_check_item_level_buff(DWORD dwLevel, LPITEM item)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (dwLevel < limitValue)
					return false;
				break;
		}
	}
	return true;
}

LPITEM CHARACTER::GetBuffWear(UINT bCell) const
{
	if (bCell >= BUFF_WINDOW_SLOT_MAX_NUM)
	{
		sys_err("CHARACTER::GetBuffWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_PlayerSlots->pBuffEquipmentItem[bCell];
}

bool CHARACTER::IsBuffEquipEmpty()
{
	for (int i = 0; i < 3; ++i) {
		if (m_PlayerSlots->pBuffEquipmentItem[i])
			return false;
	}
	return true;
}

bool CHARACTER::CheckBuffEquipmentPositionAvailable(int iWearCell)
{
	if(iWearCell < 0){
		return false;
	}
	return false;
}

LPITEM CHARACTER::GetBuffEquipmentItem(WORD wCell) const
{
	return GetItem(TItemPos(BUFF_EQUIPMENT,wCell));
}

bool CHARACTER::IsBuffEquipUniqueItem(DWORD dwItemVnum) const
{
	LPITEM item = GetBuffWear(BUFF_WEAR_UNIQUE);

	if (item && item->GetVnum() == dwItemVnum)
		return true;

	return false;
}

bool CHARACTER::EquipBuffItem(BYTE cell, LPITEM item)
{
	if (item->IsExchanging())
		return false;

	int iWearCell = item->FindBuffEquipCell(this);

	if(iWearCell != cell) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_PUT_ITEM_HERE"));
		return false;
	}
	
	if (!GetBuffNPCSystem()->IsActive()) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
		return false;
	}

	if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_SHAMAN"));
		return false;
	}

	if (false == FN_check_item_sex_buff(GetBuffNPCSystem()->GetSex(), item)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_YOU_GENDER"));
		return false;
	}
	
	if (false == FN_check_item_level_buff(GetBuffNPCSystem()->GetLevel(), item)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_TO_LOW_LEVEL"));
		return false;
	}
	
	if (GetBuffNPCSystem() != NULL) {
		if (GetBuffNPCSystem()->IsSummoned()) {
			item->StartUniqueExpireEvent();	
		}
	}

	return true;
}

bool CHARACTER::UnequipBuffItem(BYTE cell, LPITEM item)
{
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_REMOVE_ITEM_FROM_SLOT"));
		return false;
	}

	item->StopUniqueExpireEvent();
	
	return true;
}
#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
