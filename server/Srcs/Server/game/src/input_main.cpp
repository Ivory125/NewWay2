#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#include "belt_inventory_helper.h" // @fixme119
#include "../../common/CommonDefines.h"
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif
#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
#include "growth_petsystem.h"
#endif

#include "input.h"

#define ENABLE_CHAT_COLOR_SYSTEM
#define ENABLE_CHAT_LOGGING
#define ENABLE_CHAT_SPAMLIMIT
#define ENABLE_WHISPER_CHAT_SPAMLIMIT
#define ENABLE_CHECK_GHOSTMODE

#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
void CInputMain::PrivateShopSearchClose(LPCHARACTER ch, const char* data)
{
	ch->SetPrivateShopSearchState(SHOP_SEARCH_OFF);
}
void CInputMain::PrivateShopSearchBuyItem(LPCHARACTER ch, const char* data)
{
	const TPacketCGPrivateShopSearchBuyItem* p = reinterpret_cast<const TPacketCGPrivateShopSearchBuyItem*>(data);
	CShopManager::Instance().ShopSearchBuy(ch, p);
}
void CInputMain::PrivateShopSearch(LPCHARACTER ch, const char* data)
{
	const TPacketCGPrivateShopSearch* p = reinterpret_cast<const TPacketCGPrivateShopSearch*>(data);
	CShopManager::Instance().ShopSearchProcess(ch, p);
}
#endif

#ifdef ENABLE_CHAT_LOGGING
static char	__escape_string[1024];
static char	__escape_string2[1024];
#endif

static int __deposit_limit()
{
	return (1000*10000);
}

#ifdef __SEND_TARGET_INFO__
void CInputMain::TargetInfoLoad(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGTargetInfoLoad* p = (TPacketCGTargetInfoLoad*)c_pData;
	TPacketGCTargetInfo pInfo;
	pInfo.header = HEADER_GC_TARGET_INFO;
	static std::vector<LPITEM> s_vec_item;
	s_vec_item.clear();
	LPITEM pkInfoItem;
	LPCHARACTER m_pkChrTarget = CHARACTER_MANAGER::instance().Find(p->dwVID);

	// if (m_pkChrTarget && (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()))
	// {
		// if (thecore_heart->pulse - (int) ch->GetLastTargetInfoPulse() < passes_per_sec * 3)
			// return;

		// ch->SetLastTargetInfoPulse(thecore_heart->pulse);

	if (ITEM_MANAGER::instance().CreateDropItemVector(m_pkChrTarget, ch, s_vec_item) && (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()))
	{
		if (s_vec_item.size() == 0);
		else if (s_vec_item.size() == 1)
		{
			pkInfoItem = s_vec_item[0];
			pInfo.dwVID	= m_pkChrTarget->GetVID();
			pInfo.race = m_pkChrTarget->GetRaceNum();
			pInfo.dwVnum = pkInfoItem->GetVnum();
			pInfo.count = pkInfoItem->GetCount();
			ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
		}
		else
		{
			int iItemIdx = s_vec_item.size() - 1;
			while (iItemIdx >= 0)
			{
				pkInfoItem = s_vec_item[iItemIdx--];

				if (!pkInfoItem)
				{
					sys_err("pkInfoItem null in vector idx %d", iItemIdx + 1);
					continue;
				}

					pInfo.dwVID	= m_pkChrTarget->GetVID();
					pInfo.race = m_pkChrTarget->GetRaceNum();
					pInfo.dwVnum = pkInfoItem->GetVnum();
					pInfo.count = pkInfoItem->GetCount();
					ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
			}
		}
	}
	// }
}
#endif

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ä�� ���� �����Դϴ�."));
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;

	long min = (sec / 60);
	sec -= min * 60;

	char buf[128+1];

	if (hour > 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d �ð� %d �� %d �� ���� ä�ñ��� �����Դϴ�"), hour, min, sec);
	else if (hour > 0 && min == 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d �ð� %d �� ���� ä�ñ��� �����Դϴ�"), hour, sec);
	else if (hour == 0 && min > 0)
		snprintf(buf, sizeof(buf), LC_TEXT("%d �� %d �� ���� ä�ñ��� �����Դϴ�"), min, sec);
	else
		snprintf(buf, sizeof(buf), LC_TEXT("%d �� ���� ä�ñ��� �����Դϴ�"), sec);

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef boost::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.emplace(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|')
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

#ifdef ENABLE_NEWSTUFF
	if (g_bDisablePrismNeed)
		return 0;
#endif
	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;

	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

#ifdef ENABLE_WHISPER_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 10)
	{
		ch->GetDesc()->DelayedDisconnect(0);
		return (iExtraLen);
	}
#endif

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ä�� ���� �����Դϴ�."));
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;

			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER)

						{
							if (!pkChr)
							{
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1));
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						char buf[128];
						int len;
						if (3==processReturn)
							len = snprintf(buf, sizeof(buf), LC_TEXT("����Ҽ� �����ϴ�."), pTable->szLocaleName);
						else
							len = snprintf(buf, sizeof(buf), LC_TEXT("%s�� �ʿ��մϴ�."), pTable->szLocaleName);

						if (len < 0 || len >= (int) sizeof(buf))
							len = sizeof(buf) - 1;

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = sizeof(TPacketGCWhisper) + len;
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buf, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
				}

				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));

				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
#ifdef ENABLE_CHAT_LOGGING
				if (ch->IsGM())
				{
					LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, buflen);
					LogManager::instance().EscapeString(__escape_string2, sizeof(__escape_string2), pinfo->szNameTo, sizeof(pack.szNameFrom));
					LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, __escape_string2, "WHISPER", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
				}
#endif
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		m_szChat(chat), m_lenChat(len_chat),
		m_szName(name),
		m_iMapIndex(iMapIndex), m_bEmpire(empire),
		m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1;

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1;

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10);
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}
#ifdef ENABLE_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 4)
	{
		if (ch->GetChatCounter() == 10)
			ch->GetDesc()->DelayedDisconnect(0);
		return iExtraLen;
	}
#else
	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}
#endif

	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
			if (3==processReturn)
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����Ҽ� �����ϴ�."), pTable->szLocaleName);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s�� �ʿ��մϴ�."), pTable->szLocaleName);

		}

		return iExtraLen;
	}
	// @fixme133 end

	char chatbuf[CHAT_MAX_LEN + 1];
