/// 1.
// Search
	void	Notice(const char* msg);

// Replace with
#ifdef __MULTI_LANGUAGE_SYSTEM__
	void	Notice(const char* msg, ...);
#else
	void	Notice(const char* msg);
#endif