#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include <boost/algorithm/string/predicate.hpp>
#include "shop_manager.h"
#include <cctype>
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

bool CShopManager::Initialize(TShopTable * table, int size)
{
	if (!m_map_pkShop.empty())
		return false;

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.emplace(table->dwVnum, shop);
		m_map_pkShopByNPCVnum.emplace(table->dwNPCVnum, shop);
	}
	char szShopTableExFileName[256];

	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
		"%s/shop_table_ex.txt", LocaleService_GetBasePath().c_str());

	return ReadShopTableEx(szShopTableExFileName);
}

void CShopManager::Destroy()
{
	TShopMap::iterator it = m_map_pkShop.begin();

	while (it != m_map_pkShop.end())
	{
		M2_DELETE(it->second);
		++it;
	}

	m_map_pkShop.clear();
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
		return NULL;

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
		return NULL;

	return (it->second);
}

bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
	if (pkChr->IsDead()) // @fixme326
		return false;

	if (pkChr->GetShopOwner() == pkChrShopKeeper)
		return false;

	// this method is only for NPC
	if (pkChrShopKeeper->IsPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if (pkChr->IsOpenSafebox() || pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->IsCubeOpen())
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("다른 거래창이 열린상태에서는 상점거래를 할수 가 없습니다."));
		return false;
	}
	//END_PREVENT_TRADE_WINDOW

	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}

	LPSHOP pkShop;

	if (iShopVnum)
		pkShop = Get(iShopVnum);
	else
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
		bOtherEmpire = true;

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
		return NULL;

	return it->second;
}

LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, BYTE bItemCount)
{
	if (FindPCShop(ch->GetVID()))
		return NULL;

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.emplace(ch->GetVID(), pkShop);
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}

void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}

void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
#ifdef ENABLE_NEWSTUFF
	if (g_BuySellTimeLimitValue && !PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_BuySellTimeLimitValue)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
		return;
	}
#endif
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상점과의 거리가 너무 멀어 물건을 살 수 없습니다."));
		return;
	}

	CShop* pkShop = ch->GetShop();

	if (!pkShop->IsPCShop())
	{
		//if (pkShop->GetVnum() == 0)
		//	return;
		//const CMob* pkMob = CMobManager::instance().Get(pkShop->GetNPCVnum());
		//if (!pkMob)
		//	return;

		//if (pkMob->m_table.bType != CHAR_TYPE_NPC)
		//{
		//	return;
		//}
	}
	else
	{
	}

	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	int ret = pkShop->Buy(ch, pos);

	if (SHOP_SUBHEADER_GC_OK != ret)
	{
		TPacketGCShop pack;

		pack.header	= HEADER_GC_SHOP;
		pack.subheader	= ret;
		pack.size	= sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CShopManager::Sell(LPCHARACTER ch, BYTE bCell, BYTE bCount)
{
#ifdef ENABLE_NEWSTUFF
	if (g_BuySellTimeLimitValue && !PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::BoxOpening, std::chrono::milliseconds(g_BuySellTimeLimitValue)))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
		return;
	}
#endif
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (!ch->CanHandleItem())
		return;

	if (ch->GetShop()->IsPCShop())
		return;

	if (DISTANCE_APPROX(ch->GetX()-ch->GetShopOwner()->GetX(), ch->GetY()-ch->GetShopOwner()->GetY())>2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("상점과의 거리가 너무 멀어 물건을 팔 수 없습니다."));
		return;
	}

	LPITEM item = ch->GetInventoryItem(bCell);

	if (!item)
		return;

	if (item->IsEquipped() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("착용 중인 아이템은 판매할 수 없습니다."));
		return;
	}

	if (true == item->isLocked())
	{
		return;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return;

	DWORD dwPrice;

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

	dwPrice = item->GetShopBuyPrice();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = bCount;
		else
			dwPrice = bCount / dwPrice;
	}
	else
		dwPrice *= bCount;

#ifndef ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
	dwPrice /= 5;
#endif

	DWORD dwTax = 0;

#ifndef DISABLE_TAX_ON_NPC_SHOP
	int iVal = 3;

	{
		dwTax = dwPrice * iVal/100;
		dwPrice -= dwTax;
	}
#endif

	if (test_server)
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());

	const int64_t nTotalMoney = static_cast<int64_t>(ch->GetGold()) + static_cast<int64_t>(dwPrice);

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %u", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("20억냥이 초과하여 물품을 팔수 없습니다."));
		return;
	}

	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %u", ch->GetName(), item->GetName(), bCount, item->GetID(), dwPrice);

