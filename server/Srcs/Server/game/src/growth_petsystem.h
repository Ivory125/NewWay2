#ifdef ENABLE_ASLAN_GROWTH_PET_SYSTEM

class CHARACTER;

#define GROWTH_PET_BORN_COST 100000
#define GROWTH_PET_EVO_MAX_ITEMS 7

#define GROWTH_PET_TRANSPORTBOX_DURATION_DAYS 30

#define NEW_PET_AGE_MAX_BONUS_TIMES 15
#define NEW_PET_AGE_GIVE_BONUS_EVERY_DAYS 7

// Is Equipt items like experience ring, your pet earn 50% more experience
#define NEW_PET_EXP_RING_ITEM_1 72069
#define NEW_PET_EXP_RING_ITEM_2 72070
#define NEW_PET_EXP_RING_ITEM_3 72071

enum {
	GROWTH_PET_CHANGE_LOOK = 0,				// 0 = No change | 1 = Change at Level | 2 = Change at Evo
	GROWTH_PET_CHANGE_LOOK_AT_LEVEL = 80,
	GROWTH_PET_CHANGE_LOOK_AT_EVO = 3,
};

// ---------------------------------------------------------------------------------------- // 

#define PET_MAX_NUM 13
#define PET_SKILL_MAX_NUM 23
#define PET_SKILL_MAX_LEVEL 20
#define SPECIAL_BORN_PETS_MAX_NUM 1

#define FEED_SLOT_MAX_NUM 20
#define REVIVE_SLOT_MAX_NUM 10

// -------------------------------------------------------------------------------------------------------------------------------
struct SPetTable {
	DWORD dwEggVnum;
	DWORD dwSealVnum;
	DWORD dwMobVnum[2];
	DWORD dwBornAttr[3][2];
	DWORD dwBornLifetime[2];
	const char * szSpawnEffectFile;
};

