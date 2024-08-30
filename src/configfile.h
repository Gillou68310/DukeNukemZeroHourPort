#ifndef CONFIGFILE_H
#define CONFIGFILE_H

extern bool         configFullscreen;
extern bool         configDebug;
extern unsigned int configKeyA[4];
extern unsigned int configKeyB[4];
extern unsigned int configKeyStart[4];
extern unsigned int configKeyR[4];
extern unsigned int configKeyL[4];
extern unsigned int configKeyZ[4];
extern unsigned int configKeyCUp[4];
extern unsigned int configKeyCDown[4];
extern unsigned int configKeyCLeft[4];
extern unsigned int configKeyCRight[4];
extern unsigned int configKeyStickUp[4];
extern unsigned int configKeyStickDown[4];
extern unsigned int configKeyStickLeft[4];
extern unsigned int configKeyStickRight[4];
extern unsigned int configKeyJoyUp[4];
extern unsigned int configKeyJoyDown[4];
extern unsigned int configKeyJoyLeft[4];
extern unsigned int configKeyJoyRight[4];

void configfile_load(const char *filename);
void configfile_save(const char *filename);

#endif
