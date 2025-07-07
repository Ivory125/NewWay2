#pragma once

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
#define ENABLE_SHOW_CHEST_DROP
#define ELEMENT_SPELL_WORLDARD
//#define ENABLE_SELL_ITEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////
#define ENABLE_EXTENDED_PICKUP
//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###

#define ENABLE_EXTENDED_SOCKETS

// -- ASLAN SYSTEMS -- //

#define ENABLE_ASLAN_TELEPORTPANEL

#define ENABLE_ASLAN_MODULAR_ADMIN_PANEL
#ifdef ENABLE_ASLAN_MODULAR_ADMIN_PANEL
	#define ADMINPANEL_MOD_CREATE_ITEM_ASLAN
	// - For New Modules
#endif

#define ENABLE_ASLAN_GROWTH_PET_SYSTEM

#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT
	#define ASLAN_BUFF_NPC_ENABLE_EXPORT_COST
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
#endif

// -- ASLAN SYSTEMS END -- //

#define ENABLE_PACK_GET_CHECK
#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR
#define ENABLE_DISCORD_RPC
#define ENABLE_PET_SYSTEM_EX
#define ENABLE_LOCALE_EX
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_PENDANT_SYSTEM
#define ENABLE_GLOVE_SYSTEM
#define ENABLE_MOVE_CHANNEL
#define ENABLE_QUIVER_SYSTEM
#define ENABLE_4TH_AFF_SKILL_DESC
#define ENABLE_LOCALE_COMMON
#define BL_PRIVATESHOP_SEARCH_SYSTEM
#define ENABLE_SEND_TARGET_INFO
#define ENABLE_BUTTON_FLASH
#define ENABLE_FAST_ADD_STATUS
#define ENABLE_REFINE_CHECKBOX
#define ENABLE_SHOW_MONEY_ON_OFF
#define __AUTO_QUQUE_ATTACK__

#define WJ_ENABLE_TRADABLE_ICON
#define WJ_MULTI_TEXTLINE
#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
#define ENABLE_SHOW_MOBAIFLAG
#define ENABLE_SHOW_MOBLEVEL
#endif
// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
#define ENABLE_WOLFMAN_CHARACTER
#ifdef ENABLE_WOLFMAN_CHARACTER
// #define DISABLE_WOLFMAN_ON_CREATE
#endif
// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New System Defines - Extended Version ###

// if is define ENABLE_ACCE_COSTUME_SYSTEM the players can use shoulder sash
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_OBJ_SCALLING
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS

// if you want use SetMouseWheelScrollEvent or you want use mouse wheel to move the scrollbar
#define ENABLE_MOUSEWHEEL_EVENT

//if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_HIGHLIGHT_NEW_ITEM

// it shows emojis in the textlines
#define ENABLE_EMOJI_SYSTEM

// effects while hidden won't show up
#define __ENABLE_STEALTH_FIX__

// circle dots in minimap instead of squares
#define ENABLE_MINIMAP_WHITEMARK_CIRCLE

// enable the won system as a currency
#define ENABLE_CHEQUE_SYSTEM
#ifdef ENABLE_CHEQUE_SYSTEM
#define DISABLE_CHEQUE_DROP
#define ENABLE_WON_EXCHANGE_WINDOW
#endif

#define ENABLE_IKASHOP_RENEWAL
#define ENABLE_IKASHOP_ENTITIES
#define ENABLE_LARGE_DYNAMIC_PACKETS

// for debug: print received packets
// #define ENABLE_PRINT_RECV_PACKET_DEBUG

// ### New System Defines - Extended Version ###
//////////////////////////////////////////////////////////////////////////
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
