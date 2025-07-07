/// 1.
// Search @ bool LoadLocaleData
	snprintf(szItemList, sizeof(szItemList), "%s/item_list.txt", localePath);
	snprintf(szSkillTableFileName, sizeof(szSkillTableFileName), "%s/SkillTable.txt", localePath);

// Replace with
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	snprintf(szItemList, sizeof(szItemList), "locale/common/item_list.txt");
	snprintf(szSkillTableFileName, sizeof(szSkillTableFileName), "locale/common/SkillTable.txt");
#else
	snprintf(szItemList, sizeof(szItemList), "%s/item_list.txt", localePath);
	snprintf(szSkillTableFileName, sizeof(szSkillTableFileName), "%s/SkillTable.txt", localePath);
#endif