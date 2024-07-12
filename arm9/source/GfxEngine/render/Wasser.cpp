#include "../3D.h"

extern u8 EnvR,EnvG,EnvB;
extern int Blend;

void RenderWasser (int floating, f32 x, f32 y, f32 z)
{
	v16 size=floattov16(.6);
	glPolyFmt(POLY_ALPHA(10) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1|POLY_ID(1));

	glPushMatrix ();	
	glTranslatef32 (x, y, z); 

	u8 r=EnvR,g=EnvG,b=EnvB;
	if(Blend!=8){
		r=EnvR/8*Blend;
		g=EnvG/8*Blend;
		b=EnvB/8*Blend;
	}
	glColor3b(r,g,b);
	
	glBegin (GL_QUADS); 

		// Bottom left
		glTexCoord2t16 (inttot16(floating), inttot16(32+floating));
		glVertex3v16 (-size, 0, -size);

		// Top left
		glTexCoord2t16 (inttot16(32+floating), inttot16(32+floating));
		glVertex3v16 (-size, 0, size);

		// Top right
		glTexCoord2t16 (inttot16(32+floating), inttot16(floating));
		glVertex3v16 (size, 0, size);

		// Bottom right
		glTexCoord2t16 (inttot16(floating), inttot16(floating));
		glVertex3v16 (size, 0, -size);
	
	glEnd ();

	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK  | POLY_FORMAT_LIGHT0);
	glPopMatrix(1);	 
}