#ifdef ENABLE_CHAT_COLOR_SYSTEM
	static const char* colorbuf[] = {"|cFFffa200|H|h[Staff]|h|r", "|cFFff0000|H|h[Shinsoo]|h|r", "|cFFffc700|H|h[Chunjo]|h|r", "|cFF000bff|H|h[Jinno]|h|r"};
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %s : %s", (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), buf);
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
#endif
	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		if (ch->GetLevel() < g_iShoutLimitLevel)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��ġ��� ���� %d �̻� ��� ���� �մϴ�."), g_iShoutLimitLevel);
			return (iExtraLen);
		}

		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf, sizeof(p.szText));

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));

		SendShout(chatbuf, ch->GetEmpire());

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FYmirChatPacket(pack_chat,
								buf,
								strlen(buf),
								ch->GetName(),
								strlen(ch->GetName()),
								ch->GetMapIndex(),
								ch->GetEmpire(),
								ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FEmpireChatPacket(pack_chat,
								chatbuf,
								len,
								(ch->GetGMLevel() > GM_PLAYER ||
								 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
								ch->GetMapIndex(), strlen(ch->GetName())));
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, "", "NORMAL", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��Ƽ ���� �ƴմϴ�."));
				else
				{
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetParty()->GetLeaderPID(), "", "PARTY", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (!ch->GetGuild())
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��忡 �������� �ʾҽ��ϴ�."));
				else
				{
					ch->GetGuild()->Chat(chatbuf);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetGuild()->GetID(), ch->GetGuild()->GetName(), "GUILD", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;
	if (!ch)
		return;

#if defined(ENABLE_CHEQUE_SYSTEM) && !defined(DISABLE_CHEQUE_DROP)
	if (pinfo->cheque > 0)
	{
		ch->DropCheque(pinfo->cheque);
		return;
	}
#endif

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;
	if (!ch)
		return;

#if defined(ENABLE_CHEQUE_SYSTEM) && !defined(DISABLE_CHEQUE_DROP)
	if (pinfo->cheque > 0)
	{
		ch->DropCheque(pinfo->cheque);
		return;
	}
#endif
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

void CInputMain::ItemDestroy(LPCHARACTER ch, const char * data)
{
	struct command_item_destroy * pinfo = (struct command_item_destroy *) data;
	if (ch)
		ch->DestroyItem(pinfo->Cell);
}

#ifdef ENABLE_SELL_ITEM
void CInputMain::ItemSell(LPCHARACTER ch, const char* data)
{
	TPacketCGItemSell* pinfo = (TPacketCGItemSell*)data;

	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->SellItem(pinfo->Cell);
}
#endif

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
		ch->PickupItem(pinfo->vid);
}

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �޽��� �߰� �ź� �����Դϴ�."));
					return sizeof(TPacketCGMessengerAddByVID);
				}

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<�޽���> ��ڴ� �޽����� �߰��� �� �����ϴ�."));
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d)
					return sizeof(TPacketCGMessengerAddByVID);

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
				//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<�޽���> ��ڴ� �޽����� �߰��� �� �����ϴ�."));
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ���� ���ӵ� ���� �ʽ��ϴ�."), name);
				else
				{
					if (tch == ch)
						return CHARACTER_NAME_MAX_LEN;

					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �޽��� �߰� �ź� �����Դϴ�."));
					}
					else
					{
						MessengerManager::instance().RequestToAdd(ch, tch);
						//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
					}
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
				MessengerManager::instance().RemoveFromList(char_name, ch->GetName()); // @fixme183
			}
			return CHARACTER_NAME_MAX_LEN;

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}

		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}

		case SHOP_SUBHEADER_CG_SELL2:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE pos = *(c_pData++);
				BYTE count = *(c_pData);

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());
				CShopManager::instance().Sell(ch, pos, count);
				return sizeof(BYTE) + sizeof(BYTE);
			}

		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ� �� %d�� �̳��� â���� ���� �����ϴ�."), g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ� �� %d�� �̳��� â���� ���� �����ϴ�."), g_nPortalLimitTime);
		return;
	}

	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					//MONARCH_LIMIT

					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("â���� ���� %d�� �̳����� �ŷ��� �Ҽ� �����ϴ�."), g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("â���� ���� %d�� �̳����� �ŷ��� �Ҽ� �����ϴ�."), g_nPortalLimitTime);

						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (ch->GetGold() >= GOLD_MAX)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�׼��� 20�� ���� �ʰ��Ͽ� �ŷ��� �Ҽ��� �����ϴ�.."));

						sys_err("[OVERFLOG_GOLD] START (%u) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
						return;
					}

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}

					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� �ŷ����ϰ�� ���λ����� ������ �����ϴ�."));
						return;
					}

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);

				if (GOLD_MAX <= nTotalGold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ѱݾ��� 20�� ���� �ʰ��Ͽ� �ŷ��� �Ҽ��� �����ϴ�.."));

					sys_err("[OVERFLOW_GOLD] ELK_ADD (%u) id %u name %s ",
							ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						   	ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;

#ifdef ENABLE_CHEQUE_SYSTEM
		case EXCHANGE_SUBHEADER_CG_CHEQUE_ADD:	// arg1 == amount of cheque
			if (ch->GetExchange())
			{
				const int64_t nTotalCheque = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetCheque()) + static_cast<int64_t>(pinfo->arg1);

				if (CHEQUE_MAX <= nTotalCheque)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't trade because the other character exceeds 999 won."));

					sys_err("[OVERFLOW_CHEQUE] CHEQUE_ADD (%u) id %u name %s ",
						ch->GetExchange()->GetCompany()->GetOwner()->GetCheque(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddCheque(pinfo->arg1);
			}
			break;
#endif

		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if(!gHackCheckEnable) return false;

	//	if (IsStun() || IsDead()) return false;

	if (ch->IsStun() || ch->IsDead())
		return false;
	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0;
#if 0
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif

	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 300;

			//sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED),
			//	    ch->IsRiding() ? "yes" : "no");
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval((int) (ani_combo_speed(ch, 1) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}

		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
			ch->SetValidComboInterval(300);
#ifdef ENABLE_WOLFMAN_CHARACTER
		else if (bArg == 21 && idx == 2 && ch->GetComboSequence() == 5 && ch->GetJob() == JOB_WOLFMAN && ch->GetWear(WEAR_WEAPON) && ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_CLAW)
			ch->SetValidComboInterval(300);
#endif
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}

			//if (ch->IsHorseRiding())
			if (ch->IsRiding())
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			else
				ch->SetComboSequence(ch->GetComboSequence() + 1);

			ch->SetValidComboInterval((int) (ani_combo_speed(ch, bArg - 13) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

			//sys_log(0, "COMBO_HACK: 6 %s arg:%u interval:%d valid:%u atkspd:%u",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED));
		}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			// POLYMORPH_BUG_FIX

			// DELETEME

			float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
			int k = (int) (normalAttackDuration / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
			ch->SetValidComboInterval(k);
			ch->SetLastComboTime(dwTime);
			// END_OF_POLYMORPH_BUG_FIX
		}
		else
		{
			//if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
			//{
			//	LogManager::instance().HackLog("Hacker", ch);
			//	sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
			//}

			// by rtsummit
		}
	}
	else
	{
		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{
		if (get_dword_time() - ch->GetLastMountTime() > 1500)
			ch->IncreaseComboHackCount(1 + HackScalar);

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;
}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	{
		// #define ENABLE_TP_SPEED_CHECK
		#ifdef ENABLE_TP_SPEED_CHECK
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		if (((false == ch->IsRiding() && fDist > 25) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex()) // @fixme106 (changed 40 to 60)
		{
			#ifdef ENABLE_HACK_TELEPORT_LOG // @warme006
			{
				const PIXEL_POSITION & warpPos = ch->GetWarpPosition();

				if (warpPos.x == 0 && warpPos.y == 0)
					LogManager::instance().HackLog("Teleport", ch);
			}
			#endif
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif
		#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
		#endif

		DWORD dwCurTime = get_dword_time();

		bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

		if (CheckSpeedHack)
		{
			int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
			int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

			iDelta = (int) (dwCurTime - pinfo->dwTime);

			if (iDelta >= 30000)
			{
				sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}

			else if (iDelta < -(iServerDelta / 50))
			{
				sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
		}

		if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
		{
			CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack);
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);

	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Attack :name[%s] Vnum[%d] can't use skill by attack(warning)"), type->type);
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 16;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
				continue;
		}

		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;

		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}
		else if( fDist > 25.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

			ch->GetDesc()->SetPhase(PHASE_CLOSE);

			return -1;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250)
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}

// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
	    ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â���� �ű� �� ���� ������ �Դϴ�."));
	    return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �ű� �� ���� ��ġ�Դϴ�."));
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �� �������� ���� �� �����ϴ�."));
		return;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pkItem->IsEquipped())
	{
		int iWearCell = pkItem->FindEquipCell(ch);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !ch->UnequipItem(costumeWeapon))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return;
			}
		}
	}
#endif

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��Ʈ �κ��丮�� �������� �����ϸ� ������ �� �����ϴ�."));
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);
	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �ű� �� ���� ��ġ�Դϴ�."));
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �ű� �� ���� ��ġ�Դϴ�."));
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �ű� �� ���� ��ġ�Դϴ�."));
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�� �������� ��Ʈ �κ��丮�� �ű� �� �����ϴ�."));
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);

	if (!pInviter)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ��û�� �� ĳ���͸� ã���� �����ϴ�."));
	// @fixme301 BEGIN
	else if (!pInviter->GetParty() && pInviter->GetMapIndex() >= 10000)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party leader is inside a dungeon."));
	else if (pInviter->GetParty() && pInviter->GetParty()->IsPartyInAnyDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party can't invite people if members are inside a dungeon."));
		pInviter->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Party> The party can't invite people if members are inside a dungeon."));
	}
	// @fixme301 END
	else if (!p->accept)
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	else
		pInviter->PartyInviteAccept(ch);
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ������ ������ �� �ֽ��ϴ�."));
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���¸� �����Ϸ��� ����� ��Ƽ���� �ƴմϴ�."));
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}

			break;

		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����忡�� ����Ͻ� �� �����ϴ�."));
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ���� �߹��� �� �����ϴ�."));
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ������������ ��Ƽ���� �߹��� �� �����ϴ�."));
		else
		{
			if (pParty->IsPartyInAnyDungeon()) // @fixme301
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ>���� �ȿ� ��Ƽ���� �־� ��Ƽ�� �ػ� �� �� �����ϴ�."));
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ���� �߹���ϼ̽��ϴ�."));
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		// otherwise, only remove itself
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ������������ ��Ƽ�� ���� �� �����ϴ�."));
			}
			else
			{
				if (pParty->GetMemberCount() == 2)
				{
					// party disband
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ���� �����̽��ϴ�."));
					//pParty->SendPartyRemoveOneToAll(ch);
					pParty->Quit(ch->GetPlayerID());
					//pParty->SendPartyRemoveAllToOne(ch);
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �ٸ� ��Ƽ���� Ż���ų �� �����ϴ�."));
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000)
		return;

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") <
			CGuildManager::instance().GetDisbandDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �ػ��� �� %d�� �̳����� ��带 ���� �� �����ϴ�."),
				quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") <
			CGuildManager::instance().GetWithdrawDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> Ż���� �� %d�� �̳����� ��带 ���� �� �����ϴ�."),
				quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	}

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�������� ���� ��� �̸� �Դϴ�."));
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> [%s] ��尡 �����Ǿ����ϴ�."), cp.name);

		int GuildCreateFee = 200000;

		ch->PointChange(POINT_GOLD, -GuildCreateFee);
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��� ������ �����Ͽ����ϴ�."));
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ ����� ��Ƽ�常 ����� �� �ֽ��ϴ�."));
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch)
					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��ȯ�Ϸ��� ����� ã�� �� �����ϴ�."));
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��忡 �������� �ʽ��ϴ�."));
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �߸��� �ݾ��Դϴ�."));
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ �ִ� ���� �����մϴ�."));
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

				if (gold < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �߸��� �ݾ��Դϴ�."));
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �׷��� ����� ã�� �� �����ϴ�."));
					return SubPacketLen;
				}

				// @fixme145 BEGIN (+newmember ispc check)
				if (!ch->IsPC() || !newmember->IsPC())
					return SubPacketLen;
				// @fixme145 END

				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ����� �߿��� ������ Ż���ų �� �����ϴ�."));
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ ���� ��尡 �ƴմϴ�."));
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ ���� Ż�� ��ų ������ �����ϴ�."));
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ ���� Ż�� ��ų ������ �����ϴ�."));
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid))
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ ���� Ż�� ���׽��ϴ�."));
					else
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �׷��� ����� ã�� �� �����ϴ�."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� �̸��� ������ ������ �����ϴ�."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� ���� �̸��� ������ �� �����ϴ�."));
				}
				else if (!check_name(gradename))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �������� ���� ���� �̸� �Դϴ�."));
				}
				else
				{
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� ������ ������ ������ �����ϴ�."));
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� ������ ������ �� �����ϴ�."));
				}
				else
				{
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��尡 �̹� �ְ� �����Դϴ�."));
				}
				else
				{
					offer /= 100;
					offer *= 100;

					if (pGuild->OfferExp(ch, offer))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> %u�� ����ġ�� �����Ͽ����ϴ�."), offer);
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ����ġ ���ڿ� �����Ͽ����ϴ�."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���� �����մϴ�."));
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��ŷ� ȸ���� �����Ͽ����ϴ�."));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �������� �ۼ��� ������ �����ϴ�."));
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������ ������ ������ �����ϴ�."));
				else if (ch->GetPlayerID() == pid)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������� ������ ������ �� �����ϴ�."));
				else if (grade == 1)
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ��������� ������ ������ �� �����ϴ�."));
				else
					pGuild->ChangeMemberGrade(pid, grade);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> �屺�� ������ ������ �����ϴ�."));
				}
				else
				{
					if (!pGuild->ChangeMemberGeneral(pid, is_general))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ���̻� ����� ������ �� �����ϴ�."));
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
		ch->GiveItem(to_ch, p->ItemPos);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�������� �ǳ��� �� �����ϴ�."));
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);

	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ���� 20����� �Ѿ� �ŷ��� �ۼ��� �����ϴ�."));
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� �ŷ����ϰ�� ���λ����� ������ �����ϴ�."));
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

#ifdef ENABLE_SHOW_CHEST_DROP
void CInputMain::ChestDropInfo(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGChestDropInfo* p = (TPacketCGChestDropInfo*) c_pData;

	if(p->wInventoryCell >= INVENTORY_MAX_NUM)
		return;
	
	LPITEM pkItem = ch->GetInventoryItem(p->wInventoryCell);

	if (!pkItem)
		return;
	
	std::vector<TChestDropInfoTable> vec_ItemList;
	ITEM_MANAGER::instance().GetChestItemList(pkItem->GetVnum(), vec_ItemList);

	TPacketGCChestDropInfo packet;
	packet.bHeader = HEADER_GC_CHEST_DROP_INFO;
	packet.wSize = sizeof(packet) + sizeof(TChestDropInfoTable) * vec_ItemList.size();
	packet.dwChestVnum = pkItem->GetVnum();

	ch->GetDesc()->BufferedPacket(&packet, sizeof(packet));
	ch->GetDesc()->Packet(&vec_ItemList[0], sizeof(TChestDropInfoTable) * vec_ItemList.size());
}
#endif

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO,  LC_TEXT("â��,�ŷ�â���� ���� ���¿����� ������ �Ҽ��� �����ϴ�"));
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
		ch->ClearRefineMode();
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (500 <= item->GetRefineSet())
			{
				LogManager::instance().HackLog("DEVIL_TOWER_REFINE_HACK", ch);
			}
			else
			{
				if (ch->GetQuestFlag("deviltower_zone.can_refine") > 0) // @fixme158 (allow multiple refine attempts)
				{
					if (ch->DoRefine(item, true))
						ch->SetQuestFlag("deviltower_zone.can_refine", ch->GetQuestFlag("deviltower_zone.can_refine") - 1);
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "��� Ÿ�� �Ϸ� ������ �ѹ����� ��밡���մϴ�.");
				}
			}
		}
	}

	ch->ClearRefineMode();
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketAcce * sPacket = (TPacketAcce*) c_pData;
	switch (sPacket->subheader)
	{
	case ACCE_SUBHEADER_CG_CLOSE:
	{
		pkChar->CloseAcce();
	}
	break;
	case ACCE_SUBHEADER_CG_ADD:
	{
		pkChar->AddAcceMaterial(sPacket->tPos, sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REMOVE:
	{
		pkChar->RemoveAcceMaterial(sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REFINE:
	{
		pkChar->RefineAcceMaterials();
	}
	break;
	default:
		break;
	}
}
#endif
#ifdef ENABLE_IKASHOP_RENEWAL
#include "ikarus_shop.h"
#include "ikarus_shop_manager.h"

struct DecodeException : public std::exception {
	const char* what() const noexcept override{
		return "Decode failed receiving ikashop packet";
	}
};

template <class T>
const T& Decode(const char*& data, int& left, int& len)
{
	if(left < sizeof(T))
		throw DecodeException();

	auto obj = reinterpret_cast<const T*>(data);
	data += sizeof(T);
	len  += sizeof(T);
	left -= sizeof(T);
	return *obj;
}

int IkarusShopPacketCreateNewShop(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	ikashop::GetManager().RecvShopCreateNewClientPacket(ch);
	return extra;
}

int IkarusShopPacketForceCloseShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopForceCloseClientPacket(ch);
	return 0;
}

int IkarusShopPacketOpenShop(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopOpen>(data, left, extra);
	ikashop::GetManager().RecvShopOpenClientPacket(ch, pack.ownerid);
	return extra;
}

int IkarusShopPacketOpenShowOwner(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopOpenMyShopClientPacket(ch);
	return 0;
}

int IkarusShopPacketBuyItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopBuyItem>(data, left, extra);
	ikashop::GetManager().RecvShopBuyItemClientPacket(ch, pack.ownerid, pack.itemid, pack.searching, pack.seenprice);
	return extra;
}

int IkarusShopPacketAddItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAddItem>(data, left, extra);
	ikashop::GetManager().RecvShopAddItemClientPacket(ch, pack.pos, pack.price, pack.destpos);
	return extra;
}

int IkarusShopPacketRemoveItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGRemoveItem>(data, left, extra);
	ikashop::GetManager().RecvShopRemoveItemClientPacket(ch, pack.itemid);
	return extra;
}

int IkarusShopPacketEditItem(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGEditItem>(data, left, extra);
	ikashop::GetManager().RecvShopEditItemClientPacket(ch, pack.itemid, pack.price);
	return extra;
}

int IkarusShopPacketFilterRequest(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGFilterRequest>(data, left, extra);
	ikashop::GetManager().RecvShopFilterRequestClientPacket(ch, pack.filter);
	return extra;
}

int IkarusShopPacketSearchFillRequest(LPCHARACTER ch, const char* data, int left)
{
	ikashop::GetManager().RecvShopSearchFillRequestClientPacket(ch);
	return 0;
}

int IkarusShopPacketCreateOffer(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGOfferCreate>(data, left, extra);
	ikashop::GetManager().RecvShopCreateOfferClientPacket(ch, pack.offer);
	return extra;
}

int IkarusShopPacketAcceptOffer(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGOfferAccept>(data, left, extra);
	ikashop::GetManager().RecvShopAcceptOfferClientPacket(ch, pack.offerid);
	return extra;
}

int IkarusShopPacketOfferCancel(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGOfferCancel>(data, left, extra);
	ikashop::GetManager().RecvShopCancelOfferClientPacket(ch, pack.offerid, pack.ownerid);
	return extra;
}

