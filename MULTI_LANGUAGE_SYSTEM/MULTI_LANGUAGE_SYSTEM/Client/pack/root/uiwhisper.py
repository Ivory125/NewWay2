''' 1. '''
# Search
import chr

# Add below
if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
	import messenger, uiSystemOption

''' 2. '''
# Search @ class WhisperDialog.__init__
		self.eventAcceptTarget = None

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryID = 0

''' 3. '''
# Search @ class WhisperDialog.LoadDialog
		self.acceptButton.SetEvent(ui.__mem_func__(self.AcceptTarget))

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.countryFlag = ui.ExpandedImageBox()
			self.countryFlag.SetParent(self.board)
			self.countryFlag.SetPosition(130, 28)
			self.countryFlag.Show()

''' 4. '''
# Search @ class WhisperDialog.Destroy
		self.resizeButton = None

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryFlag = None
			self.countryID = 0

''' 5. '''
# Search
	def SetChatLineMax(self, max):

# Add above
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
		def RequestCountryFlag(self, targetName):
			if targetName:
				net.SendWhisperDetails(targetName)

			if self.countryFlag:
				fixedWidth = 280
				if messenger.IsFriendByName(targetName):
					self.countryFlag.SetPosition(fixedWidth - 159, 13.5)
				else:
					self.countryFlag.SetPosition(fixedWidth - 159, 13.5)

		def SetCountryFlag(self, countryID):
			self.countryID = countryID

			if countryID != 0:
				self.countryFlag.LoadImage("d:/ymir work/ui/game/flag/%s.tga" % uiSystemOption.LOCALE_LANG_DICT[countryID]["locale"])
				self.countryFlag.Show()
			else:
				self.countryFlag.Hide()

''' 6. '''
# Search @ OpenWithTarget
		self.titleNameEdit.Hide()
		self.ignoreButton.Hide()

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.RequestCountryFlag(targetName)
			self.countryFlag.Hide()

''' 7. '''
# Search @ OpenWithoutTarget
		self.titleNameEdit.Show()
		self.ignoreButton.Hide()

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
			self.countryFlag.Hide()

''' 8. '''
# Search @ AcceptTarget
			self.eventAcceptTarget(name)

# Add below
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM and app.ENABLE_EXTENDED_WHISPER_DETAILS:
				self.RequestCountryFlag(name)