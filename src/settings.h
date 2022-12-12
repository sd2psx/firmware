#pragma once

void settings_init(void);

int settings_get_ps1_card(void);
int settings_get_ps1_channel(void);
void settings_set_ps1_card(int x);
void settings_set_ps1_channel(int x);

int settings_get_ps2_card(void);
int settings_get_ps2_channel(void);
void settings_set_ps2_card(int x);
void settings_set_ps2_channel(int x);

enum {
    MODE_PS1 = 0,
    MODE_PS2 = 1,
};

int settings_get_mode(void);
void settings_set_mode(int mode);