int IkarusShopPacketOfferListRequest(LPCHARACTER ch)
{
	ikashop::GetManager().RecvOfferListRequestPacket(ch);
	return 0;
}

int IkarusShopPacketOpenSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvShopSafeboxOpenClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseMyShopBoard(LPCHARACTER ch)
{
	ikashop::GetManager().RecvCloseMyShopBoardClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseOfferList(LPCHARACTER ch)
{
	ikashop::GetManager().RecvCloseOfferListClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseShopGuest(LPCHARACTER ch)
{
	ikashop::GetManager().RecvCloseShopGuestClientPacket(ch);
	return 0;
}

int IkarusShopPacketGetItemSafebox(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopSafeboxGetItem>(data, left, extra);
	ikashop::GetManager().RecvShopSafeboxGetItemClientPacket(ch, pack.itemid);
	return extra;

}

int IkarusShopPacketGetValutesSafebox(LPCHARACTER ch, const char* data, int left)
{
	ikashop::GetManager().RecvShopSafeboxGetValutesClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::CShopManager& rManager = ikashop::GetManager();
	rManager.RecvShopSafeboxCloseClientPacket(ch);
	return 0;
}

int IkarusShopPacketListRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::CShopManager& rManager = ikashop::GetManager();
	rManager.RecvAuctionListRequestClientPacket(ch);
	return 0;
}

int IkarusShopPacketOpenAuctionRequest(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAuctionOpenRequest>(data, left, extra);
	ikashop::GetManager().RecvAuctionOpenRequestClientPacket(ch, pack.ownerid);
	return extra;
}

int IkarusShopPacketOpenMyAuctionRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvMyAuctionOpenRequestClientPacket(ch);
	return 0;
}

int IkarusShopPacketCloseMyAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvMyAuctionCloseClientPacket(ch);
	return 0;
}

int IkarusShopPacketCancelMyAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	ikashop::GetManager().RecvMyAuctionCancelClientPacket(ch);
	return 0;
}

int IkarusShopPacketCreateAuction(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAuctionCreate>(data, left, extra);
	ikashop::GetManager().RecvAuctionCreateClientPacket(ch, pack.startprice, pack.pos);
	return extra;
}

int IkarusShopPacketAddOffer(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAuctionAddOffer>(data, left, extra);
	ikashop::GetManager().RecvAuctionAddOfferClientPacket(ch, pack.ownerid, pack.price);
	return extra;
}

int IkarusShopPacketExitFromAuction(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGAuctionExitFrom>(data, left, extra);
	ikashop::GetManager().RecvAuctionExitFromAuction(ch, pack.ownerid);
	return extra;
}

#ifdef ENABLE_IKASHOP_ENTITIES
int IkarusShopPacketClickEntity(LPCHARACTER ch, const char* data, int left)
{
	int extra = 0;
	const auto& pack = Decode<ikashop::TSubPacketCGShopClickEntity>(data, left, extra);

	ikashop::GetManager().RecvShopClickEntityClientPacket(ch, pack.vid);
	return extra;
}

#endif



