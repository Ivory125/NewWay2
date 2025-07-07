import os
import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import localeInfo
import constInfo
import ime
import event
import wndMgr
import petskill
import uiToolTip
import uiCommon
import shop

def unsigned32(n):
	return n & 0xFFFFFFFFL

MAX_SKILL_NUM = 23

FEED_SLOT_X_NUM = 5
FEED_SLOT_Y_NUM = 4
FEED_SLOT_NUM = FEED_SLOT_X_NUM * FEED_SLOT_Y_NUM
EGG_SEAL_FEED_PERC_VALUE = [3, 5]

SPECIAL_FEED_ITEM = [
	# Leckerli (20%)
	[55028 , 0, 20],
	[55028 , 1, 20],
	[55028 , 2, 20],
	[55028 , 3, 20],
	
	# Leckerli+ (100%)
	[55031 , 0, 100],
	[55031 , 1, 100],
	[55031 , 2, 100],
	[55031 , 3, 100],
]


GOLDEN_COLOR = 0xFFFEE3AE
POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)
NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)

REVIVE_DESC_SHOW_LINE_COUNT_MAX = 3

AFFECT_DICT = {
		item.APPLY_MAX_HP : localeInfo.TOOLTIP_MAX_HP,
		item.APPLY_MAX_SP : localeInfo.TOOLTIP_MAX_SP,
		item.APPLY_CON : localeInfo.TOOLTIP_CON,
		item.APPLY_INT : localeInfo.TOOLTIP_INT,
		item.APPLY_STR : localeInfo.TOOLTIP_STR,
		item.APPLY_DEX : localeInfo.TOOLTIP_DEX,
		item.APPLY_ATT_SPEED : localeInfo.TOOLTIP_ATT_SPEED,
		item.APPLY_MOV_SPEED : localeInfo.TOOLTIP_MOV_SPEED,
		item.APPLY_CAST_SPEED : localeInfo.TOOLTIP_CAST_SPEED,
		item.APPLY_HP_REGEN : localeInfo.TOOLTIP_HP_REGEN,
		item.APPLY_SP_REGEN : localeInfo.TOOLTIP_SP_REGEN,
		item.APPLY_POISON_PCT : localeInfo.TOOLTIP_APPLY_POISON_PCT,
		item.APPLY_STUN_PCT : localeInfo.TOOLTIP_APPLY_STUN_PCT,
		item.APPLY_SLOW_PCT : localeInfo.TOOLTIP_APPLY_SLOW_PCT,
		item.APPLY_CRITICAL_PCT : localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,
		item.APPLY_PENETRATE_PCT : localeInfo.TOOLTIP_APPLY_PENETRATE_PCT,

		item.APPLY_ATTBONUS_WARRIOR : localeInfo.TOOLTIP_APPLY_ATTBONUS_WARRIOR,
		item.APPLY_ATTBONUS_ASSASSIN : localeInfo.TOOLTIP_APPLY_ATTBONUS_ASSASSIN,
		item.APPLY_ATTBONUS_SURA : localeInfo.TOOLTIP_APPLY_ATTBONUS_SURA,
		item.APPLY_ATTBONUS_SHAMAN : localeInfo.TOOLTIP_APPLY_ATTBONUS_SHAMAN,
		item.APPLY_ATTBONUS_MONSTER : localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER,

		item.APPLY_ATTBONUS_HUMAN : localeInfo.TOOLTIP_APPLY_ATTBONUS_HUMAN,
		item.APPLY_ATTBONUS_ANIMAL : localeInfo.TOOLTIP_APPLY_ATTBONUS_ANIMAL,
		item.APPLY_ATTBONUS_ORC : localeInfo.TOOLTIP_APPLY_ATTBONUS_ORC,
		item.APPLY_ATTBONUS_MILGYO : localeInfo.TOOLTIP_APPLY_ATTBONUS_MILGYO,
		item.APPLY_ATTBONUS_UNDEAD : localeInfo.TOOLTIP_APPLY_ATTBONUS_UNDEAD,
		item.APPLY_ATTBONUS_DEVIL : localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL,
		item.APPLY_STEAL_HP : localeInfo.TOOLTIP_APPLY_STEAL_HP,
		item.APPLY_STEAL_SP : localeInfo.TOOLTIP_APPLY_STEAL_SP,
		item.APPLY_MANA_BURN_PCT : localeInfo.TOOLTIP_APPLY_MANA_BURN_PCT,
		item.APPLY_DAMAGE_SP_RECOVER : localeInfo.TOOLTIP_APPLY_DAMAGE_SP_RECOVER,
		item.APPLY_BLOCK : localeInfo.TOOLTIP_APPLY_BLOCK,
		item.APPLY_DODGE : localeInfo.TOOLTIP_APPLY_DODGE,
		item.APPLY_RESIST_SWORD : localeInfo.TOOLTIP_APPLY_RESIST_SWORD,
		item.APPLY_RESIST_TWOHAND : localeInfo.TOOLTIP_APPLY_RESIST_TWOHAND,
		item.APPLY_RESIST_DAGGER : localeInfo.TOOLTIP_APPLY_RESIST_DAGGER,
		item.APPLY_RESIST_BELL : localeInfo.TOOLTIP_APPLY_RESIST_BELL,
		item.APPLY_RESIST_FAN : localeInfo.TOOLTIP_APPLY_RESIST_FAN,
		item.APPLY_RESIST_BOW : localeInfo.TOOLTIP_RESIST_BOW,
		item.APPLY_RESIST_FIRE : localeInfo.TOOLTIP_RESIST_FIRE,
		item.APPLY_RESIST_ELEC : localeInfo.TOOLTIP_RESIST_ELEC,
		item.APPLY_RESIST_MAGIC : localeInfo.TOOLTIP_RESIST_MAGIC,
		item.APPLY_RESIST_WIND : localeInfo.TOOLTIP_APPLY_RESIST_WIND,
		item.APPLY_REFLECT_MELEE : localeInfo.TOOLTIP_APPLY_REFLECT_MELEE,
		item.APPLY_REFLECT_CURSE : localeInfo.TOOLTIP_APPLY_REFLECT_CURSE,
		item.APPLY_POISON_REDUCE : localeInfo.TOOLTIP_APPLY_POISON_REDUCE,
		item.APPLY_KILL_SP_RECOVER : localeInfo.TOOLTIP_APPLY_KILL_SP_RECOVER,
		item.APPLY_EXP_DOUBLE_BONUS : localeInfo.TOOLTIP_APPLY_EXP_DOUBLE_BONUS,
		item.APPLY_GOLD_DOUBLE_BONUS : localeInfo.TOOLTIP_APPLY_GOLD_DOUBLE_BONUS,
		item.APPLY_ITEM_DROP_BONUS : localeInfo.TOOLTIP_APPLY_ITEM_DROP_BONUS,
		item.APPLY_POTION_BONUS : localeInfo.TOOLTIP_APPLY_POTION_BONUS,
		item.APPLY_KILL_HP_RECOVER : localeInfo.TOOLTIP_APPLY_KILL_HP_RECOVER,
		item.APPLY_IMMUNE_STUN : localeInfo.TOOLTIP_APPLY_IMMUNE_STUN,
		item.APPLY_IMMUNE_SLOW : localeInfo.TOOLTIP_APPLY_IMMUNE_SLOW,
		item.APPLY_IMMUNE_FALL : localeInfo.TOOLTIP_APPLY_IMMUNE_FALL,
		item.APPLY_BOW_DISTANCE : localeInfo.TOOLTIP_BOW_DISTANCE,
		item.APPLY_DEF_GRADE_BONUS : localeInfo.TOOLTIP_DEF_GRADE,
		item.APPLY_ATT_GRADE_BONUS : localeInfo.TOOLTIP_ATT_GRADE,
		item.APPLY_MAGIC_ATT_GRADE : localeInfo.TOOLTIP_MAGIC_ATT_GRADE,
		item.APPLY_MAGIC_DEF_GRADE : localeInfo.TOOLTIP_MAGIC_DEF_GRADE,
		item.APPLY_MAX_STAMINA : localeInfo.TOOLTIP_MAX_STAMINA,
		item.APPLY_MALL_ATTBONUS : localeInfo.TOOLTIP_MALL_ATTBONUS,
		item.APPLY_MALL_DEFBONUS : localeInfo.TOOLTIP_MALL_DEFBONUS,
		item.APPLY_MALL_EXPBONUS : localeInfo.TOOLTIP_MALL_EXPBONUS,
		item.APPLY_MALL_ITEMBONUS : localeInfo.TOOLTIP_MALL_ITEMBONUS,
		item.APPLY_MALL_GOLDBONUS : localeInfo.TOOLTIP_MALL_GOLDBONUS,
		item.APPLY_SKILL_DAMAGE_BONUS : localeInfo.TOOLTIP_SKILL_DAMAGE_BONUS,
		item.APPLY_NORMAL_HIT_DAMAGE_BONUS : localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS,
		item.APPLY_SKILL_DEFEND_BONUS : localeInfo.TOOLTIP_SKILL_DEFEND_BONUS,
		item.APPLY_NORMAL_HIT_DEFEND_BONUS : localeInfo.TOOLTIP_NORMAL_HIT_DEFEND_BONUS,
		item.APPLY_PC_BANG_EXP_BONUS : localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC,
		item.APPLY_PC_BANG_DROP_BONUS : localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC,
		item.APPLY_RESIST_WARRIOR : localeInfo.TOOLTIP_APPLY_RESIST_WARRIOR,
		item.APPLY_RESIST_ASSASSIN : localeInfo.TOOLTIP_APPLY_RESIST_ASSASSIN,
		item.APPLY_RESIST_SURA : localeInfo.TOOLTIP_APPLY_RESIST_SURA,
		item.APPLY_RESIST_SHAMAN : localeInfo.TOOLTIP_APPLY_RESIST_SHAMAN,
		item.APPLY_MAX_HP_PCT : localeInfo.TOOLTIP_APPLY_MAX_HP_PCT,
		item.APPLY_MAX_SP_PCT : localeInfo.TOOLTIP_APPLY_MAX_SP_PCT,
		item.APPLY_ENERGY : localeInfo.TOOLTIP_ENERGY,
		item.APPLY_COSTUME_ATTR_BONUS : localeInfo.TOOLTIP_COSTUME_ATTR_BONUS,
		
		item.APPLY_MAGIC_ATTBONUS_PER : localeInfo.TOOLTIP_MAGIC_ATTBONUS_PER,
		item.APPLY_MELEE_MAGIC_ATTBONUS_PER : localeInfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER,
		item.APPLY_RESIST_ICE : localeInfo.TOOLTIP_RESIST_ICE,
		item.APPLY_RESIST_EARTH : localeInfo.TOOLTIP_RESIST_EARTH,
		item.APPLY_RESIST_DARK : localeInfo.TOOLTIP_RESIST_DARK,
		item.APPLY_ANTI_CRITICAL_PCT : localeInfo.TOOLTIP_ANTI_CRITICAL_PCT,
		item.APPLY_ANTI_PENETRATE_PCT : localeInfo.TOOLTIP_ANTI_PENETRATE_PCT,
	}
	
	
def checkdiv(n):
	x = str(n/10.0)
	if len(x) > 3:
		return str(x)[0:3]
	return str(x)

def pointop(value):
	inStr = str(value)
	if value / 10 < 1:
		return "0."+inStr
	else:		
		return inStr[0:len(inStr)-1]+"."+inStr[len(inStr)-1:]
		
def GetAffectString(affectType, affectValue):
	if 0 == affectType:
		return None

	if 0 == affectValue:
		return None

	try:
		return AFFECT_DICT[affectType](affectValue)
	except TypeError:
		return "UNKNOWN_VALUE[%s] %s" % (affectType, affectValue)
	except KeyError:
		return "UNKNOWN_TYPE[%s] %s" % (affectType, affectValue)
		
