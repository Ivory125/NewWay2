import ui
import wndMgr
import grp
import item
## Ente ente ente ente

class ExampleTabs(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)
		self.isLoaded = FALSE
		if FALSE == self.isLoaded:
			self.__LoadMe()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
		
	def __LoadMe(self):
		self.SetSize(300, 300)
		self.SetCenterPosition()
		self.AddFlag('movable')
		self.SetTitleName("Example Tabs by Ente")
		self.SetCloseEvent(self.Close)
		
		thinBoard = ui.ThinBoard()
		thinBoard.SetParent(self)
		thinBoard.SetSize(300-15-15, 50)
		thinBoard.SetPosition(15, 35)
		thinBoard.Show()
		self.thinBoard = thinBoard

		## Create 5 Pages (tabs)
		## Create Page 1
		tabOnePage = ui.ThinBoard()
		tabOnePage.SetParent(self)
		tabOnePage.SetSize(300-15-15, 200-15)
		tabOnePage.SetPosition(15, 35+50+15)
		tabOnePage.Hide()
		self.tabOnePage = tabOnePage
		
		## Add Button to Page 1
		button = ui.Button()
		button.SetParent(self.tabOnePage)
		button.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
		button.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
		button.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
		button.SetText("Button")
		button.SetPosition(20,20)
		button.Show()
		self.button = button
		
		## Create Page 2
		tabTwoPage = ui.ThinBoard()
		tabTwoPage.SetParent(self)
		tabTwoPage.SetSize(300-15-15, 200-15)
		tabTwoPage.SetPosition(15, 35+50+15)
		tabTwoPage.Hide()
		self.tabTwoPage = tabTwoPage
		
		## Add Button to Page 2
		buttonExit = ui.Button()
		buttonExit.SetParent(self.tabTwoPage)
		buttonExit.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
		buttonExit.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
		buttonExit.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
		buttonExit.SetText("Exit")
		buttonExit.SetEvent(self.Close)
		buttonExit.SetPosition(50,50)
		buttonExit.Show()
		self.buttonExit = buttonExit
		
		## Create Page 3
		tabThreePage = ui.ThinBoard()
		tabThreePage.SetParent(self)
		tabThreePage.SetSize(300-15-15, 200-15)
		tabThreePage.SetPosition(15, 35+50+15)
		tabThreePage.Hide()
		self.tabThreePage = tabThreePage
		
		## Add TextLine to Page 3
		textLine = ui.TextLine()
		textLine.SetParent(self.tabThreePage)
		textLine.SetPosition(15, 35)
		textLine.SetSize(20, 20)
		textLine.SetText('Hey :) Tab 3')
		textLine.Show()
		self.textLine = textLine
		
		## Create Page 4
		tabFourPage = ui.ThinBoard()
		tabFourPage.SetParent(self)
		tabFourPage.SetSize(300-15-15, 200-15)
		tabFourPage.SetPosition(15, 35+50+15)
		tabFourPage.Hide()
		self.tabFourPage = tabFourPage
		
		## Add DragButton to Page 4
		dragButton = ui.DragButton()
		dragButton.SetParent(self.tabFourPage)
		dragButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
		dragButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
		dragButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
		dragButton.SetText("Drag me")
		dragButton.SetPosition(0, 0)
		dragButton.SetRestrictMovementArea(0, 0, self.tabFourPage.GetWidth(), self.tabFourPage.GetHeight())
		dragButton.Show()
		self.dragButton = dragButton
		
		## Create empty Page 5
		tabFivePage = ui.ThinBoard()
		tabFivePage.SetParent(self)
		tabFivePage.SetSize(300-15-15, 200-15)
		tabFivePage.SetPosition(15, 35+50+15)
		tabFivePage.Hide()
		self.tabFivePage = tabFivePage
		
		## Add 5 Pages
		self.pages = []
		self.pages.append(self.tabOnePage)
		self.pages.append(self.tabTwoPage)
		self.pages.append(self.tabThreePage)
		self.pages.append(self.tabFourPage)
		self.pages.append(self.tabFivePage)

		## Create 5 RadioButtons for tabs
		self.curTab = 0
		tabButtons = []
		for i in xrange(5):
			radioButton = ui.RadioButton()
			radioButton.SetParent(self.thinBoard)
			radioButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
			radioButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
			radioButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
			radioButton.SetText("Tab %d"%i)
			radioButton.SetPosition(10+52*i, 0)
			radioButton.SetWindowVerticalAlignCenter()
			radioButton.Show()
			tabButtons.append(radioButton)
		
		self.tabButtonGroup = ui.RadioButtonGroup.Create([
		##	[button, selectEvent, unselectEvent],
			[tabButtons[0], lambda : self._OnClickTabButton(0), None], 
			[tabButtons[1], lambda : self._OnClickTabButton(1), None], 
			[tabButtons[2], lambda : self._OnClickTabButton(2), None],
			[tabButtons[3], lambda : self._OnClickTabButton(3), None],
			[tabButtons[4], lambda : self._OnClickTabButton(4), None]
		])
		
		self.isLoaded = True
		
	def _OnClickTabButton(self, id):
		self.SetCurTab(id)

	def GetCurTab(self):
		return self.curTab

	def SetCurTab(self, tab):
		self.curTab = tab
		self.RefreshTab()
		
	def RefreshTab(self):
		for page in self.pages:
			page.Hide()
		self.pages[self.GetCurTab()].Show()

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True

	def OpenWndWithF7Key():
		wnd.Open()

app.onPressKeyDict[app.DIK_F7] = OpenWndWithF7Key

wnd = ExampleTabs()
wnd.Open()