const SPetTable PetListTable[PET_MAX_NUM] =
{
	{ 55401, 55701, { 34041, 34042 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item1.mse"},	 // Monkey
	{ 55402, 55702, { 34045, 34046 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item2.mse"},	 // Spider
	{ 55403, 55703, { 34049, 34050 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item3.mse"},	 // Razador
	{ 55404, 55704, { 34053, 34054 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item4.mse"},	 // Nemere
	{ 55405, 55705, { 34036, 34037 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item5.mse"},	 // Blue Dragon
	{ 55406, 55706, { 34064, 34065 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, "d:/ymir work/effect/etc/buff/buff_item6.mse"},	 // Mini Meley
	{ 55407, 55707, { 34073, 34074 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, ""},	 // Normal Azrael
	{ 55408, 55708, { 34075, 34076 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, ""},	 // Moppelchen
	{ 55409, 55709, { 34080, 34081 }, {{  1 , 23 }, {  1 , 20 },  { 1 , 20 }}, {1, 14}, ""},	 // Baashido
	{ 55410, 55710, { 34082, 34083 }, {{ 25 , 40 }, { 25 , 40 }, { 25 , 40 }}, {1, 14}, ""},	 // Nessi
	{ 55411, 55711, { 34047, 34048 }, {{ 25 , 40 }, { 25 , 40 }, { 25 , 40 }}, {1, 14}, ""},	 // Exedyars Brut
	{ 55412, 55712, { 34077, 34078 }, {{ 25 , 40 }, { 25 , 40 }, { 25 , 40 }}, {1, 14}, ""},	 // Zorniger Mini-Alastor
	{ 55413, 55713, { 34165, 34166 }, {{ 25 , 40 }, { 25 , 40 }, { 25 , 40 }}, {1, 14}, ""},	 // Baby Kommandant
};
// -------------------------------------------------------------------------------------------------------------------------------
struct SPetAttrTable {
	DWORD dwChance[2];
	DWORD dwHPJumpPerLv;
	DWORD dwHP[2];
	DWORD dwATKJumpPerLv;
	DWORD dwATK[2];
	DWORD dwDEFJumpPerLv;
	DWORD dwDEF[2];
};
const SPetAttrTable PetTypeAttrTable[9] =
{
	{ {  0 ,   0 },			0, { 0 , 0 },		0, { 0 , 0 },		0, { 0 , 0 }},		// None
	{ {  1 ,  12 },			6, { 1 , 5 },		6, { 1 , 2 },		6, { 1 , 2 }},		// Typ 1
	{ { 13 ,  35 },			5, { 2 , 3 },		4, { 2 , 3 },		4, { 2 , 3 }},		// Typ 2
	{ { 36 ,  50 },			6, { 3 , 7 },		6, { 1 , 3 },		6, { 1 , 3 }},		// Typ 3
	{ { 51 ,  65 },			5, { 3 , 4 },		4, { 3 , 4 },		4, { 3 , 4 }},		// Typ 4
	{ { 66 ,  75 },			6, { 4 , 9 },		6, { 1 , 4 },		6, { 1 , 4 }},		// Typ 5
	{ { 76 ,  85 },			5, { 5 , 6 },		4, { 4 , 5 },		4, { 4 , 5 }},		// Typ 6
	{ { 86 ,  93 },			6, { 1 , 10},		6, { 1 , 6 },		6, { 1 , 6 }},		// Typ 7
	{ { 94 , 100 },		5, { 5 , 7 },		4, { 5 , 6 },		4, { 5 , 6 }},		// Typ 8
};
// -------------------------------------------------------------------------------------------------------------------------------
struct SPetEvoTable {
	DWORD dwLevel;
	DWORD dwUpgradeType;	// 0 = You cannot level up without an Evo upgrade | 1 = You can Upgrade Evo later
	DWORD dwItemVnum[GROWTH_PET_EVO_MAX_ITEMS];
	DWORD dwItemCount[GROWTH_PET_EVO_MAX_ITEMS];
};
const SPetEvoTable PetEvoTable[GROWTH_PET_MAX_EVO] =
{
	//{	40, 0, { 55003, 30058, 30073, 30075, 30017, 30074, 30088 }, { 10, 10, 10, 10, 5, 5, 5 }},		// Evo 0 -> 1
	//{	80, 0, { 55004, 27994, 30035, 30089, 30031, 30011, 30080 }, { 10, 2, 10, 10, 10, 10, 5 }},		// Evo 1 -> 2
	//{	81, 1, { 55005, 30083, 27992, 27993, 30086, 30077, 30550 }, { 10, 10, 2, 2, 10, 10, 5 }},		// Evo 2 -> 3

	{	40, 0, { 55003, 27992, 27993, 27994, 0, 0, 0 }, { 10, 1, 2, 3, 0, 0, 0 }},			// Evo 0 -> 1
	{	80, 0, { 55004, 27992, 27993, 27994, 0, 0, 0 }, { 10, 5, 7, 10, 0, 0, 0 }},			// Evo 1 -> 2
	{	81, 1, { 55005, 27992, 27993, 27994, 0, 0, 0 }, { 10, 10, 15, 25, 0, 0, 0 }},		// Evo 2 -> 3
};
// -------------------------------------------------------------------------------------------------------------------------------
struct SPetSpecialItemExpTable {
	DWORD dwItemVnum;
	DWORD dwEvolution;
	DWORD dwGivePercent;
};

const SPetSpecialItemExpTable PetSpecialItemExpTable[8] =
{
	// Leckerli (20%)
	{ 55028, 0, 20},
	{ 55028, 1, 20},
	{ 55028, 2, 20},
	{ 55028, 3, 20},

	// Leckerli+ (100%)
	{ 55031, 0, 100},
	{ 55031, 1, 100},
	{ 55031, 2, 100},
	{ 55031, 3, 100},
};
// -------------------------------------------------------------------------------------------------------------------------------
struct SPetSkillTable {
	DWORD	dwBook;
	DWORD	dwAffect;
	bool	bIsActiveSkill;
	DWORD	dwCooltime;
	DWORD	dwLevelValue[PET_SKILL_MAX_LEVEL];
};
const SPetSkillTable PetSkillTable[PET_SKILL_MAX_NUM+1] =
{
	{	  0,							0,	0,	0,		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }},							// None
	{ 55010, POINT_RESIST_WARRIOR,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 }},								// 1 Resist Warrior
	{ 55011, POINT_RESIST_SURA,			0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 }},								// 2 Resist Sura
	{ 55012, POINT_RESIST_ASSASSIN,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 }},								// 3 Resist Assassin 
	{ 55013, POINT_RESIST_SHAMAN,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 }},								// 4 Resist Shaman
	{ 55014, POINT_RESIST_WOLFMAN,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 }},								// 5 Resist Lycan
	{ 55015, POINT_ATT_GRADE_BONUS,			0,	0,	{ 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10 }},								// 6 Beserker
	{ 55016, POINT_RESIST_MAGIC,	0,	0,	{ 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 10, 11, 11, 12 }},											// 7 Anti Magic
	{ 55017, POINT_CASTING_SPEED,		0,	0,		{ 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 17, 17 }},				// 8 Haste (Skill Cooldown reduce)
	{ 55018, POINT_PENETRATE_PCT,		0,	0,		{ 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9 }},								// 9 Drill
	{ 55019, 4,							1,	300,	{ 500, 750, 900, 1280, 1560, 1800, 2010, 2370, 2720, 3050, 3590, 4270, 4780, 5950, 7200, 8540, 10320, 12400, 15600, 20000 }}, // 10 Restoration
	{ 55020, POINT_STEAL_HP,			0,	0,		{ 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5 }},								// 11 Vampirism
	{ 55021, POINT_STEAL_SP,			0,	0,		{ 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 6, 6 }},								// 12 Spiritualism
	{ 55022, POINT_BLOCK,				0,	0,		{ 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10, 11, 12 }},							// 13 Bulwark
	{ 55023, POINT_REFLECT_MELEE,		0,	0,		{ 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 8 }},								// 14 Reflection
	{ 55024, POINT_GOLD_DOUBLE_BONUS,	0,	0,		{ 5, 10, 15, 20, 25, 31, 36, 41, 46, 51, 56, 61, 67, 72, 77, 82, 87, 92, 97, 103 }}, 			// 15 Yang Drop
	{ 55025, POINT_BOW_DISTANCE,		0,	0,		{ 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6 }},								// 16 Bow-Range Bonus
	{ 55026, 11,						1,	600,	{ 10, 12, 15, 19, 22, 26, 31, 38, 45, 54, 60, 66, 73, 80, 88, 97, 108, 120, 133, 150 }},		// 17 Immortal
	{ 55027, 6,							1,	150,	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }},								// 18 Panacea
	{ 55034, POINT_POTION_BONUS,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20 }},							// 19 Master Brew
	{ 55035, POINT_ATTBONUS_MONSTER,	0,	0,		{ 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 12, 15 }},								// 20 Monster Hunter
	{ 55036, POINT_MALL_EXPBONUS,		0,	0,		{ 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20 }},							// 21 Eagle Eyes
	{ 55037, POINT_KILL_HP_RECOVERY,	0,	0,		{ 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 7, 8, 9, 10 }},								// 22 Life Drain
	{ 55038, 6,							1,	150,	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }},								// 23 Featherlight
};
// -------------------------------------------------------------------------------------------------------------------------------
const DWORD Pet_SKill_TablePerc[5][21] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },							// None
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52, 55 },			// 10 Restoration
	{ 0, 2, 5, 7, 10, 12, 15, 17, 20, 23, 25, 27, 30, 32, 35, 37, 40, 42, 45, 47, 50 },			// 17 Immortal
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 25, 27, 30, 33, 36, 38, 41, 44, 47, 50, 52, 55 },			// 18 Panacea
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 25, 27, 30, 33, 36, 38, 41, 44, 47, 50, 52, 55 },			// 23 Featherlight
};
// -------------------------------------------------------------------------------------------------------------------------------

