#ifndef __INC_METIN2_COMMON_DEFINES_H__
#define __INC_METIN2_COMMON_DEFINES_H__
#pragma once
//////////////////////////////////////////////////////////////////////////
// ### Standard Features ###
#define _IMPROVED_PACKET_ENCRYPTION_
#ifdef _IMPROVED_PACKET_ENCRYPTION_
#define USE_IMPROVED_PACKET_DECRYPTED_BUFFER
#endif
#define __PET_SYSTEM__
#define __UDP_BLOCK__
//#define ENABLE_QUEST_CATEGORY

// ### END Standard Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New Features ###
#define ENABLE_NO_MOUNT_CHECK
#define ENABLE_D_NJGUILD
#define ENABLE_FULL_NOTICE
#define ENABLE_NEWSTUFF
#define ENABLE_PORT_SECURITY
#define ENABLE_BELT_INVENTORY_EX
#define ENABLE_CMD_WARP_IN_DUNGEON
// #define ENABLE_ITEM_ATTR_COSTUME
// #define ENABLE_SEQUENCE_SYSTEM
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_QUEST_DIE_EVENT
#define ENABLE_QUEST_BOOT_EVENT
#define ENABLE_QUEST_DND_EVENT
#define ENABLE_PET_SYSTEM_EX
#define ENABLE_SKILL_FLAG_PARTY
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define DISABLE_TAX_ON_NPC_SHOP
#define ENABLE_CHECK_SELL_PRICE
#define ENABLE_GOTO_LAG_FIX
#define ENABLE_MOUNT_COSTUME_EX_SYSTEM
#define ENABLE_PENDANT_SYSTEM
#define ENABLE_GLOVE_SYSTEM
#define ENABLE_MOVE_CHANNEL
#define ENABLE_QUIVER_SYSTEM
#define ENABLE_DB_SQL_LOG
#define ELEMENT_SPELL_WORLDARD
//#define ENABLE_NEWEXP_CALCULATION
#define ENABLE_EXTENDED_SOCKETS
#define WJ_ENABLE_TRADABLE_ICON

enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};

#define ENABLE_WOLFMAN_CHARACTER
#ifdef ENABLE_WOLFMAN_CHARACTER
// #define DISABLE_WOLFMAN_ON_CREATE
#define USE_MOB_BLEEDING_AS_POISON
#define USE_MOB_CLAW_AS_DAGGER
// #define USE_ITEM_BLEEDING_AS_POISON
// #define USE_ITEM_CLAW_AS_DAGGER
#define USE_WOLFMAN_STONES
#define USE_WOLFMAN_BOOKS
#endif

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
// #define USE_MAGIC_REDUCTION_STONES
#endif

// ### END New Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### Ex Features ###
#define DISABLE_STOP_RIDING_WHEN_DIE //	if DISABLE_TOP_RIDING_WHEN_DIE is defined, the player doesn't lose the horse after dying
#define ENABLE_ACCE_COSTUME_SYSTEM //fixed version
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS //enable only positive bonus in acce absorb
#define ENABLE_HIGHLIGHT_NEW_ITEM //if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_KILL_EVENT_FIX //if you want to fix the 0 exp problem about the when kill lua event (recommended)
// #define ENABLE_SYSLOG_PACKET_SENT // debug purposes

#define ENABLE_EXTEND_ITEM_AWARD //slight adjustement
#ifdef ENABLE_EXTEND_ITEM_AWARD
	// #define USE_ITEM_AWARD_CHECK_ATTRIBUTES //it prevents bonuses higher than item_attr lvl1-lvl5 min-max range limit
#endif

#define ENABLE_CHEQUE_SYSTEM
#ifdef ENABLE_CHEQUE_SYSTEM
	#define ENABLE_SHOP_USE_CHEQUE
	#define DISABLE_CHEQUE_DROP
	#define ENABLE_WON_EXCHANGE_WINDOW
#endif
// ### END Ex Features ###
//////////////////////////////////////////////////////////////////////////


#define ENABLE_IKASHOP_RENEWAL
#define ENABLE_IKASHOP_LOGS
#define ENABLE_IKASHOP_ENTITIES
#define ENABLE_LARGE_DYNAMIC_PACKETS

#define ENABLE_ASLAN_MODULAR_ADMIN_PANEL
#ifdef ENABLE_ASLAN_MODULAR_ADMIN_PANEL
	#define ADMINPANEL_MOD_CREATE_ITEM_ASLAN
	// - For New Modules
#endif

#define ENABLE_ASLAN_GROWTH_PET_SYSTEM
#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
	#define GROWTH_PET_MAX_LEVEL 99
	#define GROWTH_PET_MAX_EVO 3
#endif

#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	// You can Deactivate or Activate this
	#define ASLAN_BUFF_NPC_USE_SUMMON_LEVEL_CAP
	#define ASLAN_BUFF_NPC_USE_SKILL_17_TO_M
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT_COST
	
	// Config values ( !!! no deactivate this defines !!! ) 
	#define ASLAN_BUFF_NPC_SEAL_VNUM 71999
	#define ASLAN_BUFF_NPC_MAX_LV 99
	#define ASLAN_BUFF_NPC_MAX_LV_CAP 30 			// is active ASLAN_BUFF_NPC_USE_SUMMON_LEVEL_CAP -> Maximum cap where you can call the buff. ( 0 = you can call to your own level )
	#define ASLAN_BUFF_NPC_START_INT 4
	#define ASLAN_BUFF_NPC_MAX_INT 140
	#define ASLAN_BUFF_NPC_MIN_HP_PERC_USE_SKILL_HEAL 30
	#define ASLAN_BUFF_EXP_RING_ITEM_1 72001
	#define ASLAN_BUFF_EXP_RING_ITEM_2 72002
	#define ASLAN_BUFF_EXP_RING_ITEM_3 72003
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
