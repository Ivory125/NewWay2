''' 1. '''
# Search
blockMode = 0

# Add above
if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	import sys
	import uiScriptLocale
	import wndMgr

	def ReloadModule(name):
		if name in sys.modules:
			del sys.modules[name]

			if name in locals():
				del locals()[name]

			if name in globals():
				del globals()[name]

			module = __import__(name)

			if not module in locals():
				locals()[name] = module

			if not module in globals():
				globals()[name] = module

	LOCALE_LANG_DICT = {
		app.LOCALE_EN : { "name" : uiScriptLocale.LANGUAGE_EN, "locale" : "en", "code_page" : 1252 },
		app.LOCALE_PT : { "name" : uiScriptLocale.LANGUAGE_PT, "locale" : "pt", "code_page" : 1252 },
		app.LOCALE_ES : { "name" : uiScriptLocale.LANGUAGE_ES, "locale" : "es", "code_page" : 1252 },
		app.LOCALE_FR : { "name" : uiScriptLocale.LANGUAGE_FR, "locale" : "fr", "code_page" : 1252 },
		app.LOCALE_DE : { "name" : uiScriptLocale.LANGUAGE_DE, "locale" : "de", "code_page" : 1252 },
		app.LOCALE_RO : { "name" : uiScriptLocale.LANGUAGE_RO, "locale" : "ro", "code_page" : 1250 },
		app.LOCALE_PL : { "name" : uiScriptLocale.LANGUAGE_PL, "locale" : "pl", "code_page" : 1250 },
		app.LOCALE_IT : { "name" : uiScriptLocale.LANGUAGE_IT, "locale" : "it", "code_page" : 1252 },
		app.LOCALE_CZ : { "name" : uiScriptLocale.LANGUAGE_CZ, "locale" : "cz", "code_page" : 1250 },
		app.LOCALE_HU : { "name" : uiScriptLocale.LANGUAGE_HU, "locale" : "hu", "code_page" : 1250 },
		app.LOCALE_TR : { "name" : uiScriptLocale.LANGUAGE_TR, "locale" : "tr", "code_page" : 1254 },
	}

''' 2. '''
# Search @ def __init__
		self.__Load()

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.__CreateLanguageSelectWindow()

''' 3. '''
# Search
		self.ctrlShadowQuality = 0

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.language_change_window = None
			self.language_select_button = None
			self.language_change_button = None

			self.cur_language_text = None
			self.cur_language_text_window = None

			self.language_button_dict = {}
			self.language_select_list_open = False
			self.language_select_window_height = 0

			self.language_select_window_pos = ((wndMgr.GetScreenWidth() / 2) - 153, (wndMgr.GetScreenHeight() / 2) - 231 - 32)
			self.language_select_window_bar = None

			self.mouse_over_image = None

			self.selected_language = 0

''' 4. '''
# Search @ def __Load_BindObject
			self.cameraModeButtonList.append(GetObject("camera_short"))
			self.cameraModeButtonList.append(GetObject("camera_long"))

# Add below
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				self.language_select_button = self.GetChild("language_select_button")
				self.language_change_window = self.GetChild("language_change_window")
				self.cur_language_text = self.GetChild("cur_language_text")
				self.cur_language_text_window = self.GetChild("cur_language_text_window")
				self.language_change_button = self.GetChild("language_change_button")

''' 5. '''
# Search
		if musicInfo.fieldMusic == musicInfo.METIN2THEMA:
			self.selectMusicFile.SetText(uiSelectMusic.DEFAULT_THEMA)
		else:
			self.selectMusicFile.SetText(musicInfo.fieldMusic[:MUSIC_FILENAME_MAX_LEN])

# Add above
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			if self.language_select_button:
				self.language_select_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))
				self.cur_language_text_window.SetOnMouseLeftButtonUpEvent(ui.__mem_func__(self.__OnClickLanguageSelectButton))

			if self.language_change_button:
				self.language_change_button.SetEvent(ui.__mem_func__(self.__OnClickLanguageChangeButton))

			self.mouse_over_image = self.GetChild("mouse_over_image")
			self.mouse_over_image.Hide()

''' 6. '''
# Search @ def Close
		self.Hide()
		self.IsShow = False

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			self.__LanguageSelectShowHide(False)

