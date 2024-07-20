#ifndef GFXENGINE_LIMITS_H__
#define GFXENGINE_LIMITS_H__

#define Autotile_Max              4  // Max. number of autotile types
#define Autotile_Ignorecolors_Max 10 // Max. number of ignored colors in an autotile

#define Ground_Max 31 // Max. number of ground textures (normal or with bump mapping)

#define Villagers_Max        5  // Max. number of NPCs
#define VillagerSpeeches_Max 10 // Max. number of speech strings of an NPC

#define Enemies_Max 10 // Max. number of enemies in the map at the same time

// Object-related definitions
#define Object_Max        26 // Max. number of objects
#define ObjectTexture_Max 23 // Max. number of textures available for objects

// Hardcoded IDs used for textures for figures (character, weapon, enemies and villagers)
#define CharacterFigureTex_ID   0
#define WeaponFigureTex_ID      1
#define EnemyFigureBaseTex_ID   2

// Amount of textures for figures (character, weapon, enemies and villagers)
#define CharacterFigureTex_Max 1 // Max. number of figures for characters
#define WeaponFigureTex_Max    1 // Max. number of figures for weapons
#define EnemyFigureTex_Max     8 // Max. number of figures for enemies and villagers
#define FigureTex_Max          (CharacterFigureTex_Max + WeaponFigureTex_Max + EnemyFigureTex_Max)

#define MD2_Max 62 // Max. number of MD2 models loaded (enemies, objects, etc)

#define Edge_Max 10 // Max. number of edges

#define ItemType_Max  25  // Max. number of object types allowed
#define Inventory_Max 100 // Max. number of objects in the inventory

#define EventSpeech_Max 10 // Max. number of speech strings

#define MapChange_Max       25 // Max. number of map changes and doors
#define MapChangeSpeech_Max 10 // Max. number of speech strings of a map change

#define DynamicLight_Max 10 // Max. number of lights that can move around the map

#endif // GFXENGINE_LIMITS_H__
