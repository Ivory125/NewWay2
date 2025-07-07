import uiScriptLocale
import localeInfo
import constInfo

BOARD_WIDTH = 450
BOARD_HEIGHT = 380

IMG_PATH = "d:/ymir work/ui/aslan/adminpanel/create_item/"

window = {
	"name" : "CreateItemWindow",

	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,

	"style" : ("float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar", 
			"x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,
			"title" : "Item Erstellen",
			"children" :
			(
				{
					"name" : "ResetButton", "type" : "button", "x" : 270, "y" : 345, "text_outline" : 1, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BTN_RESET,
					"default_image" : IMG_PATH+"button_1_normal.sub",
					"over_image" : IMG_PATH+"button_1_hover.sub",
					"down_image" : IMG_PATH+"button_1_down.sub",
				},
				{
					"name" : "CreateButton", "type" : "button", "x" : 270, "y" : 345, "text_outline" : 1, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BTN_CREATE,
					"default_image" : IMG_PATH+"button_1_normal.sub",
					"over_image" : IMG_PATH+"button_1_hover.sub",
					"down_image" : IMG_PATH+"button_1_down.sub",
				},
				{
					"name" : "itemlist_search", "type" : "thinboard_circle",
					"x" : 15, "y" : 40, "width" : 200, "height" : 20,
					"children" :
					(
						{ "name" : "itemlist_searchinfo", "type":"text", "x": 5, "y": 3, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_SEARCH},
						{
							"name" : "itemsearch", "type" : "editline", "input_limit" : 24,
							"x" : 50, "y" : 3, "width" : 150, "height" : 30,
						},
					)
				},
				{
					"name" : "itemlist_background", "type" : "thinboard_circle",
					"x" : 15, "y" : 65, "width" : 200, "height" : 300,
					"children" :
					(
					)
				},
				{
					"name" : "item_config_2_board", "type" : "border_a",
					"x" : 225, "y" : 40, "width" : 210, "height" : 296,
					"children" :
					(
						{ "name" : "titlebar_item", "type" : "titlebar_with_image", "x" : 3, "y" : 2, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_TITLE_EXT_CONFIG, "image" : IMG_PATH+"titlebar_1.sub", },
						
						{ "name" : "field_select_bonus_0", "type" : "button", "x" : 8, "y" : 30+40*0, "default_image" : IMG_PATH+"textfield_1.sub", "over_image" : IMG_PATH+"textfield_2.sub", "down_image" : IMG_PATH+"textfield_2.sub",
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BONUS_1}, 
								{ "name" : "info_select_bonus_0", "type":"text", "x": 97, "y": 25, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : "---"}, 
							),
						},
						{ "name" : "field_select_bonus_1", "type" : "button", "x" : 8, "y" : 30+40*1, "default_image" : IMG_PATH+"textfield_1.sub", "over_image" : IMG_PATH+"textfield_2.sub", "down_image" : IMG_PATH+"textfield_2.sub",
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BONUS_2}, 
								{ "name" : "info_select_bonus_1", "type":"text", "x": 97, "y": 25, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : "---"}, 
							),
						},
						{ "name" : "field_select_bonus_2", "type" : "button", "x" : 8, "y" : 30+40*2, "default_image" : IMG_PATH+"textfield_1.sub", "over_image" : IMG_PATH+"textfield_2.sub", "down_image" : IMG_PATH+"textfield_2.sub",
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BONUS_3}, 
								{ "name" : "info_select_bonus_2", "type":"text", "x": 97, "y": 25, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : "---"}, 
							),
						},
						{ "name" : "field_select_bonus_3", "type" : "button", "x" : 8, "y" : 30+40*3, "default_image" : IMG_PATH+"textfield_1.sub", "over_image" : IMG_PATH+"textfield_2.sub", "down_image" : IMG_PATH+"textfield_2.sub",
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BONUS_4},
								{ "name" : "info_select_bonus_3", "type":"text", "x": 97, "y": 25, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : "---"},  
							),
						},
						{ "name" : "field_select_bonus_4", "type" : "button", "x" : 8, "y" : 30+40*4, "default_image" : IMG_PATH+"textfield_1.sub", "over_image" : IMG_PATH+"textfield_2.sub", "down_image" : IMG_PATH+"textfield_2.sub",
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_BONUS_5},
								{ "name" : "info_select_bonus_4", "type":"text", "x": 97, "y": 25, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : "---"},  
							),
						},
						
						{ "name" : "horizontal_bar", "type":"bar", "x": 8, "y": 235, "width" : 194, "height" : 1, "color" : 0xFF6d645a},  
						
						{ "name" : "field_select_socket_0", "type" : "button", "x" : 20, "y" : 246, "default_image" : IMG_PATH+"socketslot_1.sub", "over_image" : IMG_PATH+"socketslot_2.sub", "down_image" : IMG_PATH+"socketslot_2.sub",
							"children" : (   
								{ "name" : "select_socket_0", "type" : "image", "x" : 6, "y" : 4, "style" : ("not_pick",), },
							),
						},
						{ "name" : "field_select_socket_1", "type" : "button", "x" : 87, "y" : 246, "default_image" : IMG_PATH+"socketslot_1.sub", "over_image" : IMG_PATH+"socketslot_2.sub", "down_image" : IMG_PATH+"socketslot_2.sub",
							"children" : (  
								{ "name" : "select_socket_1", "type" : "image", "x" : 6, "y" : 4, "style" : ("not_pick",), },
							),
						},
						{ "name" : "field_select_socket_2", "type" : "button", "x" : 154, "y" : 246, "default_image" : IMG_PATH+"socketslot_1.sub", "over_image" : IMG_PATH+"socketslot_2.sub", "down_image" : IMG_PATH+"socketslot_2.sub",
							"children" : ( 
								{ "name" : "select_socket_2", "type" : "image", "x" : 6, "y" : 4, "style" : ("not_pick",), }, 
							),
						},
					)
				},
				{
					"name" : "item_config_board", "type" : "border_a",
					"x" : 225, "y" : 40, "width" : 210, "height" : 296,
					"children" :
					(
						{ "name" : "titlebar_item", "type" : "titlebar_with_image", "x" : 3, "y" : 2, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_TITLE_ITEM, "image" : IMG_PATH+"titlebar_1.sub", },
						{
							"name" : "itemslot_image", "type" : "image",
							"x" : 85, "y" : 28,
							"image" : IMG_PATH+"itemslot_3.sub",
							"children" :
							( 
								{
									"name" : "ItemSlot", "type" : "slot", "x" : 4, "y" : 4, "width" : 32, "height" : 96,
									"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":96,},),
								},
							),
						},
						{ "name" : "titlebar_item", "type" : "titlebar_with_image", "x" : 3, "y" : 138, "text" : localeInfo.ADMINPANEL_CREATE_ITEM_TITLE_CONFIG, "image" : IMG_PATH+"titlebar_1.sub", },
						{ "name" : "field_set_count", "type" : "image", "x" : 8, "y" : 166 + 40, "image" : IMG_PATH+"textfield_1.sub", 
							"children" : (  
								{ "name" : "info_set_count", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_SET_COUNT}, 
								{ "name" : "editline_set_count", "type" : "editline", "x": 97, "y": 19, "width" : 180, "height" : 16, "input_limit" : 3, "default_text" : "1", "only_number" : 1, "text_horizontal_align" : "center",}, 
								{ "name" : "field_set_count_overlay", "type" : "image", "x" : 0, "y" : 0, "image" : IMG_PATH+"textfield_1.sub", }
							),
						},
						{ "name" : "field_set_player", "type" : "image", "x" : 8, "y" : 166 + 80, "image" : IMG_PATH+"textfield_1.sub", 
							"children" : (  
								{ "name" : "info_set_player", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_SET_PLAYER}, 
								{ "name" : "editline_set_player", "type" : "editline", "x": 97, "y": 19, "width" : 180, "height" : 16, "input_limit" : 24, "default_text" : "1", "text_horizontal_align" : "center",}, 
								{ "name" : "field_player_overlay", "type" : "image", "x" : 0, "y" : 0, "image" : IMG_PATH+"textfield_1.sub", }
							),
						},
						{ "name" : "field_select_grade", "type" : "image", "x" : 8, "y" : 166, "image" : IMG_PATH+"textfield_1.sub", 
							"children" : (  
								{ "name" : "info_select_grade", "type":"text", "x": 43, "y": 9, "text_horizontal_align" : "center", "text_vertical_align" : "center", "text" : localeInfo.ADMINPANEL_CREATE_ITEM_SET_GRADE}, 
								{ "name" : "field_select_grade_overlay", "type" : "image", "x" : 0, "y" : 0, "image" : IMG_PATH+"textfield_1.sub", }
							),
						},
					)
				},
			)
		},
	),
}