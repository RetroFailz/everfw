/*
 * Rockchip App
 *
 * Copyright (C) 2017 Rockchip Electronics Co., Ltd.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL), available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __UI_1024X600_H__
#define __UI_1024X600_H__

#define LCD_W    1024
#define LCD_H    600

#define TTF_FONT_SIZE     16

#define BATT_PINT_X    LCD_W - 50
#define BATT_PINT_Y    20
#define BATT_PINT_W    28
#define BATT_PINT_H    15

#define BG_PINT_X    0
#define BG_PINT_Y    0
#define BG_PINT_W    LCD_W
#define BG_PINT_H    LCD_H

#define DESKTOP_DLG_X    0
#define DESKTOP_DLG_Y    80
#define DESKTOP_DLG_W    LCD_W - DESKTOP_DLG_X
#define DESKTOP_DLG_H    LCD_H - DESKTOP_DLG_Y
#define DESKTOP_DLG_STRING    "desktop"

#define TITLE_PINT_X    20
#define TITLE_PINT_Y    20
#define TITLE_PINT_W    130
#define TITLE_PINT_H    24

#define TITLE_LINE_PINT_X    0
#define TITLE_LINE_PINT_Y    46
#define TITLE_LINE_PINT_W    LCD_W
#define TITLE_LINE_PINT_H    2

//desktop_dialog
#define ICON_SPAC      150
#define GAME_PINT_W    100
#define GAME_PINT_H    100
#define GAME_PINT_X    90
#define GAME_PINT_Y    LCD_H - GAME_PINT_H - 16

#define MUSIC_PINT_X    GAME_PINT_X + ICON_SPAC
#define MUSIC_PINT_Y    GAME_PINT_Y
#define MUSIC_PINT_W    GAME_PINT_W
#define MUSIC_PINT_H    GAME_PINT_H

#define PHOTO_PINT_X    MUSIC_PINT_X + ICON_SPAC
#define PHOTO_PINT_Y    GAME_PINT_Y
#define PHOTO_PINT_W    GAME_PINT_W
#define PHOTO_PINT_H    GAME_PINT_H

#define VIDEO_PINT_X    PHOTO_PINT_X + ICON_SPAC
#define VIDEO_PINT_Y    GAME_PINT_Y
#define VIDEO_PINT_W    GAME_PINT_W
#define VIDEO_PINT_H    GAME_PINT_H

#define FOLDE_PINT_X    VIDEO_PINT_X + ICON_SPAC
#define FOLDE_PINT_Y    GAME_PINT_Y
#define FOLDE_PINT_W    GAME_PINT_W
#define FOLDE_PINT_H    GAME_PINT_H

#define SETTING_PINT_X    FOLDE_PINT_X + ICON_SPAC
#define SETTING_PINT_Y    GAME_PINT_Y
#define SETTING_PINT_W    GAME_PINT_W
#define SETTING_PINT_H    GAME_PINT_H

#define MENU_ICON_ZOOM_W    (GAME_PINT_W / 5)
#define MENU_ICON_ZOOM_H    (GAME_PINT_H / 5)

#define GAME_ICON_PINT_X    100
#define GAME_ICON_PINT_Y    200
#define GAME_ICON_PINT_W    160
#define GAME_ICON_PINT_H    90
#define GAME_ICON_SPAC      220
#define GAME_ICON_ZOOM_W    (GAME_ICON_PINT_W / 5)
#define GAME_ICON_ZOOM_H    (GAME_ICON_PINT_H / 5)

#define GAME_ICON_NUM_PERPAGE  4

#define DESKTOP_PAGE_DOT_X    (LCD_W / 2)
#define DESKTOP_PAGE_DOT_Y    (GAME_PINT_Y - 10)
#define DESKTOP_PAGE_DOT_DIA  4
#define DESKTOP_PAGE_DOT_SPAC  40

//audioplay_dialog
#define ALBUM_ICON_PINT_W    128
#define ALBUM_ICON_PINT_H    128
#define ALBUM_ICON_PINT_X    ((LCD_W - ALBUM_ICON_PINT_W) / 2)
#define ALBUM_ICON_PINT_Y    ((LCD_H - ALBUM_ICON_PINT_H) / 2 - 60)

#define FILENAME_PINT_W    LCD_W
#define FILENAME_PINT_H    24
#define FILENAME_PINT_X    0
#define FILENAME_PINT_Y    (ALBUM_ICON_PINT_Y + ALBUM_ICON_PINT_H + 20)

#define FILENUM_PINT_W    LCD_W
#define FILENUM_PINT_H    24
#define FILENUM_PINT_X    0
#define FILENUM_PINT_Y    (LCD_H - FILENUM_PINT_H - 10)

#define PROGRESSBAR_PINT_X    20
#define PROGRESSBAR_PINT_Y    (LCD_H - 50)
#define PROGRESSBAR_PINT_W    (LCD_W - PROGRESSBAR_PINT_X * 2)
#define PROGRESSBAR_PINT_H    4

#define TIME_PINT_W    100
#define TIME_PINT_H    24
#define TIME_PINT_X    (LCD_W - PROGRESSBAR_PINT_X - TIME_PINT_W)
#define TIME_PINT_Y    (PROGRESSBAR_PINT_Y - TIME_PINT_H)

#define PLAYSTATUS_PINT_W    12
#define PLAYSTATUS_PINT_H    16
#define PLAYSTATUS_PINT_X    (PROGRESSBAR_PINT_X)
#define PLAYSTATUS_PINT_Y    (PROGRESSBAR_PINT_Y - PLAYSTATUS_PINT_H - 8)

//browser_dialog
#define BROWSER_LIST_STR_PINT_X    70
#define BROWSER_LIST_STR_PINT_Y    57
#define BROWSER_LIST_STR_PINT_W    24
#define BROWSER_LIST_STR_PINT_H    24
#define BROWSER_LIST_STR_PINT_SPAC      36

#define BROWSER_LIST_PIC_PINT_W    32
#define BROWSER_LIST_PIC_PINT_H    32

#define BROWSER_LIST_SEL_PINT_H    36

#define FILE_NUM_PERPAGE  14

#define BROWSER_PAGE_DOT_X    (LCD_W / 2)
#define BROWSER_PAGE_DOT_Y    (LCD_H - 10)
#define BROWSER_PAGE_DOT_DIA  4
#define BROWSER_PAGE_DOT_SPAC  40
//setting
#define SETTING_NUM_PERPAGE    14

#define SETTING_LIST_STR_PINT_X    30
#define SETTING_LIST_STR_PINT_Y    57
#define SETTING_LIST_STR_PINT_W    24
#define SETTING_LIST_STR_PINT_H    24
#define SETTING_LIST_STR_PINT_SPAC      36

#define SETTING_LIST_SEL_PINT_H    36

#define SETTING_LIST_DOT_PINT_X    LCD_W - 40
#define SETTING_LIST_DOT_PINT_W    16
#define SETTING_LIST_DOT_PINT_H    16

#define SETTING_INFO_PINT_X    30
#define SETTING_INFO_PINT_Y    70
#define SETTING_INFO_PINT_W    LCD_W - SETTING_INFO_PINT_X *2
#define SETTING_INFO_PINT_H    24
#define SETTING_INFO_PINT_SPAC      30

#define SETTING_PAGE_DOT_X    (LCD_W / 2)
#define SETTING_PAGE_DOT_Y    (LCD_H - 10)
#define SETTING_PAGE_DOT_DIA  4
#define SETTING_PAGE_DOT_SPAC  40
//videoplay_hw_dialog
#define VIDEO_TOPBAR_H           48
#define VIDEO_BOTTOMBAR_H        80

#define VIDEO_FILENAME_PINT_W    LCD_W
#define VIDEO_FILENAME_PINT_H    24
#define VIDEO_FILENAME_PINT_X    0
#define VIDEO_FILENAME_PINT_Y    20

#define VIDEO_FILENUM_PINT_W    LCD_W
#define VIDEO_FILENUM_PINT_H    24
#define VIDEO_FILENUM_PINT_X    0
#define VIDEO_FILENUM_PINT_Y    (LCD_H - VIDEO_FILENUM_PINT_H - 10)

#define VIDEO_PROGRESSBAR_PINT_X    20
#define VIDEO_PROGRESSBAR_PINT_Y    (LCD_H - 40)
#define VIDEO_PROGRESSBAR_PINT_W    (LCD_W - VIDEO_PROGRESSBAR_PINT_X * 2)
#define VIDEO_PROGRESSBAR_PINT_H    4

#define VIDEO_TIME_PINT_W    100
#define VIDEO_TIME_PINT_H    24
#define VIDEO_TIME_PINT_X    (LCD_W - VIDEO_PROGRESSBAR_PINT_X - VIDEO_TIME_PINT_W)
#define VIDEO_TIME_PINT_Y    (VIDEO_PROGRESSBAR_PINT_Y - VIDEO_TIME_PINT_H)

#define VIDEO_PLAYSTATUS_PINT_W    12
#define VIDEO_PLAYSTATUS_PINT_H    16
#define VIDEO_PLAYSTATUS_PINT_X    (VIDEO_PROGRESSBAR_PINT_X)
#define VIDEO_PLAYSTATUS_PINT_Y    (VIDEO_PROGRESSBAR_PINT_Y - VIDEO_PLAYSTATUS_PINT_H - 8)
#endif
