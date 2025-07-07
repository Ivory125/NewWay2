''' 1. '''
# Search @ class GuildWindow.RefreshGuildMemberPageMemberList
			gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))

			if 1 != grade:
				gradeComboBox.Enable()

# Replace with
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if line > 0:
					gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
				else:
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_INFO_MASTER_VALUE)
			else:
				gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))

			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if guild.GetGradeName(grade) == "...":
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_BANKINFO_NAME)

''' 2. '''
# Search @ class GuildWindow.RefreshGuildMemberPageGradeComboBox
			gradeComboBox.ClearItem()
			for i in xrange(self.CAN_CHANGE_GRADE_COUNT):
				gradeComboBox.InsertItem(i + 2, guild.GetGradeName(i + 2))
			gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
			if 1 != grade:
				gradeComboBox.Enable()

# Replace with
			gradeComboBox.ClearItem()
			for i in xrange(self.CAN_CHANGE_GRADE_COUNT):
				if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
					if guild.GetGradeName(i + 2) == "...":
						gradeComboBox.InsertItem(i + 2, uiScriptLocale.GUILD_BANKINFO_NAME)
					else:
						gradeComboBox.InsertItem(i + 2, guild.GetGradeName(i + 2))
				else:
					gradeComboBox.InsertItem(i + 2, guild.GetGradeName(i + 2))
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if guild.GetGradeName(grade) == "...":
					gradeComboBox.SetCurrentItem(uiScriptLocale.GUILD_BANKINFO_NAME)
				else:
					gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
			else:
				gradeComboBox.SetCurrentItem(guild.GetGradeName(grade))
			if 1 != grade:
				gradeComboBox.Enable()

''' 3. '''
# Search
			slotList[self.GRADE_SLOT_NAME].SetText(name)

# Replace with
			if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
				if key > 1:
					if name == "...":
						slotList[self.GRADE_SLOT_NAME].SetText(uiScriptLocale.GUILD_BANKINFO_NAME)
					else:
						slotList[self.GRADE_SLOT_NAME].SetText(name)
				else:
					slotList[self.GRADE_SLOT_NAME].SetText(uiScriptLocale.GUILD_INFO_MASTER_VALUE)
			else:
				slotList[self.GRADE_SLOT_NAME].SetText(name)