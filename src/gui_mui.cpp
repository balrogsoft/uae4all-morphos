 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

#include <stdio.h>
#include <stdlib.h>

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"
#include "uae.h"
#include "options.h"
#include "gui.h"
//#include "menu.h"
#include "vkbd.h"
#include "debug_uae4all.h"
#include "custom.h"
#include "memory.h"
#include "xwin.h"
#include "drawing.h"
#include "sound.h"
#include "audio.h"
#include "keybuf.h"
#include "disk.h"
#include "savestate.h"


#include <mui/Aboutbox_mcc.h>

#include <clib/alib_protos.h>
#include <proto/alib.h>
#include <proto/asl.h>
#include <proto/dos.h>

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/muimaster.h>

#include <exec/types.h>
#include <libraries/locale.h>

#include <proto/locale.h>
#include <libraries/mui.h>
#include <libraries/asl.h>

#include <graphics/rpattr.h>

#include <cybergraphx/cybergraphics.h>

#include <exec/types.h>
#include <libraries/locale.h>

#include <proto/locale.h>

#define CATCOMP_BLOCK 1     /* enable CATCOMP_BLOCK */

#include "uae4all_strings.h" /* change name to correct locale header if needed */

#include <SDL.h>


#define MKRGB(r,g,b) (((r) << 16) | ((g) << 8) | (b))
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define get(obj,attr,store) GetAttr(attr,obj,(ULONG *)store)
static void goMenu(void);

enum { ID_LOAD0=1, ID_LOAD1, ID_EJECT0, ID_EJECT1, ID_SAVESTATE, ID_SAVESTATE_LOAD, ID_SAVESTATE_SAVE, ID_THROTTLE, ID_AUTO_FRAMESKIP, ID_FRAMESKIP, ID_SOUND, ID_SAVEDISK, ID_RUN, ID_RESET, ID_ABOUT, ID_QUIT, ID_CLOSE_SAVESTATE};

#ifdef PROFILER_UAE4ALL
unsigned long long uae4all_prof_initial[UAE4ALL_PROFILER_MAX];
unsigned long long uae4all_prof_sum[UAE4ALL_PROFILER_MAX];
unsigned long long uae4all_prof_executed[UAE4ALL_PROFILER_MAX];
#endif

#ifdef DREAMCAST
#include <SDL_dreamcast.h>
#define VIDEO_FLAGS_INIT SDL_HWSURFACE|SDL_FULLSCREEN
#else
#ifdef DINGOO
#define VIDEO_FLAGS_INIT SDL_SWSURFACE
#else
#ifdef MORPHOS
#define VIDEO_FLAGS_INIT SDL_HWSURFACE|SDL_FULLSCREEN
#else
#define VIDEO_FLAGS_INIT SDL_HWSURFACE
#endif
#endif
#endif

#ifdef DOUBLEBUFFER
#define VIDEO_FLAGS VIDEO_FLAGS_INIT | SDL_DOUBLEBUF
#else
#define VIDEO_FLAGS VIDEO_FLAGS_INIT
#endif

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library  *MUIMasterBase;

char *status_text = NULL;
APTR app, window, subwindow1, load_df0, eject_df0, load_df1, eject_df1, 
		savestate, savedisk, autofskip, frameskip, throttle, sound, vpos,
		savestate_pos, loadstate_but, savestate_but, reset, run, about, quit,
		aboutbox;

static char _show_message_str[40]={
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
};

int show_message=0;
char *show_message_str=(char *)&_show_message_str[0];

extern SDL_Surface *prSDLScreen;
extern struct uae_prefs changed_prefs;
extern struct uae_prefs currprefs;
extern SDL_Joystick *uae4all_joy0, *uae4all_joy1;

extern int keycode2amiga(SDL_keysym *prKeySym);
extern int uae4all_keystate[];

int emulated_mouse_speed=4;
int emulating=0;
char uae4all_image_file[128];
char uae4all_image_file2[128];

int drawfinished=0;

extern int mainMenu_throttle, mainMenu_frameskip, mainMenu_sound, mainMenu_case, mainMenu_autosave, mainMenu_vpos;

int emulated_left=0;
int emulated_right=0;
int emulated_top=0;
int emulated_bot=0;
int emulated_button1=0;
int emulated_button2=0;
int emulated_mouse=0;
int emulated_mouse_button1=0;
int emulated_mouse_button2=0;
#ifdef MORPHOS
BOOL show_status_bar = FALSE;
BOOL show_key = FALSE;
#endif

static struct Locale      *locale_locale;
static struct Catalog     *locale_catalog;

/*************************************************************************/

/* /// Locale_Open */

/*************************************************************************/

BOOL Locale_Open( STRPTR catname, ULONG version, ULONG revision)
{
	if( (LocaleBase = (struct Library *) OpenLibrary( "locale.library",0 )) ) {
#ifdef __amigaos4__
		if( (ILocale = (struct LocaleIFace *) GetInterface( LocaleBase, "main", 1, NULL)) ) {
#endif
			if( (locale_locale = OpenLocale(NULL)) ) {
				if( (locale_catalog = OpenCatalogA(locale_locale, catname, TAG_DONE)) ) {
					if(    locale_catalog->cat_Version  == version  &&
						   locale_catalog->cat_Revision == revision ) {
	                    return(TRUE);

	                }
					CloseCatalog(locale_catalog);
					locale_catalog = NULL;
	            }
	            CloseLocale(locale_locale);
	            locale_locale = NULL;
			}
#ifdef __amigaos4__
			DropInterface((struct Interface *)ILocale);
        }
#endif
		CloseLibrary(  (struct Library *) LocaleBase );
		LocaleBase = NULL;
    }
	return(FALSE);
}


