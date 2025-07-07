import uiScriptLocale
import localeInfo
import constInfo

BOARD_WIDTH = 450
BOARD_HEIGHT = 370

IMG_PATH = "d:/ymir work/ui/aslan/adminpanel/startpage/"

COLOR_RED = 0xFFfd2020
COLOR_YELLOW = 0xFFe6ed23
COLOR_BLUE = 0xFF0468ff

window = {
	"name" : "AdminpanelStartpageWindow",

	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2),
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
			"title" : "Startseite",
			"children" :
			(
				{
					"name" : "board_onlineplayer", "type" : "image", "x" : 15, "y" : 38, "image" : IMG_PATH+"board_onlineplayer.tga",
					"children" :
					( 
						{ "name" : "title_onlineplayer", "type":"text", "x": 211, "y": 3, "text_horizontal_align" : "center", "text" : localeInfo.ADMINPANEL_STARTPAGE_ONLINEBOARD_TITLE},
						{ "name" : "text_shinsoo", "type":"text", "x": 54, "y": 25, "text_horizontal_align" : "center", "color" : COLOR_RED, "text" : localeInfo.ADMINPANEL_STARTPAGE_ONLINEBOARD_RED},
						{ "name" : "text_chunjo", "type":"text", "x": 159, "y": 25, "text_horizontal_align" : "center", "color" : COLOR_YELLOW, "text" : localeInfo.ADMINPANEL_STARTPAGE_ONLINEBOARD_YELLOW},
						{ "name" : "text_jinno", "type":"text", "x": 265, "y": 25, "text_horizontal_align" : "center", "color" : COLOR_BLUE, "text" : localeInfo.ADMINPANEL_STARTPAGE_ONLINEBOARD_BLUE},
						{ "name" : "text_total", "type":"text", "x": 369, "y": 25, "text_horizontal_align" : "center", "text" : localeInfo.ADMINPANEL_STARTPAGE_ONLINEBOARD_TOTAL},
						
						{ "name" : "info_shinsoo", "type":"text", "x": 54, "y": 46, "text_horizontal_align" : "center", "text" : "0"},
						{ "name" : "info_chunjo", "type":"text", "x": 159, "y": 46, "text_horizontal_align" : "center", "text" : "0"},
						{ "name" : "info_jinno", "type":"text", "x": 265, "y": 46, "text_horizontal_align" : "center", "text" : "0"},
						{ "name" : "info_total", "type":"text", "x": 369, "y": 46, "text_horizontal_align" : "center", "text" : "0"},
					),
				},
				{ "type" : "bar", "x" : 15, "y" : 122, "width" : 422, "height" : 1, "color" : 0xFF6d6d6d, },
				{
					"name" : "command_list_background", "type" : "thinboard_circle",
					"x" : 15, "y" : 170, "width" : 422, "height" : 181,
				},
			),
		},
	),
}