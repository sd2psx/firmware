/**
 * @file lv_theme_mono.h
 *
 */

#ifndef UI_USE_THEME_MONO_H
#define UI_USE_THEME_MONO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the theme
 * @param color_primary the primary color of the theme
 * @param color_secondary the secondary color for the theme
 * @param font pointer to a font to use.
 * @return a pointer to reference this theme later
 */
lv_theme_t * ui_theme_mono_init(lv_disp_t * disp, bool dark_bg, const lv_font_t * font);

/**
* Check if the theme is initialized
* @return true if default theme is initialized, false otherwise
*/
bool ui_theme_mono_is_inited(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif
