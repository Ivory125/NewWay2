import uiScriptLocale
import localeInfo
import constInfo

PET_UI_ROOT = "d:/ymir work/ui/pet/"
BASE_SLOT_FILE = "d:/ymir work/ui/public/Slot_Base.sub"

PET_UI_BG_HEIGHT	= 456
PET_UI_BG_WIDTH		= 352

PET_PAGE_UI_BG_WIDTH	= 352
PET_PAGE_UI_BG_HEIGHT	= 408

PET_UI_TAB_X = 13
PET_UI_TAB_Y = 8
PET_UI_TAB_WIDTH = 77
PET_UI_TAB_HEIGHT = 30

## EXP Gague interval
EXP_GAGUE_INTERVAL	= 2
EXP_IMG_WIDTH		= 16
EXP_IMG_HEIGHT		= 16

## TEXT COLOR
GOLD_COLOR	= 0xFFFEE3AE
WHITE_COLOR = 0xFFFFFFFF

	
window = {
	"name" : "PetInformationWindow", "style" : ("movable", "float",), 
	"x" : SCREEN_WIDTH - 176 -200 -146 -145, "y" : SCREEN_HEIGHT - 37 - 565,
	"width" : PET_UI_BG_WIDTH, "height" : PET_UI_BG_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "window", "x" : 0, "y" : 0, "width" : PET_UI_BG_WIDTH, "height" : PET_UI_BG_HEIGHT,
			"children" :
			(
				{
					"name" : "TabControl", "type" : "window", "x" : 0, "y" : PET_UI_BG_HEIGHT - 40 - 8, "width" : PET_UI_BG_WIDTH, "height" : 48,
					"children" :
					(
						## Tab
						{ "name" : "Tab_01", "type" : "image", "x" : 0, "y" : 0, "width" : PET_UI_BG_WIDTH, "height" : 48, "image" : PET_UI_ROOT + "res/tab_1.tga", },
						{ "name" : "Tab_02", "type" : "image", "x" : 0, "y" : 0, "width" : PET_UI_BG_WIDTH, "height" : 48, "image" : PET_UI_ROOT + "res/tab_2.tga", },
						{ "name" : "Tab_03", "type" : "image", "x" : 0, "y" : 0, "width" : PET_UI_BG_WIDTH, "height" : 48, "image" : PET_UI_ROOT + "res/tab_3.tga", },
						## Button
						{ "name" : "Tab_Button_01", "type" : "button", "x" : PET_UI_TAB_X, "y" : PET_UI_TAB_Y, "width" : PET_UI_TAB_WIDTH, "height" : PET_UI_TAB_HEIGHT, },
						{ "name" : "Tab_Button_02", "type" : "button", "x" : PET_UI_TAB_X + 94, "y" : PET_UI_TAB_Y, "width" : PET_UI_TAB_WIDTH, "height" : PET_UI_TAB_HEIGHT, },
						{ "name" : "Tab_Button_03", "type" : "button", "x" : PET_UI_TAB_X + 94 + 94, "y" : PET_UI_TAB_Y, "width" : PET_UI_TAB_WIDTH, "height" : PET_UI_TAB_HEIGHT, },
					 ),	
				},
		
				## Page Area INFOPAGE
				{
					"name" : "PetInfo_Page", "type" : "window", "style" : ("attach",), "x" : 0, "y" : 0, "width" : PET_PAGE_UI_BG_WIDTH, "height" : PET_PAGE_UI_BG_HEIGHT,
					
					"children":
					[
						{ "name" : "Page01Background", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/pet/res/Pet_UI_bg.tga" },
						{
							"name" : "PetSealIconSlot", "type" : "slot", "x" : 25, "y" : 49, "width" : 32, "height" : 32,
							"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
						},
						{
							"name" : "DetermineButton", "type" : "button", "x" : 21, "y" : 89,
							"default_image" : PET_UI_ROOT + "determine_button/determine_button_default.sub",
							"over_image" : PET_UI_ROOT + "determine_button/determine_button_over.sub",
							"down_image" : PET_UI_ROOT + "determine_button/determine_button_down.sub",
						},
						
						{"name":"EvolName", "type":"text", "x":202, "y":54, "text": "", "r":1.0, "g":0.85, "b":0.0, "a":1.0, "text_horizontal_align" : "center"},
						{"name":"PetName", "type":"text", "x":202, "y":78, "text": "", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align" : "center"},

						{"name":"LevelTitle", "type":"text", "x":73, "y":127, "text": uiScriptLocale.PET_INFORMATION_LEVEL, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"LevelValue", "type":"text", "x":73, "y":150, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},
						
						{"name":"ExpTitle", "type":"text", "x":176, "y":127, "text": uiScriptLocale.PET_INFORMATION_EXP, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
	
						{
							"name" : "UpBringing_Pet_EXP_Gauge_Board", "type" : "window", "style": ("ltr",), "x" : 131, "y" : 149, 
							"width" : EXP_IMG_WIDTH * 5 + EXP_GAGUE_INTERVAL * 4, "height" : EXP_IMG_HEIGHT,

							"children" :
							(
								{ "name" : "UpBringing_Pet_EXPGauge_01", "type" : "expanded_image", "x" : 0, "y" : 0, "image" : PET_UI_ROOT + "exp_gauge/exp_on.sub" },
								{ "name" : "UpBringing_Pet_EXPGauge_02", "type" : "expanded_image", "x" : EXP_IMG_WIDTH + EXP_GAGUE_INTERVAL, "y" : 0, "image" : PET_UI_ROOT + "exp_gauge/exp_on.sub" },
								{ "name" : "UpBringing_Pet_EXPGauge_03", "type" : "expanded_image", "x" : EXP_IMG_WIDTH * 2 + EXP_GAGUE_INTERVAL * 2, "y" : 0, "image" : PET_UI_ROOT + "exp_gauge/exp_on.sub" },
								{ "name" : "UpBringing_Pet_EXPGauge_04", "type" : "expanded_image", "x" : EXP_IMG_WIDTH * 3 + EXP_GAGUE_INTERVAL * 3, "y" : 0, "image" : PET_UI_ROOT + "exp_gauge/exp_on.sub" },
								{ "name" : "UpBringing_Pet_EXPGauge_05", "type" : "expanded_image", "x" : EXP_IMG_WIDTH * 4 + EXP_GAGUE_INTERVAL * 4, "y" : 0, "image" : PET_UI_ROOT + "exp_gauge/item_exp_on.sub" },
							),
						},
						
						{"name":"AgeTitle", "type":"text", "x":276, "y":127, "text": uiScriptLocale.PET_INFORMATION_AGE, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"AgeValue", "type":"text", "x":276, "y":150, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},

						{"name":"LifeTitle", "type":"text", "x":112, "y":172, "text": uiScriptLocale.PET_INFORMATION_LIFE, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"LifeTextValue", "type":"text", "x":114, "y":197, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},
	
						{
							"name" : "LifeGaugeWindow", "type" : "window", "x" : 36, "y":216, "width" : 179, "height" : 12, "style" : ("attach",),
							"children" :
							(
								{
									"name" : "LifeGauge", "type" : "ani_image", "x" : 0, "y" : 0, "delay" : 6, "x_scale" : 1.61, "y_scale" : 1.0,
									"images" :
									(
										"D:/Ymir Work/ui/Pattern/HPGauge/01.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/02.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/03.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/04.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/05.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/06.tga",
										"D:/Ymir Work/ui/Pattern/HPGauge/07.tga",
									),
								},
							),
						},
						{
							"name" : "FeedTimeButton", "type" : "toggle_button", "x" : 207, "y" : 173,
							
							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							"disable_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							
							"text" : uiScriptLocale.PET_INFORMATION_LIFETIME_BUTTON, "text_color" : GOLD_COLOR,
						},
						{
							"name" : "FeedEvolButton", "type" : "button", "x" : 207, "y" : 194,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							"disable_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							
							"text" : uiScriptLocale.PET_INFORMATION_EVOL_BUTTON, "text_color" : GOLD_COLOR,
						},
						{
							"name" : "FeedExpButton", "type" : "toggle_button", "x" : 207, "y" : 215,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							"disable_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							
							"text" : uiScriptLocale.PET_INFORMATION_EXP_BUTTON, "text_color" : GOLD_COLOR,
						},
						{"name":"AbilitiesName", "type":"text", "x":0, "y":262, "text": uiScriptLocale.PET_INFORMATION_ABILITIES, "r":1.0, "g":0.85, "b":0.0, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						
						{"name":"HpTitle", "type":"text", "x":104, "y":285, "text": uiScriptLocale.PET_INFORMATION_HP, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"HpValue", "type":"text", "x":260, "y":286, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},
						
						{"name":"AttTitle", "type":"text", "x":104, "y":307, "text": uiScriptLocale.PET_INFORMATION_ATT, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"AttValue", "type":"text", "x":260, "y":308, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},
						
						{"name":"DefTitle", "type":"text", "x":104, "y":329, "text": uiScriptLocale.PET_INFORMATION_DEFENCE, "color":GOLD_COLOR, "text_horizontal_align" : "center"},
						{"name":"DefValue", "type":"text", "x":260, "y":330, "text": "", "color":WHITE_COLOR, "text_horizontal_align" : "center"},
						
						{"name":"PetSkillTitle", "type":"text", "x":76, "y":374, "text": uiScriptLocale.PET_INFORMATION_SKILL, "r":1.0, "g":0.85, "b":0.0, "a":1.0, "text_horizontal_align" : "center"},
						{
							"name" : "PetSkillSlot", "type" : "slot", "x" : 156, "y" : 365, "width" : 160, "height" : 32, "image" : BASE_SLOT_FILE,
							"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},{"index":1, "x":64, "y":0, "width":32, "height":32},{"index":2, "x":128, "y":0, "width":32, "height":32},),
						},
					]
				},
				
				## Page Area ATTR PAGE
				{
					"name" : "PetAttrChange_Page", "type" : "window", "style" : ("attach",), "x" : 0, "y" : 0, "width" : PET_PAGE_UI_BG_WIDTH, "height" : PET_PAGE_UI_BG_HEIGHT,
					"children" :
					[
						{ "name" : "PetAttrChangeUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/pet/res/Pet_UI_bg2.tga" },

						{"name":"PetAttrChangeTitle", "type":"text", "x":0, "y":39, "text": uiScriptLocale.PET_ATTR_CHANGE_TITLE, "r":0.85, "g":0.85, "b":0.85, "a":1.0, "text_horizontal_align" : "center", "horizontal_align" : "center"},
						
						{
							"name" : "Change_Pet_Slot", "type" : "slot", "x" : 160, "y" : 67, "width" : 32, "height" : 173,
							"slot" : (
								{"index":0, "x":0, "y":0, "width":32, "height":32},
								{"index":1, "x":0, "y":69, "width":32, "height":32},
								{"index":2, "x":0, "y":140, "width":32, "height":32},
							),
						},
						
						{"name":"ChangePetText", "type":"text", "x":0, "y":111, "text": uiScriptLocale.PET_CHANGE_TEXT, "color":WHITE_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						{"name":"AttrChangeItemText", "type":"text", "x":0, "y":180, "text": uiScriptLocale.PET_ATTR_CHANGE_ITEM_TEXT, "color":WHITE_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						{"name":"AttrChangeResultText", "type":"text", "x":0, "y":253, "text": uiScriptLocale.ACCE_RESULT, "color":WHITE_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						
						{"name":"PetDetermineInfoTitle", "type":"text", "x":0, "y":290, "text": uiScriptLocale.PET_ATTR_DETERMINE_TITLE, "color":GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						{"name":"PetDetermineInfoText", "type":"text", "x":0, "y":314, "text": " ", "color":WHITE_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center"},
						
						{
							"name" : "Pet_Change_Button", "type" : "button", "x" : 25, "y" : 379,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
					
							"text" : uiScriptLocale.PET_ATTR_CONFIRMATION, "text_color" : GOLD_COLOR,
						},
						{
							"name" : "Attr_Clear_Button", "type" : "button", "x" : 205, "y" : 379,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
					
							"text" : uiScriptLocale.PET_ATTR_OK, "text_color" : GOLD_COLOR,
						},
					],	
				},
				
				## Page Area REVIVE PAGE
				{
					"name" : "PetRevive_Page",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 0,
					"y" : 0,
					
					"width" : PET_PAGE_UI_BG_WIDTH,
					"height" : PET_PAGE_UI_BG_HEIGHT,
					
					"children" :
					[
						{ "name" : "PetReviveUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/pet/res/pet_ui_bg3.tga" },
					
						{"name" : "ReviveTitle", "type":"text", "x":0, "y":39, "text": uiScriptLocale.PET_REVIVE_TITLE, "text_horizontal_align" : "center", "horizontal_align" : "center"},
						
						{"name" : "ReviveTargetTitle", "type":"text", "x":101, "y":63, "text": uiScriptLocale.PET_REVIVE_TARGET, "text_horizontal_align" : "center"},
						{"name" : "ReviveResultTitle", "type":"text", "x":250, "y":63, "text": uiScriptLocale.PET_REVIVE_REUSLT_TEXT, "text_horizontal_align" : "center"},
						
						{"name" : "AgeTitle01", "type":"text", "x":103, "y":134, "text": "Max. " + uiScriptLocale.PET_AGE_TEXT, "text_horizontal_align" : "center"},
						{"name" : "AgeValue01", "type":"text", "x":103, "y":151, "text": "", "text_horizontal_align" : "center"},
						{"name" : "AgeTitle02", "type":"text", "x":249, "y":134, "text": uiScriptLocale.PET_AGE_TEXT, "text_horizontal_align" : "center"},
						{"name" : "AgeValue02", "type":"text", "x":249, "y":150, "text": "", "text_horizontal_align" : "center"},
						
						{"name" : "ReviveMaterialTitle", "type":"text", "x":108, "y":179, "text": uiScriptLocale.PET_REVIVE_MATERIAL_TITLE, "text_horizontal_align" : "center"},
						{"name" : "ReviveMaterialTitle", "type":"text", "x":249, "y":179, "text": "0/0", "text_horizontal_align" : "center"},
						# {"name" : "ReviveMaterialTitle", "type":"text", "x":249, "y":183, "text": localeInfo.PET_REVIVE_UI_MATERIAL_COUNT, "text_horizontal_align" : "center"},
						
						{
							"name" : "Revive_Pet_Slots", "type" : "slot", "x" : 56, "y" : 91, "width" : 240, "height" : 179,
							"slot" : (
								{"index":0, "x":31, "y":0, "width":32, "height":32},
								{"index":1, "x":177, "y":0, "width":32, "height":32},
								
								{"index":10, "x":0, "y":115, "width":32, "height":32},
								{"index":11, "x":52, "y":115, "width":32, "height":32},
								{"index":12, "x":104, "y":115, "width":32, "height":32},
								{"index":13, "x":156, "y":115, "width":32, "height":32},
								{"index":14, "x":208, "y":115, "width":32, "height":32},
								
								{"index":15, "x":0, "y":147, "width":32, "height":32},
								{"index":16, "x":52, "y":147, "width":32, "height":32},
								{"index":17, "x":104, "y":147, "width":32, "height":32},
								{"index":18, "x":156, "y":147, "width":32, "height":32},
								{"index":19, "x":208, "y":147, "width":32, "height":32},
							),
						},
						{
							"name" : "ReviveDescBox", "type" : "window",
							"x" : 24, "y" : 289, "width" : 305, "height" : 80,
							"children" :
							(
								{
									"name" : "Revive_Pet_DescPrevButton", "type" : "button", "x" : 240, "y" : 68,

									"default_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
									"over_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
									"down_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
								},
								
								{
									"name" : "Revive_Pet_DescNextButton", "type" : "button", "x" : 265, "y" : 68,

									"default_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
									"over_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
									"down_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
								},
							),
						},
						{
							"name" : "Revive_Accept_Button", "type" : "button", "x" : 25, "y" : 383,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
					
							"text" : uiScriptLocale.PET_REVIVE_BUTTON, "text_color" : GOLD_COLOR,
						},
						
						{
							"name" : "Revive_Clear_Button", "type" : "button", "x" : 205, "y" : 383,

							"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
							"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
							"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
					
							"text" : uiScriptLocale.PET_REVIVE_CANCEL_BUTTON, "text_color" : GOLD_COLOR,
						},
					],
				},
				
				{"name":"TitleName" , "type" : "text", "x" : 0, "y" : 11, "text": uiScriptLocale.PET_CHANGE_TEXT, "horizontal_align" : "center", "text_horizontal_align" : "center"},
				{ 
					"name" : "CloseButton", 
					"type" : "button", 
					"x" : PET_UI_BG_WIDTH -10-15, 
					"y" : 10, 
					"tooltip_text" : "Close", 
					"default_image" : "d:/ymir work/ui/public/close_button_01.sub",	
					"over_image" : "d:/ymir work/ui/public/close_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/close_button_03.sub",
				},	
			),
		},
	),
}
