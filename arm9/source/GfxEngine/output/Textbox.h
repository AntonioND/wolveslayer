#ifndef GFXENGINE_OUTPUT_TEXTBOX_H__
#define GFXENGINE_OUTPUT_TEXTBOX_H__

extern int npctalk;

extern int CurWord;
extern int wordnum;

extern bool nextpage;

// will be called if char talks to NPC or other things
void TextBoxmode(char TxtOffset[10][256], int cnt, int num);
// This will put char by char the text to Dialogbox...
void TextBoxmodeHandler(void);
void ShownextPage(void);

#endif // GFXENGINE_OUTPUT_TEXTBOX_H__
