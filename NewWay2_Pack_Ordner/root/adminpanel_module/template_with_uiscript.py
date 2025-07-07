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
UI_SCRIPT_FILE = "adminpanel_module/uiscript/TEMPLATE.py"

class MainWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
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
			pyScrLoader.LoadScriptFile(self, UI_SCRIPT_FILE)
		except:
			import exception
			exception.Abort("TEMPLATEWindow.LoadWindow.LoadObject")
			
		try:
			self.GetChild("board").CloseButtonHide()

		except:
			import exception
			exception.Abort("TEMPLATEWindow.LoadWindow.BindObject")

	def OnPressEscapeKey(self):
		self.Close()
		return True