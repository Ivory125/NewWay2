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

IMG_PATH = "d:/ymir work/ui/aslan/adminpanel/xx/"

class MainWindow(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)

	def Show(self):
		ui.BoardWithTitleBar.Show(self)
		self.SetTop()
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		
	def __LoadWindow(self):
		self.SetSize(430, 370)
		self.AddFlag('float')
		self.SetTitleName("TEMPLATE-WINDOW")
		self.CloseButtonHide()

	def OnPressEscapeKey(self):
		self.Close()
		return True