void Locale_Close(void)
{
    if( LocaleBase) {
        if( locale_catalog) {
            CloseCatalog(locale_catalog);
            locale_catalog = NULL;
        }
        if( locale_locale) {
            CloseLocale(locale_locale);
            locale_locale = NULL;
        }
#ifdef __amigaos4__
		DropInterface((struct Interface *)ILocale);
#endif
		CloseLibrary( (struct Library *) LocaleBase );
		LocaleBase = NULL;
    }
}
/* \\\ */

/* /// GetString */

/*************************************************************************/

STRPTR GetString(long id)
{
	LONG   *l;
	UWORD  *w;
	STRPTR  builtin;

	l = (LONG *)CatCompBlock;

    while (*l != id ) {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtin = (STRPTR)((ULONG)l + 6);

    if ( locale_catalog && LocaleBase ) {
        return( (STRPTR) GetCatalogStr( locale_catalog, id, builtin));
    }
    return(builtin);
}

BOOL Open_Libs(void)
{
  if ( !(IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",39)) )
    return(0);

  if ( !(GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0)) )
  {
    CloseLibrary((struct Library *)IntuitionBase);
    return(0);
  }

  if ( !(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,19)) )
  {
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)IntuitionBase);
    return(0);
  }

  return(1);
}

void Close_Libs(void)
{
  if (IntuitionBase)
    CloseLibrary((struct Library *)IntuitionBase);

  if (GfxBase)
    CloseLibrary((struct Library *)GfxBase);

  if (MUIMasterBase)
    CloseLibrary(MUIMasterBase);
}


void norom_request(void)
{
	struct EasyStruct myES =
    {
    sizeof(struct EasyStruct),
    0,
    (UBYTE*)"UAE4All",
    (UBYTE*)GetString(MSG_NOROM),
    (UBYTE*)"Ok",
    };
    
 	LONG  answer = EasyRequest(NULL, &myES, NULL, NULL);

}


static void getChanges(void)
{
    m68k_speed=mainMenu_throttle;
#ifndef NO_SOUND
    if (mainMenu_sound)
    {
	    changed_produce_sound=2;
	    sound_default_evtime();
    }
    else
#endif
	    changed_produce_sound=0;
    changed_gfx_framerate=mainMenu_frameskip;
    init_hz();
}

