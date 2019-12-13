static char *menu_msg="               Amiga emulator "
#ifdef DREAMCAST
 "for Dreamcast "
#else 
#ifdef DINGOO
 "for Dingoo "
#endif
#endif
#ifdef MORPHOS
 "by Chui.         MorphOS porting v0.31 by Pedro Gil (BalrogSoft)         Third release Candidate.          GPL License.                            ";
#else
 "by Chui.                    Third release Candidate.                               GPL License.                                          ";
#endif
#define MAX_SCROLL_MSG (-1500)
