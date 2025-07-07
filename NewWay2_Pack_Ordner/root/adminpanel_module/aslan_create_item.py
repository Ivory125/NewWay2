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
import time
import item
import dbg
import uiToolTip

MAX_ITEM_STACK_COUNT = 200

STONE_MAX_GRADE = 4	# +4
STONE_VNUM_RANGE = [28030, 28043]

BONUS_BLACK_LIST = [ item.APPLY_PC_BANG_EXP_BONUS, item.APPLY_PC_BANG_DROP_BONUS ]

AFFECT_LIST = uiToolTip.ItemToolTip.AFFECT_DICT

IMG_PATH = "d:/ymir work/ui/aslan/adminpanel/create_item/"

NORMAL_COLOR = grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0)
POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)
NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)

class MainWindow(ui.ScriptWindow):

	def __init__(self):
		self.wndSelect = None

		self.isItemlistLoadet = False
		self.itemList = None
		self.itemDestCount = 0
		self.itemCurCount = 0

		self.selectedVnum = 0
		self.selectedRefine = 0
		self.selectedItemType = [-1, -1]
		
		self.animWindowWidth = [450, 672]
		self.curWindowWidth = 450.0
		self.destWindowWidth = 450.0
		
		self.animConfigWndPos = [225, 445]
		self.curConfigWndPos = 225.0
		self.destConfigWndPos = 225.0
		
		self.animResetBtnPos = [270, 490]
		self.curResetBtnPos = 270.0
		self.destResetBtnPos = 270.0
		
		self.socketInfo = [0 for i in xrange(player.ITEM_STONES_MAX_NUM)]
		self.metinSlot = [0 for i in xrange(player.ITEM_STONES_MAX_NUM)]
		self.attrSlot = [[0,0] for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
		
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		self.isItemlistLoadet = False
		self.itemList = None
		self.itemDestCount = 0
		self.itemCurCount = 0
		
		self.selectedVnum = 0
		self.selectedRefine = 0
		self.selectedItemType = [-1, -1]

		ui.ScriptWindow.__del__(self)

	def Show(self):
		if self.isItemlistLoadet == False:
			self.LoadItemList()
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "adminpanel_module/uiscript/aslan_create_item.py")
		except:
			import exception
			exception.Abort("CreateItemWindow.LoadWindow.LoadObject")
			
		try:
			self.wndSelect = SelectWindow(self)

			self.GetChild("board").CloseButtonHide()
			
			self.configFields = [self.GetChild("field_select_grade"), self.GetChild("field_set_count"), self.GetChild("field_set_player")]
			self.configOverlays = [self.GetChild("field_select_grade_overlay"), self.GetChild("field_set_count_overlay"), self.GetChild("field_player_overlay")]
			self.configOverlays[0].SetAlpha(0.7)
			self.configOverlays[1].SetAlpha(0.7)
			self.configOverlays[2].SetAlpha(0.7)
			self.configFields[1].SetStringEvent("MOUSE_LEFT_BUTTON_DOWN", ui.__mem_func__(self.SetEditLineFocus), 0)
			self.configFields[2].SetStringEvent("MOUSE_LEFT_BUTTON_DOWN", ui.__mem_func__(self.SetEditLineFocus), 1)
			
			for i in xrange(5):
				self.GetChild("field_select_bonus_%d" % i).SetEvent(ui.__mem_func__(self.ClickBonusButton), i)
	
			for i in xrange(3):
				self.GetChild("field_select_socket_%d" % i).SetEvent(ui.__mem_func__(self.ClickSocketButton), i)
				self.GetChild("field_select_socket_%d" % i).SetOverEvent(ui.__mem_func__(self.OverInSockets), i)
				self.GetChild("field_select_socket_%d" % i).SetOverOutEvent(ui.__mem_func__(self.OverOutItem))
	
			self.GetChild("editline_set_count").SetTabEvent(lambda arg = 1 : self.SetEditLineFocus(arg))
			self.GetChild("editline_set_count").SetReturnEvent(ui.__mem_func__(self.ClickCreateButton))
			self.GetChild("editline_set_player").SetTabEvent(lambda arg = 0 : self.SetEditLineFocus(arg))
			self.GetChild("editline_set_player").SetReturnEvent(ui.__mem_func__(self.ClickCreateButton))
		
			listBoxSearch = ListBoxSearch()
			listBoxSearch.SetParent(self.GetChild("itemlist_background"))
			listBoxSearch.SetPosition(4, 4)
			listBoxSearch.SetSize(200-4-4, 17 * listBoxSearch.stepSize)
			listBoxSearch.SetEvent(ui.__mem_func__(self.OnSelectItem))
			listBoxSearch.SetMinSearchTextLen(1)
			listBoxSearch.SetEditLine(self.GetChild("itemsearch"))
			listBoxSearch.Show()
			self.listBoxSearch = listBoxSearch
				
			self.selectRefine = ui.ComboBoxEx()
			self.selectRefine.SetParent(self.GetChild("field_select_grade"))
			self.selectRefine.AddFlag('float')
			self.selectRefine.SetPosition(3, 18)
			self.selectRefine.SetSize(188, 15)
			self.selectRefine.SetEvent(self.OnSelectRefine)
		
			self.GetChild("ItemSlot").SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.GetChild("ItemSlot").SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.GetChild("CreateButton").SetEvent(ui.__mem_func__(self.ClickCreateButton))
			self.GetChild("ResetButton").SetEvent(ui.__mem_func__(self.ClickResetButton))
			
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

		except:
			import exception
			exception.Abort("CreateItemWindow.LoadWindow.BindObject")

	def SetEditLineFocus(self, index):
		editLines = [self.GetChild("editline_set_count"), self.GetChild("editline_set_player")]
		item.SelectItem(self.selectedVnum)

		if item.ITEM_TYPE_WEAPON == item.GetItemType() or item.ITEM_TYPE_ARMOR == item.GetItemType() or item.ITEM_TYPE_BELT == item.GetItemType():
			return

		if not item.IsFlag(item.ITEM_FLAG_STACKABLE):
			return

		editLines[index].SetFocus()

	def ClickBonusButton(self, bonusSlot):
		self.wndSelect.Show(0, bonusSlot)

	def ClickSocketButton(self, socket):
		item.SelectItem(self.selectedVnum)
		if item.ITEM_TYPE_ARMOR == item.GetItemType() and item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			if self.socketInfo[socket] == 0:
				accesory_material = constInfo.GET_ACCESSORY_MATERIAL_VNUM(self.selectedVnum, item.GetItemSubType())
				self.socketInfo[socket] = accesory_material
				self.metinSlot[0] += 1
				item.SelectItem(accesory_material)
				self.GetChild("select_socket_%d" % socket).LoadImage(item.GetIconImageFileName())
				self.GetChild("select_socket_%d" % socket).SetPosition(2,1)
				self.GetChild("select_socket_%d" % socket).Show()
				self.OverInSockets(socket)
			else:
				self.socketInfo[socket] = 0
				self.metinSlot[0] -= 1
				self.GetChild("select_socket_%d" % socket).Hide()
				self.OverOutItem()
		else:
			self.wndSelect.Show(1, socket, self.selectedVnum)
	
	def ResetSockets(self):
		for i in xrange(player.ITEM_STONES_MAX_NUM):
			self.socketInfo[i] = 0
			self.metinSlot[i] = 0
			self.GetChild("select_socket_%d" % i).Hide()

	def ClickResetButton(self):
		self.socketInfo = [0 for i in xrange(player.ITEM_STONES_MAX_NUM)]
		self.metinSlot = [0 for i in xrange(player.ITEM_STONES_MAX_NUM)]
		self.attrSlot = [[0,0] for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM)]
		for i in xrange(player.ITEM_STONES_MAX_NUM):
			self.GetChild("select_socket_%d" % i).Hide()
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			self.GetChild("info_select_bonus_%d" % i).SetText("---")
			self.GetChild("info_select_bonus_%d" % i).SetPackedFontColor(self.GetChangeTextLineColor(0))
		
	def ClickCreateButton(self):
		item_vnum = self.selectedVnum + self.selectedRefine

		item_count = 1
		if self.GetChild("editline_set_count").GetText() != "":
			item_count = int(self.GetChild("editline_set_count").GetText())

		cmd_string = "/adminpanel_create_item "
		cmd_string += ("%d %d " % (item_vnum, item_count))
		for i in xrange(player.ITEM_STONES_MAX_NUM):
			cmd_string += ("%d " % self.metinSlot[i])
		for i in xrange(player.ATTRIBUTE_SLOT_MAX_NUM):
			cmd_string += ("%d %d " % (self.attrSlot[i][0], self.attrSlot[i][1]))

		if self.GetChild("editline_set_player").GetText() != "":
			player_name = self.GetChild("editline_set_player").GetText()
			cmd_string += ("%s" % player_name)
	
		chat.AppendChat(chat.CHAT_TYPE_INFO, "%s" % cmd_string)
		net.SendChatPacket("%s" % cmd_string)

	def OnSelectRefine(self, refine):
		self.selectedRefine = refine
		item.SelectItem(self.selectedVnum + refine)
		self.selectRefine.SetCurrentItem(item.GetItemName())
		
	def LoadItemList(self):
		self.itemList = item.AdminPanelGetItemList()
		self.itemCurCount = 0
		self.itemDestCount = len(self.itemList)
		
	def OnSelectItem(self, vnum, text):
		self.configOverlays[0].Show()
		self.configOverlays[1].Show()
		self.configOverlays[2].Show()
		self.GetChild("editline_set_count").SetText("")
		
		self.selectedVnum = vnum
		self.selectedRefine = 0
		self.GetChild("ItemSlot").SetItemSlot(0, vnum, 0)
		item.SelectItem(vnum)

		if item.GetItemType() != self.selectedItemType[0] and item.GetItemSubType() != self.selectedItemType[1]:
			self.ResetSockets()

		self.selectedItemType = [ item.GetItemType(), item.GetItemSubType()]
		
		if item.ITEM_TYPE_WEAPON == item.GetItemType() or item.ITEM_TYPE_ARMOR == item.GetItemType() or item.ITEM_TYPE_BELT == item.GetItemType():
			self.configOverlays[0].Hide()
			self.selectRefine.ClearItem()
			self.selectRefine.SetCurrentItem(item.GetItemName())
			for i in xrange(10):
				item.SelectItem(vnum+i)
				self.selectRefine.InsertItem(i, item.GetItemName())
			self.selectRefine.Show()
			self.destWindowWidth = self.animWindowWidth[1]
			self.destConfigWndPos = self.animConfigWndPos[1]
			self.destResetBtnPos = self.animResetBtnPos[1]
			self.GetChild("editline_set_player").SetFocus()
		else:
			self.selectRefine.Hide()
			self.destWindowWidth = self.animWindowWidth[0]
			self.destConfigWndPos = self.animConfigWndPos[0]
			self.destResetBtnPos = self.animResetBtnPos[0]
			if item.IsFlag(item.ITEM_FLAG_STACKABLE):
				self.GetChild("editline_set_count").SetText("1")
				self.GetChild("editline_set_count").SetFocus()
				self.GetChild("editline_set_count").SetEndPosition()
				self.configOverlays[1].Hide()
			else:
				self.GetChild("editline_set_player").SetFocus()
				self.GetChild("editline_set_player").SetEndPosition()
				
		self.configOverlays[2].Hide()
	
	def SetBonusInfo(self, index, bonus, value):
		self.GetChild("info_select_bonus_%d" % index).SetText(AFFECT_LIST[bonus](value))
		self.GetChild("info_select_bonus_%d" % index).SetPackedFontColor(self.GetChangeTextLineColor(value))
		self.attrSlot[index] = [bonus, value]
	
	def ClearBonusInfo(self, index):
		self.GetChild("info_select_bonus_%d" % index).SetText("---")
		self.GetChild("info_select_bonus_%d" % index).SetPackedFontColor(self.GetChangeTextLineColor(0))
		self.attrSlot[index] = [0, 0]
	
	def SetStoneSlot(self, index, vnum):
		self.socketInfo[index] = vnum
		self.metinSlot[index] = vnum
		item.SelectItem(vnum)
		self.GetChild("select_socket_%d" % index).LoadImage(item.GetIconImageFileName())
		self.GetChild("select_socket_%d" % index).Show()
		self.GetChild("select_socket_%d" % index).SetPosition(2,1)
	
	def ClearStoneSlot(self, index):
		self.socketInfo[index] = 0
		self.metinSlot[index] = 0
		self.GetChild("select_socket_%d" % index).Hide()
		
	def GetChangeTextLineColor(self, value):
		if value > 0:
			return POSITIVE_COLOR
		if 0 == value:
			return NORMAL_COLOR
		return NEGATIVE_COLOR
		
	def OverInItem(self, overSlotPos):
		if self.tooltipItem and self.selectedVnum != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.selectedVnum + self.selectedRefine, self.metinSlot, self.attrSlot )
			self.tooltipItem.ShowToolTip()

	def OverInSockets(self, overSlotPos):
		if self.tooltipItem and self.socketInfo[overSlotPos] != 0:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.socketInfo[overSlotPos],metinSlot = [0 for i in xrange(player.ITEM_STONES_MAX_NUM)] )
			self.tooltipItem.ShowToolTip()

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.listBoxSearch.scrollBar.OnUp()
		else:
			self.listBoxSearch.scrollBar.OnDown()

	def OnUpdate(self):
		if self.GetChild("editline_set_count").GetText() != "" and int(self.GetChild("editline_set_count").GetText()) > MAX_ITEM_STACK_COUNT:
			self.GetChild("editline_set_count").SetText(str(MAX_ITEM_STACK_COUNT))
	
		if self.itemDestCount != self.itemCurCount:
			for i in xrange(12):
				if self.itemDestCount != self.itemCurCount:
					itemvnum = self.itemList[self.itemCurCount]
					item.SelectItem(itemvnum)
					self.itemCurCount += 1
					if item.GetItemName() != "":
						self.listBoxSearch.InsertItem(itemvnum, "%s (%d)" % (item.GetItemName(), itemvnum))
		else:
			self.isItemlistLoadet = True
	
		if self.destWindowWidth != self.curWindowWidth:
			self.curWindowWidth += (self.destWindowWidth - self.curWindowWidth) / 3.0
			if abs(self.curWindowWidth - self.destWindowWidth) < 0.003:
				self.curWindowWidth = self.destWindowWidth
			self.GetChild("board").SetSize(self.curWindowWidth, 380)
			self.SetSize(self.curWindowWidth, 380)

			self.curConfigWndPos += (self.destConfigWndPos - self.curConfigWndPos) / 3.0
			if abs(self.curConfigWndPos - self.destConfigWndPos) < 0.003:
				self.curConfigWndPos = self.destConfigWndPos
			self.GetChild("item_config_2_board").SetPosition(self.curConfigWndPos, 40)

			self.curResetBtnPos += (self.destResetBtnPos - self.curResetBtnPos) / 3.0
			if abs(self.curResetBtnPos - self.destResetBtnPos) < 0.003:
				self.curResetBtnPos = self.destResetBtnPos
			self.GetChild("ResetButton").SetPosition(self.curResetBtnPos, 345)

	def OnPressEscapeKey(self):
		self.Close()
		return True