class CGrowthPetSystem
{
	public:
		enum ENewPetOptions
		{
			EPetOption_Followable		= 1 << 0,
			EPetOption_Mountable		= 1 << 1,
			EPetOption_Summonable		= 1 << 2,
			EPetOption_Combatable		= 1 << 3,		
		};

		CGrowthPetSystem(LPCHARACTER owner, DWORD options = EPetOption_Followable | EPetOption_Summonable);
		virtual ~CGrowthPetSystem();

		virtual bool	Update(DWORD deltaTime);
		virtual bool	_UpdateFollowAI();
		virtual bool	_UpdatAloneActionAI(float fMinDist, float fMaxDist);

	private:
		void			Reset();
		bool			Follow(float fMinDistance = 150.f);

	public:
		void			Destroy();

		LPCHARACTER		GetCharacter()	const					{ return m_pkChar; }
		LPCHARACTER		GetOwner()	const						{ return m_pkOwner; }
		DWORD			GetVID() const							{ return m_dwVID; }
		DWORD			GetVnum() const							{ return m_dwVnum; }

		bool			HasOption(ENewPetOptions option) const		{ return m_dwOptions & option; }

		void			SetName(const char* petName);
		void			SetLevel(DWORD level);
		void			SetNextLevel();

		DWORD			Summon(LPITEM pSummonItem, bool bSpawnFar = false);
		void			Unsummon();

