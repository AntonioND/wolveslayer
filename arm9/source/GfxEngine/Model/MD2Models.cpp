#include "MD2.h"

MD2Entity Models[MD2_Count];
bool ModelEnable[MD2_Count];

u16* ColorContainer;

void Precalcmd2light(int n,int num){
	if ( (n < 0) || (n > Models[num].header.num_frames - 1))n=Models[num].header.num_frames - 1;
	if(ModelEnable[num]==false)return;

	nds_vertex_t* vert=Models[num].rahmen[n].verts;
	free(ColorContainer);
	ColorContainer = (u16 *)malloc (2*Models[num].header.num_vertices);
	 
	int rot=GetLightRot()*2;  
	for(int i = 0; i < Models[num].header.num_vertices; ++i)
		GrapMd2Light( (vert[i].colorindex & (3<<rot))>>rot ,ColorContainer[i]);
}

//Functions
void RenderMD2Model (int n,int num){	
	int i, j,stnum,vertnum;
	//MD2Entity mod=Models[num];
	if ( (n < 0) || (n > Models[num].header.num_frames - 1))n=Models[num].header.num_frames - 1;
	if(ModelEnable[num]==false)return;
	
	nds_vertex_t* vert=Models[num].rahmen[n].verts;
		glBegin (GL_TRIANGLES);		
			for (i = 0; i < Models[num].header.num_tris; ++i){
				for (j = 0; j < 3; ++j){	
					stnum=Models[num].dreiecke[i].st[j];
					vertnum=Models[num].dreiecke[i].vertex[j];
					GFX_COLOR = (vuint32)ColorContainer[vertnum];
					glTexCoord2t16 (Models[num].flaechenkoords[stnum].s, Models[num].flaechenkoords[stnum].t);
					glVertex3v16 (vert[vertnum].v[0],vert[vertnum].v[1], vert[vertnum].v[2]);     
				}
			}
		glEnd ();
}

void RenderMD2ModelMirrowed (int n,int num){	
	int i, j,stnum,vertnum;
	//MD2Entity mod=&Models[num];
	if ( (n < 0) || (n > Models[num].header.num_frames - 1))n=Models[num].header.num_frames - 1;
	if(ModelEnable[num]==false)return;
	
	nds_vertex_t* vert=Models[num].rahmen[n].verts;
		glBegin (GL_TRIANGLES);		
			for (i = 0; i < Models[num].header.num_tris; ++i){
				for (j = 0; j < 3; ++j){	
					stnum=Models[num].dreiecke[i].st[j];
					vertnum=Models[num].dreiecke[i].vertex[j];
					GFX_COLOR = (vuint32)ColorContainer[vertnum];
					glTexCoord2t16 (Models[num].flaechenkoords[stnum].s, Models[num].flaechenkoords[stnum].t);
					glVertex3v16 (vert[vertnum].v[0],vert[vertnum].v[1], -vert[vertnum].v[2]);     
				}
			}
		glEnd ();
}