int gui_init (void)
{
	
	Open_Libs();
	
	Locale_Open("uae4all.catalog",0,32);
	
	app = ApplicationObject,
		MUIA_Application_Title  , "UAE4All",
		MUIA_Application_Version , "$VER: 0.3.2 (06.08.14)",
		MUIA_Application_Copyright , GetString(MSG_About_Auth),
		MUIA_Application_Author  , "UAE4All by Chui",
		MUIA_Application_Description, GetString(MSG_About_Desc),
		MUIA_Application_Base  , "UAE4All",

		SubWindow, window = WindowObject,
			MUIA_Window_Title, "UAE4All",
			MUIA_Window_ID , MAKE_ID('U','A','E','4'),
			//MUIA_Window_AltWidth, MUIV_Window_Width_Visible(50),
			//MUIA_Window_AltHeight, MUIV_Window_Height_Visible(50),
			 MUIA_Window_Activate, TRUE,
			/*
			 MUIA_Window_Height, 300,
			 MUIA_Window_Width, 400,
			 MUIA_Window_LeftEdge, 75,
			 MUIA_Window_TopEdge, 75,*/
			MUIA_Window_Width, MUIV_Window_Width_MinMax(70),
			MUIA_Window_Height, MUIV_Window_Height_MinMax(70),
			//MUIA_Window_Height, MUIV_Window_Height_MinMax(50),
			/*MUIA_Window_AltWidth, MUIV_Window_Width_Screen(50),
			MUIA_Window_AltHeight, MUIV_Window_Height_Screen(50),
			//MUIA_Window_DepthGadget, FALSE,
			*/
			MUIA_Window_SizeGadget, TRUE,
			
			
			WindowContents, VGroup,
	
				Child, ColGroup(2),
					Child, load_df0  = MUI_MakeObject(MUIO_Button,GetString(MSG_Load_Df0)),
					Child, eject_df0 = MUI_MakeObject(MUIO_Button,GetString(MSG_Eject_Df0)),
					Child, load_df1  = MUI_MakeObject(MUIO_Button,GetString(MSG_Load_Df1)),	
					Child, eject_df1 = MUI_MakeObject(MUIO_Button,GetString(MSG_Eject_Df1)),
				End,
				  
				Child, savestate = MUI_MakeObject(MUIO_Button, GetString(MSG_Save_State)),

				Child, MUI_MakeObject(MUIO_HBar,1),
				
				Child, ColGroup(2),
					Child, CLabel(GetString(MSG_Throttle)),
					Child, throttle  = MUI_MakeObject(MUIO_Slider,NULL,0,6),
				End,	
				
				Child, ColGroup(2),
					Child, CLabel(GetString(MSG_Screen_VPos)),
					Child, vpos  = MUI_MakeObject(MUIO_Slider,NULL,0,5),
				End,	
				
				Child, ColGroup(4),
					Child, CLabel(GetString(MSG_Frameskip)),
					Child, frameskip = MUI_MakeObject(MUIO_Slider,NULL,0,5),
					Child, autofskip = CheckMark(TRUE),//MUI_MakeObject(MUIO_Checkmark, "A",MUIA_Selected,TRUE),
					Child, CLabel(GetString(MSG_Frameskip_Auto)),
				End,
						
				Child, ColGroup(5), 
					Child, sound = CheckMark(TRUE),
					Child, CLabel(GetString(MSG_Sound)),
					Child, HSpace(5),
					Child, savedisk = CheckMark(TRUE),
					Child, CLabel(GetString(MSG_Save_Disk)),
				End,
				
				Child, MUI_MakeObject(MUIO_HBar,1),
				
				Child, reset = MUI_MakeObject(MUIO_Button,GetString(MSG_Reset)),
				Child, run   = MUI_MakeObject(MUIO_Button,GetString(MSG_Run)),
				
				Child, MUI_MakeObject(MUIO_HBar,1),
				
				Child, ColGroup(2),
					Child, about = MUI_MakeObject(MUIO_Button,GetString(MSG_About)),
					Child, quit  = MUI_MakeObject(MUIO_Button,GetString(MSG_Quit)),
				End,
				
			End,
		End,
		  
		 	SubWindow, aboutbox = AboutboxObject, 
				MUIA_Window_ID,                 MAKE_ID('A','B','O','U'), 
			//	MUIA_Aboutbox_Credits,          credits,
			End,

		/****************/

		SubWindow, subwindow1 = WindowObject,
			MUIA_Window_Title, GetString(MSG_Save_State),
			MUIA_Window_ID , MAKE_ID('S','A','V','E'),
			//UIA_Window_AltWidth, MUIV_Window_Width_Visible(25),
			//MUIA_Window_AltHeight, MUIV_Window_Height_Visible(10),
			//MUIA_Window_CloseGadget, FALSE,
			MUIA_Window_DepthGadget, FALSE,
			MUIA_Window_SizeGadget, FALSE, 
			
			WindowContents, VGroup,
			
				Child, ColGroup(2),
					Child, CLabel(GetString(MSG_State_Position)),
					Child, savestate_pos = MUI_MakeObject(MUIO_Slider,NULL,1,4),
				
					Child, loadstate_but = MUI_MakeObject(MUIO_Button,GetString(MSG_State_Load)),
					Child, savestate_but = MUI_MakeObject(MUIO_Button,GetString(MSG_State_Save)),
				End,
			End,
		End,
		
	End;
	
	DoMethod((Object*)load_df0,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_LOAD0);
	DoMethod((Object*)load_df1,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_LOAD1);
	DoMethod((Object*)eject_df0,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_EJECT0);
	DoMethod((Object*)eject_df1,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_EJECT1);
	DoMethod((Object*)savestate,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_SAVESTATE);
	DoMethod((Object*)loadstate_but,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_SAVESTATE_LOAD);
	DoMethod((Object*)savestate_but,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_SAVESTATE_SAVE);
	DoMethod((Object*)quit,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_QUIT);
	DoMethod((Object*)reset,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_RESET);
	DoMethod((Object*)about,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_ABOUT);
	DoMethod((Object*)run,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)app,2,MUIM_Application_ReturnID,ID_RUN);
	
	DoMethod((Object*)autofskip,MUIM_Notify,MUIA_Selected,MUIV_EveryTime,(Object*)frameskip,3,MUIM_Set,MUIA_Disabled,MUIV_TriggerValue);
	
	DoMethod((Object*)throttle, MUIM_Set, MUIA_Slider_Level, 3);
	
	DoMethod((Object*)vpos, MUIM_Set, MUIA_Slider_Level, 1);
	
	DoMethod((Object*)frameskip,MUIM_Set,MUIA_Disabled,TRUE);
	
	DoMethod((Object*)window,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			(Object*)app,2,MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);
	
	DoMethod((Object*)subwindow1,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			(Object*)app,2,MUIM_Application_ReturnID,ID_CLOSE_SAVESTATE);
		
    FILE *f = fopen(romfile, "rb");

    if (f == NULL) {
    norom_request();
    do_leave_program();
    exit(0);
    }
    else
    		fclose(f);	
	//set(window,MUIA_Window_Open,TRUE);
 
#ifndef MORPHOS
//Se ejecuta justo despues del MAIN
    if (prSDLScreen==NULL)
	prSDLScreen=SDL_SetVideoMode(320,240,16,VIDEO_FLAGS);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);
    if (prSDLScreen!=NULL)
    {
#endif
	emulating=0;
#if !defined(DEBUG_UAE4ALL) && !defined(PROFILER_UAE4ALL) && !defined(AUTO_RUN) && !defined(AUTO_FRAMERATE)
	uae4all_image_file[0]=0;
	uae4all_image_file2[0]=0;
#else
	strcpy(uae4all_image_file,"prueba.adz");
	strcpy(uae4all_image_file2,"prueba2.adz");
#endif
	vkbd_init();
	
#ifndef MORPHOS
	init_text(0);
	run_mainMenu();
	quit_text();
#else
	goMenu();
	//strcpy(uae4all_image_file,"df0.adf");
#endif
	uae4all_pause_music();
	emulating=1;
	getChanges();
	check_all_prefs();
	reset_frameskip();
#ifdef DEBUG_FRAMERATE
	uae4all_update_time();
#endif
#ifdef PROFILER_UAE4ALL
	uae4all_prof_init();
	uae4all_prof_add("M68K");			// 0
	uae4all_prof_add("EVENTS");			// 1
	uae4all_prof_add("HSync");			// 2
	uae4all_prof_add("Copper");			// 3
	uae4all_prof_add("Audio");			// 4
	uae4all_prof_add("CIA");			// 5
	uae4all_prof_add("Blitter");			// 6
	uae4all_prof_add("Vsync");			// 7
	uae4all_prof_add("update_fetch");		// 8
	uae4all_prof_add("linetoscr");			// 9
	uae4all_prof_add("do_long_fetch");		// 10
	uae4all_prof_add("pfield_doline");		// 11
	uae4all_prof_add("draw_sprites_ecs");		// 12
	uae4all_prof_add("flush_block");		// 13
	uae4all_prof_add("SET_INTERRUPT");		// 14
/*
	uae4all_prof_add("15");		// 15
	uae4all_prof_add("16");		// 16
	uae4all_prof_add("17");		// 17
	uae4all_prof_add("18");		// 18
	uae4all_prof_add("19");		// 19
	uae4all_prof_add("20");		// 20
	uae4all_prof_add("21");		// 21
	uae4all_prof_add("22");		// 22
*/
#endif
#ifdef DREAMCAST
	SDL_DC_EmulateKeyboard(SDL_FALSE);
#endif
	return 0;
#ifndef MORPHOS
    }
    return -1;
