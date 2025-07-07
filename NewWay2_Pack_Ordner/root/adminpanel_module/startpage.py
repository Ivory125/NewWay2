import os
import ui
import net
import app
import snd
import chat
import grp
import ime
import wndMgr
import mouseModule
import uiScriptLocale
import localeInfo
import constInfo
import player

REFRESH_PLAYERCOUNT_SEC = 5

TEXTLINES = {
	0 : [ # Allgemein
		"Fortschritt Speichern = /flush",
		"Tag / Nacht wechsel = /eclipse <0/1>",
		"Monster rufen = /m <mob_vnum> <anzahl>",
		"Aggresives Monster rufen = /ma <mob_vnum> <anzahl>",
		"GM Chat-Nachricht = /n <text>",
		"Zu einem Spieler teleportieren = /warp <name>",
		"Einen Spieler zu sich teleportieren = /transfer <name>",
		"Item erstellen = /item <item_vnum>",
		"",
		"Spieler kicken = /dc <name>",
		"Spieler töten = /kill <name>",
		"Spieler Ohnmachten = /stun <name>",
		"",
		"Komplettes Inventar löschen = /ip",
		"Volle TP und HP bekommen = /r",
		"Unsterblich sein = /cannot_dead",
		"",
		"Yang verändern = /set <name> gold <value>",
		"EXP verändern = /set <name> exp <value>",
		"Rangpunkte verändern = /set <name> align <value>",
		"",
		"Verwandlung in ein Monster = /poly <mob_vnum>",
		"Verwandlungskugel erstellen = /polyitem <mob_vnum>",
	],
	
	1 : [ # Fertigkeiten
		"Fertigkeiten bei sich selbst erhöhen = /setsk <skill_id> <level>",
		"Fertigkeiten anderen erhöhen = /setskillother <player_name> <skill_id> <level>",
		"Level 20 = M1 | Level 30 = G1 | Level 40 = P",
		"_______________________________________________",
		"",
		"-------------- Skill IDs Krieger --------------",
		"1 - Dreiwege-Schnitt",
		"2 - Schwertwirbel",
		"3 - Kampfrausch",
		"4 - Aura des Schwertes",
		"5 - Sausen",
		"",
		"16 - Durchschlag",
		"17 - Heftiges Schlagen",
		"18 - Stampfer",
		"19 - Starker Körper",
		"20 - Schwertschlag",
		"",
		"-------------- Skill IDs Ninja --------------",
		"31 - Hinterhalt",
		"32 - Blitzangriff",
		"33 - Degenwirbel",
		"34 - Tarnung",
		"35 - Giftwolke",
		"",
		"46 - Wiederholter Schuss",
		"47 - Pfeilregen",
		"48 - Feuerpfeil",
		"49 - Federschreiten",
		"50 - Giftpfeil",
		"",
		"-------------- Skill IDs Sura --------------",
		"61 - Fingerschlag",
		"62 - Drachenwirbel",
		"63 - Verzauberte Klinge",
		"64 - Furcht",
		"65 - Verzauberte Rüstung",
		"66 - Zauber aufheben",
		"",
		"76 - Dunkler Schlag",
		"77 - Flammenschlag",
		"78 - Geist der Flamme",
		"79 - Dunkler Schutz",
		"80 - Geisterschlag",
		"81 - Dunkler Schutz",
		"",
		"-------------- Skill IDs Schamane --------------",
		"91 - Fliegender Talisman",
		"92 - Drachenschießen",
		"93 - Drachengebrüll",
		"94 - Segen",
		"95 - Reflektieren",
		"96 - Hilfe des Drachen",
		"",
		"106 - Blitzwurf",
		"107 - Blitz heraufbeschwören",
		"108 - Blitzkralle",
		"109 - Kurieren",
		"110 - Schnelligkeit",
		"111 - Angriff+",
		"",
		"-------------- Skill IDs Sonstige --------------",
		"121 - Führung",
		"122 - Combobeherschung",
		"124 - Bergbau",
		"126 - Rote Sprache (Shinsoo)",
		"127 - Gelbe Sprache (Chunjo)",
		"128 - Blaue Sprache (Jinno)",
		"129 - Verwandlung",
		"131 - Pferd Rufen",
		"",
		"-------------- Skill IDs Pferde --------------",
		"137 - Kampf von Pferdrücken",
		"138 - Pferdestampfer",
		"139 - Kraftwelle",
	],
	
	2 : [ # Events
		"Reich-Boni = /priv_empire <empire> <type> <percent> <duration>",
		"--- <empire> | 0 = all | 1 = red | 2 = yellow | 3 = blue",
		"--- <type> | 1 = item-drop | 2 = gold-drop | 3 = gold10-drop | 4 = exp",
		"--- <percent> = percent",
		"--- <duration> = hour",
		"",
		"-------------- Item-Drops --------------",
		"Alle befehle mit ausführen mit = /event <type> <value>",
		"Bei manchen Drop Events wird mit value die dropwarscheinlichkeit angegeben.",
		"zb. /event 2006_drop 2000 (= 2%)",
		"",
		"Mondlichtschatztruhen = drop_moon",
		"Sechseckige Schatztruhe = 2006_drop",
		"Glyphenstein = hc_drop",
		"Fußball = football_drop",
		"Rose + Schokolade = valentine_drop",
		"Socke = xmas_sock",
		"",
		"Weitere -> item_manager.cpp (void ITEM_MANAGER::CreateQuestDropItem)"
	],
 }

