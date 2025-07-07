/// 1.
// Search @ void CInputLogin::Entergame
	SECTREE_MANAGER::instance().SendNPCPosition(ch);

// Replace with
#if !defined(__BINARY_ATLAS_MARK_INFO__) && !defined(__MULTI_LANGUAGE_SYSTEM__)
	SECTREE_MANAGER::instance().SendNPCPosition(ch);
#endif