#endif
}

#ifdef DINGOO
static void show_mhz(void)
{
    extern int dingoo_get_clock(void);
    char n[40];
    sprintf((char *)&n[0],"Dingoo at %iMHz",dingoo_get_clock());
    gui_set_message((char *)&n[0], 50);
}

static void show_brightness(void)
{
    extern int dingoo_get_brightness(void);
    char n[40];
    sprintf((char *)&n[0],"Brightness %i%%",dingoo_get_brightness());
    gui_set_message((char *)&n[0], 40);
}

static void show_volumen(void)
{
    extern int dingoo_get_volumen(void);
    char n[40];
    sprintf((char *)&n[0],"Volumen %i%%",dingoo_get_volumen());
    gui_set_message((char *)&n[0], 40);
}

static void inc_dingoo_mhz(void)
{
	extern void dingoo_set_clock(unsigned int);
	extern unsigned int dingoo_get_clock(void);
	dingoo_set_clock(dingoo_get_clock()+5);
	show_mhz();
}

static void dec_dingoo_mhz(void)
{
	extern void dingoo_set_clock(unsigned int);
	extern unsigned int dingoo_get_clock(void);
	dingoo_set_clock(dingoo_get_clock()-5);
	show_mhz();
}

static void inc_dingoo_brightness(void)
{
	extern void dingoo_set_brightness(int);
	extern int dingoo_get_brightness(void);
	dingoo_set_brightness(dingoo_get_brightness()+2);
	show_brightness();
}

static void dec_dingoo_brightness(void)
{
	extern void dingoo_set_brightness(int);
	extern int dingoo_get_brightness(void);
	dingoo_set_brightness(dingoo_get_brightness()-2);
	show_brightness();
}
static void inc_dingoo_volumen(void)
{
	extern void dingoo_set_volumen(int);
	extern int dingoo_get_volumen(void);
	dingoo_set_volumen(dingoo_get_volumen()+1);
	show_volumen();
}

static void dec_dingoo_volumen(void)
{
	extern void dingoo_set_volumen(int);
	extern int dingoo_get_volumen(void);
	dingoo_set_volumen(dingoo_get_volumen()-1);
	show_volumen();
}

#else
#define show_mhz()
#define inc_dingoo_mhz()
#define dec_dingoo_mhz()
#define inc_dingoo_brightness()
#define dec_dingoo_brightness()
#define inc_dingoo_volumen()
#define dec_dingoo_volumen()
#endif

int gui_update (void)
{
    extern char *savestate_filename;
#ifndef NO_SAVE_MENU
    extern int saveMenu_n_savestate;
#endif
// SE EJECUTA DESPUES DE INICIAR EL CORE 68k
    strcpy(changed_df[0],uae4all_image_file);
    strcpy(changed_df[1],uae4all_image_file2);
    strcpy(savestate_filename,uae4all_image_file);
#ifndef NO_SAVE_MENU
    switch(saveMenu_n_savestate)
    {
	    case 1:
    		strcat(savestate_filename,"-1.asf");
	    case 2:
    		strcat(savestate_filename,"-2.asf");
	    case 3:
    		strcat(savestate_filename,"-3.asf");
	    default: 
    	   	strcat(savestate_filename,".asf");
    }
#endif
    real_changed_df[0]=1;
    real_changed_df[1]=1;
    show_mhz();
    return 0;
}