class SelectWindow(ui.BoardWithTitleBar):
	def __init__(self, parent):
		self.parent = parent
		self.SelectedType = -1
		self.SelectedItemVnum = -1
		self.SelectedSlot = -1
		self.selectedItem = -1
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		self.parent = None
		self.SelectedType = -1
		self.SelectedItemVnum = -1
		self.SelectedSlot = -1
		self.selectedItem = -1
		ui.BoardWithTitleBar.__del__(self)

	def Show(self, type, bonus, itemvnum = -1):
		self.SelectedType = type
		self.SelectedItemVnum = itemvnum
		self.SelectedSlot = bonus
		ui.BoardWithTitleBar.Show(self)
		self.SearchEditLine.SetText("")
		if type == 0:
			self.LoadAffect()
		elif type == 1:
			self.LoadStone()
		self.SetCenterPosition()
		self.SetTop()
		self.SearchEditLine.SetFocus()

	def Close(self):
		self.SelectedSlot = -1
		self.selectedItem = -1
		ui.BoardWithTitleBar.Hide(self)
	
	def __LoadWindow(self):
		self.SetSize(390, 340)
		self.SetCenterPosition()
		self.AddFlag('movable')
		self.AddFlag('float')
		self.SetTitleName("Bonus auswählen")
		self.SetCloseEvent(self.Close)

		self.SearchBoard = ui.ThinBoardCircle()
		self.SearchBoard.SetParent(self)
		self.SearchBoard.SetPosition(15,37)
		self.SearchBoard.SetSize(360,20)
		self.SearchBoard.Show()
		
		self.SearchTextLine = ui.AddTextLine(self.SearchBoard, 5, 3, "Suche :")
		
		self.SearchEditLine = ui.EditLine()
		self.SearchEditLine.SetParent(self.SearchBoard)
		self.SearchEditLine.SetSize(250, 30)
		self.SearchEditLine.SetPosition(50, 3)
		self.SearchEditLine.SetMax(24)
		self.SearchEditLine.Show()

		self.ListBoard = ui.ThinBoardCircle()
		self.ListBoard.SetParent(self)
		self.ListBoard.SetPosition(15,37+20)
		self.ListBoard.SetSize(360,220)
		self.ListBoard.Show()

		self.listBoxSearch = ListBoxSearch()
		self.listBoxSearch.SetParent(self.ListBoard)
		self.listBoxSearch.SetPosition(4, 4)
		self.listBoxSearch.SetSize(360-4-4, 12 * self.listBoxSearch.stepSize)
		self.listBoxSearch.SetEvent(ui.__mem_func__(self.OnSelectItem))
		self.listBoxSearch.SetMinSearchTextLen(1)
		self.listBoxSearch.SetEditLine(self.SearchEditLine)
		self.listBoxSearch.Show()
		
		self.ValueBoard = ui.ThinBoardCircle()
		self.ValueBoard.SetParent(self)
		self.ValueBoard.SetPosition(15,37+20+220)
		self.ValueBoard.SetSize(360,20)
		self.ValueBoard.Show()
		
		self.ValueTextLine = ui.AddTextLine(self.ValueBoard, 5, 3, "Value :")
		
		self.ValueEditLine = ui.EditLine()
		self.ValueEditLine.SetParent(self.ValueBoard)
		self.ValueEditLine.SetSize(250, 30)
		self.ValueEditLine.SetPosition(50, 3)
		self.ValueEditLine.SetMax(4)
		self.ValueEditLine.SetNumberMode()
		self.ValueEditLine.SetText("1")
		self.ValueEditLine.Show()
	
		self.AcceptButton = ui.Button()
		self.AcceptButton.SetParent(self)
		self.AcceptButton.SetWindowHorizontalAlignCenter()
		self.AcceptButton.SetPosition(-80, 37+20+220+30)
		self.AcceptButton.SetUpVisual(IMG_PATH + "button_1_normal.sub")
		self.AcceptButton.SetOverVisual(IMG_PATH + "button_1_hover.sub")
		self.AcceptButton.SetDownVisual(IMG_PATH + "button_1_down.sub")
		self.AcceptButton.SetEvent(ui.__mem_func__(self.ClickAcceptButton))
		self.AcceptButton.SetText("OK")
		self.AcceptButton.SetOutline()
		self.AcceptButton.Show()
	
		self.ClearButton = ui.Button()
		self.ClearButton.SetParent(self)
		self.ClearButton.SetWindowHorizontalAlignCenter()
		self.ClearButton.SetPosition(80, 37+20+220+30)
		self.ClearButton.SetUpVisual(IMG_PATH + "button_1_normal.sub")
		self.ClearButton.SetOverVisual(IMG_PATH + "button_1_hover.sub")
		self.ClearButton.SetDownVisual(IMG_PATH + "button_1_down.sub")
		self.ClearButton.SetEvent(ui.__mem_func__(self.ClickClearButton))
		self.ClearButton.SetText("Zurücksetzen")
		self.ClearButton.SetOutline()
		self.ClearButton.Show()
		
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
	
		self.SearchEditLine.SetTabEvent(lambda arg = 1 : self.SetEditLineFocus(arg))
		self.SearchEditLine.SetReturnEvent(ui.__mem_func__(self.ClickAcceptButton))
		self.ValueEditLine.SetTabEvent(lambda arg = 0 : self.SetEditLineFocus(arg))
		self.ValueEditLine.SetReturnEvent(ui.__mem_func__(self.ClickAcceptButton))
	
	def LoadAffect(self):
		self.SetTitleName(localeInfo.ADMINPANEL_CREATE_ITEM_WND_BONUS_TITLE)
		self.SetSize(390, 340)
		self.ValueBoard.Show()
		self.AcceptButton.SetPosition(-80, 37+20+220+30)
		self.ClearButton.SetPosition(80, 37+20+220+30)
		self.listBoxSearch.ClearItem()
		for affect, name in AFFECT_LIST.items():
			if not affect in BONUS_BLACK_LIST:
				self.listBoxSearch.InsertItem(affect, "%s (%d)" % (name(0), affect))
		self.listBoxSearch.SelectItem(0)
	
	def LoadStone(self):
		self.SetTitleName(localeInfo.ADMINPANEL_CREATE_ITEM_WND_STONE_TITLE)
		self.SetSize(390, 320)
		self.ValueBoard.Hide()
		self.AcceptButton.SetPosition(-80, 37+220+30)
		self.ClearButton.SetPosition(80, 37+220+30)
		self.listBoxSearch.ClearItem()
		for vnum in xrange(STONE_VNUM_RANGE[0], STONE_VNUM_RANGE[1]+1):
			for grade in xrange(STONE_MAX_GRADE+1):
				stone_vnum = vnum + grade * 100
				item.SelectItem(self.SelectedItemVnum)
				
				if item.ITEM_TYPE_WEAPON == item.GetItemType():
					item.SelectItem(stone_vnum)
					if item.IsWearableFlag(item.WEARABLE_WEAPON):
						self.listBoxSearch.InsertItem(stone_vnum, "%s (%d)" % (item.GetItemName(), stone_vnum))

				if item.ITEM_TYPE_ARMOR == item.GetItemType():
					item.SelectItem(stone_vnum)
					if item.IsWearableFlag(item.WEARABLE_BODY):
						self.listBoxSearch.InsertItem(stone_vnum, "%s (%d)" % (item.GetItemName(), stone_vnum))

	def ClickAcceptButton(self):
		if self.SelectedType == 0:
			value = 1
			if self.selectedItem == -1:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ADMINPANEL_CREATE_ITEM_WND_BONUS_CHAT_SELECT_BONUS)
				return

			if self.ValueEditLine.GetText() != "" and int(self.ValueEditLine.GetText()) != 0:
				value = int(self.ValueEditLine.GetText())
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.ADMINPANEL_CREATE_ITEM_WND_BONUS_CHAT_SET_VALUE)
				return

			self.parent.SetBonusInfo(self.SelectedSlot, self.selectedItem, value)
			
		if self.SelectedType == 1:
			self.parent.SetStoneSlot(self.SelectedSlot, self.selectedItem)
			
		self.Close()

	def ClickClearButton(self):
		if self.SelectedType == 0:
			self.parent.ClearBonusInfo(self.SelectedSlot)
			self.Close()
		if self.SelectedType == 1:
			self.parent.ClearStoneSlot(self.SelectedSlot)
			self.Close()
	
	def SetEditLineFocus(self, index):
		if self.SelectedType == 1:
			editLines[0].SetFocus()
		else:
			editLines = [self.SearchEditLine, self.ValueEditLine]
			editLines[index].SetFocus()

	def OnSelectItem(self, id, text):
		self.selectedItem = id
	
	def OnRunMouseWheel(self, nLen):
		if nLen > 0:
			self.listBoxSearch.scrollBar.OnUp()
		else:
			self.listBoxSearch.scrollBar.OnDown()

	def OnPressEscapeKey(self):
		self.Close()
		return True

