''' 1. '''
# Search @ class LoginWindow.Open
		app.ShowCursor()

# Add below
		if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
			import uiSystemOption
			localeName = app.GetLoca()
			for i, k in uiSystemOption.LOCALE_LANG_DICT.items():
				if localeName in uiSystemOption.LOCALE_LANG_DICT[i]["locale"]:
					net.SetLanguage(i)
					break