static void goMenu(void)
{
   int exitmode=0;
   int autosave=mainMenu_autosave;
	pause_sound();
   if (quit_program != 0)
	    return;
#ifdef PROFILER_UAE4ALL
   uae4all_prof_show();
#endif
#ifdef DEBUG_FRAMERATE
   uae4all_show_time();
#endif
   emulating=1;
//   vkbd_quit();
#ifndef MORPHOS
   init_text(0);
   pause_sound();
   menu_raise();
   exitmode=run_mainMenu();
#else
    ULONG sigs = 0;
	ULONG id = 0;
			
	DoMethod((Object*)window,MUIM_Set,MUIA_Window_Open,TRUE);
	DoMethod((Object*)window,MUIM_Set,MUIA_Window_Activate,TRUE);
	//struct Screen *screen = GetDefaultPubScreen("Workbench");
	//ScreenToFront(screen);
	DoMethod((Object*)window,MUIM_Window_ScreenToFront);
	//	DoMethod((Object*)autofskip, MUIM_Notify, MUIA_Pressed, FALSE, (Object*)app, 0);
	//DoMethod((Object*)autofskip, MUIM_Notify, MUIA_Pressed, TRUE,(Object*)app,0);
	//DoMethod((Object*)autofskip,MUIM_Set,MUIA_Selected,FALSE);//,(Object*)app,0);
	//DoMethod((Object*)autofskip,MUIM_Set,MUIA_Selected,TRUE);//,(Object*)app,0);
		
    while (TRUE)
    {
		id = DoMethod((Object*)app, MUIM_Application_Input, &sigs);
		
		if (id == ID_QUIT ||
			id == MUIV_Application_ReturnID_Quit)
		{
			do_leave_program();
			exit(0);
			break;
		}
		else if (id == ID_LOAD0)
		{
			FileRequester* freq = (struct FileRequester *)MUI_AllocAslRequest(ASL_FileRequest, NULL);
			MUI_AslRequest(freq, 0L);
			char path[128];
			strcpy(path, freq->fr_Drawer);
			AddPart(path, freq->fr_File, 128);
			strcpy(changed_df[0],path);
			real_changed_df[0]=1;
			
			MUI_FreeAslRequest(freq);
			//strcpy(changed_df[0],uae4all_image_file);
			//strcpy(savestate_filename,uae4all_image_file);
		}
		else if (id == ID_LOAD1)
		{
			FileRequester* freq = (struct FileRequester *)MUI_AllocAslRequest(ASL_FileRequest, NULL);
			MUI_AslRequest(freq, 0L);
			
			char path[128];
			strcpy(path, freq->fr_Drawer);
			AddPart(path, freq->fr_File, 128);
			strcpy(changed_df[1],path);
			real_changed_df[1]=1;
			
			MUI_FreeAslRequest(freq);
			
			//strcpy(changed_df[1],uae4all_image_file2);
		}
		else if (id == ID_EJECT0)
		{
		    changed_df[0][0]=0;
			uae4all_image_file[0]=0;
			disk_eject(0);
		}
		else if (id == ID_EJECT1)
		{
		    changed_df[1][0]=0;
			uae4all_image_file2[0]=0;
			disk_eject(1);
		}
		else if (id == ID_SAVESTATE)
		{
			DoMethod((Object*)subwindow1,MUIM_Set,MUIA_Window_Open,TRUE);
		}
		else if (id == ID_SAVESTATE_LOAD)
		{
			ULONG status = 1;
			get((Object*)savestate_pos,MUIA_Slider_Level,&status);
			
			strcpy(uae4all_image_file,changed_df[0]);
			strcpy(savestate_filename,uae4all_image_file);
			
			switch(status)
			{
				case 1:
					strcat(savestate_filename,"-1.asf");
					break;
				case 2:
					strcat(savestate_filename,"-2.asf");
					break;
				case 3:
					strcat(savestate_filename,"-3.asf");
					break;
				case 4:
					strcat(savestate_filename,"-4.asf");
					break;
				default: 
					strcat(savestate_filename,".asf");
					break;
    	    }

			FILE *f=fopen(savestate_filename,"rb");

			if (f)
			{
				fclose(f);
				savestate_state = STATE_DORESTORE;
				id = ID_RUN;
				DoMethod((Object*)subwindow1,MUIM_Set,MUIA_Window_Open,FALSE);
			}
		}
		else if (id == ID_SAVESTATE_SAVE)
		{
		    extern char *savestate_filename;
			ULONG status = 1;
			strcpy(uae4all_image_file,changed_df[0]);
			get((Object*)savestate_pos,MUIA_Slider_Level,&status);
			
			if (strcmp(changed_df[0],uae4all_image_file))
			{
					strcpy(changed_df[0],uae4all_image_file);
				real_changed_df[0]=1;
			}
			if (strcmp(changed_df[1],uae4all_image_file2))
			{
					strcpy(changed_df[1],uae4all_image_file2);
				real_changed_df[1]=1;
			}
			strcpy(savestate_filename,uae4all_image_file);

			switch(status)
			{
				case 1:
					strcat(savestate_filename,"-1.asf");
					break;
				case 2:
					strcat(savestate_filename,"-2.asf");
					break;
				case 3:
					strcat(savestate_filename,"-3.asf");
					break;
				case 4:
					strcat(savestate_filename,"-4.asf");
					break;
				default: 
					strcat(savestate_filename,".asf");
					break;
    	    }

    	    savestate_state =STATE_DOSAVE;
    	    id = ID_RUN;
			DoMethod((Object*)subwindow1,MUIM_Set,MUIA_Window_Open,FALSE);
		}
		else if (id == ID_CLOSE_SAVESTATE)
		{
			DoMethod((Object*)subwindow1,MUIM_Set,MUIA_Window_Open,FALSE);
		}
		else if (id == ID_ABOUT)
		{
			DoMethod((Object*)aboutbox,MUIM_Set,MUIA_Window_Open,TRUE);
		}
		if (id == ID_RUN || id == ID_RESET)
		{
			ULONG status = 0;
			
			get((Object*)autofskip,MUIA_Selected,&status);
			
			if (status == 1)
			{
				mainMenu_frameskip = -1;
			}
			else
			{
				get((Object*)frameskip,MUIA_Slider_Level,&status);
				mainMenu_frameskip = status;
			}
			
			get((Object*)throttle,MUIA_Slider_Level,&status);
			mainMenu_throttle = status;
			
			
			get((Object*)vpos,MUIA_Slider_Level,&status);
			mainMenu_vpos = status;
			
			get((Object*)sound,MUIA_Selected,&status);
			mainMenu_sound=status;
			
			get((Object*)savedisk,MUIA_Selected,&status);
			mainMenu_autosave=status;
			
			if (id == ID_RESET) 
			{
				if (autosave!=mainMenu_autosave)
				{
					prefs_df[0][0]=0;
					prefs_df[1][0]=0;
				}
				black_screen_now();
				show_mhz();
				uae_reset();
			}
			
			exitmode = 0;
			break;
		}
		
		if (sigs)
		{
			sigs = Wait(sigs | SIGBREAKF_CTRL_C);
			
			if (sigs & SIGBREAKF_CTRL_C)  {
			
				break;
			}
		}

	}
	
	DoMethod((Object*)aboutbox,MUIM_Set,MUIA_Window_Open,FALSE);
	DoMethod((Object*)window,MUIM_Window_ScreenToBack);
	DoMethod((Object*)window,MUIM_Set,MUIA_Window_Open,FALSE);
		
#endif

   notice_screen_contents_lost();
   resume_sound();
   if ((!(strcmp(prefs_df[0],uae4all_image_file))) || ((!(strcmp(prefs_df[1],uae4all_image_file2)))))
#ifndef MORPHOS
	   menu_unraise();
   quit_text();
#endif
//   vkbd_init();
#ifdef DREAMCAST
   SDL_DC_EmulateKeyboard(SDL_FALSE);
#endif
    getChanges();
    vkbd_init_button2();
    if (exitmode==1 || exitmode==2)
    {
    	    extern char *savestate_filename;
#ifndef NO_SAVE_MENU
    	    extern int saveMenu_n_savestate;
#endif
	    if (strcmp(changed_df[0],uae4all_image_file))
	    {
            	strcpy(changed_df[0],uae4all_image_file);
	    	real_changed_df[0]=1;
	    }
	    if (strcmp(changed_df[1],uae4all_image_file2))
	    {
            	strcpy(changed_df[1],uae4all_image_file2);
	    	real_changed_df[1]=1;
	    }
    	    strcpy(savestate_filename,uae4all_image_file);
#ifndef NO_SAVE_MENU
	    switch(saveMenu_n_savestate)
    	    {
	    	case 1:
    			strcat(savestate_filename,"-1.asf");
	    	case 2:
    			strcat(savestate_filename,"-2.asf");
	    	case 3:
    			strcat(savestate_filename,"-3.asf");
	    	default: 
    	   		strcat(savestate_filename,".asf");
    	    }
#endif
    }
    if (exitmode==3)
    {
    	    extern char *savestate_filename;
#ifndef NO_SAVE_MENU
    	    extern int saveMenu_n_savestate;
#endif
	    changed_df[1][0]=0;
	    if (strcmp(changed_df[0],uae4all_image_file))
	    { 
            	strcpy(changed_df[0],uae4all_image_file);
	    	real_changed_df[0]=1;
	    }
    	    strcpy(savestate_filename,uae4all_image_file);
#ifndef NO_SAVE_MENU
    	    switch(saveMenu_n_savestate)
    	    {
	   	 case 1:
    			strcat(savestate_filename,"-1.asf");
	    	case 2:
    			strcat(savestate_filename,"-2.asf");
	    	case 3:
    			strcat(savestate_filename,"-3.asf");
	    	default: 
    	  	 	strcat(savestate_filename,".asf");
    	    }
#endif
	    uae4all_image_file2[0]=0;
	    disk_eject(1);
    }
    if (exitmode==2)
    {
	    if (autosave!=mainMenu_autosave)
	    {
	    	prefs_df[0][0]=0;
	   	prefs_df[1][0]=0;
	    }
	    black_screen_now();
	    show_mhz();
	    uae_reset ();
    }
    check_all_prefs();
    gui_purge_events();
    notice_screen_contents_lost();
#ifdef DEBUG_FRAMERATE
    uae4all_update_time();
#endif
#ifdef PROFILER_UAE4ALL
    uae4all_prof_init();
#endif
	resume_sound();
	
}
static int nowSuperThrottle=0, goingSuperThrottle=0, goingVkbd=0, goingEmouse;