class ListBoxSearch(ui.ListBox):
	SEARCH_UPDATE_TIME = 0.1

	def __init__(self):
		ui.ListBox.__init__(self)
		self.visibleDict = {}
		self.textDictLower = {}
		self.lastSearchText = ""
		self.lastSearchTime = 0
		self.minSearchTextLen = 3
		self.realSelectedLine = -1
		self.editLine = None
		self.scrollBar = ui.ScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetScrollEvent(self.__OnScroll)
		self.scrollBar.SetScrollStep(0.01)
		self.scrollBar.Hide()

	def SetMinSearchTextLen(self, minSearchTextLen):
		self.minSearchTextLen = minSearchTextLen

	def SetEditLine(self, editLine):
		self.editLine=editLine

	def SetSize(self, width, height):
		ui.ListBox.SetSize(self, width - ui.ScrollBar.SCROLLBAR_WIDTH, height)
		ui.Window.SetSize(self, width, height)
		
		self.scrollBar.SetPosition(width - ui.ScrollBar.SCROLLBAR_WIDTH, 0)
		self.scrollBar.SetScrollBarSize(height)

	def ClearItem(self):
		ui.ListBox.ClearItem(self)
		self.visibleDict = {}
		self.textDictLower = {}
		self.lastSearchText = ""
		self.lastSearchTime = 0
		self.realSelectedLine = -1
		self.scrollBar.SetPos(0)

	def SelectItem(self, line): 
		lineDict = []
		for key in range(len(self.itemList)):
			if not self.visibleDict[key]:
				continue
			
			lineDict.append(key)

		realLine = lineDict[line]

		if not self.keyDict.has_key(realLine):
			return

		if line == self.selectedLine:
			return

		self.selectedLine = line
		self.realSelectedLine = realLine
		self.event(self.keyDict.get(realLine, 0), self.textDict.get(realLine, "None"))

	def GetSelectedItem(self):
		return self.keyDict.get(self.realSelectedLine, 0)

	def InsertItem(self, number, text, doLocate = True):
		self.keyDict[len(self.itemList)] = number
		self.textDict[len(self.itemList)] = text
		self.textDictLower[len(self.itemList)] = text.lower()
		self.visibleDict[len(self.itemList)] = True

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetText(text)
		textLine.Show()

		if self.itemCenterAlign:
			textLine.SetWindowHorizontalAlignCenter()
			textLine.SetHorizontalAlignCenter()

		self.itemList.append(textLine)

		if doLocate:
			self._LocateItem()

	def LocateItem(self):
		self._LocateItem()

	def _LocateItem(self):
		self.selectedLine = -1
		self.realSelectedLine = -1
		skipCount = self.basePos
		yPos = 0
		self.showLineCount = 0

		for key, value in self.keyDict.items():
			textLine = self.itemList[key]
			textLine.Hide()

			if not self.visibleDict[key]:
				continue
		
			if skipCount > 0:
				skipCount -= 1
				continue

			textLine.SetPosition(0, yPos + 3)

			yPos += self.stepSize

			if yPos <= self.GetHeight():
				self.showLineCount += 1
				textLine.Show()
	
		if self.showLineCount < self.GetItemCount():
			self.scrollBar.SetMiddleBarSize(float(self.GetViewItemCount())/self.GetItemCount())
			self.scrollBar.Show()
		else:
			self.scrollBar.Hide()

	def __OnScroll(self):
		scrollLen = self.GetItemCount()-self.GetViewItemCount()
		if scrollLen < 0:
			scrollLen = 0
		self.SetBasePos(int(self.scrollBar.GetPos()*scrollLen))

	def GetItemCount(self):
		return sum(1 for key, value in self.keyDict.items() if self.visibleDict[key])

	def ReSearchItems(self):
		self._SearchItems(self.editLine.GetText())

	def _SearchItems(self, text):
		if len(text) < self.minSearchTextLen and len(text) != 0:
			return

		text = text.lower()

		for key, value in self.keyDict.items():
			textLineText = self.textDictLower[key]
			self.visibleDict[key] = False
			if textLineText.find(text) != -1:
			# if (self.startsWithSearchMode and textLineText.startswith(text)) or (not self.startsWithSearchMode and text in textLineText) or (self.startsWithSearchMode and textLineText.find(text)):
				self.visibleDict[key] = True

		self.scrollBar.SetPos(0)
		self.SetBasePos(0)

	def OnUpdate(self):
		self.overLine = -1
		if self.scrollBar.IsShow():
			min_number = 1.00 / float(self.GetItemCount())*5
			if min_number < 0.005:
				min_number = 0.005
			self.scrollBar.SetScrollStep(min_number)

		if self.IsIn():
			x, y = self.GetGlobalPosition()
			height = self.GetHeight()
			xMouse, yMouse = wndMgr.GetMousePosition()

			if yMouse - y < height - 1:
				self.overLine = (yMouse - y) / self.stepSize

				if self.overLine < 0:
					self.overLine = -1
				if self.overLine >= self.GetItemCount():
					self.overLine = -1

		if self.lastSearchTime < time.clock():
			searchText = self.editLine.GetText()
			if self.lastSearchText != searchText:
				self.lastSearchText = searchText
				self._SearchItems(searchText)
			self.lastSearchTime = time.clock() + self.SEARCH_UPDATE_TIME