import os
import sys
import ui
import net
import app
from _weakref import proxy
import snd
import chat
import grp
import wndMgr
import localeInfo
import constInfo
import player
import imp
import sys
import dbg
import pack

######################################################
######## 	---- v1.0 ::: Modular-AdminPanel ::: by ASLAN ----	########
######################################################

#------------------------
GM_PLAYER				= 0
GM_LOW_WIZARD	= 1
GM_WIZARD			= 2
GM_HIGH_WIZARD	= 3
GM_GOD					= 4
GM_IMPLEMENTOR	= 5
#------------------------

MODULES_PY_PATH = "adminpanel_module/"
MODULES_LOCALE_PATH = "%s/adminpanel/" % app.GetLocalePath()

MODULES_DICT = {
	0	: { "modulename" : "startpage" ,							"button_text" : "Startseite",						"access" : GM_LOW_WIZARD,},
	1	: { "modulename" : "aslan_create_item",				"button_text" : "Item Erstellen",					"access" : GM_IMPLEMENTOR,},
	10	: { "modulename" : "TEMPLATE_with_uiscript" ,		"button_text" : "Templ. UiScript",				"access" : GM_LOW_WIZARD,},
	11	: { "modulename" : "TEMPLATE_without_uiscript" ,	"button_text" : "Templ. without UiScript",		"access" : GM_LOW_WIZARD,},
}

UI_PATH = "d:/ymir work/ui/aslan/adminpanel/"