class PetSystemMain(ui.ScriptWindow):
	
	TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
	NORMAL_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
	
	CONDITION_COLOR = 0xffBEB47D
	
	POSITIVE_COLOR = grp.GenerateColor(0.2862, 0.8196, 0.1843, 1.0)
	NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)
	
	class TextToolTip(ui.Window):
		def __init__(self, y):
			ui.Window.__init__(self, "TOP_MOST")

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignLeft()
			textLine.SetOutline()
			textLine.Show()
			self.y = y
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 60 + self.y)

	class ReviveDescriptionBox(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.descReviveIndex = -1
			
		def __del__(self):
			ui.Window.__del__(self)
			
		def SetIndex(self, index):
			self.descReviveIndex = index
			
		def OnRender(self):
			event.RenderEventSet(self.descReviveIndex)
			
	def __init__(self):
		self.wndFeed = None
		self.wndEvoInfo = None
		self.wndPetInfoMini = None

		self.tabDict = None
		self.tabButtonDict = None
		self.pageDict = None

		self.SealVnum = 0
		self.SealSlot = 0
		self.PetName = ""
		self.PetLevel = 0
		self.PetAge = 0
		self.PetBirthday = 0
		self.PetEvo = 0
		self.PetCanEvo = 0
		self.PetExp = 0
		self.PetExpItem = 0
		self.PetLevelExpMob = 0
		self.PetLevelExpItem = 0
		self.PetExpItemRest = 0
		self.SealDuration = 0
		self.BornDuration = 0
		
		self.PetBonusList = [0, 0, 0]
		self.PetSkillList = [[-1,-1], [-1,-1], [-1,-1]]
		self.PetEvoItemList = []
		
		self.SlotAttrPagePos = [-1, -1, -1]
		self.SlotReviveIndex = {}
		self.reviveDestItemCount = 0
		self.reviveCurItemCount = 0
		self.descReviveBox = None
		self.descReviveIndex = -1
		self.descY = 0
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.interface = None
			self.wndInventory = None
			self.AttrLockItems = {i:(-1,-1) for i in range(2)}
			self.ReviveLockItems = {i:(-1,-1) for i in range(20)}

		ui.ScriptWindow.__init__(self)
		self.LoadWindow()
		
	def __del__(self):
		self.wndFeed = None
		self.wndPetInfoMini = None
		self.descReviveBox = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.interface = None
			self.wndInventory = None
			self.AttrLockItems = {i:(-1,-1) for i in range(2)}
			self.ReviveLockItems = {i:(-1,-1) for i in range(20)}
		ui.ScriptWindow.__del__(self)
			
	def Show(self):
		if self.wndPetInfoMini:
			self.wndPetInfoMini.Show()
		if self.wndFeed:
			if self.wndFeed.IsOpen():
				self.wndFeed.Show()
		if self.descReviveBox:
			self.descReviveBox.Show()
		ui.ScriptWindow.Show(self)

	def Hide(self):
		ui.ScriptWindow.Hide(self)
		if self.wndFeed:
			self.wndFeed.Hide()
		if self.wndPetInfoMini:
			self.wndPetInfoMini.Hide()
		if self.descReviveBox:
			self.descReviveBox.Hide()
		
	def Close(self):
		event.ClearEventSet(self.descReviveIndex)
		self.descReviveIndex = -1
		if self.descReviveBox:
			self.descReviveBox.Hide()
		if app.WJ_ENABLE_TRADABLE_ICON:
			for privatePos, (itemInvenPage, itemSlotPos) in self.AttrLockItems.items():
				if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
			for privatePos, (itemInvenPage, itemSlotPos) in self.ReviveLockItems.items():
				if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)

			self.AttrLockItems = {i:(-1,-1) for i in range(2)}
			self.ReviveLockItems = {i:(-1,-1) for i in range(20)}
			self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			self.interface.RefreshMarkInventoryBag()
		ui.ScriptWindow.Hide(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/PetInformationWindow.py")
		except:
			import exception
			exception.Abort("PetInformationWindow.LoadWindow.LoadObject")
			
		try:
			self.tabDict = {
				"MAIN"		: self.GetChild("Tab_01"),
				"ATTR"		: self.GetChild("Tab_02"),
				"REVIVE"		: self.GetChild("Tab_03"),
			}

			self.tabButtonDict = {
				"MAIN"		: self.GetChild("Tab_Button_01"),
				"ATTR"		: self.GetChild("Tab_Button_02"),
				"REVIVE"		: self.GetChild("Tab_Button_03"),
			}

			self.pageDict = {
				"MAIN"		: self.GetChild("PetInfo_Page"),
				"ATTR"		: self.GetChild("PetAttrChange_Page"),
				"REVIVE"		: self.GetChild("PetRevive_Page"),
			}
			
			for (tabKey, tabButton) in self.tabButtonDict.items():
				tabButton.SetEvent(ui.__mem_func__(self.__OnClickTabButton), tabKey)
			
			self.SlotItemImage = self.GetChild("PetSealIconSlot")
			self.SlotItemImage.SetOverInItemEvent(ui.__mem_func__(self.OverInSealSlot))
			self.SlotItemImage.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSealSlot))
			
			self.expwind = self.GetChild("UpBringing_Pet_EXP_Gauge_Board")
			self.tooltipexp = []
			for i in range(0,4):
				self.tooltipexp.append(self.TextToolTip(15*i))
				self.tooltipexp[i].Hide()
	
			self.PetEXPBubbles = [
				self.GetChild("UpBringing_Pet_EXPGauge_01"),
				self.GetChild("UpBringing_Pet_EXPGauge_02"),
				self.GetChild("UpBringing_Pet_EXPGauge_03"),
				self.GetChild("UpBringing_Pet_EXPGauge_04"),
				self.GetChild("UpBringing_Pet_EXPGauge_05"),
			]
			
			for exp in self.PetEXPBubbles:
				exp.SetSize(0, 0)
			
			self.FeedButtonList = [self.GetChild("FeedTimeButton"), self.GetChild("FeedEvolButton"), self.GetChild("FeedExpButton")]
			
			self.PetSkillSlots = self.GetChild("PetSkillSlot")
			self.PetSkillSlots.SetSlot(0, 2, 32, 32, petskill.GetEmptySkill())
			self.PetSkillSlots.SetSlot(1, 2, 32, 32, petskill.GetEmptySkill())
			self.PetSkillSlots.SetSlot(2, 2, 32, 32, petskill.GetEmptySkill())
			
			self.PetSkillSlots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectSkillSlotWithItem))
			self.PetSkillSlots.SetOverInItemEvent(ui.__mem_func__(self.PetSkillTooltipShow))
			self.PetSkillSlots.SetOverOutItemEvent(ui.__mem_func__(self.PetSkillTooltipHide))	

			self.SetDefaultInfo()

			PET_FILE_NAME = "%s/pet_skill.txt" % app.GetLocalePath()
			PET_FILE_SKILL = "%s/pet_skill_bonus.txt" % app.GetLocalePath()
			self.linesPetDesc = open(PET_FILE_NAME, "r").readlines()
			self.linesPetSkills = open(PET_FILE_SKILL, "r").readlines()
			self.SkillTooltip = uiToolTip.ToolTip(180)

			#Event
			self.GetChild("DetermineButton").SetEvent(ui.__mem_func__(self.SendPetAskPettype))
			self.GetChild("CloseButton").SetEvent(ui.__mem_func__(self.Close))
			self.GetChild("FeedEvolButton").SetEvent(ui.__mem_func__(self.ClickEvolutionButton))
			self.GetChild("FeedTimeButton").SetToggleDownEvent(lambda arg=1,arg1=0: self.ClickFeedButton(arg,arg1))
			self.GetChild("FeedTimeButton").SetToggleUpEvent(lambda arg=0,arg1=0: self.ClickFeedButton(arg,arg1))
			self.GetChild("FeedExpButton").SetToggleDownEvent(lambda arg=1,arg1=2: self.ClickFeedButton(arg,arg1))
			self.GetChild("FeedExpButton").SetToggleUpEvent(lambda arg=0,arg1=2: self.ClickFeedButton(arg,arg1))
			
			#Skill-Remove
			self.questionSkillDelDlg = uiCommon.QuestionDialog2()
			
			##################################################
			## Page : ATTR
			
			self.newAttrInfoText = {
				1 : localeInfo.PET_ATTR_DETERMINE_TYPE1,
				2 : localeInfo.PET_ATTR_DETERMINE_TYPE2,
				3 : localeInfo.PET_ATTR_DETERMINE_TYPE3,
				4 : localeInfo.PET_ATTR_DETERMINE_TYPE4,
				5 : localeInfo.PET_ATTR_DETERMINE_TYPE5,
				6 : localeInfo.PET_ATTR_DETERMINE_TYPE6,
				7 : localeInfo.PET_ATTR_DETERMINE_TYPE7,
				8 : localeInfo.PET_ATTR_DETERMINE_TYPE8
			}
			
			
			self.GetChild("Change_Pet_Slot").SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyAttrSlot))
			self.GetChild("Change_Pet_Slot").SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.GetChild("Change_Pet_Slot").SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GetChild("Change_Pet_Slot").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.GetChild("Pet_Change_Button").SetEvent(ui.__mem_func__(self.SendPetRenewAttr))
			self.GetChild("Attr_Clear_Button").SetEvent(ui.__mem_func__(self.ResetAttrPage))

			##################################################
			## Page : REVIVE

			self.GetChild("AgeTitle01").Hide()
			self.GetChild("AgeValue01").Hide()
			self.GetChild("AgeTitle02").Hide()
			self.GetChild("AgeValue02").Hide()
			
			self.GetChild("Revive_Pet_Slots").SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptyReviveSlot))
			self.GetChild("Revive_Pet_Slots").SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.GetChild("Revive_Pet_Slots").SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GetChild("Revive_Pet_Slots").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.GetChild("Revive_Pet_DescPrevButton").SetEvent(ui.__mem_func__(self.PrevDescriptionPage))
			self.GetChild("Revive_Pet_DescNextButton").SetEvent(ui.__mem_func__(self.NextDescriptionPage))
			
			self.descReviveBox = self.ReviveDescriptionBox()
			self.descReviveBox.SetParent(self.GetChild("ReviveDescBox"))
			self.descReviveBox.Show()
			
			self.GetChild("Revive_Accept_Button").SetEvent(ui.__mem_func__(self.SendPetRevive))
			self.GetChild("Revive_Clear_Button").SetEvent(ui.__mem_func__(self.ResetRevivePage))
			
			##################################################
			
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()
			
			self.state = "MAIN"
			self.SetTabState("MAIN")
			self.SetDefaultInfo()
			
		except:
			import exception
			exception.Abort("PetInformationWindow.LoadWindow.BindObject")
		
	def ClickFeedButton(self, isDown, type):
		if self.SealVnum != 0:
			if isDown == 1:
				if self.wndFeed:
					self.wndFeed.Close()
					self.wndFeed = None
				self.wndFeed = PetFeedWindow(self, type)
				if self.wndInventory:
					if not self.wndInventory.IsShow():
						self.wndInventory.Show()
			else:
				self.wndFeed.Close()
				self.wndFeed = None
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_SUMMON_FIRST)
			self.FeedButtonList[type].SetUp()
	
	def ClickEvolutionButton(self):
		if self.wndEvoInfo:
			if not self.wndEvoInfo.IsShow():
				net.SendChatPacket("/petevolutionopen")
			else:
				self.wndEvoInfo.SetTop()
		else:
				net.SendChatPacket("/petevolutionopen")
		
	def DestroyFeedWindow(self, type):
		self.FeedButtonList[type].SetUp()
		self.wndFeed = None
		
	def SetDefaultInfo(self):
		if self.wndPetInfoMini:
			self.wndPetInfoMini = None

		self.SealVnum = 0
		self.SealSlot = 0
		self.PetName = ""
		self.PetLevel = 0
		self.PetBirthday = 0
		self.PetAge = 0
		self.PetEvo = -1
		self.PetCanEvo = 0
		self.PetExp = 0
		self.PetExpItem = 0
		self.PetExpItemRest = 0
		self.PetLevelExpMob = 0
		self.PetLevelExpItem = 0
		self.SealDuration = 0
		self.BornDuration = 0
		
		self.PetBonusList = [-1, -1, -1]
		self.PetSkillList = [[-1,-1], [-1,-1], [-1,-1]]
		
		self.GetChild("PetSealIconSlot").ClearSlot(0)
		self.PetSkillSlots.ClearSlot(0)
		self.PetSkillSlots.ClearSlot(1)
		self.PetSkillSlots.ClearSlot(2)
		self.PetSkillSlots.SetSlot(0, 2, 32, 32, petskill.GetEmptySkill())
		self.PetSkillSlots.SetSlot(1, 2, 32, 32, petskill.GetEmptySkill())
		self.PetSkillSlots.SetSlot(2, 2, 32, 32, petskill.GetEmptySkill())

		self.UpdateInformation()
		self.UpdateSkillInformation()

	def UpdateInformation(self):
		if not self.wndPetInfoMini and self.SealVnum != 0:
			self.wndPetInfoMini = PetSystemMini(self)
			self.wndPetInfoMini.Show()
		if self.wndPetInfoMini:
			self.wndPetInfoMini.UpdateInfo()
	
		if self.SealVnum != 0:
			self.GetChild("PetSealIconSlot").SetItemSlot(0, self.SealVnum, 0)
		self.GetChild("PetName").SetText(self.PetName)
		
		if self.PetEvo != -1:
			EvoName = [localeInfo.PET_INFORMATION_STAGE1, localeInfo.PET_INFORMATION_STAGE2, localeInfo.PET_INFORMATION_STAGE3, localeInfo.PET_INFORMATION_STAGE4]
			self.GetChild("EvolName").SetText(EvoName[self.PetEvo])
		else:
			self.GetChild("EvolName").SetText("")
	
		duration = self.SealDuration - app.GetGlobalTimeStamp()	
		if self.SealDuration > 0:
			self.GetChild("LifeGauge").SetPercentage(duration, self.BornDuration)
			self.GetChild("LifeGauge").Show()
		else:
			self.GetChild("LifeGauge").Hide()

		self.GetChild("LifeTextValue").SetText(localeInfo.SecondToPetTime(duration)+" / "+localeInfo.SecondToPetTime(self.BornDuration))
		
		if self.SealVnum != 0:
			self.GetChild("LevelValue").SetText(str(self.PetLevel))
			self.GetChild("AgeValue").SetText(localeInfo.PetTimeTextInfo(self.PetAge))
			self.GetChild("HpValue").SetText(pointop(self.PetBonusList[0])+"%")
			self.GetChild("AttValue").SetText(pointop(self.PetBonusList[1])+"%")
			self.GetChild("DefValue").SetText(pointop(self.PetBonusList[2])+"%")
		else:
			self.GetChild("LevelValue").SetText("")
			self.GetChild("AgeValue").SetText("")
			self.GetChild("HpValue").SetText("")
			self.GetChild("AttValue").SetText("")
			self.GetChild("DefValue").SetText("")
		
		if self.PetCanEvo == 1:
			self.FeedButtonList[1].Enable()
			self.FeedButtonList[1].EnableFlash()
		else:
			self.FeedButtonList[1].DisableFlash()
			self.FeedButtonList[1].Disable()
		
		######## EXP ##########

		expm = self.PetExp
		expi = self.PetExpItem
		destexpmob = self.PetLevelExpMob
		destexpitem = self.PetLevelExpItem
		
		if destexpmob > 0:
			totexpm = destexpmob
			totexpi = destexpitem
			expi = min(expi, totexpi)
			expmp =  float(expm) / totexpm * 100
			expip =  float(expi) / totexpi * 100
		else:
			(totexpm, totexpi, expmp, expip) = (0, 0, 0, 0)
			
		
		curPoint = int(min(expm, totexpm))
		curPoint = int(max(expm, 0))
		maxPoint = int(max(totexpm, 0))
		
		curPointi = int(min(expi, totexpi))
		curPointi = int(max(expi, 0))
		maxPointi = int(max(totexpi, 0))

		quarterPoint = maxPoint / 4
		quarterPointi = maxPointi 
		(FullCount, FullCounti) = (0, 0)

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)
		if 0 != quarterPointi:
			FullCounti = min(1, curPointi / quarterPointi)
		for i in xrange(4):
			self.PetEXPBubbles[i].Hide()
			
		self.PetEXPBubbles[4].Hide()

		for i in xrange(FullCount):
			self.PetEXPBubbles[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.PetEXPBubbles[i].Show()
		for i in xrange(FullCounti):
			self.PetEXPBubbles[4].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.PetEXPBubbles[4].Show()
		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.PetEXPBubbles[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.PetEXPBubbles[FullCount].Show()
		if 0 != quarterPointi:
			if FullCounti < 1:
				Percentage = float(curPointi % quarterPointi) / quarterPointi - 1.0
				self.PetEXPBubbles[4].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.PetEXPBubbles[4].Show()

		self.tooltipexp[0].SetText(uiScriptLocale.PET_INFORMATION_EXP +": "+localeInfo.NumberWithPoint(str(expm))+" / "+localeInfo.NumberWithPoint(str(totexpm))+" (%.2f%%)" % expmp)
		self.tooltipexp[1].SetText(uiScriptLocale.PET_INFORMATION_EXP_BUTTON +": "+localeInfo.NumberWithPoint(str(expi))+" / "+localeInfo.NumberWithPoint(str(totexpi))+" (%.2f%%)" % expip)
		self.PetExpItemRest = totexpi - expi
		if self.wndFeed:
			self.wndFeed.UpdateInfoText()

	def SetBasicInfo(self, SealVnum, SealSlot, Name, Age, Birthday, SealDuration, BornDuration):
		self.SealVnum = SealVnum
		self.SealSlot = SealSlot
		self.PetName = Name
		self.PetAge = Age
		self.PetBirthday = Birthday
		self.SealDuration = SealDuration
		self.BornDuration = BornDuration
		
		self.UpdateInformation()

	def SetLevelInfo(self, Level, Evolution, CanEvoUpgrade, Exp, ExpItem, LevelExpMob, LevelExpItem):
		self.PetLevel = Level
		self.PetEvo = Evolution
		self.PetCanEvo = CanEvoUpgrade
		self.PetExp = Exp
		self.PetExpItem = ExpItem
		self.PetLevelExpMob = LevelExpMob
		self.PetLevelExpItem = LevelExpItem
		
		self.UpdateInformation()
	
	def SetBonusInfo(self, ValueATK, ValueHP, ValueDEF):
		self.PetBonusList = [ValueATK, ValueHP, ValueDEF]
		
		self.UpdateInformation()
	
	def SetSkillInfo(self, Skill0, SkillLevel0, Skill1, SkillLevel1, Skill2, SkillLevel2):
		self.PetSkillList = [[ Skill0 , SkillLevel0 ], [ Skill1 , SkillLevel1 ], [ Skill2 , SkillLevel2 ]]
		
		self.UpdateSkillInformation()
	
	def UpdateSkillInformation(self):
		if self.wndPetInfoMini:
			self.wndPetInfoMini.UpdateInfo()
		for slot in xrange(3):
			if self.PetSkillList[slot][0] != -1:
				self.PetSkillSlots.ClearSlot(slot)
				self.PetSkillSlots.SetPetSkillSlot(slot, self.PetSkillList[slot][0], self.PetSkillList[slot][1])
				self.PetSkillSlots.SetCoverButton(slot)
				self.PetSkillSlots.SetAlwaysRenderCoverButton(slot, TRUE)
			else:
				self.PetSkillSlots.SetSlot(slot, 2, 32, 32, petskill.GetEmptySkill())
	
	def SelectEmptyAttrSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
		
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			selectedItemVnum = player.GetItemIndex(attachedSlotPos)
			item.SelectItem(selectedItemVnum)


			if self.SlotAttrPagePos[2] != -1:
				return

			if selectedSlotPos == 0:
				if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_SEAL:
				
					socket = []
					for i in xrange(player.METIN_SOCKET_MAX_NUM):
						socket.append(player.GetItemMetinSocket(attachedSlotPos, i))
			
					if socket[1] < app.GetGlobalTimeStamp():
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_REVIVE_BEFORE_CHANGE_ATTR)
						mouseModule.mouseController.DeattachObject()
						return

					self.Slot0Index = attachedItemIndex
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_CANNOT_SET_ITEM_HERE)
					mouseModule.mouseController.DeattachObject()
					return
			elif selectedSlotPos == 1:
				if not item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_ATTR:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_CANNOT_SET_ITEM_HERE)
					mouseModule.mouseController.DeattachObject()
					return

			if attachedItemCount == 1:
				attachedItemCount = 0
				
			if player.SLOT_TYPE_INVENTORY == attachedSlotType and not attachedSlotPos in self.SlotAttrPagePos:
				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				self.SlotAttrPagePos[selectedSlotPos] = attachedSlotPos
				self.GetChild("Change_Pet_Slot").SetItemSlot(selectedSlotPos, attachedItemIndex, attachedItemCount)
				snd.PlaySound("sound/ui/drop.wav")
				if app.WJ_ENABLE_TRADABLE_ICON:
					self.CantTradableItem("ATTR", selectedSlotPos, attachedSlotPos)
				if selectedSlotPos == 1 and item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_ATTR and attachedItemCount > 1:
					self.enchant_item_index = attachedItemIndex
					self.enchant_item_count = attachedItemCount
				
			mouseModule.mouseController.DeattachObject()

	def SelectEmptyReviveSlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():
		
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
			selectedItemVnum = player.GetItemIndex(attachedSlotPos)
			item.SelectItem(selectedItemVnum)
	
			if selectedSlotPos == 0 and item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_SEAL:
				socket = []
				for i in xrange(player.METIN_SOCKET_MAX_NUM):
					socket.append(player.GetItemMetinSocket(attachedSlotPos, i))
		
				if socket[1] > app.GetGlobalTimeStamp():
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_PET_HAS_LIFE_CANNOT_REVIVE)
					mouseModule.mouseController.DeattachObject()
					return
				
				if attachedItemCount == 1:
					attachedItemCount = 0

				if player.SLOT_TYPE_INVENTORY == attachedSlotType and not self.SlotReviveIndex.has_key(selectedSlotPos):
					self.SlotReviveIndex[selectedSlotPos] = [attachedSlotPos, selectedItemVnum]
					self.SlotReviveIndex[selectedSlotPos+1] = [attachedSlotPos, selectedItemVnum]
					self.GetChild("Revive_Pet_Slots").SetItemSlot(selectedSlotPos, attachedItemIndex, attachedItemCount)
					self.GetChild("Revive_Pet_Slots").SetItemSlot(1, attachedItemIndex, attachedItemCount)
					snd.PlaySound("sound/ui/drop.wav")
					self.GetChild("AgeTitle01").Show()
					self.GetChild("AgeValue01").Show()
					self.GetChild("AgeTitle02").Show()
					self.GetChild("AgeValue02").Show()

					self.ReviveSealAgeTime =  socket[4] - socket[2]
					
					self.GetChild("AgeValue01").SetText(localeInfo.SecondToPetReviveLifeTime(self.ReviveSealAgeTime))
					self.GetChild("AgeValue02").SetText(localeInfo.SecondToPetReviveLifeTime(0))
					
					self.reviveDestItemCount = localeInfo.SecondToPetReviveLifeTime(self.ReviveSealAgeTime, "VALUE")
					if self.reviveDestItemCount == 0:
						self.reviveDestItemCount = 1
					self.GetChild("ReviveMaterialTitle").SetText("%d/%d" % (self.reviveCurItemCount, self.reviveDestItemCount))
					
					if app.WJ_ENABLE_TRADABLE_ICON and self.interface:
						self.CantTradableItem("REVIVE", selectedSlotPos, attachedSlotPos)
						self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PAGE_REVIVE_2)
						self.interface.RefreshMarkInventoryBag()
			elif selectedSlotPos >= 10 and item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_REVIVE and selectedItemVnum == 55100:
				if not self.SlotReviveIndex.has_key(0):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_SELECT_EMPTY_SEAL)
					mouseModule.mouseController.DeattachObject()
					return
				
				if self.reviveCurItemCount >= self.reviveDestItemCount:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NO_NEED_MORE_ITEMS)
					mouseModule.mouseController.DeattachObject()
					return
					
				if player.SLOT_TYPE_INVENTORY == attachedSlotType and not self.SlotReviveIndex.has_key(selectedSlotPos):
					for key in self.SlotReviveIndex:
						if self.SlotReviveIndex[key][0] == attachedSlotPos:
							return
					if attachedItemCount == 0:
						attachedItemCount = 1
					if self.reviveCurItemCount + attachedItemCount > self.reviveDestItemCount:
						attachedItemCount = self.reviveDestItemCount - self.reviveCurItemCount
					self.GetChild("Revive_Pet_Slots").SetItemSlot(selectedSlotPos, attachedItemIndex, attachedItemCount)
					self.SlotReviveIndex[selectedSlotPos] = [attachedSlotPos, selectedItemVnum, attachedItemCount]
					snd.PlaySound("sound/ui/drop.wav")
					self.reviveCurItemCount += attachedItemCount
					self.GetChild("ReviveMaterialTitle").SetText("%d/%d" % (self.reviveCurItemCount, self.reviveDestItemCount))
					self.GetChild("AgeValue02").SetText(localeInfo.SecondToPetReviveLifeTime(60 * 60 * 24 * self.reviveCurItemCount))
					if app.WJ_ENABLE_TRADABLE_ICON:
						self.CantTradableItem("REVIVE", selectedSlotPos, attachedSlotPos)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_CANNOT_SET_ITEM_HERE)
				
			mouseModule.mouseController.DeattachObject()
	
	def SelectItemSlot(self, itemSlotIndex):
		snd.PlaySound("sound/ui/pickup_item_in_inventory.wav")
		if self.state == "ATTR":
			if itemSlotIndex == 1:
				self.enchant_item_count = 0
			if itemSlotIndex == 2:
				self.GetChild("PetDetermineInfoText").Hide()
			self.SlotAttrPagePos[itemSlotIndex] = -1
			self.GetChild("Change_Pet_Slot").ClearSlot(itemSlotIndex)
			self.GetChild("Change_Pet_Slot").RefreshSlot()
			if app.WJ_ENABLE_TRADABLE_ICON:
				if itemSlotIndex != 2:
					(itemInvenPage, itemSlotPos) = self.AttrLockItems[itemSlotIndex]
					if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
						self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
					self.AttrLockItems[itemSlotIndex] = (-1, -1)
	
		if self.state == "REVIVE":
			if itemSlotIndex == 0:
				self.GetChild("Revive_Pet_Slots").ClearSlot(0)
				self.GetChild("Revive_Pet_Slots").ClearSlot(1)
				for i in xrange(10, 20):
					self.GetChild("Revive_Pet_Slots").ClearSlot(i)
				self.GetChild("Revive_Pet_Slots").RefreshSlot()
				self.GetChild("AgeTitle01").Hide()
				self.GetChild("AgeValue01").Hide()
				self.GetChild("AgeTitle02").Hide()
				self.GetChild("AgeValue02").Hide()
				self.reviveDestItemCount = 0
				self.reviveCurItemCount = 0
				self.GetChild("ReviveMaterialTitle").SetText("%d/%d" % (self.reviveCurItemCount, self.reviveDestItemCount))
				self.SlotReviveIndex = {}
				if app.WJ_ENABLE_TRADABLE_ICON and self.interface:
					self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PAGE_REVIVE_1)
					self.interface.RefreshMarkInventoryBag()
			elif itemSlotIndex >= 10:
				self.reviveCurItemCount -= self.SlotReviveIndex[itemSlotIndex][2]
				self.GetChild("AgeValue02").SetText(localeInfo.SecondToPetReviveLifeTime(60 * 60 * 24 * self.reviveCurItemCount))
				self.GetChild("ReviveMaterialTitle").SetText("%d/%d" % (self.reviveCurItemCount, self.reviveDestItemCount))
				del self.SlotReviveIndex[itemSlotIndex]
				self.GetChild("Revive_Pet_Slots").ClearSlot(itemSlotIndex)
				self.GetChild("Revive_Pet_Slots").RefreshSlot()

			if app.WJ_ENABLE_TRADABLE_ICON:
				(itemInvenPage, itemSlotPos) = self.ReviveLockItems[itemSlotIndex]
				if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
				self.ReviveLockItems[itemSlotIndex] = (-1, -1)

	def SendPetRevive(self):
		for key in self.SlotReviveIndex:
			if key >= 10 and self.SlotReviveIndex[key][0] != -1:
				net.SendChatPacket("/petfeedadditem %d %d %d" % (3, key, self.SlotReviveIndex[key][0]))
		net.SendChatPacket("/pet_revive_special %d" % (self.SlotReviveIndex[0][0]))
		self.ResetRevivePage()

	def SelectSkillSlotWithItem(self, slot):
		if not mouseModule.mouseController.isAttached():
			return
				
		attachedSlotType	= mouseModule.mouseController.GetAttachedType()			
		attachedSlotPos		= mouseModule.mouseController.GetAttachedSlotNumber()
		attachedItemCount	= mouseModule.mouseController.GetAttachedItemCount()
		attachedItemVnum	= player.GetItemIndex(attachedSlotPos)
		item.SelectItem(attachedItemVnum)		
		
		itemType	= item.GetItemType()
		itemSubType = item.GetItemSubType()			
		
		self.RemoveSkillItemInfo  = [ slot, attachedItemVnum, attachedSlotPos]

		if attachedItemVnum == 55029:
			self.questionSkillDelDlg = uiCommon.QuestionDialog2()
			self.questionSkillDelDlg.SetAcceptEvent(lambda flag=TRUE: self.SendRemoveSkillPacket(flag))
			self.questionSkillDelDlg.SetCancelEvent(lambda flag=FALSE: self.SendRemoveSkillPacket(flag))
				
			self.questionSkillDelDlg.SetText1(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG2)
			self.questionSkillDelDlg.SetText2(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG_2)
	
		if attachedItemVnum == 55030:
			self.questionSkillDelDlg = uiCommon.QuestionDialog2()
			self.questionSkillDelDlg.SetAcceptEvent(lambda flag=TRUE: self.SendRemoveSkillPacket(flag))
			self.questionSkillDelDlg.SetCancelEvent(lambda flag=FALSE: self.SendRemoveSkillPacket(flag))
				
				
			self.questionSkillDelDlg.SetText1(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG1)
			self.questionSkillDelDlg.SetText2(localeInfo.PET_SKILL_DELETE_QUESTION_DLG_MSG_2)
			
		mouseModule.mouseController.DeattachObject()
		self.questionSkillDelDlg.SetTop()
		self.questionSkillDelDlg.Open()
			
	def SendRemoveSkillPacket(self, flag):
		if flag == TRUE:
			net.SendChatPacket("/petremoveskill %d %d %d" % (self.RemoveSkillItemInfo[0], self.RemoveSkillItemInfo[1], self.RemoveSkillItemInfo[2]))
			
		self.RemoveSkillItemInfo  = [ 0, 0, 0]
		self.questionSkillDelDlg.Close()
		
	def SetSkillSlotCoolTime(self, slot, time):
		self.PetSkillSlots.SetSlotCoolTime(int(slot), int(time))
	
	def OverInSealSlot(self, overSlotPos):
		if None != self.tooltipItem:
			self.ShowToolTip(self.SealSlot)

	def OverOutSealSlot(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()
				
	def OverOutItem(self):
		self.GetChild("Change_Pet_Slot").SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		if None != self.tooltipItem:
			if self.state == "ATTR":
				slot_pos = self.SlotAttrPagePos[overSlotPos]
				self.ShowToolTip(slot_pos)
			if self.state == "REVIVE":
				slot_pos = self.SlotReviveIndex[overSlotPos][0]
				self.ShowToolTip(slot_pos)

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)
	
	def PetSkillTooltipShow(self, slot):
		if self.PetSkillList[slot][0] > 0:
			token_desc = self.linesPetDesc[self.PetSkillList[slot][0]-1][:-1].split("\t")
			token_skill_0 = self.linesPetSkills[self.PetSkillList[slot][0]-1][:-1].split("\t")
			token_skill_cooldown_10 = self.linesPetSkills[MAX_SKILL_NUM-1+0][:-1].split("\t")
			token_skill_cooldown_17 = self.linesPetSkills[MAX_SKILL_NUM-1+1][:-1].split("\t")
			token_skill_cooldown_23 = self.linesPetSkills[MAX_SKILL_NUM-1+3][:-1].split("\t")
			self.SkillTooltip.ClearToolTip()
			self.SkillTooltip.AutoAppendTextLine(token_desc[1], self.TITLE_COLOR)
			self.SkillTooltip.AppendDescription(token_desc[4], 20, self.CONDITION_COLOR)
			self.SkillTooltip.AutoAppendTextLine("__________________", self.TITLE_COLOR)
			self.SkillTooltip.AppendSpace(5)
			
			self.SkillTooltip.AutoAppendTextLine(localeInfo.TOOLTIP_SKILL_LEVEL % (int(self.PetSkillList[slot][1])))
			
			for i in xrange(1, 3):
				desc_color = [self.NORMAL_COLOR, self.NEGATIVE_COLOR]
				if i == 2 and self.PetSkillList[slot][1] < 20:
					self.SkillTooltip.AutoAppendTextLine("__________________", self.TITLE_COLOR)
					self.SkillTooltip.AppendSpace(5)
					self.SkillTooltip.AutoAppendTextLine(localeInfo.TOOLTIP_NEXT_SKILL_LEVEL_1 % (int(self.PetSkillList[slot][1]+i), 20), desc_color[i-1])
				
				if i == 1 or i == 2 and self.PetSkillList[slot][1] < 20:
					if self.PetSkillList[slot][0] != 10 and self.PetSkillList[slot][0] != 17 and self.PetSkillList[slot][0] != 18:
						self.SkillTooltip.AutoAppendTextLine(GetAffectString(int(token_skill_0[1]), int(token_skill_0[self.PetSkillList[slot][1]+i])), desc_color[i-1])
						self.SkillTooltip.AppendSpace(5)

					elif self.PetSkillList[slot][0] == 10:
						self.SkillTooltip.AutoAppendTextLine(uiScriptLocale.PET_TOOLTIP_RESTORE_HP + str(token_skill_0[self.PetSkillList[slot][1]+i]), desc_color[i-1])
						self.SkillTooltip.AutoAppendTextLine(uiScriptLocale.PET_TOOLTIP_CHANCE_RESTORE_HP  + str(token_skill_cooldown_10[self.PetSkillList[slot][1]+i]) + "%", desc_color[i-1])
						self.SkillTooltip.AutoAppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + token_desc[5], desc_color[i-1])
					elif self.PetSkillList[slot][0] == 17:
						self.SkillTooltip.AutoAppendTextLine(uiScriptLocale.PET_TOOLTIP_IMMORTALITY_TIME  + checkdiv(int(token_skill_0[self.PetSkillList[slot][1]+i])) + localeInfo.SECOND, desc_color[i-1])
						self.SkillTooltip.AutoAppendTextLine(uiScriptLocale.PET_TOOLTIP_CHANCE_IMMORTALITY  + str(token_skill_cooldown_17[self.PetSkillList[slot][1]+i]) + "%", desc_color[i-1])
						self.SkillTooltip.AutoAppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + token_desc[5], desc_color[i-1])
					elif self.PetSkillList[slot][0] == 18:
						self.SkillTooltip.AutoAppendTextLine(uiScriptLocale.PET_TOOLTIP_CHANCE_PANACEA  + str(token_skill_0[self.PetSkillList[slot][1]+i]) + "%", desc_color[i-1])
						self.SkillTooltip.AutoAppendTextLine(localeInfo.TOOLTIP_SKILL_COOL_TIME + token_desc[5], desc_color[i-1])
			
			self.SkillTooltip.AlignHorizonalCenter()
			self.SkillTooltip.ShowToolTip()
		
	def PetSkillTooltipHide(self):
		self.SkillTooltip.HideToolTip()

	def SendPetAskPettype(self):
		net.SendChatPacket("/petgettype")
			
	def AnswerPetType(self, type):
		self.popup = uiCommon.PetTypePopupDialog()
		self.popup.SetWidth(350)
		self.popup.SetText(self.newAttrInfoText[int(type)], True)
		self.popup.Open()
	
	def SendPetRenewAttr(self):
		if self.SlotAttrPagePos[0] == -1 or self.SlotAttrPagePos[1] == -1:
			return
		else:
			net.SendChatPacket("/petchangeattr %d %d" % (self.SlotAttrPagePos[0], self.SlotAttrPagePos[1]))
			
			self.SlotAttrPagePos[2] = self.SlotAttrPagePos[0]
			
			self.SlotAttrPagePos[0] = -1
			self.GetChild("Change_Pet_Slot").ClearSlot(0)
			
			if self.enchant_item_count > 0:
				self.enchant_item_count -= 1
				if self.enchant_item_count == 0:
					self.attachedItemIndex = -1
					self.SlotAttrPagePos[1] = -1
					self.GetChild("Change_Pet_Slot").ClearSlot(1)
				else:
					self.GetChild("Change_Pet_Slot").SetItemSlot(1, self.enchant_item_index, self.enchant_item_count)
			else:
				self.attachedItemIndex = -1
				self.SlotAttrPagePos[1] = -1
				self.GetChild("Change_Pet_Slot").ClearSlot(1)

			self.GetChild("Change_Pet_Slot").RefreshSlot()
			
	def RecivePetRenewAttr(self, type):
		self.GetChild("Change_Pet_Slot").SetItemSlot(2, self.Slot0Index, 0)
		if app.WJ_ENABLE_TRADABLE_ICON:
			(itemInvenPage, itemSlotPos) = self.AttrLockItems[0]
			if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
				self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
			self.AttrLockItems[0] = (-1, -1)
		if int(type) == 5 or int(type) == 6:
			self.TEXT_COLOR = grp.GenerateColor(1.0, 0.845, 0.0, 1.0)
			self.OUTLINE = False
		elif int(type) ==7 or int(type) == 8:
			self.TEXT_COLOR = grp.GenerateColor(1.0, 0.45, 1.0, 1.0)
			self.OUTLINE = True
		else:
			self.TEXT_COLOR = grp.GenerateColor(1.0, 1.0, 1.0, 1.0)
			self.OUTLINE = False
			
		self.GetChild("PetDetermineInfoText").Show()
		self.GetChild("PetDetermineInfoText").SetOutline(self.OUTLINE)
		self.GetChild("PetDetermineInfoText").SetPackedFontColor(self.TEXT_COLOR)
		self.GetChild("PetDetermineInfoText").SetEnterToken(True)
		self.GetChild("PetDetermineInfoText").SetText(self.newAttrInfoText[int(type)])
		
	def SetEvoInfoItems(self, vnum, count):
		self.PetEvoItemList.append([vnum, count])
	
	def OpenEvoInfo(self):
		if self.wndEvoInfo:
			self.wndEvoInfo.Close()
			self.wndEvoInfo = None
		self.wndEvoInfo = PetEvolutionWindow(self, self.PetEvoItemList)
		self.PetEvoItemList = []
	
	def BindInterface(self, interface):
		self.interface = interface
			
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def __OnClickTabButton(self, stateKey):
		self.SetTabState(stateKey)
	
	def ResetAttrPage(self):
		for i in xrange(0, 3):
			self.SlotAttrPagePos[i] = -1
			self.GetChild("Change_Pet_Slot").ClearSlot(i)
		self.GetChild("Change_Pet_Slot").RefreshSlot()
		self.GetChild("PetDetermineInfoText").Hide()
		if app.WJ_ENABLE_TRADABLE_ICON and self.interface:
			for key in self.AttrLockItems:
				(itemInvenPage, itemSlotPos) = self.AttrLockItems[key]
				if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
				self.AttrLockItems[key] = (-1, -1)
			self.AttrLockItems = {i:(-1,-1) for i in range(2)}
		
	def ResetRevivePage(self):
		self.GetChild("Revive_Pet_Slots").ClearSlot(0)
		self.GetChild("Revive_Pet_Slots").ClearSlot(1)
		for i in xrange(10, 20):
			self.GetChild("Revive_Pet_Slots").ClearSlot(i)
		self.GetChild("Revive_Pet_Slots").RefreshSlot()
		self.SlotReviveIndex = {}
		self.reviveDestItemCount = 0
		self.reviveCurItemCount = 0
		self.GetChild("ReviveMaterialTitle").SetText("%d/%d" % (self.reviveCurItemCount, self.reviveDestItemCount))
		self.GetChild("AgeTitle01").Hide()
		self.GetChild("AgeValue01").Hide()
		self.GetChild("AgeTitle02").Hide()
		self.GetChild("AgeValue02").Hide()
			
		if app.WJ_ENABLE_TRADABLE_ICON and self.interface:
			for key in self.ReviveLockItems:
				(itemInvenPage, itemSlotPos) = self.ReviveLockItems[key]
				if itemInvenPage == self.wndInventory.GetInventoryPageIndex():
					self.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
				self.ReviveLockItems[key] = (-1, -1)
			self.ReviveLockItems = {i:(-1,-1) for i in range(20)}
			self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PAGE_REVIVE_1)
			self.interface.RefreshMarkInventoryBag()

	def SetTabState(self, stateKey):
		if self.state == "ATTR":
			self.ResetAttrPage()
		elif self.state == "REVIVE":
			self.ResetRevivePage()
		self.state = stateKey

		for (tabKey, tabButton) in self.tabButtonDict.items():
			if stateKey!=tabKey:
				tabButton.SetUp()

		for tabValue in self.tabDict.itervalues():
			tabValue.Hide()

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()
			
		self.tabDict[stateKey].Show()
		self.pageDict[stateKey].Show()
		self.ResetAttrPage()

		if self.wndFeed:
			self.wndFeed.Close()
			self.wndFeed = None
		
		if app.WJ_ENABLE_TRADABLE_ICON and self.interface:
			if self.state == "MAIN":
				self.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
			if self.state == "ATTR":
				self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PAGE_ATTR)
			elif self.state == "REVIVE":
				self.interface.SetOnTopWindow(player.ON_TOP_WND_PET_PAGE_REVIVE_1)
	
				event.ClearEventSet(self.descReviveIndex)
				self.descReviveIndex = event.RegisterEventSet(uiScriptLocale.PET_PREMIUM_FEED_DESC)
				event.SetFontColor(self.descReviveIndex, 0.7843, 0.7843, 0.7843, 1.0)
				event.SetVisibleLineCount(self.descReviveIndex, REVIVE_DESC_SHOW_LINE_COUNT_MAX)
				event.SetRestrictedCount(self.descReviveIndex, 80)
				event.Skip(self.descReviveIndex)
		
			self.interface.RefreshMarkInventoryBag()
					
	def GetTabState(self):
		return self.state
	
	def IsPetFeedShow(self):
		if self.wndFeed:
			return self.wndFeed.IsShow()
		return False

	def PetFeedAutoSetItem(self, slotIndex):
		if self.wndFeed:
			self.wndFeed.AutoSetItem(slotIndex)
		
	def PrevDescriptionPage(self):
		if True == event.IsWait(self.descReviveIndex):
			if event.GetVisibleStartLine(self.descReviveIndex)-REVIVE_DESC_SHOW_LINE_COUNT_MAX >= 0:
				event.SetVisibleStartLine(self.descReviveIndex, event.GetVisibleStartLine(self.descReviveIndex)-REVIVE_DESC_SHOW_LINE_COUNT_MAX)
				event.Skip(self.descReviveIndex)
		else:
			event.Skip(self.descReviveIndex)

	def NextDescriptionPage(self):
		if True == event.IsWait(self.descReviveIndex):
			event.SetVisibleStartLine(self.descReviveIndex, event.GetVisibleStartLine(self.descReviveIndex)+REVIVE_DESC_SHOW_LINE_COUNT_MAX)
			event.Skip(self.descReviveIndex)
		else:
			event.Skip(self.descReviveIndex)
			
	def OnUpdate(self):
		if self.state == "MAIN":
			if TRUE == self.expwind.IsIn():
				for i in range(0,4):				
					self.tooltipexp[i].Show()
			else:
				for i in range(0,4):				
					self.tooltipexp[i].Hide()
		if self.state == "REVIVE":
			(xposEventSet, yposEventSet) = self.GetChild("ReviveDescBox").GetGlobalPosition()
			event.UpdateEventSet(self.descReviveIndex, xposEventSet, -(yposEventSet))
			self.descReviveBox.SetIndex(self.descReviveIndex)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, window, destSlotIndex, srcSlotIndex):
			itemInvenPage = srcSlotIndex / player.INVENTORY_PAGE_SIZE
			localSlotPos = srcSlotIndex - (itemInvenPage * player.INVENTORY_PAGE_SIZE)
			if window == "ATTR":
				self.AttrLockItems[destSlotIndex] = (itemInvenPage, localSlotPos)
				if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
					self.wndInventory.wndItem.SetCantMouseEventSlot(localSlotPos)
			if window == "REVIVE":
				self.ReviveLockItems[destSlotIndex] = (itemInvenPage, localSlotPos)
				if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
					self.wndInventory.wndItem.SetCantMouseEventSlot(localSlotPos)
			
		def RefreshLockedSlot(self):
			if self.wndInventory.wndItem:
				if self.state == "MAIN":
					if self.wndFeed:
						self.wndFeed.RefreshLockedSlot()
				if self.state == "ATTR":
					for privatePos, (itemInvenPage, itemSlotPos) in self.AttrLockItems.items():
						if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
							self.wndInventory.wndItem.SetCantMouseEventSlot(itemSlotPos)
				if self.state == "REVIVE":
					for privatePos, (itemInvenPage, itemSlotPos) in self.ReviveLockItems.items():
						if self.wndInventory.GetInventoryPageIndex() == itemInvenPage:
							self.wndInventory.wndItem.SetCantMouseEventSlot(itemSlotPos)
				
		def BindInterface(self, interface):
			self.interface = interface

		def SetInven(self, wndInventory):
			from _weakref import proxy
			self.wndInventory = proxy(wndInventory)
		
		def GetInterface(self):
			return self.interface
			
		def GetInventory(self):
			return self.wndInventory