		bool			IsSummoned() const			{ return 0 != m_pkChar; }
		void			SetSummonItem (LPITEM pItem);
		DWORD			GetSummonItemVID () { return m_dwSummonItemVID; }
		DWORD			GetSummonItemID () { return m_dwSummonItemID; }
		DWORD			GetEvolution() { return m_dwEvo; }
		DWORD			GetLevel() { return m_dwLevel; }
		DWORD			GetPetType() { return m_dwPetType; }
		DWORD			GetPetSkillType(int slot) { return m_dwSkill[slot][0]; }
		DWORD			GetPetSkillLevel(int slot) { return m_dwSkill[slot][1]; }
		void			SetEvolution(int lv);
		void			UpdateExperience(DWORD exp, int mode);
		DWORD			GetExpMonster() { return m_dwExpMob; }
		DWORD			GetExpItem() { return m_dwExpItem; }
		void			SetNextExp();
		int				GetNextExpFromMob() { return m_dwNextExpMob; }
		int				GetNextExpFromItem() { return m_dwNextExpItem; }
		int				GetLevelStep() { return m_dwLevelStep; }
		bool			CanEvolution();

		void			IncreasePetBonus();
		bool			CheckFeedItem(BYTE wndType, LPITEM item);
		void			SetItemFeed(int pos, int invpos);
		void			ResetItemFeed();
		void			ItemFeed(int type);
		void			SetReviveItem(int pos, int invpos);
		void			ResetReviveItem();
		void			RevivePet(LPITEM SealItem, bool isSpecialItem = false);
		void			DoPetSkillAlone();
		void			UpdateTime();
		void			SaveSQL(LPITEM SpecifiyItem = NULL);

		bool			ChangePetAttr(LPITEM SpecifiyItem = NULL);
		void			RemovePetSkill(int skillslot, int itemvnum);
		bool			IncreasePetSkill(int skill);
		bool			IncreasePetEvolution();

		void			GiveBuff();
		void			ClearBuff();
		void			SendPetInfoPacket(bool isUnsummon = false);

	private:
		std::string		m_szName;
		DWORD			m_dwVnum;
		DWORD			m_dwVID;
		DWORD			m_dwLevel;
		BYTE			m_dwLevelStep;
		DWORD			m_dwExpMob;
		DWORD			m_dwExpItem;
		DWORD			m_dwNextExpMob;
		DWORD			m_dwNextExpItem;
		BYTE			m_dwEvo;
		int				m_dwAge;
		int				m_dwTimePet;
		int				m_dwslotimm;

		DWORD			m_dwImmTime;

		DWORD			m_dwFeedSlotItem[FEED_SLOT_MAX_NUM];
		DWORD			m_dwReviveSlotItem[REVIVE_SLOT_MAX_NUM];
		int				m_dwSkill[3][2];
		BYTE			m_dwBonus[3][2];
		BYTE			m_dwTypeBonus[3];

		DWORD			m_dwOptions;
		DWORD			m_dwLastActionTime;
		DWORD			m_dwSummonItemVID;
		DWORD			m_dwSummonItemID;
		DWORD			m_dwSummonItemVnum;

		DWORD			m_dwItemDuration;
		DWORD			m_dwBornDuration;
		DWORD			m_dwBirthday;
		DWORD			m_dwAgeBonus;
	
		DWORD			m_dwPetType;

		short			m_originalMoveSpeed;


		LPCHARACTER		m_pkChar;
		LPCHARACTER		m_pkOwner;
		LPEVENT			m_pkGrowthPetUpdateEvent;
};
#endif