static void goSuperThrottle(void)
{
	if (!nowSuperThrottle)
	{
		nowSuperThrottle=1;
		m68k_speed=1; //6;
		changed_produce_sound=0;
		changed_gfx_framerate=80;
		check_prefs_changed_cpu();
		check_prefs_changed_audio();
		check_prefs_changed_custom();
		gui_set_message("SuperThrottle On",5);
	}
}

static void leftSuperThrottle(void)
{
	if (nowSuperThrottle)
	{
		nowSuperThrottle=0;
		getChanges();
		check_prefs_changed_cpu();
		check_prefs_changed_audio();
		check_prefs_changed_custom();
		gui_set_message("SuperThrottle Off",50);
	}
}

static void inc_throttle(int sgn)
{
	char n[40];
	static Uint32 last=0;
	Uint32 now=SDL_GetTicks();
	if (now-last<555)
		return;
	last=now;
	if (sgn>0)
	{
		if (mainMenu_throttle<5)
			mainMenu_throttle++;
	}
	else if (sgn<0)
		if (mainMenu_throttle>0)
			mainMenu_throttle--;
	nowSuperThrottle=0;
	getChanges();
	check_prefs_changed_cpu();
	check_prefs_changed_audio();
	check_prefs_changed_custom();
	sprintf((char *)&n[0],"Throttle %i",mainMenu_throttle*20);
	gui_set_message((char *)&n[0],50);
}

static int in_goMenu=0;

