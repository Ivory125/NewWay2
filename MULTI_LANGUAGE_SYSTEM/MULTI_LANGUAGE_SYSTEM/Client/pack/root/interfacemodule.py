''' 1. '''
# Search
	## ĳ���� �޴��� 1:1 ��ȭ �ϱ⸦ �������� �̸��� ������ �ٷ� â�� ���� �Լ�
	def OpenWhisperDialog(self, name):

# Add above
	if app.ENABLE_EXTENDED_WHISPER_DETAILS and app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		def RecieveWhisperDetails(self, name, country):
			if self.whisperDialogDict.has_key(name):
				self.whisperDialogDict[name].SetCountryFlag(country)
			else:
				btn = self.__FindWhisperButton(name)
				if btn != 0:
					btn.countryID = country