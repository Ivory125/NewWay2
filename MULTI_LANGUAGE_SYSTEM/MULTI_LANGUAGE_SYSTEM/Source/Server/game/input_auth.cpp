/// 1.
// Search @ void CInputAuth::Login
	// CHANNEL_SERVICE_LOGIN
	if (Login_IsInChannelService(szLogin))
	{
		sys_log(0, "ChannelServiceLogin [%s]", szLogin);

		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
			"SELECT '%s', password, securitycode, social_id, id, status,"
			"availDt - NOW() > 0,"
			"UNIX_TIMESTAMP(silver_expire),"
			"UNIX_TIMESTAMP(gold_expire),"
			"UNIX_TIMESTAMP(safebox_expire),"
			"UNIX_TIMESTAMP(autoloot_expire),"
			"UNIX_TIMESTAMP(fish_mind_expire),"
			"UNIX_TIMESTAMP(marriage_fast_expire),"
			"UNIX_TIMESTAMP(money_drop_rate_expire),"
			"UNIX_TIMESTAMP(create_time)"
			" FROM account WHERE login='%s'",
			szPasswd, szLogin);
	}

// Replace with
	// CHANNEL_SERVICE_LOGIN
	if (Login_IsInChannelService(szLogin))
	{
		sys_log(0, "ChannelServiceLogin [%s]", szLogin);

		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
			"SELECT '%s', password, securitycode, social_id, id, status,"
#ifdef __MULTI_LANGUAGE_SYSTEM__
			"language,"
#endif
			"availDt - NOW() > 0,"
			"UNIX_TIMESTAMP(silver_expire),"
			"UNIX_TIMESTAMP(gold_expire),"
			"UNIX_TIMESTAMP(safebox_expire),"
			"UNIX_TIMESTAMP(autoloot_expire),"
			"UNIX_TIMESTAMP(fish_mind_expire),"
			"UNIX_TIMESTAMP(marriage_fast_expire),"
			"UNIX_TIMESTAMP(money_drop_rate_expire),"
			"UNIX_TIMESTAMP(create_time)"
			" FROM account WHERE login='%s'",
			szPasswd, szLogin);
	}

/// 2.
// Search @ void CInputAuth::Login
	// END_OF_CHANNEL_SERVICE_LOGIN
	else
	{
		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
			"SELECT PASSWORD('%s'), password, securitycode, social_id, id, status,"
			"availDt - NOW() > 0,"
			"UNIX_TIMESTAMP(silver_expire),"
			"UNIX_TIMESTAMP(gold_expire),"
			"UNIX_TIMESTAMP(safebox_expire),"
			"UNIX_TIMESTAMP(autoloot_expire),"
			"UNIX_TIMESTAMP(fish_mind_expire),"
			"UNIX_TIMESTAMP(marriage_fast_expire),"
			"UNIX_TIMESTAMP(money_drop_rate_expire),"
			"UNIX_TIMESTAMP(create_time)"
			" FROM account WHERE login='%s'",
			szPasswd, szLogin);
	}

// Replace with
	// END_OF_CHANNEL_SERVICE_LOGIN
	else
	{
		DBManager::instance().ReturnQuery(QID_AUTH_LOGIN, dwKey, p,
			"SELECT PASSWORD('%s'), password, securitycode, social_id, id, status,"
#ifdef __MULTI_LANGUAGE_SYSTEM__
			"language,"
#endif
			"availDt - NOW() > 0,"
			"UNIX_TIMESTAMP(silver_expire),"
			"UNIX_TIMESTAMP(gold_expire),"
			"UNIX_TIMESTAMP(safebox_expire),"
			"UNIX_TIMESTAMP(autoloot_expire),"
			"UNIX_TIMESTAMP(fish_mind_expire),"
			"UNIX_TIMESTAMP(marriage_fast_expire),"
			"UNIX_TIMESTAMP(money_drop_rate_expire),"
			"UNIX_TIMESTAMP(create_time)"
			" FROM account WHERE login='%s'",
			szPasswd, szLogin);
	}