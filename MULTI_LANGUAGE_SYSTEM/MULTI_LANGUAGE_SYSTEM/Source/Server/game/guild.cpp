/// 1.
// Search @ CGuild::CGuild
	strlcpy(m_data.grade_array[0].grade_name, LC_TEXT("길드장"), sizeof(m_data.grade_array[0].grade_name));

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	strlcpy(m_data.grade_array[0].grade_name, "Leader", sizeof(m_data.grade_array[0].grade_name));
#else
	strlcpy(m_data.grade_array[0].grade_name, LC_TEXT("길드장"), sizeof(m_data.grade_array[0].grade_name));
#endif

/// 2.
// Search @ CGuild::CGuild
		strlcpy(m_data.grade_array[i].grade_name, LC_TEXT("길드원"), sizeof(m_data.grade_array[i].grade_name));

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
		strlcpy(m_data.grade_array[i].grade_name, "...", sizeof(m_data.grade_array[i].grade_name));
#else
		strlcpy(m_data.grade_array[i].grade_name, LC_TEXT("길드원"), sizeof(m_data.grade_array[i].grade_name));
#endif