class PetSystemMini(ui.ScriptWindow):
	class TextToolTip(ui.Window):
		def __init__(self, y):
			ui.Window.__init__(self, "TOP_MOST")

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignLeft()
			textLine.SetOutline()
			textLine.Show()
			self.y = y
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX, mouseY - 60 + self.y)

	def __init__(self, wndMain):
		ui.ScriptWindow.__init__(self)
		self.wndMain = wndMain
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
	
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/PetMiniInformationWindow.py")
		except:
			import exception
			exception.Abort("PetMiniInformationWindow.LoadWindow.LoadObject")
			
		try:
			self.tooltipexp = []
			for i in range(2):
				self.tooltipexp.append(self.TextToolTip(15*i))
				self.tooltipexp[i].Hide()
				
			self.pet_icon_slot_ani_img = self.GetChild("pet_icon_slot_ani_img")
			self.PetEXPBubbles = [
				self.GetChild("pet_mini_EXPGauge_01"),
				self.GetChild("pet_mini_EXPGauge_02"),
				self.GetChild("pet_mini_EXPGauge_03"),
				self.GetChild("pet_mini_EXPGauge_04"),
				self.GetChild("pet_mini_EXPGauge_05"),
			]
			self.UpdateInfo()
			
		except:
			import exception
			exception.Abort("PetMiniInformationWindow.LoadWindow.BindObject")

	def UpdateInfo(self):
		self.GetChild("pet_icon_slot").SetItemSlot(0, self.wndMain.SealVnum, 0)
		self.GetChild("pet_icon_slot").SetOverInItemEvent(ui.__mem_func__(self.OverInSealSlot))
		self.GetChild("pet_icon_slot").SetOverOutItemEvent(ui.__mem_func__(self.OverOutSealSlot))
		self.GetChild("pet_icon_slot").SetSelectItemSlotEvent(ui.__mem_func__(self.OpenMainWindow))
		
		duration = self.wndMain.SealDuration - app.GetGlobalTimeStamp()	
		if self.wndMain.SealDuration > 0:
			self.GetChild("LifeGauge").SetPercentage(duration, self.wndMain.BornDuration)
			self.GetChild("LifeGauge").Show()
		else:
			self.GetChild("LifeGauge").Hide()

		for slot in xrange(3):
			if self.wndMain.PetSkillList[slot][0] != -1:
				self.GetChild("mini_skill_slot0").ClearSlot(slot)
				self.GetChild("mini_skill_slot0").SetPetSkillSlot(slot, self.wndMain.PetSkillList[slot][0], self.wndMain.PetSkillList[slot][1], 0.5, 0.5)
				self.GetChild("mini_skill_slot0").SetCoverButton(int(slot), "d:/ymir work/ui/pet/mini_window/pet_slot_corvermini.sub", "d:/ymir work/ui/pet/mini_window/pet_slot_corvermini.sub", "d:/ymir work/ui/pet/mini_window/pet_slot_corvermini.sub" , "d:/ymir work/ui/pet/mini_window/pet_slot_corvermini.sub")
				self.GetChild("mini_skill_slot0").SetAlwaysRenderCoverButton(int(slot), TRUE)

		expm = self.wndMain.PetExp
		expi = self.wndMain.PetExpItem
		destexpmob = self.wndMain.PetLevelExpMob
		destexpitem = self.wndMain.PetLevelExpItem
		
		if destexpmob > 0:
			totexpm = destexpmob
			totexpi = destexpitem
			expi = min(expi, totexpi)
			expmp =  float(expm) / totexpm * 100
			expip =  float(expi) / totexpi * 100
		else:
			(totexpm, totexpi, expmp, expip) = (0, 0, 0, 0)

		curPoint = int(min(expm, totexpm))
		curPoint = int(max(expm, 0))
		maxPoint = int(max(totexpm, 0))
		
		curPointi = int(min(expi, totexpi))
		curPointi = int(max(expi, 0))
		maxPointi = int(max(totexpi, 0))

		quarterPoint = maxPoint / 4
		quarterPointi = maxPointi 
		FullCount = 0
		FullCounti = 0

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)
			
		if 0 != quarterPointi:
			FullCounti = min(1, curPointi / quarterPointi)

		for i in xrange(4):
			self.PetEXPBubbles[i].Hide()
			
		self.PetEXPBubbles[4].Hide()

		for i in xrange(FullCount):
			self.PetEXPBubbles[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.PetEXPBubbles[i].Show()
			
		for i in xrange(FullCounti):
			self.PetEXPBubbles[4].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.PetEXPBubbles[4].Show()

		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.PetEXPBubbles[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.PetEXPBubbles[FullCount].Show()
				
		if 0 != quarterPointi:
			if FullCounti < 1:
				Percentage = float(curPointi % quarterPointi) / quarterPointi - 1.0
				self.PetEXPBubbles[4].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.PetEXPBubbles[4].Show()
			
		
		#####
		self.tooltipexp[0].SetText(uiScriptLocale.PET_INFORMATION_EXP +": "+localeInfo.NumberWithPoint(str(expm))+" / "+localeInfo.NumberWithPoint(str(totexpm))+" (%.2f%%)" % expmp)
		self.tooltipexp[1].SetText(uiScriptLocale.PET_INFORMATION_EXP_BUTTON +": "+localeInfo.NumberWithPoint(str(expi))+" / "+localeInfo.NumberWithPoint(str(totexpi))+" (%.2f%%)" % expip)
	
	def OpenMainWindow(self):
		if self.wndMain.IsShow():
			self.wndMain.SetTop()
		else:
			self.wndMain.Show()
			self.wndMain.SetTop()
	
	def OverInSealSlot(self, overSlotPos):
		if None != self.wndMain.tooltipItem:
			self.ShowToolTip(self.wndMain.SealSlot)

	def OverOutSealSlot(self):
		if None != self.wndMain.tooltipItem:
			self.wndMain.tooltipItem.HideToolTip()
			
	def ShowToolTip(self, slotIndex):
		if None != self.wndMain.tooltipItem:
			self.wndMain.tooltipItem.SetInventoryItem(slotIndex)
			
	def OnUpdate(self):
		if TRUE == self.GetChild("pet_mini_info_exp_gauge_board").IsIn():
			for i in range(2):				
				self.tooltipexp[i].Show()
		else:
			for i in range(2):				
				self.tooltipexp[i].Hide()

class PetSystemIncubator(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, type, pet, cost):
		self.LoadPetIncubatorImg(type, pet, cost)
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
	
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/PetHatchingWindow.py")
		except:
			import exception
			exception.Abort("PetHatchingWindow.LoadWindow.LoadObject")
			
		try:
			self.board = self.GetChild("board")
			self.boardtitle = self.GetChild("PetHatching_TitleBar")
			self.boardtitlename = self.GetChild("TitleName")
			
			self.petimg = self.GetChild("HatchingItemSlot")
			self.costText = self.GetChild("HatchingMoney")
			self.HatchingButton = self.GetChild("HatchingButton")
			
			self.boardtitlename.SetText(uiScriptLocale.PET_HATCHING_WINDOW_TITLE)
			self.GetChild("pet_name").SetText("")
			self.GetChild("pet_name").SetFocus()

			self.boardtitle.SetCloseEvent(ui.__mem_func__(self.Close))
			self.HatchingButton.SetEvent(ui.__mem_func__(self.RequestHatching))
			
		except:
			import exception
			exception.Abort("PetHatchingWindow.LoadWindow.BindObject")
			
	def LoadPetIncubatorImg(self, type, pet, cost):
		self.petimg.SetItemSlot(0, (int(pet)), 0)
		self.petimg.SetAlwaysRenderCoverButton(0, TRUE)
		self.costText.SetText(uiScriptLocale.GUILD_BUILDING_PRICE+": "+str(localeInfo.NumberToMoneyString(int(cost))))
		
	def RequestHatching(self):
		if self.GetChild("pet_name").GetText() == "" or len(self.GetChild("pet_name").GetText()) < 4:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NAME_IS_FALSE)
			return
			
		net.RequestBornPet(self.GetChild("pet_name").GetText())
		self.Close()

class PetSystemRenewName(ui.ScriptWindow):
	def __init__(self, petItemVnum, petitem, scrollitem):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()
		self.petItem = petitem
		self.scrollItem = scrollitem
		self.LoadPetIncubatorImg(petItemVnum)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
		self.petItem = -1
		self.scrollItem = -1
	
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/PetHatchingWindow.py")
		except:
			import exception
			exception.Abort("PetHatchingWindow.LoadWindow.LoadObject")
			
		try:
			self.board = self.GetChild("board")
			self.boardtitle = self.GetChild("PetHatching_TitleBar")
			self.boardtitlename = self.GetChild("TitleName")
			
			self.petimg = self.GetChild("HatchingItemSlot")
			self.costText = self.GetChild("HatchingMoney")
			self.petname = self.GetChild("pet_name")
			self.HatchingButton = self.GetChild("HatchingButton")
			
			self.boardtitlename.SetText(uiScriptLocale.PET_NAME_CHANGE_WINDOW_TITLE)
			
			self.petname.SetText("")
			self.petname.SetFocus()
			
			#Event
			self.boardtitle.SetCloseEvent(ui.__mem_func__(self.Close))
			self.HatchingButton.SetEvent(ui.__mem_func__(self.RequestHatching))
			
			
		except:
			import exception
			exception.Abort("PetHatchingWindow.LoadWindow.BindObject")
			
	def LoadPetIncubatorImg(self, petItemVnum):
		self.petimg.SetItemSlot(0, (int(petItemVnum)), 0)
		self.petimg.SetAlwaysRenderCoverButton(0, TRUE)
		
	def RequestHatching(self):
		if self.petname.GetText() == "" or len(self.petname.GetText()) < 4:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NAME_IS_FALSE)
			return

		net.RequestRenewPetName(self.petname.GetText(), self.petItem, self.scrollItem)
		self.Close()