class AdminPanelBase(ui.BoardWithTitleBar):
	def __init__(self):
		self.selectedWindow = "startpage"
		self.windowList = {}
		self.ScrollBarStep = 0.2
		self.curScrollbarPos = 0.0
		self.destScrollbarPos = 0.0
		self.isMouseWheel = False
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		self.selectedWindow = "startpage"
		self.windowList = {}
		self.ScrollBarStep = 0.2
		self.curScrollbarPos = 0.0
		self.destScrollbarPos = 0.0
		self.isMouseWheel = False
		ui.BoardWithTitleBar.__del__(self)

	def Show(self):
		if player.GetGMLevel() > 0:
			if len(self.windowList) != 0:
				self.ModuleButtonList.SetSelectModule(self.selectedWindow)
			ui.BoardWithTitleBar.Show(self)
			self.SetTop()

	def Hide(self):
		if len(self.windowList) != 0:
			for key in MODULES_DICT:
				self.windowList[MODULES_DICT[key]["modulename"]].Hide()
		ui.BoardWithTitleBar.Hide(self)
			
	def Close(self):
		if len(self.windowList) != 0:
			for key in MODULES_DICT:
				self.windowList[MODULES_DICT[key]["modulename"]].Hide()
		self.Hide()
	
	def Destroy(self):
		for key in MODULES_DICT:
			self.windowList[MODULES_DICT[key]["modulename"]].Hide()
		self.windowList = {}
		self.Hide()
		
	def __LoadWindow(self):
		self.SetSize(230, 370)
		self.SetCenterPosition()
		(xLocal, yLocal) = self.GetLocalPosition() 
		self.SetPosition(xLocal-250, yLocal)
		self.AddFlag('movable')
		self.AddFlag('float')
		self.SetTitleName("Admin-Panel")
		self.SetCloseEvent(self.Close)

		self.ListBox = ui.BorderA()
		self.ListBox.SetParent(self)
		self.ListBox.SetSize(200, 320)
		self.ListBox.SetPosition(15, 35)
		self.ListBox.Show()
		
		self.TitleBar = ui.MakeImageBox(self.ListBox, UI_PATH + "base/list_titlebar.tga", 3, 2)
		self.TitleBarText = ui.MakeNewTextLine(self.TitleBar)
		self.TitleBarText.SetText("Adminpanel-Module")
		self.TitleBarText.SetPackedFontColor(0xFFFEE3AE)
		self.TitleBarText.SetOutline()
		
		self.scrollBarBG = ui.MakeImageBox(self.ListBox, UI_PATH + "base/scrollbar_bg.tga", 182, 30)
		self.scrollBarArea = ui.Window()
		self.scrollBarArea.SetParent(self.scrollBarBG)
		self.scrollBarArea.SetSize(5, 277)
		self.scrollBarArea.SetPosition(3, 3)
		self.scrollBarArea.Show()
		
		self.scrollBar = ScrollBar()
		self.scrollBar.SetParent(self.scrollBarArea)
		self.scrollBar.SetScrollEvent(ui.__mem_func__(self.OnScroll))
		self.scrollBar.SetMovementArea(0, 0, 5, 277)
		self.scrollBar.SetPosition(0, 0)
		if len(MODULES_DICT) > 7:
			self.scrollBar.Show()
		
		self.ListArea = ui.Window()
		self.ListArea.SetParent(self.ListBox)
		self.ListArea.SetSize(171, 283)
		self.ListArea.SetPosition(6, 30)
		self.ListArea.Show()
		
		self.ModuleButtonList = ModuleButtonList()
		self.ModuleButtonList.SetParent(self.ListArea)
		self.ModuleButtonList.SetGlobalParent(self)
		self.ModuleButtonList.SetSize(171, 283)
		self.ModuleButtonList.SetPosition(0, 0)
		self.ModuleButtonList.Show()
		
		self.windowList = {}
		for key in MODULES_DICT:
			if player.GetGMLevel() >= MODULES_DICT[key]["access"]:
				modules = import_pack_file(MODULES_DICT[key]["modulename"], ("%s" % (MODULES_PY_PATH + MODULES_DICT[key]["modulename"] + ".py")))
				i = __import__(MODULES_DICT[key]["modulename"])
				window = i.MainWindow()
				self.windowList[MODULES_DICT[key]["modulename"]] = window
				self.ModuleButtonList.AppendModule(key, MODULES_DICT[key]["modulename"])

	def SetModule(self, modulename):
		self.selectedWindow = modulename
		for key in MODULES_DICT:
			self.windowList[MODULES_DICT[key]["modulename"]].Hide()
		self.windowList[modulename].Show()
		self.windowList[modulename].SetTop()

	def OnScroll(self):
		self.ModuleButtonList.OnScroll(self.scrollBar.GetPos())

	def OnMoveWindow(self, x, y):
		for key in self.windowList:
			if self.windowList[key].IsShow():
				self.windowList[key].SetTop()
		
	def OnRunMouseWheel(self, nLen):
		if self.scrollBar.IsShow():
			if nLen > 0:
				pos = self.destScrollbarPos - self.ScrollBarStep
			else:
				pos = self.destScrollbarPos + self.ScrollBarStep
			pos = max(0.0, pos)
			pos = min(1.0, pos)
			
			self.isMouseWheel = True
			self.destScrollbarPos = pos

	def OnUpdate(self):
		for key in self.windowList:
			if self.windowList[key].IsShow():
				(xLocal, yLocal) = self.GetLocalPosition() 
				self.windowList[key].SetPosition(xLocal + self.GetWidth(), yLocal)
				# self.windowList[key].SetTop()
			
		if self.isMouseWheel:
			self.curScrollbarPos += (self.destScrollbarPos - self.curScrollbarPos) / 10.0
			if abs(self.curScrollbarPos - self.destScrollbarPos) < 0.005:
				self.curScrollbarPos = self.destScrollbarPos
				self.isMouseWheel = False
			self.scrollBar.SetPos(self.curScrollbarPos)
		else:
			self.curScrollbarPos = self.scrollBar.GetPos()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