void gui_handle_events (void)
{
#ifndef DREAMCAST
	Uint8 *keystate = SDL_GetKeyState(NULL);

#ifdef EMULATED_JOYSTICK
	if (keystate[SDLK_ESCAPE])
	{
		if (keystate[SDLK_LCTRL])
		{
			keystate[SDLK_LCTRL]=0;
			inc_dingoo_mhz();
		}
		else if (keystate[SDLK_LALT])
		{
			keystate[SDLK_LALT]=0;
			dec_dingoo_mhz();
		}
		else if (keystate[SDLK_SPACE])
		{
			keystate[SDLK_SPACE]=0;
			inc_throttle(1);
		}
		else if (keystate[SDLK_LSHIFT])
		{
			keystate[SDLK_LSHIFT]=0;
			inc_throttle(-1);
		}
		else if (keystate[SDLK_UP])
		{
			keystate[SDLK_UP]=0;
			inc_dingoo_brightness();
		}
		else if (keystate[SDLK_DOWN])
		{
			keystate[SDLK_DOWN]=0;
			dec_dingoo_brightness();
		}
		else if (keystate[SDLK_RIGHT])
		{
			keystate[SDLK_RIGHT]=0;
			inc_dingoo_volumen();
		}
		else if (keystate[SDLK_LEFT])
		{
			keystate[SDLK_LEFT]=0;
			dec_dingoo_volumen();
		}
	}
	else
	if (emulated_mouse)
	{
		if (keystate[SDLK_LEFT])
		{
			lastmx -= emulated_mouse_speed;
	    		newmousecounters = 1;
		}
		else
		if (keystate[SDLK_RIGHT])
		{
			lastmx += emulated_mouse_speed;
	    		newmousecounters = 1;
		}
		if (keystate[SDLK_UP])
		{
			lastmy -= emulated_mouse_speed;
	    		newmousecounters = 1;
		}
		else
		if (keystate[SDLK_DOWN])
		{
			lastmy += emulated_mouse_speed;
	    		newmousecounters = 1;
		}
	}
	else
	{
		emulated_left=keystate[SDLK_LEFT];
		emulated_right=keystate[SDLK_RIGHT];
		emulated_top=keystate[SDLK_UP];
		emulated_bot=keystate[SDLK_DOWN];
	}
	emulated_button1=keystate[SDLK_LCTRL];
	emulated_button2=keystate[SDLK_LALT];
	emulated_mouse_button1=keystate[SDLK_SPACE];
	emulated_mouse_button2=keystate[SDLK_LSHIFT];
#endif
#ifndef EMULATED_JOYSTICK
	if ( keystate[SDLK_PAGEUP] )
		goSuperThrottle();
	else
		leftSuperThrottle();
#endif
#if !defined(MORPHOS) && !defined(DINGOO) && !defined(DREAMCAST)
	if ( keystate[SDLK_F12] )
		SDL_WM_ToggleFullScreen(prSDLScreen);
	else
#endif
#ifdef MORPHOS
	if ( keystate[SDLK_F12] && show_key == FALSE)
    {
        show_key = TRUE;
	    show_status_bar = !show_status_bar;
        set_statusBar();
    }
    else if (!keystate[SDLK_F12])
        show_key = FALSE;
    
#endif
	if (( keystate[SDLK_F11] )
#ifdef EMULATED_JOYSTICK
			||((keystate[SDLK_RETURN])&&(keystate[SDLK_ESCAPE]))
#endif
	   )
#else
	if (SDL_JoystickGetButton(uae4all_joy0,3) || SDL_JoystickGetButton(uae4all_joy1,3) )
#endif
	{
#ifdef EMULATED_JOYSTICK
		keystate[SDLK_RETURN]=keystate[SDLK_ESCAPE]=keystate[SDLK_TAB]=keystate[SDLK_BACKSPACE]=0;
#endif
		if (!savestate_state && !in_goMenu)
			in_goMenu=SDL_GetTicks();
	} else {
		if (in_goMenu) {
			if (SDL_GetTicks()-in_goMenu>333) {
#ifdef DREAMCAST
				if (SDL_JoystickGetAxis (uae4all_joy0, 3) && !savestate_state)
					savestate_state = STATE_DOSAVE;
				else if (SDL_JoystickGetAxis (uae4all_joy0, 2) && !savestate_state)
					savestate_state = STATE_DORESTORE;
				else
#endif
				goMenu();
			} else {
				goMenu();
			}
			in_goMenu=0;
		}
	}
#ifdef EMULATED_JOYSTICK

#ifdef MORPHOS
	if ( keystate[SDLK_PAGEUP] )
		goSuperThrottle();
	else
		leftSuperThrottle();
#else
	if (keystate[SDLK_RETURN])
	{
		if (goingSuperThrottle<3)
			goingSuperThrottle++;
		else
		{
			goingSuperThrottle=-333;
			if (nowSuperThrottle)
				leftSuperThrottle();
			else
				goSuperThrottle();
		}
	}
	else
		goingSuperThrottle=0;
#endif		
		
		
	if (keystate[SDLK_TAB])
	{
		if (keystate[SDLK_ESCAPE])
		{
			keystate[SDLK_ESCAPE]=keystate[SDLK_TAB]=0;
			savestate_state = STATE_DOSAVE;
		}
		else
		if (!vkbd_mode)
			goingEmouse=1;
	}
	else if (goingEmouse)
	{
		goingEmouse=0;
		if (emulated_mouse)
   			notice_screen_contents_lost();
		emulated_mouse=~emulated_mouse;
	}
	if (keystate[SDLK_BACKSPACE])
	{
		if (keystate[SDLK_ESCAPE])
		{
			extern char *savestate_filename;
			FILE *f=fopen(savestate_filename,"rb");
			keystate[SDLK_ESCAPE]=keystate[SDLK_BACKSPACE]=0;
			if (f)
			{
				fclose(f);
				savestate_state = STATE_DORESTORE;
			}
			else
    				gui_set_message("Failed: Savestate not found", 100);
		}
		else
		if (!emulated_mouse)
			goingVkbd=1;
		else
		{
			char str[40];
			static Uint32 last=0;
			Uint32 now=SDL_GetTicks();
			if (now-last>100)
			{
				if (emulated_mouse_speed==15)
					emulated_mouse_speed=1;
				else
					emulated_mouse_speed++;
				sprintf((char *)&str[0],"  Mouse %i speed",emulated_mouse_speed);
    				gui_set_message((char *)&str[0], 40);
				last=now;
			}
		}
	}
	else if (goingVkbd)
	{
		goingVkbd=0;
		if (vkbd_mode)
		{
			vkbd_mode=0;
   			notice_screen_contents_lost();
		}
		else
			vkbd_mode=1;
	}
#endif
#ifdef DREAMCAST
	if (SDL_JoystickGetAxis (uae4all_joy0, 2))
	{
		if (in_goMenu) {
			if (SDL_GetTicks()-in_goMenu>333) {
				if (!savestate_state) {
					FILE *f=fopen(savestate_filename,"rb");
					if (!f)  {
						char *ad=(char *)calloc(strlen(savestate_filename)+16,1);
						strcpy(ad,"/sd/uae4all/");
						strcat(ad,savestate_filename);
						f=fopen(ad,"rb");
						free(ad);
					}
					if (f) {
						savestate_state = STATE_DORESTORE;
						fclose(f);
					}
					else {
						SDL_Event evt;

						SDL_Delay(555);
    						gui_set_message("Failed: Savestate not found", 100);
						while (SDL_PollEvent(&evt))
							SDL_Delay(20);
					}
				}
				in_goMenu=0;
			}
		} else
		if (vkbd_mode)
		{
			vkbd_mode=0;
			goingVkbd=0;
   			notice_screen_contents_lost();
		}
		else
			goingSuperThrottle=1;
	}
	else
		if (!nowSuperThrottle)
			goingSuperThrottle=0;
		else
			goingVkbd=0;
	if (SDL_JoystickGetAxis (uae4all_joy0, 3))
	{
		if (in_goMenu) {
			if (SDL_GetTicks()-in_goMenu>333) {
				if (!savestate_state)
					savestate_state = STATE_DOSAVE;
				in_goMenu=0;
			}
		} else
		if (goingSuperThrottle)
			goSuperThrottle();
		else
			if (goingVkbd>4)
				vkbd_mode=1;
			else
				goingVkbd++;
	}
	else
		if (nowSuperThrottle)
			leftSuperThrottle();
		else
			goingVkbd=0;
#endif
	if (vkbd_key)
	{
		if (vkbd_keysave==-1234567)
		{
			SDL_keysym ks;
			ks.sym=vkbd_key;
			vkbd_keysave=keycode2amiga(&ks);
			if (vkbd_keysave >= 0)
			{
				if (!uae4all_keystate[vkbd_keysave])
				{
					uae4all_keystate[vkbd_keysave]=1;
					record_key(vkbd_keysave<<1);
				}
			}
		}
	}
	else
		if (vkbd_keysave!=-1234567)
		{
			if (vkbd_keysave >= 0)
			{
				uae4all_keystate[vkbd_keysave]=0;
				record_key((vkbd_keysave << 1) | 1);
			}
			vkbd_keysave=-1234567;
		}
}