IMG_PATH = "d:/ymir work/ui/aslan/adminpanel/startpage/"

SHOW_TEXTLINES = 10

class MainWindow(ui.ScriptWindow):
	def __init__(self):
		self.selectedButton = -1
		self.askNextPlayercount = 0
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		self.selectedButton = -1
		self.askNextPlayercount = 0
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "adminpanel_module/uiscript/startpage.py")
		except:
			import exception
			exception.Abort("BiologistAdminWindow.LoadWindow.LoadObject")
			
		try:
		
			self.CommandButtons = []
			self.CommandButtonsText = [localeInfo.ADMINPANEL_STARTPAGE_INFOLIST_BUTTON_1, localeInfo.ADMINPANEL_STARTPAGE_INFOLIST_BUTTON_2, localeInfo.ADMINPANEL_STARTPAGE_INFOLIST_BUTTON_3]
			for i in xrange(3):
				radioButton = ui.RadioButton()
				radioButton.SetParent(self)
				radioButton.SetUpVisual(IMG_PATH + "button_normal.sub")
				radioButton.SetOverVisual(IMG_PATH + "button_hover.sub")
				radioButton.SetDownVisual(IMG_PATH + "button_down.sub")
				radioButton.SetText(self.CommandButtonsText[i])
				radioButton.SetPosition(25+141*i, 137)
				radioButton.Show()
				self.CommandButtons.append(radioButton)
				
			self.radioButtonGroup = ui.RadioButtonGroup.Create([
				[self.CommandButtons[0], lambda : self._OnClickCommandButton(0), None], 
				[self.CommandButtons[1], lambda : self._OnClickCommandButton(1), None], 
				[self.CommandButtons[2], lambda : self._OnClickCommandButton(2), None]
			])
			self.radioButtonGroup.OnClick(0)

			self.GetChild("board").CloseButtonHide()
			
		except:
			import exception
			exception.Abort("BiologistAdminWindow.LoadWindow.BindObject")

	def RecivePlayerCount(self, count_red, count_yellow, count_blue, count_total):
		self.GetChild("info_shinsoo").SetText("%d" % count_red)
		self.GetChild("info_chunjo").SetText("%d" % count_yellow)
		self.GetChild("info_jinno").SetText("%d" % count_blue)
		self.GetChild("info_total").SetText("%d" % count_total)

	def _OnClickCommandButton(self, id):
		self.selectedButton = id
		self.textLines = []
		for i in xrange(SHOW_TEXTLINES):
			textLine = ui.TextLine()
			textLine.SetParent(self.GetChild("command_list_background"))
			textLine.SetPosition(14, 6+17*i)
			textLine.SetText(TEXTLINES[self.selectedButton][i])
			textLine.Show()
			self.textLines.append(textLine)
			
			self.scrollBar = ui.ScrollBar()
			self.scrollBar.SetParent(self.GetChild("command_list_background"))
			self.scrollBar.SetScrollBarSize(171)
			self.scrollBar.SetPosition(422-25, 5)
			self.scrollBar.SetScrollEvent(self.__OnScroll)
			# if id == 0:
				# self.scrollBar.SetScrollStep(0.07)
			# if id == 1:
				# self.scrollBar.SetScrollStep(0.03)
			# if id == 2:
				# self.scrollBar.SetScrollStep(0.08)
		self.scrollBar.Show()
		self.scrollBar.SetMiddleBarSize(float(SHOW_TEXTLINES)/len(TEXTLINES[self.selectedButton]))

	def __OnScroll(self):
		pos = int(self.scrollBar.GetPos() * (len(TEXTLINES[self.selectedButton]) - SHOW_TEXTLINES))
		for i in xrange(SHOW_TEXTLINES):
			self.textLines[i].SetText(TEXTLINES[self.selectedButton][i+pos])

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.scrollBar.OnUp()
		else:
			self.scrollBar.OnDown()
	
	def OnUpdate(self):
		if self.askNextPlayercount < app.GetGlobalTimeStamp():
			self.askNextPlayercount = app.GetGlobalTimeStamp() + REFRESH_PLAYERCOUNT_SEC
			net.SendChatPacket("/adminpanel_get_player_count")

		if self.scrollBar.IsShow():
			min_number = 1.00 / float(len(TEXTLINES[self.selectedButton]))*2
			if min_number < 0.005:
				min_number = 0.005
			self.scrollBar.SetScrollStep(min_number)
	
	def OnPressEscapeKey(self):
		self.Close()
		return True

