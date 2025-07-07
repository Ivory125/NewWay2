/// 1.
// Search
					// make when argument
					ostringstream os;
					while (lexstate.lookahead.token == '.')
					{
						next(&lexstate);
						os << '.';
						next(&lexstate);
						t = lexstate.t;
						// if (t.token == TK_STRING)
							// t.token = TK_NAME;
						os << t;
						lookahead(&lexstate);
					}

// Add below
					// ADAPTION_MULTI_LANGUAGE_SYSTEM
					/**
					* .gameforge[pc.get_language()][#]
					**/

					// Accept functions as valid arguments
					const char TK_OPEN_PARENTHESIS = '(', TK_CLOSE_PARENTHESIS = ')';
					const char TK_OPEN_BRACKETS = '[', TK_CLOSE_BRACKETS = ']';

					if (lexstate.lookahead.token == TK_OPEN_PARENTHESIS)
					{
						int funcDepth = 0;
						while (lexstate.lookahead.token != TK_CLOSE_PARENTHESIS || funcDepth > 1)
						{
							if (lexstate.lookahead.token == TK_OPEN_PARENTHESIS)
								funcDepth++;
							else if (lexstate.lookahead.token == TK_CLOSE_PARENTHESIS)
								funcDepth--;

							next(&lexstate);
							t = lexstate.t;
							os << t;
							lookahead(&lexstate);
						}
						os << TK_CLOSE_PARENTHESIS;
						lookahead(&lexstate);
					}
					// Accept arrays as valid arguments
					else if (lexstate.lookahead.token == TK_OPEN_BRACKETS)
					{
						int arrDepth = 0;
						while (lexstate.lookahead.token != TK_CLOSE_BRACKETS || arrDepth > 1)
						{
							if (lexstate.lookahead.token == TK_OPEN_BRACKETS)
								arrDepth++;
							else if (lexstate.lookahead.token == TK_CLOSE_BRACKETS)
								arrDepth--;

							next(&lexstate);
							t = lexstate.t;
							os << t;
							lookahead(&lexstate);
						}
						os << TK_CLOSE_BRACKETS;
						lookahead(&lexstate);
					}

					if (lexstate.lookahead.token == TK_OPEN_BRACKETS)
					{
						int arrDepth = 0;
						while (lexstate.lookahead.token != TK_CLOSE_BRACKETS || arrDepth > 1)
						{
							if (lexstate.lookahead.token == TK_OPEN_BRACKETS)
								arrDepth++;
							else if (lexstate.lookahead.token == TK_CLOSE_BRACKETS)
								arrDepth--;

							next(&lexstate);
							t = lexstate.t;
							os << t;
							lookahead(&lexstate);
						}
						os << TK_CLOSE_BRACKETS;
						lookahead(&lexstate);
					}
					else
					{
						// .gameforge[pc.get_language()].test._1_say
						while (lexstate.lookahead.token == '.')
						{
							next(&lexstate);
							os << '.';
							next(&lexstate);
							t = lexstate.t;
							// if (t.token == TK_STRING)
								// t.token = TK_NAME;
							os << t;
							lookahead(&lexstate);
						}
					}
					// END_OF_ADAPTION_MULTI_LANGUAGE_SYSTEM