class PetEvolutionWindow(ui.BoardWithTitleBar):
	
	WINDOW_HEIGHT = 147
	SLOT_X_WIDTH = 46
	BOARD_TITLE = "Pet-Evolution"
	SLOT_IMG = "d:/ymir work/ui/pet/evo_item_slot.tga"

	def __init__(self, wndMain, itemTable):
		ui.BoardWithTitleBar.__init__(self)
		self.wndMain = wndMain
		self.PetEvoItemList = itemTable
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		
		self.__CreateWindow()

		ui.BoardWithTitleBar.Show(self)

	def __del__(self):
		self.wndMain = None
		ui.BoardWithTitleBar.__del__(self)

	def Show(self):
		ui.BoardWithTitleBar.Show(self)
		
	def Close(self):
		self.Hide()
		del self
				
	def Destroy(self):
		self.Hide()
	
	def __CreateWindow(self):
		try:
			self.AddFlag("attach")
			self.AddFlag("movable")
			self.AddFlag('float')
			self.SetTitleName(self.BOARD_TITLE)
			self.SetCloseEvent(self.Close)

			self.HasAllNeedCount = True
			self.SlotList = []

			for slot in xrange(len(self.PetEvoItemList)):
				self.SlotImage = ui.ImageBox()
				self.SlotImage.SetParent(self)
				self.SlotImage.LoadImage(self.SLOT_IMG)
				self.SlotImage.SetPosition(15 + self.SLOT_X_WIDTH * slot, 35)
				self.SlotImage.Show()
				
				item.SelectItem(self.PetEvoItemList[slot][0])
				self.ItemImage = ui.ImageBox()
				self.ItemImage.SetParent(self.SlotImage)
				self.ItemImage.SetWindowHorizontalAlignCenter()
				self.ItemImage.LoadImage(item.GetIconImageFileName())
				self.ItemImage.SetOnMouseOverInEvent(ui.__mem_func__(self.MouseOverIn), slot)
				self.ItemImage.SetOnMouseOverOutEvent(ui.__mem_func__(self.MouseOverOut))
				self.ItemImage.SetPosition(0, 7)
				self.ItemImage.Show()
				
				needCount = self.PetEvoItemList[slot][1]
				hasCount = player.GetItemCountByVnum(self.PetEvoItemList[slot][0])
				TEXT_COLOR = NEGATIVE_COLOR
				if hasCount >= needCount:
					TEXT_COLOR = POSITIVE_COLOR
				else:
					self.HasAllNeedCount = False

				self.CountInfo = ui.TextLine()
				self.CountInfo.SetParent(self.SlotImage)
				self.CountInfo.SetWindowHorizontalAlignCenter()
				self.CountInfo.SetHorizontalAlignCenter()
				self.CountInfo.SetText("%d / %d" % (hasCount, needCount))
				self.CountInfo.SetPackedFontColor(TEXT_COLOR)
				self.CountInfo.SetPosition(-1, 50)
				self.CountInfo.Show()
				
				self.SlotList.append([self.SlotImage, self.ItemImage, self.CountInfo])

			self.SetSize(15 + self.SLOT_X_WIDTH * len(self.PetEvoItemList) + 15, self.WINDOW_HEIGHT)
			self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2, wndMgr.GetScreenHeight()/2 - self.GetHeight()/2)
			self.Show()
			
			self.EvoButton = ui.Button()
			self.EvoButton.SetParent(self)
			self.EvoButton.SetUpVisual("d:/ymir work/ui/pet/feed_button/feed_button_default.sub")
			self.EvoButton.SetOverVisual("d:/ymir work/ui/pet/feed_button/feed_button_over.sub")
			self.EvoButton.SetDownVisual("d:/ymir work/ui/pet/feed_button/feed_button_down.sub")
			self.EvoButton.SetDisableVisual("d:/ymir work/ui/pet/feed_button/feed_button_down.sub")
			self.EvoButton.SetWindowVerticalAlignBottom()
			self.EvoButton.SetText(uiScriptLocale.OK)
			self.EvoButton.SetPosition((self.GetWidth() / 2) - (self.EvoButton.GetWidth() / 2) , 35)
			self.EvoButton.SetEvent(ui.__mem_func__(self.SendUpgradeEvolution))
			self.EvoButton.Show()
			if self.HasAllNeedCount == True:
				self.EvoButton.EnableFlash()
			else:
				self.EvoButton.Disable()

		except:
			import exception
			exception.Abort("HuntingMiniWindow.LoadWindow.BindObject")

	def SendUpgradeEvolution(self):
		if self.HasAllNeedCount == True:
			net.SendChatPacket("/petevolutionupgrade")
			self.Close()
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NOT_ENOUGHT_ITEMS_FOR_EVO)
				
	def MouseOverIn(self, slot):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.PetEvoItemList[slot][0], metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])
			self.tooltipItem.ShowToolTip()

	def MouseOverOut(self):
		if self.tooltipItem:
			self.tooltipItem.Hide()
	
	def OnUpdate(self):
		mHasAllNeedCount = self.HasAllNeedCount
		for slot in xrange(len(self.PetEvoItemList)):
			needCount = self.PetEvoItemList[slot][1]
			hasCount = player.GetItemCountByVnum(self.PetEvoItemList[slot][0])
			TEXT_COLOR = NEGATIVE_COLOR
			if hasCount >= needCount:
				TEXT_COLOR = POSITIVE_COLOR
				self.HasAllNeedCount = True
			else:
				self.HasAllNeedCount = False
			self.SlotList[slot][2].SetText("%d / %d" % (hasCount, needCount))
			self.SlotList[slot][2].SetPackedFontColor(TEXT_COLOR)


		if mHasAllNeedCount != self.HasAllNeedCount:
			self.UpdateEvoButton()
	
	def UpdateEvoButton(self):
		if self.HasAllNeedCount == True:
			self.EvoButton.Enable()
			self.EvoButton.EnableFlash()
		else:
			self.EvoButton.DisableFlash()
			self.EvoButton.Disable()
				