int IkarusShopPacket(const char* data , LPCHARACTER ch, int iBufferLeft)
{
	BYTE subheader = 255;

	try
	{
		int extra = 0;
		const auto& pack = Decode<TPacketCGNewIkarusShop>(data, iBufferLeft, extra);
		subheader = pack.subheader;

		switch (pack.subheader)
		{

		case ikashop::SUBHEADER_CG_SHOP_CREATE_NEW:
			return IkarusShopPacketCreateNewShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_FORCE_CLOSE:
			return IkarusShopPacketForceCloseShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OPEN:
			return IkarusShopPacketOpenShop(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OPEN_OWNER:
			return IkarusShopPacketOpenShowOwner(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_BUY_ITEM:
			return IkarusShopPacketBuyItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_ADD_ITEM:
			return IkarusShopPacketAddItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_REMOVE_ITEM:
			return IkarusShopPacketRemoveItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_EDIT_ITEM:
			return IkarusShopPacketEditItem(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_FILTER_REQUEST:
			return IkarusShopPacketFilterRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SEARCH_FILL_REQUEST:
			return IkarusShopPacketSearchFillRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OFFER_CREATE:
			return IkarusShopPacketCreateOffer(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_OFFER_ACCEPT:
			return IkarusShopPacketAcceptOffer(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_REQUEST_OFFER_LIST:
			return IkarusShopPacketOfferListRequest(ch);

		case ikashop::SUBHEADER_CG_SHOP_OFFER_CANCEL:
			return IkarusShopPacketOfferCancel(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN:
			return IkarusShopPacketOpenSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM:
			return IkarusShopPacketGetItemSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES:
			return IkarusShopPacketGetValutesSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE:
			return IkarusShopPacketCloseSafebox(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_AUCTION_LIST_REQUEST:
			return IkarusShopPacketListRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_AUCTION_OPEN_REQUEST:
			return IkarusShopPacketOpenAuctionRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_MY_AUCTION_OPEN_REQUEST:
			return IkarusShopPacketOpenMyAuctionRequest(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_MY_AUCTION_CLOSE:
			return IkarusShopPacketCloseMyAuction(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_MY_AUCTION_CANCEL:
			return IkarusShopPacketCancelMyAuction(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_CREATE_AUCTION:
			return IkarusShopPacketCreateAuction(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_AUCTION_ADD_OFFER:
			return IkarusShopPacketAddOffer(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_EXIT_FROM_AUCTION:
			return IkarusShopPacketExitFromAuction(ch, data, iBufferLeft);

		case ikashop::SUBHEADER_CG_CLOSE_MY_SHOP_BOARD:
			return IkarusShopPacketCloseMyShopBoard(ch);

		case ikashop::SUBHEADER_CG_CLOSE_OFFER_LIST:
			return IkarusShopPacketCloseOfferList(ch);

		case ikashop::SUBHEADER_CG_CLOSE_SHOP_GUEST:
			return IkarusShopPacketCloseShopGuest(ch);

#ifdef ENABLE_IKASHOP_ENTITIES
		case ikashop::SUBHEADER_CG_CLICK_ENTITY:
			return IkarusShopPacketClickEntity(ch, data, iBufferLeft);
#endif
		default:
			sys_err("UNKNOWN SUBHEADER %d ", subheader);
			return -1;
		}
	}

	catch(const DecodeException& e)
	{
		sys_err("FAILED TO DECODE SUBHEADER %u", subheader);
		return -1;
	}
}
#endif

#ifdef ELEMENT_SPELL_WORLDARD
void CInputMain::ElementsSpell(LPCHARACTER ch, const char* data)
{
	struct SPacketCGElementsSpell * pinfo = (struct SPacketCGElementsSpell *) data;

	if (!ch->IsOpenElementsSpell())
	{
		return;
	}

	switch (pinfo->subheader)
	{
		case ELEMENTS_SPELL_SUB_HEADER_CLOSE:
		{
			ch->SetOpenElementsSpell(false);

		}
		break;

		case ELEMENTS_SPELL_SUB_HEADER_ADD_BONUS:
		{


			if (pinfo->pos < 0)
			{
				ch->SetOpenElementsSpell(false);
				return;
			}

			ch->ElementsSpellItemFunc(pinfo->pos);
			ch->SetOpenElementsSpell(false);
			
		}
		break;

		case ELEMENTS_SPELL_SUB_HEADER_CHANGE_BONUS:
		{

			if (pinfo->pos < 0)
			{
				ch->SetOpenElementsSpell(false);
				return;
			}

			BYTE type_select = pinfo->type_select;

			if (type_select < 99 || type_select > 104)
			{
				ch->SetOpenElementsSpell(false);
				return;
			}

			ch->ElementsSpellItemFunc(pinfo->pos,type_select);
			ch->SetOpenElementsSpell(false);
			
		}
		break;
	}
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
int CInputMain::ReciveBuffNPCCreate(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGBuffNPCCreate* p = (TPacketCGBuffNPCCreate*)c_pData;
	
	switch (p->bAction)
	{
		case 1:
			{
				if (!ch->GetDesc()) { return 0; }
				// if (item_vid == 0) { return 0; }
				
				if(!ch->GetBuffNPCSystem()->IsActive()) {
					LPITEM item = ch->GetInventoryItem(ch->GetBuffSealVID());
					
					if (item->GetVnum() != ASLAN_BUFF_NPC_SEAL_VNUM) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						break;
					}
					if (item->GetSocket(0) != 0) {
						ch->ChatPacket(CHAT_TYPE_INFO, "Seal Socket is zero");
						break;
					}

					char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
					DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szName, strlen(p->szName));
					
					if (check_name(szEscapeName) == 0) {
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_USE_THIS_NAME"));
						break;
					}
					if (item->GetCount() > 0) {
						item->RemoveFromCharacter();
						std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("INSERT INTO player.player_buff_npc SET player_id=%d, player_name='%s', name='%s', sex=%d, intvalue=%d", ch->GetPlayerID(), ch->GetName(), szEscapeName, p->bSex, ASLAN_BUFF_NPC_START_INT));
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_SUCCESSFULLY_CREATED"), szEscapeName);
						ch->SetQuestFlag("buff_npc.is_summon", 1);
						ch->GetBuffNPCSystem()->LoadBuffNPC();
						ch->GetBuffNPCSystem()->Summon();
					}
					else {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						// return 0;
					}
				}
				else {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_HAVE_A_ACTIVE_SHAMAN"));
					return 0;
				}
			}
			break;

		case 2:
			{
				if (!ch->GetDesc()) { return 0; }
				// if (item_vid == 0) { return 0; }
				
				if(ch->GetBuffNPCSystem()->IsActive()) {
					LPITEM item_rename = ch->GetBuffEquipmentItem(BUFF_SLOT_CHANGE_NAME);
					
					if (item_rename == NULL) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - No renew name item avaible!");
						return 0;
					}
					if (item_rename->GetVnum() != 71055) {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						break;
					}

					char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
					DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->szName, strlen(p->szName));
					
					if (check_name(szEscapeName) == 0) {
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_USE_THIS_NAME"));
						break;
					}
					if (item_rename->GetCount() > 0) {
						item_rename->SetCount(item_rename->GetCount() - 1);
						// DBManager::instance().DirectQuery("UPDATE player.player_buff_npc SET name='%s' WHERE player_id=%d ", szEscapeName, ch->GetPlayerID());
						ch->GetBuffNPCSystem()->SetName(szEscapeName);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_SUCCESSFULLY_NAME_CHANGEND"), szEscapeName);
						if (ch->GetBuffNPCSystem()->IsSummoned())
							ch->GetBuffNPCSystem()->Unsummon();
						ch->GetBuffNPCSystem()->Summon();
					}
					else {
						ch->ChatPacket(CHAT_TYPE_INFO, "ERROR - This is not the correct item");
						// return 0;
					}
				}
				else {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
					return 0;
				}
			}
			break;
			
		case 3:
			break;

		default:
			break;
	}

	return 0;
}


int CInputMain::ReciveBuffNPCAction(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGBuffNPCAction* p = (TPacketCGBuffNPCAction*)c_pData;

	if (ch->GetBuffNPCSystem() == NULL)
		return 0;

	switch (p->bAction)
	{
		case 1:
			{
				if (!ch->GetBuffNPCSystem()->IsActive()) {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
					return 0;
				}
				if (ch->GetBuffNPCSystem()->IsSummoned()) {
					ch->SetQuestFlag("buff_npc.is_summon", 0);
					ch->GetBuffNPCSystem()->Unsummon();
				}
				else {
#ifdef ASLAN_BUFF_NPC_USE_SUMMON_LEVEL_CAP
				if (ch->GetBuffNPCSystem()->GetLevel() > ch->GetLevel()) {
					if ((ch->GetBuffNPCSystem()->GetLevel() - ch->GetLevel()) > ASLAN_BUFF_NPC_MAX_LV_CAP) {
						if(ASLAN_BUFF_NPC_MAX_LV_CAP == 0)
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_SUMMON_BECAUSE_OVER_YOUR_LEVEL"));
						else
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_SUMMON_BECAUSE_OVER_LEVELCAP"), ASLAN_BUFF_NPC_MAX_LV_CAP);
						return 0;
					}
				}
#endif
					ch->SetQuestFlag("buff_npc.is_summon", 1);
					ch->GetBuffNPCSystem()->Summon();
				}
			}
			break;

		case 2:
			ch->GetBuffNPCSystem()->LearnSkillByPoint(p->dValue0);
			break;
			
		case 3:
			ch->GetBuffNPCSystem()->LearnSkillByBook(p->dValue0);
			break;
			
		case 4:
			ch->GetBuffNPCSystem()->LearnSkillBySoulstone(p->dValue0);
			break;
			
		case 5:
			ch->GetBuffNPCSystem()->SetSkillUseStatus(p->dValue0, p->dValue1);
			break;
			
		case 6:
			ch->GetBuffNPCSystem()->SetChangeSex();
			break;
			
		case 7:
			ch->GetBuffNPCSystem()->SetClearSkill(p->dValue0);
			break;
			
		case 8:
			{
#ifdef ASLAN_BUFF_NPC_ENABLE_EXPORT
				if (!ch->GetBuffNPCSystem()->IsActive()) {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
					return 0;
				}
#ifdef ASLAN_BUFF_NPC_ENABLE_EXPORT_COST
				int iExportCost[10][3] = { 
				// level_min | level_max | cost
					{  0, 10, 50000 },			//Lv.	 0-10
					{ 11, 20, 250000 },			//Lv. 11-20
					{ 21, 30, 1000000 },		//Lv. 21-30
					{ 31, 40, 2500000 },		//Lv. 31-40
					{ 41, 50, 7500000 },		//Lv. 41-50
					{ 51, 60, 15000000 },		//Lv. 51-60
					{ 61, 70, 30000000 },		//Lv. 61-70
					{ 71, 80, 50000000 },		//Lv. 71-80
					{ 81, 90, 100000000 },		//Lv. 81-90
					{ 91, 99, 250000000 },		//Lv. 91-99
				};
				
				int iCost;
				
				DWORD TableSize = (sizeof iExportCost/sizeof iExportCost[0]);
				for (int i = 0; i < TableSize; ++i) {
					if (ch->GetBuffNPCSystem()->GetLevel() >= iExportCost[i][0] && ch->GetBuffNPCSystem()->GetLevel() <= iExportCost[i][1]) {
						iCost = iExportCost[i][2];
						break;
					}
				}
				
				if (ch->GetGold() < iCost) {
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_NOT_ENOUGHT_MONEY"));
					return 0;
				}
#endif
				if (!ch->IsBuffEquipEmpty()){
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_REMOVE_EQUIPMENT_FROM_BUFF"));
					return 0;
				}
				
				LPITEM SealItem = ITEM_MANAGER::instance().CreateItem(ASLAN_BUFF_NPC_SEAL_VNUM);
				
				if (SealItem)
				{	
#ifdef ENABLE_SPECIAL_INVENTORY
					int iEmptyPos = ch->GetEmptyInventory(SealItem);
#else
					int iEmptyPos = ch->GetEmptyInventory(SealItem->GetSize());
#endif
					if (iEmptyPos != -1)
					{
						if (ch->GetBuffNPCSystem()->IsSummoned()) {
							ch->SetQuestFlag("buff_npc.is_summon", 0);
							ch->GetBuffNPCSystem()->Unsummon();
						}
		
						std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT id, sex, level, exp, skillpoint, intvalue, skill0lv, skill1lv, skill2lv, skill3lv, skill4lv, skill5lv FROM player_buff_npc WHERE player_id = %lu", ch->GetPlayerID()));
						if (pMsg->Get()->uiNumRows != 0)
						{
							MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
							
							float exp = atoi(row[3]);
							float exp_next = ch->GetBuffNPCSystem()->GetNextExp();
							int exp_perc;
							
							if (exp > 0)
								exp_perc = (int(exp / exp_next * 10000.f));
							else
								exp_perc = 0;
								
							SealItem->SetSocket(0, atoi(row[0]));	// ID
							SealItem->SetSocket(1, atoi(row[1]));	// Sex
							SealItem->SetSocket(2, atoi(row[2]));	// Level
							SealItem->SetSocket(3, exp_perc);		// EXP
							SealItem->SetSocket(4, atoi(row[4]));	// Skillpoints
							SealItem->SetSocket(5, atoi(row[5]));	// IntValue
							
							SealItem->SetForceAttribute(0, atoi(row[6]), atoi(row[7]));
							SealItem->SetForceAttribute(1, atoi(row[8]), atoi(row[9]));
							SealItem->SetForceAttribute(2, atoi(row[10]), atoi(row[11]));
							
							SealItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos)); 

							int iSkillVnum[6] = { 94, 95, 96, 109, 110, 111 };
							for (int i = 0; i < 6; ++i) {
								char flag_next_read[128+1];
								memset(flag_next_read, 0, sizeof(flag_next_read));
								snprintf(flag_next_read, sizeof(flag_next_read), "buff_npc_learn.%u.next_read", iSkillVnum[i]);
								char flag_read_count[128+1];
								memset(flag_read_count, 0, sizeof(flag_read_count));
								snprintf(flag_read_count, sizeof(flag_read_count), "buff_npc_learn.%u.read_count", iSkillVnum[i]);
								ch->SetQuestFlag(flag_next_read, 0);
								ch->SetQuestFlag(flag_read_count, 0);
							}
#ifdef ASLAN_BUFF_NPC_ENABLE_EXPORT_COST
							ch->PointChange(POINT_GOLD, -iCost, true);
#endif
							DBManager::instance().DirectQuery("UPDATE player.player_buff_npc SET player_name='----', player_id=0 WHERE player_id=%d ", ch->GetPlayerID());
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_EXPORT_IN_SEAL"));
							ch->GetBuffNPCSystem()->ClearBuffNPC();
						}
					}
					else
					{
						M2_DESTROY_ITEM(SealItem);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_NOT_ENOUGHT_INVENTORY_SPACE"));
					}
				}
			}
#endif
			break;

		default:
			break;
	}

	return 0;
}
#endif

#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
void CInputMain::ReciveRequestBornPet(LPCHARACTER ch, const char* c_pData)
{
	if (!ch->GetDesc()) { return; }
	int itemvnum = ch->GetEggVid();
	if (itemvnum == 0) { return; }

	TPacketCGBornPet* p = (TPacketCGBornPet*)c_pData;

	char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
	DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->petname, strlen(p->petname));

	if (check_name(szEscapeName) == 0) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_CANNOT_USE_THIS_NAME"));
		return;
	}

	if (ch->GetGold() < GROWTH_PET_BORN_COST)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_COST_FOR_INCUBATION"), GROWTH_PET_BORN_COST);

	if (ch->CountSpecifyItem(itemvnum) > 0) {
		int randpet = number(0, 4);

		LPITEM PetItem = ITEM_MANAGER::instance().CreateItem(itemvnum + 300);

		if (PetItem)
		{
			int iEmptyPos = ch->GetEmptyInventory(PetItem->GetSize());

			if (iEmptyPos != -1)
			{
				ch->PointChange(POINT_GOLD, -GROWTH_PET_BORN_COST, true);
				ch->RemoveSpecifyItem(itemvnum, 1);

				int tmpslot = number(1, 3);
				int tmpskill[3] = { -1, -1, -1 };

				DWORD bornduration = 0;
				DWORD rand_hp = 0;
				DWORD rand_atk = 0;
				DWORD rand_def = 0;

				for (int i = 0; i < PET_MAX_NUM; ++i) {
					if (itemvnum == (PetListTable[i].dwEggVnum)) {
						bornduration = number(PetListTable[i].dwBornLifetime[0], PetListTable[i].dwBornLifetime[1]) * 60 * 60 * 24;
						rand_hp = number(PetListTable[i].dwBornAttr[0][0], PetListTable[i].dwBornAttr[0][1]);
						rand_atk = number(PetListTable[i].dwBornAttr[1][0], PetListTable[i].dwBornAttr[1][1]);
						rand_def = number(PetListTable[i].dwBornAttr[2][0], PetListTable[i].dwBornAttr[2][1]);
					}
				}

				DWORD pettype = 1;
				DWORD rand_typebonus0 = 0;
				DWORD rand_typebonus1 = 0;
				DWORD rand_typebonus2 = 0;

				//	Type Chance:				2	3	4	5	6
				const int prob_type_table[] = { 30, 50, 70,	80, 90 };
				int rand = number(1, 100);

				for (int i = 0; i < 6; ++i) {
					if (rand > prob_type_table[i]) {
						pettype += 1;
					}
				}

				if (pettype == 2 || pettype == 4 || pettype == 6 || pettype == 8) {
					rand_typebonus0 = number(PetTypeAttrTable[pettype].dwATK[0], PetTypeAttrTable[pettype].dwATK[1]);
					rand_typebonus1 = number(PetTypeAttrTable[pettype].dwHP[0], PetTypeAttrTable[pettype].dwHP[1]);
					rand_typebonus2 = number(PetTypeAttrTable[pettype].dwDEF[0], PetTypeAttrTable[pettype].dwDEF[1]);
				}

				int realtime_tmpdur = time(0) + bornduration;
				PetItem->SetSocket(1, realtime_tmpdur);
				PetItem->SetSocket(2, time(0)); 		// Borntime (UNIX)
				PetItem->SetSocket(3, bornduration);	// Bornduration (UNIX)

				PetItem->SetForceAttribute(0, 1, 0);			// Pet Level , Pet Evolution
				PetItem->SetForceAttribute(1, rand_hp, rand_atk);
				PetItem->SetForceAttribute(2, rand_def, 0);
				PetItem->SetForceAttribute(3, 0, -1);
				PetItem->SetForceAttribute(4, 0, -1);
				PetItem->SetForceAttribute(5, 0, -1);

				PetItem->StartCheckPetDeadEvent();

				PetItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));

				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_SUCCESS_CREATE_NEW_PET"));

				DBManager::instance().DirectQuery("INSERT INTO player_petsystem VALUES(%lu,'%s', 1, 0, 0, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, NOW(), 0, 0, %d, %d, %d, %d)",
					PetItem->GetID(), szEscapeName, rand_hp, rand_atk, rand_def, tmpskill[0], 0, tmpskill[1], 0, tmpskill[2], 0, realtime_tmpdur, bornduration, pettype, rand_typebonus0, rand_typebonus1, rand_typebonus2);
			}
			else
			{
				M2_DESTROY_ITEM(PetItem);
				ch->ChatPacket(CHAT_TYPE_INFO, "Not enough inventory space.");
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[PET] #%u item not exist by that vnum.", itemvnum);
		}
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "[PET] Failure create new pet.");
}

void CInputMain::ReciveRenewPetName(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGRenewPetName* p = (TPacketCGRenewPetName*)c_pData;

	char szEscapeName[CHARACTER_NAME_MAX_LEN + 1];
	DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), p->petname, strlen(p->petname));
	if (check_name(szEscapeName) == 0) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_USE_THIS_NAME"));
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->itempos1);
	LPITEM item2 = ch->GetInventoryItem(p->itempos2);

	if (item != NULL && item2 != NULL && check_name(p->petname) != 0)
	{
		if (ch->GetGrowthPetSystem()->IsSummoned() && item->GetID() == ch->GetGrowthPetSystem()->GetSummonItemID())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_UNSUMMON_FIRST"));
			return;
		}
		else
		{
			item2->SetCount(item2->GetCount() - 1);
			DBManager::instance().DirectQuery ( "UPDATE player_petsystem SET name = '%s' WHERE id = %lu ", szEscapeName, item->GetID());
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_GROWTH_PET_RENEW_PET_NAME"), szEscapeName);
		}
	}
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			// @fixme103 (removed CheckClientVersion since useless in here)
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
				ItemDestroy(ch, c_pData);
		break;

