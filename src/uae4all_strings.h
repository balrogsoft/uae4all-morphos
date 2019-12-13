#ifndef UAE4ALL_STRINGS_H
#define UAE4ALL_STRINGS_H 1

/* Locale Catalog Source File
 *
 * Automatically created by SimpleCat V3
 * Do NOT edit by hand!
 *
 * SimpleCat ©1992-2011 Guido Mersmann
 *
 */

#define CATCOMP_NUMBERS 1


/****************************************************************************/


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif



/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_Load_Df0 0
#define MSG_Load_Df1 1
#define MSG_Eject_Df0 2
#define MSG_Eject_Df1 3
#define MSG_Save_State 4
#define MSG_State_Position 5
#define MSG_State_Load 6
#define MSG_State_Save 7
#define MSG_Throttle 8
#define MSG_Screen_VPos 9
#define MSG_Frameskip 10
#define MSG_Frameskip_Auto 11
#define MSG_Sound 12
#define MSG_Save_Disk 13
#define MSG_Reset 14
#define MSG_Run 15
#define MSG_About 16
#define MSG_Quit 17
#define MSG_About_Desc 18
#define MSG_About_Auth 19
#define MSG_NOROM 20

#define CATCOMP_LASTID 20

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_Load_Df0_STR "Load Df0 ..."
#define MSG_Load_Df1_STR "Load Df1 ..."
#define MSG_Eject_Df0_STR "Eject Df0 ..."
#define MSG_Eject_Df1_STR "Eject Df1 ..."
#define MSG_Save_State_STR "Save State ..."
#define MSG_State_Position_STR "Position:"
#define MSG_State_Load_STR "Load"
#define MSG_State_Save_STR "Save"
#define MSG_Throttle_STR "Throttle:"
#define MSG_Screen_VPos_STR "Screen V. Pos:"
#define MSG_Frameskip_STR "Frameskip:"
#define MSG_Frameskip_Auto_STR "Auto"
#define MSG_Sound_STR "Sound"
#define MSG_Save_Disk_STR "Save Disk"
#define MSG_Reset_STR "Reset"
#define MSG_Run_STR "Run"
#define MSG_About_STR "About ..."
#define MSG_Quit_STR "Quit"
#define MSG_About_Desc_STR "UAE4All Amiga 500 Emulator."
#define MSG_About_Auth_STR "MorphOS port by Pedro Gil Guirado."
#define MSG_NOROM_STR "Can't find kick.rom file."

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MSG_Load_Df0,(STRPTR)MSG_Load_Df0_STR},
    {MSG_Load_Df1,(STRPTR)MSG_Load_Df1_STR},
    {MSG_Eject_Df0,(STRPTR)MSG_Eject_Df0_STR},
    {MSG_Eject_Df1,(STRPTR)MSG_Eject_Df1_STR},
    {MSG_Save_State,(STRPTR)MSG_Save_State_STR},
    {MSG_State_Position,(STRPTR)MSG_State_Position_STR},
    {MSG_State_Load,(STRPTR)MSG_State_Load_STR},
    {MSG_State_Save,(STRPTR)MSG_State_Save_STR},
    {MSG_Throttle,(STRPTR)MSG_Throttle_STR},
    {MSG_Screen_VPos,(STRPTR)MSG_Screen_VPos_STR},
    {MSG_Frameskip,(STRPTR)MSG_Frameskip_STR},
    {MSG_Frameskip_Auto,(STRPTR)MSG_Frameskip_Auto_STR},
    {MSG_Sound,(STRPTR)MSG_Sound_STR},
    {MSG_Save_Disk,(STRPTR)MSG_Save_Disk_STR},
    {MSG_Reset,(STRPTR)MSG_Reset_STR},
    {MSG_Run,(STRPTR)MSG_Run_STR},
    {MSG_About,(STRPTR)MSG_About_STR},
    {MSG_Quit,(STRPTR)MSG_Quit_STR},
    {MSG_About_Desc,(STRPTR)MSG_About_Desc_STR},
    {MSG_About_Auth,(STRPTR)MSG_About_Auth_STR},
    {MSG_NOROM,(STRPTR)MSG_NOROM_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x0E"
    MSG_Load_Df0_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x0E"
    MSG_Load_Df1_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x0E"
    MSG_Eject_Df0_STR "\x00"
    "\x00\x00\x00\x03\x00\x0E"
    MSG_Eject_Df1_STR "\x00"
    "\x00\x00\x00\x04\x00\x10"
    MSG_Save_State_STR "\x00\x00"
    "\x00\x00\x00\x05\x00\x0A"
    MSG_State_Position_STR "\x00"
    "\x00\x00\x00\x06\x00\x06"
    MSG_State_Load_STR "\x00\x00"
    "\x00\x00\x00\x07\x00\x06"
    MSG_State_Save_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x0A"
    MSG_Throttle_STR "\x00"
    "\x00\x00\x00\x09\x00\x10"
    MSG_Screen_VPos_STR "\x00\x00"
    "\x00\x00\x00\x0A\x00\x0C"
    MSG_Frameskip_STR "\x00\x00"
    "\x00\x00\x00\x0B\x00\x06"
    MSG_Frameskip_Auto_STR "\x00\x00"
    "\x00\x00\x00\x0C\x00\x06"
    MSG_Sound_STR "\x00"
    "\x00\x00\x00\x0D\x00\x0A"
    MSG_Save_Disk_STR "\x00"
    "\x00\x00\x00\x0E\x00\x06"
    MSG_Reset_STR "\x00"
    "\x00\x00\x00\x0F\x00\x04"
    MSG_Run_STR "\x00"
    "\x00\x00\x00\x10\x00\x0A"
    MSG_About_STR "\x00"
    "\x00\x00\x00\x11\x00\x06"
    MSG_Quit_STR "\x00\x00"
    "\x00\x00\x00\x12\x00\x1C"
    MSG_About_Desc_STR "\x00"
    "\x00\x00\x00\x13\x00\x24"
    MSG_About_Auth_STR "\x00\x00"
    "\x00\x00\x00\x14\x00\x1A"
    MSG_NOROM_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/



struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};





#ifdef CATCOMP_CODE

#include <libraries/locale.h>
#include <proto/locale.h>


STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */



/****************************************************************************/



#endif /* UAE4ALL_STRINGS_H */

