''' 1. '''
# Search
	LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)

# Replace with
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		LOCALE_UISCRIPT_PATH = "locale/common/ui/"
	else:
		LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)

''' 2. '''
# Search
	LOGIN_PATH = "%s/ui/login/" % (name)

# Replace with
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		LOGIN_PATH = "locale/common/ui/login/"
	else:
		LOGIN_PATH = "%s/ui/login/" % (name)

''' 3. '''
# Search
	GUILD_PATH = "%s/ui/guild/" % (name)

# Replace with
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		GUILD_PATH = "locale/common/ui/guild/"
	else:
		GUILD_PATH = "%s/ui/guild/" % (name)
