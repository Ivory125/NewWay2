import uiScriptLocale
import localeInfo
import constInfo

BOARD_WIDTH = 480
BOARD_HEIGHT = 370

window = {
	"name" : "AdminPanelTemplateWindow",
	"x" : (SCREEN_WIDTH / 2) - (BOARD_WIDTH / 2), 
	"y" : (SCREEN_HEIGHT / 2) - (BOARD_HEIGHT / 2) ,
	"width" : BOARD_WIDTH, 
	"height" : BOARD_HEIGHT,
	"style" : ("float",),
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,
			"title" : "TEMPLATE-WINDOW",
			"children" :
			(
			)
		},
	),
}