class PetFeedWindow(ui.BoardWithTitleBar):
	
	WINDOW_WIDTH = { 0 : 190, 2 : 190}
	WINDOW_HEIGHT = { 0 : 260, 2 : 268}
	BOARD_TITLE = { 0 : localeInfo.GROWTH_PET_TITLE_FEED_LIFE, 2 : localeInfo.GROWTH_PET_TITLE_FEED_ITEMEXP}
	INFO_BOARD_IMG = { 0 : "d:/ymir work/ui/pet/feed_info_hp.tga", 2 : "d:/ymir work/ui/pet/feed_info_exp.tga"}
	GRID_SLOT_START_X = { 0 : 85, 2 : 100}
	

	def __init__(self, wndMain, type):
		ui.BoardWithTitleBar.__init__(self)
		self.__CreateWindow(type)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.wndMain = wndMain
		self.FeedType = type
		
		self.FeedSlotIndex = {}
		self.isOpen = True
		constInfo.IS_SHOW_PET_FEED_ITEM = True
		
		self.isDurationPerc = (float(self.wndMain.SealDuration - app.GetGlobalTimeStamp()) / float(self.wndMain.BornDuration)) * 100.0
		self.infoLifeTime = [0, 0]
		self.infoSelectEXPValue = 0
		
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.wndMain.interface:
				self.lockedItems = {i:(-1,-1) for i in range(FEED_SLOT_NUM)}
				if type == 0:
					self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_PET_FEED_LIFE)
				if type == 2:
					self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_PET_FEED_EQUIP)
				self.wndMain.interface.RefreshMarkInventoryBag()

		ui.BoardWithTitleBar.Show(self)
		self.UpdateInfoText()

	def __del__(self):
		constInfo.IS_SHOW_PET_FEED_ITEM = False
		self.wndMain = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.wndMain.interface:
				self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.wndMain.interface.RefreshMarkInventoryBag()
		ui.BoardWithTitleBar.__del__(self)

	def IsOpen(self):
		return self.isOpen
	
	def Show(self):
		ui.BoardWithTitleBar.Show(self)
		
	def Close(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.wndMain.interface:
				for slot in xrange(FEED_SLOT_NUM):
					(itemInvenPage, itemSlotPos) = self.lockedItems[slot]
					if itemInvenPage == self.wndMain.wndInventory.GetInventoryPageIndex():
						self.wndMain.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
				self.lockedItems = {i:(-1,-1) for i in range(FEED_SLOT_NUM)}
				self.RefreshLockedSlot()
				self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.wndMain.interface.RefreshMarkInventoryBag()
		constInfo.IS_SHOW_PET_FEED_ITEM = False
		self.Hide()
		self.wndMain.DestroyFeedWindow(self.FeedType)
				
	def Destroy(self):
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.wndMain.interface:
				for slot in xrange(FEED_SLOT_NUM):
					(itemInvenPage, itemSlotPos) = self.lockedItems[slot]
					if itemInvenPage == self.wndMain.wndInventory.GetInventoryPageIndex():
						self.wndMain.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
				self.lockedItems = {i:(-1,-1) for i in range(FEED_SLOT_NUM)}
				self.RefreshLockedSlot()
				self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_NONE)
				self.wndMain.interface.RefreshMarkInventoryBag()
		constInfo.IS_SHOW_PET_FEED_ITEM = False
		self.Hide()
	
	def __CreateWindow(self, type):
		try:
			self.AddFlag("attach")
			self.AddFlag("movable")
			self.AddFlag('float')
			self.SetSize(self.WINDOW_WIDTH[type], self.WINDOW_HEIGHT[type])
			self.SetPosition(wndMgr.GetScreenWidth() - 500 + 328 -190, wndMgr.GetScreenHeight() - 595 + 240)
			self.SetTitleName(self.BOARD_TITLE[type])
			self.SetCloseEvent(self.Close)
			self.Show()
			
			self.InfoBoard = ui.ImageBox()
			self.InfoBoard.AddFlag("attach")
			self.InfoBoard.SetParent(self)
			self.InfoBoard.LoadImage(self.INFO_BOARD_IMG[type])
			self.InfoBoard.SetPosition(15, 35)
			self.InfoBoard.Show()
			
			if type == 0:
				self.infoHP = ui.TextLine()
				self.infoHP.SetParent(self.InfoBoard)
				self.infoHP.SetWindowHorizontalAlignCenter()
				self.infoHP.SetHorizontalAlignCenter()
				self.infoHP.SetPackedFontColor(GOLDEN_COLOR)
				self.infoHP.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
				self.infoHP.SetPosition(0, 25)
				self.infoHP.Show()
			if type == 2:
				self.infoDestEXP = ui.TextLine()
				self.infoDestEXP.SetParent(self.InfoBoard)
				self.infoDestEXP.SetWindowHorizontalAlignCenter()
				self.infoDestEXP.SetHorizontalAlignRight()
				self.infoDestEXP.SetPackedFontColor(GOLDEN_COLOR)
				self.infoDestEXP.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
				self.infoDestEXP.SetPosition(55, 24)
				self.infoDestEXP.Show()
				
				self.infoCurEXP = ui.TextLine()
				self.infoCurEXP.SetParent(self.InfoBoard)
				self.infoCurEXP.SetWindowHorizontalAlignCenter()
				self.infoCurEXP.SetHorizontalAlignRight()
				self.infoCurEXP.SetPackedFontColor(GOLDEN_COLOR)
				self.infoCurEXP.SetFontName(localeInfo.UI_DEF_FONT_LARGE)
				self.infoCurEXP.SetPosition(55, 40)
				self.infoCurEXP.Show()

			self.GridSlot = ui.GridSlotWindow()
			self.GridSlot.SetParent(self)
			self.GridSlot.SetPosition(15, self.GRID_SLOT_START_X[type])
			self.GridSlot.ArrangeSlot(0, FEED_SLOT_X_NUM, FEED_SLOT_Y_NUM, 32, 32, 0, 0)
			self.GridSlot.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			self.GridSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.GridSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.RemoveFromSlot))
			self.GridSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.RemoveFromSlot))
			self.GridSlot.SetUseSlotEvent(ui.__mem_func__(self.RemoveFromSlot))
			self.GridSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GridSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.GridSlot.Show()
			
			self.FeedButton = ui.Button()
			self.FeedButton.SetParent(self)
			self.FeedButton.SetUpVisual("d:/ymir work/ui/dragonsoul/m_button01.tga")
			self.FeedButton.SetOverVisual("d:/ymir work/ui/dragonsoul/m_button02.tga")
			self.FeedButton.SetDownVisual("d:/ymir work/ui/dragonsoul/m_button03.tga")
			self.FeedButton.SetWindowVerticalAlignBottom()
			self.FeedButton.SetText(uiScriptLocale.PET_FFED_BUTTON_TEXT)
			self.FeedButton.SetPosition((self.WINDOW_WIDTH[type] / 2) - (self.FeedButton.GetWidth() / 2) , 35)
			self.FeedButton.SetEvent(ui.__mem_func__(self.SendPetItem))
			self.FeedButton.Show()
		except:
			import exception
			exception.Abort("HuntingMiniWindow.LoadWindow.BindObject")
		
	def SetItemInSlot(self, invslot, feedslot, itemIndex, itemCount):
		if itemIndex != 0:
			item.SelectItem(itemIndex)
			itemsize = item.GetItemSize()[1]
			
			if self.CheckItem(itemIndex) == False:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_CANNOT_USE_THIS_ITEM)
				return

			if itemCount == 1:
				itemCount = 0
			
			if itemsize == 2:
				if self.FeedSlotIndex.has_key(feedslot + FEED_SLOT_X_NUM) or (feedslot + FEED_SLOT_X_NUM) > FEED_SLOT_NUM - 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_FEED_WINDOW_IS_FULL)
					return
					
			if itemsize == 3:
				if self.FeedSlotIndex.has_key(feedslot + FEED_SLOT_X_NUM) and self.FeedSlotIndex.has_key(feedslot + FEED_SLOT_X_NUM * 2) or (feedslot + FEED_SLOT_X_NUM * 2) > FEED_SLOT_NUM - 1:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_FEED_WINDOW_IS_FULL)
					return

			for i in xrange(itemsize):
				if self.FeedSlotIndex.has_key(feedslot + FEED_SLOT_X_NUM * i):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_FEED_WINDOW_IS_FULL)
					return

			self.FeedSlotIndex[feedslot] = [invslot, itemIndex]
			if itemsize >= 2:
				self.FeedSlotIndex[feedslot + FEED_SLOT_X_NUM] = [-1, 0]
			if itemsize == 3:
				self.FeedSlotIndex[feedslot + FEED_SLOT_X_NUM * 2] = [-1, 0]

			self.GridSlot.SetItemSlot(feedslot, itemIndex, itemCount)
			if app.WJ_ENABLE_TRADABLE_ICON:
				self.CantTradableItem(feedslot, invslot)
				
			if self.FeedType == 0:
				if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == 1 or item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == 2:
					self.infoLifeTime[0] += float(EGG_SEAL_FEED_PERC_VALUE[0])
					self.infoLifeTime[1] += float(EGG_SEAL_FEED_PERC_VALUE[1])
				if itemIndex == 55001:
					self.infoLifeTime[0] += 100
					self.infoLifeTime[1] += 100
			if self.FeedType == 2:
				if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_FEED:
					for i in xrange(len(SPECIAL_FEED_ITEM)):
						if (itemIndex == SPECIAL_FEED_ITEM[i][0] and self.wndMain.PetEvo == SPECIAL_FEED_ITEM[i][1]):
							if SPECIAL_FEED_ITEM[i][2] >= 100:
								self.infoSelectEXPValue += self.wndMain.PetExpItemRest
							else:
								self.infoSelectEXPValue += (self.wndMain.PetExpItemRest / 100) * SPECIAL_FEED_ITEM[i][2]
				else:
					self.infoSelectEXPValue += player.GetISellItemPrice(player.SLOT_TYPE_INVENTORY, invslot) / 2
			self.UpdateInfoText()

	def AutoSetItem(self, slotIndex):
		if self.FeedType == 0 and (self.isDurationPerc + self.infoLifeTime[0]) >= 100.0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NOT_NEED_MORE_ITEMS_FOR_LIFETIME)
			return
		if self.FeedType == 2 and self.infoSelectEXPValue >= self.wndMain.PetExpItemRest:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NO_NEED_MORE_ITEMS)
			return
			
		itemIndex = player.GetItemIndex(slotIndex)
		itemCount = player.GetItemCount(slotIndex)
		item.SelectItem(itemIndex)
		itemsize = item.GetItemSize()[1]
		canSetItem = False
		
		for slot in xrange(FEED_SLOT_NUM):
			for i in xrange(itemsize):
				if self.FeedSlotIndex.has_key(slot + FEED_SLOT_X_NUM * i):
					break
				if i == itemsize-1:
					canSetItem = True
			if canSetItem:
				self.SetItemInSlot(slotIndex, slot, itemIndex, itemCount)
				return
		
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_FEED_WINDOW_IS_FULL)

	def SelectEmptySlot(self, selectedSlotPos):
		if self.FeedType == 0 and (self.isDurationPerc + self.infoLifeTime[0]) >= 100.0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NOT_NEED_MORE_ITEMS_FOR_LIFETIME)
			return
		if self.FeedType == 2 and self.infoSelectEXPValue >= self.wndMain.PetExpItemRest:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_NO_NEED_MORE_ITEMS)
			return

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()
				
			if player.SLOT_TYPE_INVENTORY == attachedSlotType and not self.FeedSlotIndex.has_key(selectedSlotPos):
				for key in self.FeedSlotIndex:
					if self.FeedSlotIndex[key][1] == attachedSlotPos:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.GROWTH_PET_CHAT_ITEM_IS_IN_SLOT)
						mouseModule.mouseController.DeattachObject()
						return

				self.SetItemInSlot(attachedSlotPos, selectedSlotPos, attachedItemIndex, attachedItemCount)

			mouseModule.mouseController.DeattachObject()
	
	def RemoveFromSlot(self, slotIndex):
		self.GridSlot.ClearSlot(slotIndex)
		self.GridSlot.RefreshSlot()
		
		item.SelectItem(self.FeedSlotIndex[slotIndex][1])
		itemsize = item.GetItemSize()[1]

		if self.FeedType == 0:
			if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == 1 or item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == 2:
				self.infoLifeTime[0] -= float(EGG_SEAL_FEED_PERC_VALUE[0])
				self.infoLifeTime[1] -= float(EGG_SEAL_FEED_PERC_VALUE[1])
			if self.FeedSlotIndex[slotIndex][1] == 55001:
				self.infoLifeTime[0] -= 100.0
				self.infoLifeTime[1] -= 100.0
		if self.FeedType == 2:
			if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_FEED:
				for i in xrange(len(SPECIAL_FEED_ITEM)):
					if (self.FeedSlotIndex[slotIndex][1] == SPECIAL_FEED_ITEM[i][0] and self.wndMain.PetEvo == SPECIAL_FEED_ITEM[i][1]):
						if SPECIAL_FEED_ITEM[i][2] >= 100:
							self.infoSelectEXPValue -= self.wndMain.PetExpItemRest
						else:
							self.infoSelectEXPValue -= (self.wndMain.PetExpItemRest / 100) * SPECIAL_FEED_ITEM[i][2]
						break
			else:
				self.infoSelectEXPValue -= player.GetISellItemPrice(player.SLOT_TYPE_INVENTORY, self.FeedSlotIndex[slotIndex][0]) / 2
		self.UpdateInfoText()
			
		del self.FeedSlotIndex[slotIndex]
		if itemsize >= 2:
			del self.FeedSlotIndex[slotIndex + FEED_SLOT_X_NUM]
		if itemsize == 3:
			del self.FeedSlotIndex[slotIndex + FEED_SLOT_X_NUM * 2]

		if app.WJ_ENABLE_TRADABLE_ICON:
			(itemInvenPage, itemSlotPos) = self.lockedItems[slotIndex]
			if itemInvenPage == self.wndMain.wndInventory.GetInventoryPageIndex():
				self.wndMain.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)

			self.lockedItems[slotIndex] = (-1, -1)
	
	def SendPetItem(self):
		for key in self.FeedSlotIndex:
			if self.FeedSlotIndex[key][0] != -1:
				net.SendChatPacket("/petfeedadditem %d %d %d" % (self.FeedType, key, self.FeedSlotIndex[key][0]))
		net.SendChatPacket("/petfeedeat %d" % (self.FeedType))
		for slot in xrange(FEED_SLOT_NUM):
			if self.FeedSlotIndex.has_key(slot):
				del self.FeedSlotIndex[slot]
				self.GridSlot.ClearSlot(slot)
				self.GridSlot.RefreshSlot()
			if app.WJ_ENABLE_TRADABLE_ICON:
				(itemInvenPage, itemSlotPos) = self.lockedItems[slot]
				if itemInvenPage == self.wndMain.wndInventory.GetInventoryPageIndex():
					self.wndMain.wndInventory.wndItem.SetCanMouseEventSlot(itemSlotPos)
		self.infoSelectEXPValue = 0
		self.infoLifeTime = [0, 0]
		self.UpdateInfoText()
		if app.WJ_ENABLE_TRADABLE_ICON:
			if self.wndMain.interface:
				self.lockedItems = {i:(-1,-1) for i in range(FEED_SLOT_NUM)}
				self.wndMain.RefreshLockedSlot()
				self.wndMain.interface.RefreshMarkInventoryBag()
			
	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, slotIndex):
		if self.tooltipItem:
			self.tooltipItem.SetInventoryItem(self.FeedSlotIndex[slotIndex][0])
	
	def CheckItem(self, itemIndex):
		item.SelectItem(itemIndex)
		if self.FeedType == 0:
			if item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_EGG or item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_SEAL or itemIndex == 55001:
				return True
		if self.FeedType == 2:
			if item.GetItemType() == item.ITEM_TYPE_WEAPON or item.GetItemType() == item.ITEM_TYPE_ARMOR or item.GetItemType() == item.ITEM_TYPE_PET_ASLAN and item.GetItemSubType() == item.PET_ASLAN_FEED:
				return True
				
		return False
	
	def UpdateInfoText(self):
		if self.FeedType == 0:
			self.isDurationPerc = (float(self.wndMain.SealDuration - app.GetGlobalTimeStamp()) / float(self.wndMain.BornDuration)) * 100.0
			if (self.isDurationPerc + self.infoLifeTime[0]) >= 100.0:
				self.infoHP.SetText("100%")
			elif self.infoLifeTime[0] == self.infoLifeTime[1]:
				text1 = "%0.2f%%" % float(self.isDurationPerc + self.infoLifeTime[0])
				if self.isDurationPerc + self.infoLifeTime[0] >= 100.0:
					text1 = "100%"
				self.infoHP.SetText(text1)
			else:
				text1 = "%0.2f%%" % float(self.isDurationPerc + self.infoLifeTime[0])
				text2 = "%0.2f%%" % float(self.isDurationPerc + self.infoLifeTime[1])
				if self.isDurationPerc + self.infoLifeTime[1] >= 100.0:
					text2 = "100%"
				self.infoHP.SetText(text1 + " - " + text2)
		if self.FeedType == 2:
			TEXT_COLOR = NEGATIVE_COLOR
			if self.infoSelectEXPValue >= self.wndMain.PetExpItemRest:
				TEXT_COLOR = POSITIVE_COLOR
			if self.wndMain.PetExpItemRest == 0:
				TEXT_COLOR = GOLDEN_COLOR

			self.infoDestEXP.SetText(localeInfo.NumberWithPoint(str(self.wndMain.PetExpItemRest)) + " EXP")
			self.infoCurEXP.SetText(localeInfo.NumberWithPoint(str(self.infoSelectEXPValue)) + " EXP")
			self.infoCurEXP.SetPackedFontColor(TEXT_COLOR)

	if app.WJ_ENABLE_TRADABLE_ICON:
		def CantTradableItem(self, destSlotIndex, srcSlotIndex):
			itemInvenPage = srcSlotIndex / player.INVENTORY_PAGE_SIZE
			localSlotPos = srcSlotIndex - (itemInvenPage * player.INVENTORY_PAGE_SIZE)
			self.lockedItems[destSlotIndex] = (itemInvenPage, localSlotPos)
			if self.wndMain.wndInventory.GetInventoryPageIndex() == itemInvenPage:
				self.wndMain.wndInventory.wndItem.SetCantMouseEventSlot(localSlotPos)

		def RefreshLockedSlot(self):
			if self.wndMain.wndInventory:
				for privatePos, (itemInvenPage, itemSlotPos) in self.lockedItems.items():
					if self.wndMain.wndInventory.GetInventoryPageIndex() == itemInvenPage:
						if self.wndMain.wndInventory.wndItem:
							self.wndMain.wndInventory.wndItem.SetCantMouseEventSlot(itemSlotPos)

		def OnUpdate(self):
			if player.IsOpenPrivateShop():
				self.Close()
		
		def OnTop(self):
			if self.wndMain.interface:
				if type == 0:
					self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_PET_FEED_LIFE)
				if type == 2:
					self.wndMain.interface.SetOnTopWindow(player.ON_TOP_WND_PET_FEED_EQUIP)
				self.wndMain.interface.RefreshMarkInventoryBag()