class ModuleButtonList(ui.Window):
	class Item(ui.Window):
		def __init__(self, parent, moduleindex, modulename):
			ui.Window.__init__(self)
			
			ui.Window.SetParent(self, parent)
			self.parent = proxy(parent)
			
			self.BUTTON_LIST = [
				UI_PATH + "base/list_button_normal.sub",
				UI_PATH + "base/list_button_hover.sub",
				UI_PATH + "base/list_button_down.sub",
			]
			
			self.SetWindowName("ButtonList_Item")
			self.bIsSelected = False
			self.xBase, self.yBase = 0, 0
			
			self.ModuleIndex = moduleindex
			self.ModuleName = modulename
			self.ButtonImage = None

			self.ButtonImage = ui.MakeExpandedImageBox(self, self.BUTTON_LIST[0], 0, 0, "not_pick")
			self.ButtonModuleName = ui.TextLine()
			self.ButtonModuleName.SetParent(self.ButtonImage)
			self.ButtonModuleName.SetWindowHorizontalAlignCenter()
			# self.ModuleName.SetWindowVerticalAlignCenter()
			self.ButtonModuleName.SetHorizontalAlignCenter()
			self.ButtonModuleName.SetPosition(0, 10)
			self.ButtonModuleName.SetText(MODULES_DICT[moduleindex]["button_text"])
			self.ButtonModuleName.Show()

		def __del__(self):
			ui.Window.__del__(self)
			self.bIsSelected = False
			self.xBase, self.yBase = 0, 0
			self.ModuleIndex = 0
			self.ModuleName = ""
			self.ButtonImage = None
			self.ButtonModuleName = None

		def SetBasePosition(self, x, y):
			self.xBase = x
			self.yBase = y

		def GetBasePosition(self):
			return (self.xBase, self.yBase)

		def OnMouseOverIn(self):
			if self.bIsSelected == True:
				self.ButtonImage.LoadImage(self.BUTTON_LIST[2])
			else:
				self.ButtonImage.LoadImage(self.BUTTON_LIST[1])

		def OnMouseOverOut(self):
			if self.bIsSelected == True:
				self.ButtonImage.LoadImage(self.BUTTON_LIST[2])
			else:
				self.ButtonImage.LoadImage(self.BUTTON_LIST[0])

		def OnMouseLeftButtonUp(self):
			snd.PlaySound("sound/ui/click.wav")
			self.parent.SetSelectModule(self.ModuleName)
			
		def Select(self):
			self.bIsSelected = True
			self.ButtonImage.LoadImage(self.BUTTON_LIST[2])

		def Deselect(self):
			self.bIsSelected = False
			self.ButtonImage.LoadImage(self.BUTTON_LIST[0])

		def GetModuleIndex(self):
			return self.ModuleIndex
			
		def GetModuleName(self):
			return self.ModuleName

		def Show(self):
			ui.Window.Show(self)
		
		def OnRender(self):
			xList, yList = self.parent.GetGlobalPosition()

			if self.ButtonImage:
				self.ButtonImage.SetClipRect(xList, yList, xList + self.parent.GetWidth(), yList + self.parent.GetHeight())

				
			if self.ButtonModuleName:
				xText, yText = self.ButtonModuleName.GetGlobalPosition()
				wText, hText = self.ButtonModuleName.GetTextSize()

				if yText < yList or (yText + hText > yList + self.parent.GetHeight()):
					self.ButtonModuleName.Hide()
				else:
					self.ButtonModuleName.Show()

	def __init__(self):
		ui.Window.__init__(self)
		self.SetWindowName("ModuleButtonList")
		self.globalParent = None
		self.modulelist = []

	def __del__(self):
		ui.Window.__del__(self)
		self.globalParent = None
		self.modulelist = []

	def SetSelectModule(self, modulename):
		if len(self.modulelist) != 0:
			for i in xrange(len(self.modulelist)):
				if modulename == self.modulelist[i].GetModuleName():
					self.modulelist[i].Select()
				else:
					self.modulelist[i].Deselect()

			if self.globalParent:
				self.globalParent.SetModule(modulename)

	def GetSelectedMission(self):
		return self.selectedMap

	def DelSelectAll(self):
		if len(self.modulelist) != 0:
			for i in xrange(len(self.modulelist)):
				self.modulelist[i].Deselect()

	def GetMapCount(self):
		count = 0
		for i in xrange(len(self.modulelist)):
			count += 1
		return count

	def SetGlobalParent(self, parent):
		self.globalParent = proxy(parent)

	def OnScroll(self, scrollPos):
		totalHeight = 0
		for item in self.modulelist:
			totalHeight += item.GetHeight() 

		totalHeight -= self.GetHeight()

		for i in xrange(len(self.modulelist)):
			x, y = self.modulelist[i].GetLocalPosition()
			xB, yB = self.modulelist[i].GetBasePosition()
			setPos = yB - int(scrollPos * totalHeight)
			self.modulelist[i].SetPosition(xB, setPos)
	
	def AppendModule(self, ModuleIndex, ModuleName):
		item = self.Item(self, ModuleIndex, ModuleName)
		item.SetSize(171, 34 + 5)

		if len(self.modulelist) == 0:
			item.SetPosition(0, 0)
			item.SetBasePosition(0, 0)
		else:
			x, y = self.modulelist[-1].GetLocalPosition()
			item.SetPosition(0, y + self.modulelist[-1].GetHeight())
			item.SetBasePosition(0, y + self.modulelist[-1].GetHeight())
		
		item.Show()
		self.modulelist.append(item)

