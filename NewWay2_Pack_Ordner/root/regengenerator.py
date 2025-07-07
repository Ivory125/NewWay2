import ui
import dbg
import app
import constInfo
class Dialog1(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.BuildWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def BuildWindow(self):
		self.Board = ui.BoardWithTitleBar()
		self.Board.SetSize(305, 179)
		self.Board.SetCenterPosition()
		self.Board.AddFlag('movable')
		self.Board.AddFlag('float')
		self.Board.SetTitleName('Settings')
		self.Board.SetCloseEvent(self.Close)
		self.Board.Show()
		self.comp = Component()

		self.save = self.comp.Button(self.Board, 'Speichern', '', 15, 129, self.save_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.close = self.comp.Button(self.Board, 'Schlieﬂen', '', 197, 129, self.close_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.reset = self.comp.Button(self.Board, 'Reset', '', 106, 129, self.reset_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.slotbar_typinput, self.typinput = self.comp.EditLine(self.Board, 'm', 66, 42, 60, 15, 10)
		self.slotbar_umkreisinput, self.umkreisinput = self.comp.EditLine(self.Board, '10', 192, 42, 60, 15, 10)
		self.slotbar_timeinput, self.timeinput = self.comp.EditLine(self.Board, '1s', 66, 82, 60, 15, 10)
		self.slotbar_idvnum, self.idvnum = self.comp.EditLine(self.Board, '', 192, 81, 60, 15, 10)
		self.typlabel = self.comp.TextLine(self.Board, 'Typ:', 25, 42, self.comp.RGB(255, 255, 255))
		self.umkr = self.comp.TextLine(self.Board, 'Umkreis:', 145, 42, self.comp.RGB(255, 255, 255))
		self.vnuml = self.comp.TextLine(self.Board, 'Monster:', 145, 83, self.comp.RGB(255, 255, 255))
		self.timelabel = self.comp.TextLine(self.Board, 'Zeit:', 25, 83, self.comp.RGB(255, 255, 255))
	
	def save_func(self):
		a = "/"
		constInfo.regengenerator = str(self.typinput.GetText())+a+str(self.umkreisinput.GetText())+a+str(self.timeinput.GetText())+a+str(self.idvnum.GetText())
		self.Board.Hide()

	def close_func(self):
		self.Board.Hide()
	
	def reset_func(self):
		constInfo.regengenerator = "0/0/0/0"
		self.umkreisinput.SetText("10")
		self.typinput.SetText("m")
		self.idvnum.SetText("")
		self.umkreisinput.SetText("10")

		return True
	
	def Close(self):
		self.Board.Hide()

class Component:
	def Button(self, parent, buttonName, tooltipText, x, y, func, UpVisual, OverVisual, DownVisual):
		button = ui.Button()
		if parent != None:
			button.SetParent(parent)
		button.SetPosition(x, y)
		button.SetUpVisual(UpVisual)
		button.SetOverVisual(OverVisual)
		button.SetDownVisual(DownVisual)
		button.SetText(buttonName)
		button.SetToolTipText(tooltipText)
		button.Show()
		button.SetEvent(func)
		return button

	def EditLine(self, parent, editlineText, x, y, width, heigh, max):
		SlotBar = ui.SlotBar()
		if parent != None:
			SlotBar.SetParent(parent)
		SlotBar.SetSize(width, heigh)
		SlotBar.SetPosition(x, y)
		SlotBar.Show()
		Value = ui.EditLine()
		Value.SetParent(SlotBar)
		Value.SetSize(width, heigh)
		Value.SetPosition(1, 1)
		Value.SetMax(max)
		Value.SetLimitWidth(width)
		Value.SetMultiLine()
		Value.SetText(editlineText)
		Value.Show()
		return SlotBar, Value

	def TextLine(self, parent, textlineText, x, y, color):
		textline = ui.TextLine()
		if parent != None:
			textline.SetParent(parent)
		textline.SetPosition(x, y)
		if color != None:
			textline.SetFontColor(color[0], color[1], color[2])
		textline.SetText(textlineText)
		textline.Show()
		return textline

	def RGB(self, r, g, b):
		return (r*255, g*255, b*255)

