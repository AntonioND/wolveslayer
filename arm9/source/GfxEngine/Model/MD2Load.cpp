#include "MD2.h"

/* table of precalculated normals */
vec3_t anorms_table[162] = {
#include "../Anorms.h"
};

// The table should be copied and ocnverted to here
v16 anormtable16[162][3];

v16 zero = floattov16(0.0f);
extern MD2Entity Models[MD2_Count];
extern bool ModelEnable[MD2_Count];

extern void MakeCollisionMap(int modelnum);

void InitTableOfNormal(void)
{
    for (int a = 0; a < 162; a++) {
        anormtable16[a][0] = floattov16(anorms_table[a][0]);
        anormtable16[a][1] = floattov16(anorms_table[a][1]);
        anormtable16[a][2] = floattov16(anorms_table[a][2]);
    }
}

void FreeModels(void)
{
    int a, N;
    for (a = 0; a < MD2_Count; a++) {
        if (ModelEnable[a] == true) {
            for (N = 0; N < Models[a].header.num_frames; ++N)
                free(Models[a].rahmen[N].verts);
            free(Models[a].rahmen);
            free(Models[a].flaechenkoords);
            free(Models[a].dreiecke);
            ModelEnable[a] = false;
        }
    }
}

void LoadMD2Model(char Filename[], int num, int widthheight, int scale)
{
    bool texturereverse = true;

    // If the last character of the name is m, reverse the texture. This is done
    // to tell apart models with the extension md2 and mdm.
    if (Filename[strlen(Filename) - 1] == 'm')
        texturereverse = false;

    FILE *fp = fopen(Filename, "rb");
    if (fp == NULL) {
        PrintOUT(Filename, 5, 5, false, strlen(Filename));
        PrintOUT("wasnt found", 5, 15, false, strlen("wasnt found"));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    int i;

    /* read header */
    fread(&Models[num].header, 1, sizeof(md2_header_t), fp);

    if ((Models[num].header.ident != 844121161) || (Models[num].header.version != 8)) {
        /* error! */
        fprintf(stderr, "error: bad version!");
        while (1 == 1) {
        } // Can't go on so lets hang arround
    }
    /* memory allocation */
    Models[num].texcoords = (md2_texCoord_t *)malloc(sizeof(md2_texCoord_t) * Models[num].header.num_st);
    Models[num].triangles = (md2_triangle_t *)malloc(sizeof(md2_triangle_t) * Models[num].header.num_tris);
    Models[num].frames    = (md2_frame_t *)malloc(sizeof(md2_frame_t) * Models[num].header.num_frames);

    // Read float textcoords
    fseek(fp, Models[num].header.offset_st, SEEK_SET);
    fread(Models[num].texcoords, sizeof(md2_texCoord_t), Models[num].header.num_st, fp);

    // read that float triangles
    fseek(fp, Models[num].header.offset_tris, SEEK_SET);
    fread(Models[num].triangles, sizeof(md2_triangle_t), Models[num].header.num_tris, fp);

    /* read frames */
    fseek(fp, Models[num].header.offset_frames, SEEK_SET);
    for (i = 0; i < Models[num].header.num_frames; ++i) {
        /* memory allocation for vertices of this frame */
        Models[num].frames[i].verts = (md2_vertex_t *)malloc(sizeof(md2_vertex_t) * Models[num].header.num_vertices);

        /* read frame data */
        fread(Models[num].frames[i].scale, sizeof(vec3_t), 1, fp);
        fread(Models[num].frames[i].translate, sizeof(vec3_t), 1, fp);
        fread(Models[num].frames[i].name, sizeof(char), 16, fp);
        fread(Models[num].frames[i].verts, sizeof(md2_vertex_t), Models[num].header.num_vertices, fp);
    }

    fclose(fp);

    //<<<<<<<<Extatcly here we got a standard md2 model stored in a standard way
    // Now we convert them for a superfast nds renderway
    // ok erstma den shit mallocen
    Models[num].flaechenkoords = (nds_texCoord_t *)malloc(sizeof(nds_texCoord_t) * Models[num].header.num_st);
    Models[num].dreiecke       = (nds_triangle_t *)malloc(sizeof(nds_triangle_t) * Models[num].header.num_tris);
    Models[num].rahmen         = (nds_frame_t *)malloc(sizeof(nds_frame_t) * Models[num].header.num_frames);
    for (i = 0; i < Models[num].header.num_frames; ++i)
        Models[num].rahmen[i].verts = (nds_vertex_t *)malloc(sizeof(nds_vertex_t) * Models[num].header.num_vertices);

    // nun schön brav eins nach dem anderen von float und den dreck in fixed point math konvertieren
    int I, j, N;
    float s, t;
    vec3_t v;
    md2_frame_t *pframe;
    md2_vertex_t *pvert; // der kram is nötig dafür

    // also framedata in rahmendaten umwandeln
    for (N = 0; N < Models[num].header.num_frames; ++N) {
        for (I = 0; I < Models[num].header.num_tris; ++I) {
            for (j = 0; j < 3; ++j) {
                pframe = &Models[num].frames[N];
                pvert  = &pframe->verts[Models[num].triangles[I].vertex[j]];
                // dekomresse die aktuelle framedate
                v[0] = (pframe->scale[0] * pvert->v[0]) + pframe->translate[0];
                v[1] = (pframe->scale[1] * pvert->v[1]) + pframe->translate[1];
                v[2] = (pframe->scale[2] * pvert->v[2]) + pframe->translate[2];
                // Lager die unkompressed framevertexe
                Models[num].rahmen[N].verts[Models[num].triangles[I].vertex[j]].v[0] = floattov16(float(v[0] / 128) * scale);
                Models[num].rahmen[N].verts[Models[num].triangles[I].vertex[j]].v[1] = floattov16(float(v[1] / 128) * scale);
                Models[num].rahmen[N].verts[Models[num].triangles[I].vertex[j]].v[2] = floattov16(float(v[2] / 128) * scale);
                // Kopiere den framename
                strcpy(Models[num].rahmen[N].name, pframe->name);
            }
        }
    }

    f32 normx = 0, normz = 0;
    // Normal indices
    int g;
    for (N = 0; N < Models[num].header.num_frames; ++N) {
        for (I = 0; I < Models[num].header.num_vertices; ++I) {
            Models[num].rahmen[N].verts[I].colorindex = 0;
            for (g = 0; g < 4; g++) {
                if (g == 0) {
                    normz = anormtable16[Models[num].frames[N].verts[I].normalIndex][0];
                    normx = anormtable16[Models[num].frames[N].verts[I].normalIndex][1];
                }
                if (g == 1) {
                    normz = anormtable16[Models[num].frames[N].verts[I].normalIndex][1];
                    normx = -anormtable16[Models[num].frames[N].verts[I].normalIndex][0];
                }
                if (g == 2) {
                    normz = -anormtable16[Models[num].frames[N].verts[I].normalIndex][0];
                    normx = -anormtable16[Models[num].frames[N].verts[I].normalIndex][1];
                }
                if (g == 3) {
                    normz = -anormtable16[Models[num].frames[N].verts[I].normalIndex][1];
                    normx = anormtable16[Models[num].frames[N].verts[I].normalIndex][0];
                }

                if (normz <= zero) {
                    if (g == 0)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= 0;
                    if (g == 0)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= 1;
                    if (g == 1)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (0 << 2);
                    if (g == 1)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (1 << 2);
                    if (g == 2)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (0 << 4);
                    if (g == 2)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (1 << 4);
                    if (g == 3)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (0 << 6);
                    if (g == 3)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (1 << 6);
                }

                if (normz > zero) {
                    if (g == 0)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex = 2;
                    if (g == 0)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex = 3;
                    if (g == 1)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (2 << 2);
                    if (g == 1)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (3 << 2);
                    if (g == 2)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (2 << 4);
                    if (g == 2)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (3 << 4);
                    if (g == 3)
                        if (normx < zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (2 << 6);
                    if (g == 3)
                        if (normx >= zero)
                            Models[num].rahmen[N].verts[I].colorindex |= (3 << 6);
                }
            }
        }
    }

    // Nun zu den texturkooridinaten
    for (I = 0; I < Models[num].header.num_tris; ++I) {
        for (j = 0; j < 3; ++j) {
            if (texturereverse) {
                s = (float)Models[num].texcoords[Models[num].triangles[I].st[j]].s / Models[num].header.skinwidth;
                t = (float)Models[num].texcoords[Models[num].triangles[I].st[j]].t / Models[num].header.skinheight;
                Models[num].flaechenkoords[Models[num].triangles[I].st[j]].s = floattot16(float(s) * widthheight);
                Models[num].flaechenkoords[Models[num].triangles[I].st[j]].t = floattot16(float(1 - t) * widthheight);
            }

            if (!texturereverse) {
                s = (float)Models[num].texcoords[Models[num].triangles[I].st[j]].s / Models[num].header.skinwidth;
                t = (float)Models[num].texcoords[Models[num].triangles[I].st[j]].t / Models[num].header.skinheight;
                Models[num].flaechenkoords[Models[num].triangles[I].st[j]].s = floattot16(float(s) * widthheight);
                Models[num].flaechenkoords[Models[num].triangles[I].st[j]].t = floattot16(float(t) * widthheight);
            }
            Models[num].dreiecke[I].st[j]     = Models[num].triangles[I].st[j];     // Index for texture coords
            Models[num].dreiecke[I].vertex[j] = Models[num].triangles[I].vertex[j]; // Index for vertices
        }
    }

    for (N = 0; N < Models[num].header.num_frames; ++N)
        free(Models[num].frames[N].verts);
    free(Models[num].frames);
    free(Models[num].texcoords);
    free(Models[num].triangles);
    ModelEnable[num] = true;

    // Now calculate the collision...
    MakeCollisionMap(num);
}

void LoadModelTexture(char filename[], int *Target, int *TargetPal, int num, int *w)
{
    u8 *buffer8 = NULL;
    u16 pal[256];
    int width  = 0;
    int height = 0;

    FILE *bmp = fopen(filename, "r");
    if (bmp == NULL) {
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("wasnt found", 5, 15, false, strlen("wasnt found"));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 18, SEEK_SET);

    fread(&width, 4, 1, bmp);
    fread(&height, 4, 1, bmp);
    w[num] = width;

    if ((width != 64 || height != 64) && (width != 128 || height != 128)) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 64x64 or 128x128...", 5, 15, false, strlen("Texture isnt 64x64 or 128x128..."));
        sprintf(bug, "its %dx%d", width, height);
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    unsigned long colorCoding;
    fread(&colorCoding, 4, 1, bmp);

    if (((colorCoding & 0xFFFF0000) >> 16) != 8) {
        char bug[40];
        PrintOUT(filename, 5, 5, false, strlen(filename));
        PrintOUT("Texture isnt 8bit...", 5, 15, false, strlen("Texture isnt 8bit..."));
        sprintf(bug, "its %dBit", int((colorCoding & 0xFFFF0000) >> 16));
        PrintOUT(bug, 5, 25, false, strlen(bug));
        while (!(keysDown() & KEY_A))
            scanKeys();
        ScreenModeLOADING();
        return;
    }

    fseek(bmp, 34, SEEK_SET);
    unsigned long dataLength;
    fread(&dataLength, 4, 1, bmp);

    fseek(bmp, 46, SEEK_SET);
    unsigned long impcol;
    fread(&impcol, 4, 1, bmp);

    int i, q;

    switch ((colorCoding & 0xFFFF0000) >> 16) {
        case 8:
            // First read the pal
            fseek(bmp, 54, SEEK_SET);
            for (i = 0; i < 256; i++) {
                unsigned char r, g, b;
                unsigned long color;
                fread(&color, 4, 1, bmp);
                b = (color & 0x0FF);
                g = ((color >> 8) & 0x0FF);
                r = ((color >> 16) & 0x0FF);
                // pal[i]=RGB15(r>>3,g>>3,b>>3)| BIT(15);
                pal[i] = RGB15(r >> 3, g >> 3, b >> 3);
            }
            buffer8 = (u8 *)malloc(width * height);
            for (i = 0; i < height; i++) {
                for (q = 0; q < width; q++) {
                    u8 color;
                    fread(&color, 1, 1, bmp);
                    buffer8[q + (((height - 1) - i) * width)] = color;
                }
            }
            break;
        default:
            fprintf(stderr, "Invalid BMP format: %s", filename);
            while (1)
                ;
            break;
    };

    fclose(bmp);

    glGenTextures(1, &Target[num]);
    glBindTexture(0, Target[num]);

    WaitForFreeVblank();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB256, width, height, 0, TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, buffer8);
    glColorTableEXT(GL_TEXTURE_2D, 0, 256, 0, 0, pal);

    free(buffer8);
}