class ScrollBar(ui.DragButton):
	def __init__(self):
		ui.DragButton.__init__(self)
		self.AddFlag("float")
		self.AddFlag("movable")
		self.AddFlag("restrict_x")
		self.SetUpVisual(UI_PATH + "base/scrollbar.tga")
		self.SetOverVisual(UI_PATH + "base/scrollbar.tga")
		self.SetDownVisual(UI_PATH + "base/scrollbar.tga")

		self.eventScroll = lambda *arg: None
		self.movearea = 0
		self.currentPos = 0.0

	def __del__(self):
		ui.DragButton.__del__(self)
		self.movearea = 0
		self.currentPos = 0.0
		self.eventScroll = lambda *arg: None

	def SetMovementArea(self, x, y, width, height):
		self.movearea = height - y - self.GetHeight()
		self.SetRestrictMovementArea(x, y, width, height)

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		yPos = float(pos * self.movearea)

		self.SetPosition(12, yPos)
		self.OnMove()

	def GetPos(self):
		return self.currentPos
		
	def OnMove(self):
		(xLocal, yLocal) = self.GetLocalPosition()
		self.currentPos = float(yLocal) / float(self.movearea) 

		self.eventScroll()

	def GetCurrentMission():
		for i in xrange(app.BIOLOGIST_MISSION_MAX_NUM):
			if player.GetBiologistState(i) != 0 and player.GetBiologistState(i) != 4:
				return i

		return 0

def import_pack_file(modulname, filename):	
	if modulname in sys.modules:
		return sys.modules[modulname]

	newmodule = _process_result(compile(pack_file(filename).read(), filename, 'exec'), modulname)	
	module_do(newmodule)
	return newmodule

def _process_result(code, fqname):
	is_module = isinstance(code, _ModuleType)
	if is_module:
		module = code
	else:
		module = imp.new_module(fqname)
	sys.modules[fqname] = module
	if not is_module:
		exec code in module.__dict__
	module = sys.modules[fqname]
	module.__name__ = fqname
	return module

_ModuleType = type(sys)

module_do = lambda x:None

class pack_file(object):
	def __init__(self, filename, mode = 'rb'):
		if not pack.Exist(filename):
			raise IOError, 'No file or directory'
		self.data = pack.Get(filename)

	def __iter__(self):
		return pack_file_iterator(self)

	def read(self, len = None):
		if not self.data:
			return ''
		if len:
			tmp = self.data[:len]
			self.data = self.data[len:]
			return tmp
		else:
			tmp = self.data
			self.data = ''
			return tmp