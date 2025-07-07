''' 1. '''
# Search all
	FN_GM_MARK = "%s/effect/gm.mse" % app.GetLocalePath()

# Replace all with
	if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
		FN_GM_MARK = "locale/common/effect/gm.mse"
	else:
		FN_GM_MARK = "%s/effect/gm.mse" % app.GetLocalePath()