''' 7. '''
# Add to the bottom of the document
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		""" Based on dumped meta data"""
		# 2019.12.28.OSP (aka. Owsap)
		# Multi Language
		def LanguageChange(self):
			if self.selected_language != 0:
				locale = LOCALE_LANG_DICT[self.selected_language]["locale"]
				code_page = LOCALE_LANG_DICT[self.selected_language]["code_page"]

				if self.__SaveLoca(code_page, locale):
					# Notes:
					# Selected locale may not load the full pack file, regarding map names, some locale text and other images.
					# A full client restart is required.
					ReloadModule("localeInfo")
					ReloadModule("uiScriptLocale")
					ReloadModule("introLogin")
					if constInfo2.NEW_SELECT_UI == True:
						ReloadModule("New_introSelect")
						ReloadModule("New_introCreate")
					else:
						ReloadModule("introSelect")
						ReloadModule("introCreate")
					ReloadModule("introEmpire")
					# -------------------------------------------------------------------------------------------------------

					net.SendChangeLanguagePacket(self.selected_language)
					net.ExitGame() # net.ExitGameLanguageChange() # ReloadModule

		def __AdjustLanguageSelectWindowPosition(self):
			x, y = self.language_select_window_pos

			(lx, ly) = self.language_change_window.GetLocalPosition()
			if self.language_select_window_bar:
				self.language_select_window_bar.SetPosition(x + lx + 30, y + ly + 36)

		def __CreateLanguageSelectWindow(self):
			if self.language_button_dict:
				return

			languageList = LOCALE_LANG_DICT
			if not LOCALE_LANG_DICT:
				return

			self.cur_language_text.SetText(LOCALE_LANG_DICT[self.__GetCurLanguageKey()]["name"])

			button_height = 16
			dict_len = len(languageList)
			self.language_select_window_height = dict_len * button_height

			self.language_select_window_bar = ui.Bar("TOP_MOST")
			self.language_select_window_bar.SetSize(210, self.language_select_window_height)
			self.language_select_window_bar.Hide()

			for index, key in enumerate(LOCALE_LANG_DICT):
				button = ui.Button()
				button.SetParent(self.language_select_window_bar)
				button.SetPosition(0, button_height * index)

				if 1 == dict_len:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")
				elif index == 0:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")
				elif index == dict_len - 1:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_bottom.sub")
				else:
					button.SetUpVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetDownVisual("d:/ymir work/ui/quest_re/button_middle.sub")
					button.SetOverVisual("d:/ymir work/ui/quest_re/button_middle.sub")

				button.SetEvent(ui.__mem_func__(self.__OnClickLanguageSelect), key)
				button.SetOverEvent(ui.__mem_func__(self.__OnClickLanguageButtonOver), key)
				button.SetOverOutEvent(ui.__mem_func__(self.__OnClickLanguageButtonOverOut), key)
				button.SetText(LOCALE_LANG_DICT[key]["name"])
				button.Hide()

				self.language_button_dict[key] = button

			self.mouse_over_image.SetParent(self.language_select_window_bar)

			self.__AdjustLanguageSelectWindowPosition()

		def __GetCurLanguageKey(self):
			for key in LOCALE_LANG_DICT.keys():
				localeName = LOCALE_LANG_DICT[key]["locale"]
				if app.GetLocaleName() == localeName:
					return key

		def __GetStringCurLanguage(self):
			for key in LOCALE_LANG_DICT.keys():
				localeName = LOCALE_LANG_DICT[key]["locale"]
				if app.GetLocaleName() == localeName:
					return localeName

		def __LanguageSelectShowHide(self, is_show):
			if True == is_show:
				self.language_select_list_open = True

				if self.language_select_window_bar:
					self.language_select_window_bar.SetSize(210, self.language_select_window_height)
					self.language_select_window_bar.Show()

				for button in self.language_button_dict.values():
					button.Show()
			else:
				self.language_select_list_open = False

				if self.language_select_window_bar:
					self.language_select_window_bar.SetSize(210, 0)

				for button in self.language_button_dict.values():
					button.Hide()

		def __OnClickLanguageButtonOver(self, index):
			if not self.mouse_over_image:
				return

			button = self.language_button_dict.get(index, 0)
			if 0 == button:
				return

			(button_x, button_y) = button.GetLocalPosition()
			self.mouse_over_image.SetPosition(button_x, button_y)
			self.mouse_over_image.Show()

		def __OnClickLanguageButtonOverOut(self, index):
			if not self.mouse_over_image:
				return

			self.mouse_over_image.Hide()

		def __OnClickLanguageChangeButton(self):
			if self.__GetCurLanguageKey() == self.selected_language:
				return

			if self.selected_language != 0:
				if constInfo2.MULTI_LANGUAGE_NEED_RESTART_CLIENT == True:
					self.ConfirmLanguageChange()
				else:
					self.LanguageChange()

		def __OnClickLanguageSelect(self, index):
			for button in self.language_button_dict.values():
				button.Hide()

			self.__LanguageSelectShowHide(False)

			self.selected_language = index

			if self.cur_language_text:
				self.cur_language_text.SetText(LOCALE_LANG_DICT[index]["name"])

		def __OnClickLanguageSelectButton(self):
			self.__CreateLanguageSelectWindow()

			if self.language_select_list_open:
				self.__LanguageSelectShowHide(False)
			else:
				self.__LanguageSelectShowHide(True)

		def __OnLanguageSelectScroll(self):
			pass # todo for > 5 languages

		def __SaveLoca(self, code_page, locale):
			if app.SetLoca(code_page, locale):
				return True

			return False

		def OnTop(self):
			if self.language_select_window_bar:
				self.language_select_window_bar.SetTop()

		def OnMoveWindow(self, x, y):
			self.language_select_window_pos = x, y

			self.__AdjustLanguageSelectWindowPosition()

		def ConfirmLanguageChange(self):
			questionDialog = uiCommon.QuestionDialog2()
			questionDialog.SetText1(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_1)
			questionDialog.SetText2(localeInfo.RESTART_CLIENT_DO_YOU_ACCEPT_2)
			questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnAcceptLanguageChange))
			questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			questionDialog.SetWidth(450)
			questionDialog.Open()
			self.questionDialog = questionDialog

		def OnEndCountDown(self):
			net.Disconnect()
			app.Exit()

		def OnPressExitKey(self):
			pass

		def OnAcceptLanguageChange(self):
			self.OnCloseQuestionDialog()

			if self.selected_language != 0:
				locale = LOCALE_LANG_DICT[self.selected_language]["locale"]
				code_page = LOCALE_LANG_DICT[self.selected_language]["code_page"]

				if self.__SaveLoca(code_page, locale):
					net.SendChangeLanguagePacket(self.selected_language)

					import introLogin
					self.popUpTimer = introLogin.ConnectingDialog()
					self.popUpTimer.Open(3.0)
					self.popUpTimer.SetText(localeInfo.LEFT_TIME)
					self.popUpTimer.SAFE_SetTimeOverEvent(self.OnEndCountDown)
					self.popUpTimer.SAFE_SetExitEvent(self.OnPressExitKey)
			else:
				pass # no language selected