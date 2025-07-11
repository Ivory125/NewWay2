import uiScriptLocale
import app

QUEST_ICON_BACKGROUND = 'd:/ymir work/ui/game/quest/slot_base.sub'

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_00.sub"
MIDDLE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
ICON_SLOT_FILE = "d:/ymir work/ui/public/Slot_Base.sub"
FACE_SLOT_FILE = "d:/ymir work/ui/game/windows/box_face.sub"
ROOT_PATH = "d:/ymir work/ui/game/windows/"

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH


PATTERN_PATH = "d:/ymir work/ui/pattern/"
QUEST_BOARD_WINDOW_WIDTH	= 231
QUEST_BOARD_WINDOW_HEIGHT	= 297
QUEST_BOARD_PATTERN_X_COUNT = 12
QUEST_BOARD_PATTERN_Y_COUNT = 16

window = {
	"name" : "CharacterWindow",
	"style" : ("movable", "float",),

	"x" : 24,
	"y" : (SCREEN_HEIGHT - 37 - 361) / 2,

	"width" : 253,
	"height" : 361,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 253,
			"height" : 361,

			"children" :
			[
				{
					"name" : "Skill_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : 238,
					"color" : "red",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_SKILL, "all_align":"center" },
						#{ "name":"TitleName", "type":"image", "style" : ("attach",), "x":101, "y" : 1, "image" : LOCALE_PATH+"title_skill.sub", },
					),
				},
				{
					"name" : "Emoticon_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : 238,
					"color" : "red",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_ACTION, "all_align":"center" },
					),
				},
				{
					"name" : "Quest_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : 238,
					"color" : "red",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_QUEST, "all_align":"center" },
					),
				},

				## Tab Area
				{
					"name" : "TabControl",
					"type" : "window",

					"x" : 0,
					"y" : 328,

					"width" : 250,
					"height" : 31,

					"children" :
					(
						## Tab
						{
							"name" : "Tab_01",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 250,
							"height" : 31,

							"image" : LOCALE_PATH+"tab_1.sub",
						},
						{
							"name" : "Tab_02",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 250,
							"height" : 31,

							"image" : LOCALE_PATH+"tab_2.sub",
						},
						{
							"name" : "Tab_03",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 250,
							"height" : 31,

							"image" : LOCALE_PATH+"tab_3.sub",
						},
						{
							"name" : "Tab_04",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 250,
							"height" : 31,

							"image" : LOCALE_PATH+"tab_4.sub",
						},
						## RadioButton
						{
							"name" : "Tab_Button_01",
							"type" : "radio_button",

							"x" : 6,
							"y" : 5,

							"width" : 53,
							"height" : 27,
						},
						{
							"name" : "Tab_Button_02",
							"type" : "radio_button",

							"x" : 61,
							"y" : 5,

							"width" : 67,
							"height" : 27,
						},
						{
							"name" : "Tab_Button_03",
							"type" : "radio_button",

							"x" : 130,
							"y" : 5,

							"width" : 61,
							"height" : 27,
						},
						{
							"name" : "Tab_Button_04",
							"type" : "radio_button",

							"x" : 192,
							"y" : 5,

							"width" : 55,
							"height" : 27,
						},
					),
				},

				## Page Area
				{
					"name" : "Character_Page",
					"type" : "window",
					"style" : ("attach",),

					"x" : 0,
					"y" : 0,

					"width" : 250,
					"height" : 304,

					"children" :
					(

						## Title Area
						{
							"name" : "Character_TitleBar", "type" : "titlebar", "style" : ("attach",), "x" : 61, "y" : 7, "width" : 185, "color" : "red",
							"children" :
							(
								#{ "name" : "TitleName", "type" : "image", "style" : ("attach",), "x" : 70, "y" : 1, "image" : LOCALE_PATH+"title_status.sub", },
								{ "name" : "TitleName", "type":"text", "x":0, "y":-1, "text":uiScriptLocale.CHARACTER_MAIN, "all_align":"center" },
							),
						},

						## Guild Name Slot
						{
							"name" : "Guild_Name_Slot",
							"type" : "image",
							"x" : 155,
							"y" :27+7,
							"image" : LARGE_VALUE_FILE,

							"children" :
							(
								{
									"name" : "Guild_Name",
									"type":"text",
									"text":"길드 이름",
									"x":0,
									"y":0,
									"r":1.0,
									"g":1.0,
									"b":1.0,
									"a":1.0,
									"all_align" : "center",
								},
							),
						},

						## Character Name Slot
						{
							"name" : "Character_Name_Slot",
							"type" : "image",
							"x" : 153,
							"y" :27+7,
							"image" : LARGE_VALUE_FILE,

							"children" :
							(
								{
									"name" : "Character_Name",
									"type":"text",
									"text":"캐릭터 이름",
									"x":0,
									"y":0,
									"r":1.0,
									"g":1.0,
									"b":1.0,
									"a":1.0,
									"all_align" : "center",
								},
							),
						},

						## Header
						{ 
							"name":"Status_Header", "type":"window", "x":3, "y":31, "width":0, "height":0, 
							"children" :
							(
								## Lv
								{
									"name":"Status_Lv", "type":"window", "x":9, "y":30, "width":37, "height":42, 
									"children" :
									(
										{ "name":"Level_Header", "type":"image", "x":0, "y":0, "image":LOCALE_PATH+"label_level.sub" },
										{ "name":"Level_Value", "type":"text", "x":19, "y":19, "fontsize":"LARGE", "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},

								## EXP
								{
									"name":"Status_CurExp", "type":"window", "x":53, "y":30, "width":87, "height":42,
									"children" :
									(
										{ "name":"Exp_Slot", "type":"image", "x":0, "y":0, "image":LOCALE_PATH+"label_cur_exp.sub" },
										{ "name":"Exp_Value", "type":"text", "x":46, "y":19, "fontsize":"LARGE", "text":"12345678901", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },									),
								},

								## REXP
								{
									"name":"Status_RestExp", "type":"window", "x":150, "y":30, "width":50, "height":20, 
									"children" :
									(
										{ "name":"RestExp_Slot", "type":"image", "x":0, "y":0, "image":LOCALE_PATH+"label_last_exp.sub" },
										{ "name":"RestExp_Value", "type":"text", "x":46, "y":19, "fontsize":"LARGE", "text":"12345678901", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},
							),
						},

						## Face Slot
						{ "name" : "Face_Image", "type" : "image", "x" : 11, "y" : 11, "image" : "d:/ymir work/ui/game/windows/face_warrior.sub" },
						{ "name" : "Face_Slot", "type" : "image", "x" : 7, "y" : 7, "image" : FACE_SLOT_FILE, },

						## 기본 능력
						{
							"name":"Status_Standard", "type":"window", "x":3, "y":100, "width":200, "height":250,
							"children" :
							(
								## 기본 능력 제목
								{ "name":"Character_Bar_01", "type":"horizontalbar", "x":12, "y":8, "width":223, },
								{ "name":"Character_Bar_01_Text", "type" : "image", "x" : 13, "y" : 9, "image" : LOCALE_PATH+"label_std.sub", },
								
								## 능력 수련 수치
								{ 
									"name":"Status_Plus_Label", 
									"type":"image", 
									"x":150, "y":11, 
									"image":LOCALE_PATH+"label_uppt.sub", 
									
									"children" :
									(
										{ "name":"Status_Plus_Value", "type":"text", "x":48, "y":0, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},

								## 기본 능력 아이템 리스트
								{"name":"Status_Standard_ItemList1", "type" : "image", "x":17, "y":31, "image" : LOCALE_PATH+"label_std_item1.sub", },
								{"name":"Status_Standard_ItemList2", "type" : "image", "x":100, "y":30, "image" : LOCALE_PATH+"label_std_item2.sub", },

								## HTH
								{
									"name":"HTH_Label", "type":"window", "x":50, "y":32, "width":60, "height":20,
									"children" :
									(
										{ "name":"HTH_Slot", "type":"image", "x":0, "y":0, "image":SMALL_VALUE_FILE },
										{ "name":"HTH_Value", "type":"text", "x":20, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"HTH_Plus", "type" : "button", "x":41, "y":3, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
									),
								},
								## INT
								{
									"name":"INT_Label", "type":"window", "x":50, "y":32+23, "width":60, "height":20,
									"children" :
									(
										{ "name":"INT_Slot", "type":"image", "x":0, "y":0, "image":SMALL_VALUE_FILE },
										{ "name":"INT_Value", "type":"text", "x":20, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"INT_Plus", "type" : "button", "x" : 41, "y" : 3, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
									)
								},
								## STR
								{
									"name":"STR_Label", "type":"window", "x":50, "y":32+23*2, "width":60, "height":20,
									"children" :
									(
										{ "name":"STR_Slot", "type":"image", "x":0, "y":0, "image":SMALL_VALUE_FILE },
										{ "name":"STR_Value", "type":"text", "x":20, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"STR_Plus", "type" : "button", "x" : 41, "y" : 3, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
									)
								},
								## DEX
								{
									"name":"DEX_Label", "type":"window", "x":50, "y":32+23*3, "width":60, "height":20, 
									"children" :
									(
										{ "name":"DEX_Slot", "type":"image", "x":0, "y":0, "image":SMALL_VALUE_FILE },
										{ "name":"DEX_Value", "type":"text", "x":20, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
										{ "name":"DEX_Plus", "type" : "button", "x" : 41, "y" : 3, "default_image" : ROOT_PATH+"btn_plus_up.sub", "over_image" : ROOT_PATH+"btn_plus_over.sub", "down_image" : ROOT_PATH+"btn_plus_down.sub", },
									)
								},

								{ "name":"HTH_Minus", "type" : "button", "x":9, "y":35, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{ "name":"INT_Minus", "type" : "button", "x":9, "y":35+23, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{ "name":"STR_Minus", "type" : "button", "x":9, "y":35+23*2, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },
								{ "name":"DEX_Minus", "type" : "button", "x":9, "y":35+23*3, "default_image" : ROOT_PATH+"btn_minus_up.sub", "over_image" : ROOT_PATH+"btn_minus_over.sub", "down_image" : ROOT_PATH+"btn_minus_down.sub", },

								####

								## HP
								{
									"name":"HEL_Label", "type":"window", "x":145, "y":32, "width":50, "height":20,
									"children" :
									(
										{ "name":"HP_Slot", "type":"image", "x":0, "y":0, "image":LARGE_VALUE_FILE },
										{ "name":"HP_Value", "type":"text", "x":45, "y":3, "text":"9999/9999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},
								## SP
								{
									"name":"SP_Label", "type":"window", "x":145, "y":32+23, "width":50, "height":20, 
									"children" :
									(
										{ "name":"SP_Slot", "type":"image", "x":0, "y":0, "image":LARGE_VALUE_FILE },
										{ "name":"SP_Value", "type":"text", "x":45, "y":3, "text":"9999/9999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
								## ATT
								{
									"name":"ATT_Label", "type":"window", "x":145, "y":32+23*2, "width":50, "height":20, 
									"children" :
									(
										{ "name":"ATT_Slot", "type":"image", "x":0, "y":0, "image":LARGE_VALUE_FILE },
										{ "name":"ATT_Value", "type":"text", "x":45, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},
								## DEF
								{
									"name":"DEF_Label", "type":"window", "x":145, "y":32+23*3, "width":50, "height":20, 
									"children" :
									(
										{ "name":"DEF_Slot", "type":"image", "x":0, "y":0, "image":LARGE_VALUE_FILE },
										{ "name":"DEF_Value", "type":"text", "x":45, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},
							),
						},
						
						## 부가 능력
						{ 
							"name":"Status_Extent", "type":"window", "x":3, "y":221, "width":200, "height":50, 
							"children" :
							(

								## 부가 능력 제목
								{ "name":"Status_Extent_Bar", "type":"horizontalbar", "x":12, "y":6, "width":223, },
								{ "name":"Status_Extent_Label", "type" : "image", "x" : 13, "y" : 8, "image" : LOCALE_PATH+"label_ext.sub", },

								## 기본 능력 아이템 리스트
								{"name":"Status_Extent_ItemList1", "type" : "image", "x":11, "y":31, "image" : LOCALE_PATH+"label_ext_item1.sub", },
								{"name":"Status_Extent_ItemList2", "type" : "image", "x":128, "y":32, "image" : LOCALE_PATH+"label_ext_item2.sub", },

								## MSPD - 이동 속도
								{
									"name":"MOV_Label", "type":"window", "x":66, "y":33, "width":50, "height":20, 
									"children" :
									(
										{ "name":"MSPD_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"MSPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

								## ASPD - 공격 속도
								{
									"name":"ASPD_Label", "type":"window", "x":66, "y":33+23, "width":50, "height":20, 
									"children" :
									(
										{ "name":"ASPD_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"ASPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

								## CSPD - 주문 속도
								{
									"name":"CSPD_Label", "type":"window", "x":66, "y":33+23*2, "width":50, "height":20, 
									"children" :
									(
										{ "name":"CSPD_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"CSPD_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

								## MATT - 마법 공격력
								{
									"name":"MATT_Label", "type":"window", "x":183, "y":33, "width":50, "height":20, 
									"children" :
									(
										{ "name":"MATT_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"MATT_Value", "type":"text", "x":26, "y":3, "text":"999-999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

								## MDEF - 마법 방어력
								{
									"name":"MDEF_Label", "type":"window", "x":183, "y":33+23, "width":50, "height":20, 
									"children" :
									(
										{ "name":"MDEF_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"MDEF_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

								## 회피율
								{
									"name":"ER_Label", "type":"window", "x":183, "y":33+23*2, "width":50, "height":20, 
									"children" :
									(
										{ "name":"ER_Slot", "type":"image", "x":0, "y":0, "image":MIDDLE_VALUE_FILE },
										{ "name":"ER_Value", "type":"text", "x":26, "y":3, "text":"999", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									)
								},

							),
						},
					),
				},
				{
					"name" : "Skill_Page",
					"type" : "window",
					"style" : ("attach",),

					"x" : 0,
					"y" : 24,

					"width" : 250,
					"height" : 304,

					"children" :
					(

						{
							"name":"Skill_Active_Title_Bar", "type":"horizontalbar", "x":15, "y":17, "width":223,

							"children" :
							(
								{ 
									"name":"Active_Skill_Point_Label", "type":"image", "x":145, "y":3, "image":LOCALE_PATH+"label_uppt.sub",
									"children" :
									(
										{ "name":"Active_Skill_Point_Value", "type":"text", "x":47, "y":0, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},

								## Group Button
								{
									"name" : "Skill_Group_Button_1",
									"type" : "radio_button",

									"x" : 5,
									"y" : 2,

									"text" : "Group1",
									"text_color" : 0xFFFFE3AD,

									"default_image" : "d:/ymir work/ui/game/windows/skill_tab_button_01.sub",
									"over_image" : "d:/ymir work/ui/game/windows/skill_tab_button_02.sub",
									"down_image" : "d:/ymir work/ui/game/windows/skill_tab_button_03.sub",
								},

								{
									"name" : "Skill_Group_Button_2",
									"type" : "radio_button",

									"x" : 50,
									"y" : 2,

									"text" : "Group2",
									"text_color" : 0xFFFFE3AD,

									"default_image" : "d:/ymir work/ui/game/windows/skill_tab_button_01.sub",
									"over_image" : "d:/ymir work/ui/game/windows/skill_tab_button_02.sub",
									"down_image" : "d:/ymir work/ui/game/windows/skill_tab_button_03.sub",
								},

								{
									"name" : "Active_Skill_Group_Name",
									"type" : "text",

									"x" : 7,
									"y" : 1,
									"text" : "Active",

									"vertical_align" : "center",
									"text_vertical_align" : "center",
									"color" : 0xFFFFE3AD,
								},

							),
						},

						{
							"name":"Skill_ETC_Title_Bar", "type":"horizontalbar", "x":15, "y":200, "width":223,

							"children" :
							(
								{
									"name" : "Support_Skill_Group_Name",
									"type" : "text",

									"x" : 7,
									"y" : 1,
									"text" : uiScriptLocale.SKILL_SUPPORT_TITLE,

									"vertical_align" : "center",
									"text_vertical_align" : "center",
									"color" : 0xFFFFE3AD,
								},

								{ 
									"name":"Support_Skill_Point_Label", "type":"image", "x":145, "y":3, "image":LOCALE_PATH+"label_uppt.sub",
									"children" :
									(
										{ "name":"Support_Skill_Point_Value", "type":"text", "x":62, "y":0, "text":"99", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									),
								},
							),
						},
						{ "name":"Skill_Board", "type":"image", "x":13, "y":38, "image":"d:/ymir work/ui/game/windows/skill_board.sub", },

						## Active Slot
						{
							"name" : "Skill_Active_Slot",
							"type" : "slot",

							"x" : 0 + 16,
							"y" : 0 + 15 + 23,

							"width" : 223,
							"height" : 223,
							"image" : ICON_SLOT_FILE,

							"slot" :	(
											{"index": 1, "x": 1, "y":  4, "width":32, "height":32},
											{"index":21, "x":38, "y":  4, "width":32, "height":32},
											{"index":41, "x":75, "y":  4, "width":32, "height":32},

											{"index": 3, "x": 1, "y": 40, "width":32, "height":32},
											{"index":23, "x":38, "y": 40, "width":32, "height":32},
											{"index":43, "x":75, "y": 40, "width":32, "height":32},

											{"index": 5, "x": 1, "y": 76, "width":32, "height":32},
											{"index":25, "x":38, "y": 76, "width":32, "height":32},
											{"index":45, "x":75, "y": 76, "width":32, "height":32},

											{"index": 7, "x": 1, "y":112, "width":32, "height":32},
											{"index":27, "x":38, "y":112, "width":32, "height":32},
											{"index":47, "x":75, "y":112, "width":32, "height":32},

											####

											{"index": 2, "x":113, "y":  4, "width":32, "height":32},
											{"index":22, "x":150, "y":  4, "width":32, "height":32},
											{"index":42, "x":187, "y":  4, "width":32, "height":32},

											{"index": 4, "x":113, "y": 40, "width":32, "height":32},
											{"index":24, "x":150, "y": 40, "width":32, "height":32},
											{"index":44, "x":187, "y": 40, "width":32, "height":32},

											{"index": 6, "x":113, "y": 76, "width":32, "height":32},
											{"index":26, "x":150, "y": 76, "width":32, "height":32},
											{"index":46, "x":187, "y": 76, "width":32, "height":32},

											{"index": 8, "x":113, "y":112, "width":32, "height":32},
											{"index":28, "x":150, "y":112, "width":32, "height":32},
											{"index":48, "x":187, "y":112, "width":32, "height":32},
										),
						},

						## ETC Slot
						{
							"name" : "Skill_ETC_Slot",
							"type" : "grid_table",
							"x" : 18,
							"y" : 221,
							"start_index" : 101,
							"x_count" : 6,
							"y_count" : 2,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 5,
							"y_blank" : 4,
							"image" : ICON_SLOT_FILE,
						},

					),
				},
			],
		},
	),
}

window["children"][0]["children"] = window["children"][0]["children"] + [
{
	"name" : "Quest_Page",
	"type" : "window",
	"style" : ("attach",),

	"x" : 0,
	"y" : 24,

	"width" : 250,
	"height" : 304,
		
	"children" :
	(
		{
			"name" : "quest_page_board_window",
			"type" : "window",
			"style" : ("attach", "ltr",),
			
			"x" : 10,
			"y" : 7,
			
			"width"		: QUEST_BOARD_WINDOW_WIDTH,
			"height"	: QUEST_BOARD_WINDOW_HEIGHT,
			
			"children" :
			(
				## LeftTop 1
				{
					"name" : "LeftTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},
				## RightTop 2
				{
					"name" : "RightTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : QUEST_BOARD_WINDOW_WIDTH - 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},
				## LeftBottom 3
				{
					"name" : "LeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : QUEST_BOARD_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},
				## RightBottom 4
				{
					"name" : "RightBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : QUEST_BOARD_WINDOW_WIDTH - 16,
					"y" : QUEST_BOARD_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},
				## topcenterImg 5
				{
					"name" : "TopCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, QUEST_BOARD_PATTERN_X_COUNT, 0),
				},
				## leftcenterImg 6
				{
					"name" : "LeftCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, QUEST_BOARD_PATTERN_Y_COUNT),
				},
				## rightcenterImg 7
				{
					"name" : "RightCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : QUEST_BOARD_WINDOW_WIDTH - 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, QUEST_BOARD_PATTERN_Y_COUNT),
				},
				## bottomcenterImg 8
				{
					"name" : "BottomCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : QUEST_BOARD_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, QUEST_BOARD_PATTERN_X_COUNT, 0),
				},
				## centerImg
				{
					"name" : "CenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, QUEST_BOARD_PATTERN_X_COUNT, QUEST_BOARD_PATTERN_Y_COUNT),
				},
				
				{
					"name" : "quest_object_board_window",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 3,
					"y" : 3,
					
					"width"		: QUEST_BOARD_WINDOW_WIDTH - 3,		# 228
					"height"	: QUEST_BOARD_WINDOW_HEIGHT - 3,	# 294
				},
			),
		},
		
		{
			"name" : "Quest_ScrollBar",
			"type" : "scrollbar",

			"x" : 25,
			"y" : 12,
			"size" : 290,
			"horizontal_align" : "right",
		},
	),
},]

window["children"][0]["children"] = window["children"][0]["children"] + [
{
	"name" : "Emoticon_Page",
	"type" : "window",
	"style" : ("attach",),

	"x" : 0,
	"y" : 24,

	"width" : 250,
	"height" : 304,

	"children" :
	[
			## 기본 액션 제목
		{ "name":"Action_Bar", "type":"horizontalbar", "x":12, "y":11, "width":223, },
		{ "name":"Action_Bar_Text", "type":"text", "x":15, "y":13, "text":uiScriptLocale.CHARACTER_NORMAL_ACTION },

		## Basis Action Slot
		{
			"name" : "SoloEmotionSlot",
			"type" : "grid_table",
			"x" : 30,
			"y" : 33,
			#"horizontal_align" : "center",
			"start_index" : 1,
			"x_count" : 6,
			"y_count" : 3,
			"x_step" : 32,
			"y_step" : 32,
			"x_blank" : 0,
			"y_blank" : 0,
			"image" : ICON_SLOT_FILE,
		},

		## 상호 액션 제목
		{ "name":"Reaction_Bar", "type":"horizontalbar", "x":12, "y":8+130, "width":223, },
		{ "name":"Reaction_Bar_Text", "type":"text", "x":15, "y":10+130, "text":uiScriptLocale.CHARACTER_MUTUAL_ACTION },

		## Reaction Slot
		{
			"name" : "DualEmotionSlot",
			"type" : "grid_table",
			"x" : 30,
			"y" : 160,
			"start_index" : 51,
			"x_count" : 6,
			"y_count" : 1,
			"x_step" : 32,
			"y_step" : 32,
			"x_blank" : 0,
			"y_blank" : 0,
			"image" : ICON_SLOT_FILE,
		},
		
		## 특수 액션 제목
		{ "name":"Special_Action_Bar", "type":"horizontalbar", "x":12, "y":8+190, "width":223, },
		{ "name":"Special_Action_Bar_Text", "type":"text", "x":15, "y":10+190, "text":uiScriptLocale.CHARACTER_SPECTIAL_ACTION },
		
		## Special_Action_Slot
		{				
			"name" : "SpecialEmotionSlot",
			"type" : "grid_table",
			
			"x" : 30,
			"y" : 220,
			"start_index" : app.SPECIAL_ACTION_START_INDEX,
			"x_count" : 6,
			"y_count" : 2,
			"x_step" : 32,
			"y_step" : 32,
			"x_blank" : 0,
			"y_blank" : 0,
			"image" : ICON_SLOT_FILE,
		},
	],
},]