#ifndef DISABLE_TAX_ON_NPC_SHOP
	if (iVal > 0)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("판매금액의 %d %% 가 세금으로 나가게됩니다"), iVal);
#endif

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

	if (bCount == item->GetCount())
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	else
		item->SetCount(item->GetCount() - bCount);

	CMonarch::instance().SendtoDBAddMoney(dwTax, ch->GetEmpire(), ch);

	ch->PointChange(POINT_GOLD, dwPrice, false);
	if (test_server)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item sold for %d yang"), dwPrice);
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
	else
		return true;
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	if (boost::iequals(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (boost::iequals(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	int itemGroupSize = pItemGroup->GetRowCount();
	std::vector <TShopItemTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (int i = 0; i < itemGroupSize; i++)
	{
		if (!pItemGroup->GetValue(i, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}
		if (!pItemGroup->GetValue(i, "price", shopItems[i].price))
		{
			sys_err("row(%d) of group items of group %s does not have price column", i, pNode->GetNodeName().c_str());
			return false;
		}
	}
	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
	{
		stSort = "None";
	}

	if (boost::iequals(stSort, "Asc"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	}
	else if(boost::iequals(stSort, "Desc"))
	{
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
	}

	CGrid grid = CGrid(5, 9);
	int iPos;

	msl::refill(shopTable.items);

	for (size_t i = 0; i < shopItems.size(); i++)
	{
		TItemTable * item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
	}

	shopTable.byItemCount = shopItems.size();
	return true;
}

bool CShopManager::ReadShopTableEx(const char* stFileName)
{
	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
		return true;
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;
	for (int i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.emplace(npcVnum, table);
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.emplace(npcVnum, pkShopEx);
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (pkShopEx->GetVnum() != 0 && m_map_pkShop.find(pkShopEx->GetVnum()) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", pkShopEx->GetVnum());
			return false;
		}
		m_map_pkShop.emplace(pkShopEx->GetVnum(), pkShopEx);
		pkShopEx->AddShopTable(table);
	}

	return true;
}
///Add
#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
void CShopManager::ShopSearchProcess(LPCHARACTER ch, const TPacketCGPrivateShopSearch* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_CLOSE_TABS"));
		return;
	}

	TEMP_BUFFER buf;

	for (std::map<DWORD, CShop*>::const_iterator it = m_map_pkShopByPC.begin(); it != m_map_pkShopByPC.end(); ++it)
	{
		CShop* tShopTable = it->second;
		if (tShopTable == NULL)
			continue;

		LPCHARACTER GetOwner = tShopTable->GetShopOwner();
		if (GetOwner == NULL || ch == GetOwner)
			continue;

		const std::vector<CShop::SHOP_ITEM>& vItemVec = tShopTable->GetItemVector();
		for (std::vector<CShop::SHOP_ITEM>::const_iterator ShopIter = vItemVec.begin(); ShopIter != vItemVec.end(); ++ShopIter)
		{
			LPITEM item = ShopIter->pkItem;
			if (item == NULL)
				continue;

			/*First n character equals(case insensitive)*/
			if (strncasecmp(item->GetName(), p->szItemName, strlen(p->szItemName)))
				continue;

			if ((p->iMinRefine <= item->GetRefineLevel() && p->iMaxRefine >= item->GetRefineLevel()) == false)
				continue;

			if ((p->iMinLevel <= item->GetLevelLimit() && p->iMaxLevel >= item->GetLevelLimit()) == false)
				continue;

			if ((p->iMinGold <= ShopIter->price && p->iMaxGold >= ShopIter->price) == false)
				continue;

#if defined(ENABLE_CHEQUE_SYSTEM)
			if ((p->iMinCheque <= ShopIter->cheque && p->iMaxCheque >= ShopIter->cheque) == false)
				continue;
#endif

			if (p->bMaskType != ITEM_NONE && p->bMaskType != item->GetType()) // ITEM_NONE: All Categories
				continue;

			if (p->iMaskSub != -1 && p->iMaskSub != item->GetSubType()) // -1: No SubType Check
				continue;

			switch (p->bJob)
			{
			case JOB_WARRIOR:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
					continue;
				break;

			case JOB_ASSASSIN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
					continue;
				break;

			case JOB_SHAMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
					continue;
				break;

			case JOB_SURA:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
					continue;
				break;

#if defined(ENABLE_WOLFMAN_CHARACTER)
			case JOB_WOLFMAN:
				if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
					continue;
				break;
#endif
			}

			TPacketGCPrivateShopSearchItem pack2;
			pack2.item.vnum = ShopIter->vnum;
			pack2.item.price = ShopIter->price;
			pack2.item.count = ShopIter->count;
#if defined(ENABLE_CHEQUE_SYSTEM)
			pack2.item.cheque = ShopIter->cheque;
#endif
			pack2.item.display_pos = static_cast<BYTE>(std::distance(vItemVec.begin(), ShopIter));
			pack2.dwShopPID = GetOwner->GetPlayerID();
			std::memcpy(&pack2.szSellerName, GetOwner->GetName(), sizeof(pack2.szSellerName));
			std::memcpy(&pack2.item.alSockets, item->GetSockets(), sizeof(pack2.item.alSockets));
			std::memcpy(&pack2.item.aAttr, item->GetAttributes(), sizeof(pack2.item.aAttr));
#if defined(__BL_TRANSMUTATION__)
			pack2.item.dwTransmutationVnum = item->GetTransmutationVnum();
#endif
			buf.write(&pack2, sizeof(pack2));
		}
	}

	if (buf.size() <= 0)
		return;

	TPacketGCPrivateShopSearch pack;
	pack.header = HEADER_GC_PRIVATE_SHOP_SEARCH;
	pack.size = static_cast<WORD>(sizeof(pack) + buf.size());
	ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

#include "unique_item.h"
#include "target.h"
void CShopManager::ShopSearchBuy(LPCHARACTER ch, const TPacketCGPrivateShopSearchBuyItem* p)
{
	if (ch == NULL || ch->GetDesc() == NULL || p == NULL)
		return;

	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_CLOSE_TABS"));
		return;
	}

	LPCHARACTER ShopCH = CHARACTER_MANAGER::instance().FindByPID(p->dwShopPID);
	if (ShopCH == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_NO_SHOP"));
		return;
	}

	if (ch == ShopCH) // what?
		return;

	CShop* pkShop = ShopCH->GetMyShop();
	if (pkShop == NULL || pkShop->IsPCShop() == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_NO_SHOP"));
		return;
	}

	const BYTE bState = ch->GetPrivateShopSearchState();
	switch (bState)
	{
	case SHOP_SEARCH_LOOKING:
	{
		if (ch->CountSpecifyItem(PRIVATE_SHOP_SEARCH_LOOKING_GLASS) == 0)
		{
			const TItemTable* GlassTable = ITEM_MANAGER::instance().GetTable(PRIVATE_SHOP_SEARCH_LOOKING_GLASS);
			if (GlassTable)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_WHERE_IS_ITEM"), GlassTable->szLocaleName);
			return;
		}
		if (ch->GetMapIndex() != ShopCH->GetMapIndex())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_SAMEMAP_ERR"));
			return;
		}

		const DWORD dwSellerVID(ShopCH->GetVID());
		if (CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID))
			CTargetManager::instance().DeleteTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");

		CTargetManager::Instance().CreateTarget(ch->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__", TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID, 0, ch->GetMapIndex(), "Shop Search", 1);

		if (CTargetManager::instance().GetTargetInfo(ch->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, dwSellerVID))
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_SUCCESS_TARGET"));
		break;
	}

	case SHOP_SEARCH_TRADING:
	{
		if (ch->CountSpecifyItem(PRIVATE_SHOP_SEARCH_TRADING_GLASS) == 0)
		{
			const TItemTable* GlassTable = ITEM_MANAGER::instance().GetTable(PRIVATE_SHOP_SEARCH_TRADING_GLASS);
			if (GlassTable)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_WHERE_IS_ITEM"), GlassTable->szLocaleName);
			return;
		}

		ch->SetMyShopTime();
		int ret = pkShop->Buy(ch, p->bPos, true);

		if (SHOP_SUBHEADER_GC_OK != ret)
		{
			TPacketGCShop pack;
			pack.header = HEADER_GC_SHOP;
			pack.subheader = static_cast<BYTE>(ret);
			pack.size = sizeof(TPacketGCShop);
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_SEARCH_OK"));

		break;
	}
	default:
		sys_err("ShopSearchBuy ch(%s) wrong state(%d)", ch->GetName(), bState);
		break;
	}
}
#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
