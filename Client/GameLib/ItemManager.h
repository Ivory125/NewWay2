#pragma once

#include "ItemData.h"

class CItemManager : public CSingleton<CItemManager>
{
	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		enum EItemScaleColumn
		{
			ITEMSCALE_VNUM,
			ITEMSCALE_JOB,
			ITEMSCALE_SEX,
			ITEMSCALE_SCALE_X,
			ITEMSCALE_SCALE_Y,
			ITEMSCALE_SCALE_Z,
			ITEMSCALE_POSITION_X, // facultative
			ITEMSCALE_POSITION_Y, // facultative
			ITEMSCALE_POSITION_Z, // facultative
			ITEMSCALE_NUM,
			ITEMSCALE_REQ = ITEMSCALE_SCALE_Z + 1,
			ITEMSCALE_AURA_NUM = ITEMSCALE_POSITION_X + 1,
		};
#endif

	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;

	public:
		CItemManager();
		virtual ~CItemManager();

		void			Destroy();

		BOOL			SelectItemData(DWORD dwIndex);
		CItemData *		GetSelectedItemDataPointer();

		BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);

		/////
		bool			LoadItemDesc(const char* c_szFileName);
		bool			LoadItemList(const char* c_szFileName);
		bool			LoadItemTable(const char* c_szFileName);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool			LoadItemScale(const char* c_szFileName);
#endif
		CItemData *		MakeItemData(DWORD dwIndex);
#ifdef ENABLE_IKASHOP_RENEWAL
		void			GetItemsNameMap(std::map<DWORD, std::string>& inMap);
#endif

#ifdef ADMINPANEL_MOD_CREATE_ITEM_ASLAN
		TItemMap* GetAllItemVnums();
		bool			IsAdminPanelItemBlackList(DWORD dwVnum);
		bool			IsRefineble(DWORD dwVnum);
		DWORD			GetItemStartRefineVnum(DWORD dwVnum);
		std::string		GetItemBaseRefineName(DWORD dwVnum);
#endif
	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*>  m_vec_ItemRange;
		CItemData * m_pSelectedItemData;
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