void gui_changesettings (void)
{
	dbg("GUI: gui_changesettings");
}

void gui_update_gfx (void)
{
// ANTES DE LA ENTRADA EN VIDEO
//	dbg("GUI: gui_update_gfx");
}

void gui_set_message(char *msg, int t)
{
	show_message=t;
	strncpy(show_message_str, msg, 36);
}

void gui_show_window_bar(int per, int max, int case_title)
{
	char *title;
	if (case_title)
		title="  Restore State";
	else
		title="  Save State";
#ifndef MORPHOS
	_text_draw_window_bar(prSDLScreen,80,64,172,48,per,max,title);
#endif
#if defined(DOUBLEBUFFER) || defined(DINGOO)
	SDL_Flip(prSDLScreen);
#endif
}

/*
int run_menuGame() {}
int run_menuControl() {}
*/


#ifdef PROFILER_UAE4ALL

static unsigned uae4all_prof_total_initial=0;
unsigned uae4all_prof_total=0;
static char *uae4all_prof_msg[UAE4ALL_PROFILER_MAX];

void uae4all_prof_init(void)
{
	unsigned i;
#ifndef DREAMCAST
	unsigned long long s=SDL_GetTicks();
#else
	unsigned long long s=timer_us_gettime64();
#endif
	for(i=0;i<UAE4ALL_PROFILER_MAX;i++)
	{
		uae4all_prof_initial[i]=s;
		uae4all_prof_sum[i]=0;
		uae4all_prof_executed[i]=0;
		if (!uae4all_prof_total)
			uae4all_prof_msg[i]=NULL;
	}
	uae4all_prof_total_initial=s;
}

void uae4all_prof_add(char *msg)
{
	if (uae4all_prof_total<UAE4ALL_PROFILER_MAX)
	{
		uae4all_prof_msg[uae4all_prof_total]=msg;	
		uae4all_prof_total++;
	}
}

void uae4all_prof_show(void)
{
	unsigned i;
	double toper=0;
#ifndef DREAMCAST
	unsigned long long to=SDL_GetTicks()-uae4all_prof_total_initial;
#else
	unsigned long long to=uae4all_prof_sum[0]+uae4all_prof_sum[1];
	for(i=0;i<uae4all_prof_total;i++)
		if (uae4all_prof_sum[i]>to)
			uae4all_prof_sum[i]=0;
#endif

	puts("\n\n\n\n");
	puts("--------------------------------------------");
	for(i=0;i<uae4all_prof_total;i++)
	{
		unsigned long long t0=uae4all_prof_sum[i];
		double percent=(double)t0;
		percent*=100.0;
		percent/=(double)to;
		toper+=percent;
#ifdef DREAMCAST
		t0/=1000;
#endif
		printf("%s: %.2f%% -> Ticks=%i -> %iK veces\n",uae4all_prof_msg[i],percent,((unsigned)t0),(unsigned)(uae4all_prof_executed[i]>>10));
	}
	printf("TOTAL: %.2f%% -> Ticks=%i\n",toper,to);
	puts("--------------------------------------------"); fflush(stdout);
}
#endif
void gui_quit(void)
{

	DoMethod((Object*)window,MUIM_Set,MUIA_Window_Open,FALSE);
	MUI_DisposeObject((Object*)app);
	
	Locale_Close();
	
	Close_Libs();
}