#ifdef ENABLE_SELL_ITEM
		case HEADER_CG_ITEM_SELL:
			if (!ch->IsObserverMode())
				ItemSell(ch, c_pData);
			break;
#endif

		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;
	#if defined(BL_PRIVATESHOP_SEARCH_SYSTEM)
		case HEADER_CG_PRIVATE_SHOP_SEARCH:
			PrivateShopSearch(ch, c_pData);
			break;
		case HEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE:
			PrivateShopSearchClose(ch, c_pData);
			break;
		case HEADER_CG_PRIVATE_SHOP_SEARCH_BUY_ITEM:
			PrivateShopSearchBuyItem(ch, c_pData);
			break;
	#endif			
			

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;

		case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
			ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
			AnswerMakeGuild(ch, c_pData);
#endif
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case HEADER_CG_ACCE:
			Acce(ch, c_pData);
			break;
#endif

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

#ifdef ENABLE_SHOW_CHEST_DROP
		case HEADER_CG_CHEST_DROP_INFO:
			ChestDropInfo(ch, c_pData);
			break;
#endif

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;

#ifdef __SEND_TARGET_INFO__
		case HEADER_CG_TARGET_INFO_LOAD:
			{
				TargetInfoLoad(ch, c_pData);
			}
			break;
#endif

#ifdef ELEMENT_SPELL_WORLDARD
		case HEADER_CG_ELEMENTS_SPELL:
			ElementsSpell(ch, c_pData);
			break;
#endif

		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}

			break;
#ifdef ENABLE_IKASHOP_RENEWAL
		case HEADER_CG_NEW_OFFLINESHOP:
			if((iExtraLen = IkarusShopPacket(c_pData, ch, m_iBufferLeft))< 0)
				return -1;
			break;
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case HEADER_CG_BUFF_NPC_CREATE:
			ReciveBuffNPCCreate(ch, c_pData);
			break;
			
		case HEADER_CG_BUFF_NPC_ACTION:
			ReciveBuffNPCAction(ch, c_pData);
			break;
#endif
#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM
		case HEADER_CG_PET_BORN:
			ReciveRequestBornPet(ch, c_pData);
			break;

		case HEADER_CG_PET_RENEW_NAME:
			ReciveRenewPetName(ch, c_pData);
			break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return (0);
	}

	return (iExtraLen);
}
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
