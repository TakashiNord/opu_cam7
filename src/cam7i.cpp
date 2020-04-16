//////////////////////////////////////////////////////////////////////////////
//
//  cam4.cpp
//
//  Description:
//      Contains Unigraphics entry points for the application.
//
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
//  Include files
#include <uf.h>
#include <uf_obj.h>
#include <uf_object_types.h>
#include <uf_ui.h>
#include <uf_disp.h>
#include <uf_assem.h>
#include <uf_part.h>
#include <uf_attr.h>
#include <uf_vec.h>
#include <uf_csys.h>
#include <uf_modl.h>
#include <uf_defs.h>
#include <uf_mtx.h>
#include <uf_ugmgr.h>
#include <uf_ncgroup.h>
#include <uf_obj.h>
#include <uf_setup.h>
#include <uf_ui_ont.h>

#include <uf_oper.h>
#include <uf_path.h>

#include <uf_cam.h>
#include <uf_ncgroup.h>
#include <uf_oper.h>
#include <uf_param.h>
#include <uf_setup.h>
#include <uf_cutter.h>
#include <uf_cfi.h>


#include <uf_part.h>
#include <uf_param.h>
#include <uf_param_indices.h>


#if ! defined ( __hp9000s800 ) && ! defined ( __sgi ) && ! defined ( __sun )
# include <strstream>
  using std::ostrstream;
  using std::endl;
  using std::ends;
#else
# include <strstream.h>
#endif
#include <iostream.h>

#include "cam7.h"


#define UF_CALL(X) (report( __FILE__, __LINE__, #X, (X)))

static int report( char *file, int line, char *call, int irc)
{
  if (irc)
  {
     char    messg[133];
     printf("%s, line %d:  %s\n", file, line, call);
     (UF_get_fail_message(irc, messg)) ?
       printf("    returned a %d\n", irc) :
       printf("    returned error %d:  %s\n", irc, messg);
  }
  return(irc);
}


#define COUNT_GRP 300

struct GRP
{
   int     num;   
   tag_t   tag;
   char    name[UF_OPER_MAX_NAME_LEN+1];
} ;

static struct GRP grp_list[COUNT_GRP] ;
int grp_list_count=0;

/* */
/*----------------------------------------------------------------------------*/
static logical cycleGenerateCb( tag_t   tag, void   *data )
{
   logical  is_group ;
   char     name[UF_OPER_MAX_NAME_LEN + 1];
   int      ecode;

   ecode = UF_NCGROUP_is_group( tag, &is_group );
   if( is_group != TRUE ) return( TRUE );

   name[0]='\0';
   ecode = UF_OBJ_ask_name(tag, name);// спросим имя обьекта
   //UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(name);
   
   grp_list[grp_list_count].num=grp_list_count;
   grp_list[grp_list_count].tag=tag;
   grp_list[grp_list_count].name[0]='\0'; 
   sprintf(grp_list[grp_list_count].name,"%s",name);
   grp_list_count++;

   return( TRUE );
}


int do_cam7();

//----------------------------------------------------------------------------
//  Activation Methods
//----------------------------------------------------------------------------

//  Explicit Activation
//      This entry point is used to activate the application explicitly, as in
//      "File->Execute UG/Open->User Function..."
extern "C" DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
    /* Initialize the API environment */
    int errorCode = UF_initialize();

    if ( 0 == errorCode )
    {
        /* TODO: Add your application code here */
        do_cam7();

        /* Terminate the API environment */
        errorCode = UF_terminate();
    }

    /* Print out any error messages */
    PrintErrorMessage( errorCode );
}

//----------------------------------------------------------------------------
//  Utilities
//----------------------------------------------------------------------------

// Unload Handler
//     This function specifies when to unload your application from Unigraphics.
//     If your application registers a callback (from a MenuScript item or a
//     User Defined Object for example), this function MUST return
//     "UF_UNLOAD_UG_TERMINATE".
extern "C" int ufusr_ask_unload( void )
{
     /* unload immediately after application exits*/
    return ( UF_UNLOAD_IMMEDIATELY );

     /*via the unload selection dialog... */
     //return ( UF_UNLOAD_SEL_DIALOG );
     /*when UG terminates...              */
     //return ( UF_UNLOAD_UG_TERMINATE );
}

/* PrintErrorMessage
**
**     Prints error messages to standard error and the Unigraphics status
**     line. */
static void PrintErrorMessage( int errorCode )
{
    if ( 0 != errorCode )
    {
        /* Retrieve the associated error message */
        char message[133];
        UF_get_fail_message( errorCode, message );

        /* Print out the message */
        UF_UI_set_status( message );

        fprintf( stderr, "%s\n", message );
    }
}


int do_cam7()
{
    char str[133];

    int cnt_generate;

    tag_t setup_tag, group ;
 
    int i ;
    int ecode ;
    logical generated;
    int generat;
    int response ;
    char **list;

    UF_UI_toggle_stoplight(1);

  	UF_UI_open_listing_window();
/*
    generat=1;
    UF_UI_message_dialog("Cam.....", UF_UI_MESSAGE_QUESTION, mes2, 1, TRUE, &buttons2, &generat);
    if (generat==2) { generat=0; }
*/
    ecode = UF_SETUP_ask_setup(&setup_tag);
    if( ecode != 0 ) { ; }
    ecode = UF_SETUP_ask_geom_root(setup_tag,&group);
    if( ecode != 0 ) { ; }
    ecode = UF_NCGROUP_cycle_members( group, cycleGenerateCb,&list );
    if( ecode != 0 ) { ; }
    
   for (i=0;i<grp_list_count;i++) {
   	str[0]='\0';
   	sprintf(str,"%d) (%d) %s",i,grp_list_count,grp_list[i].name);
   	UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(str);
   }
   
    //UF_free(tags);
    UF_UI_toggle_stoplight(0);

    //UF_DISP_refresh ();



 return (0);
}



//    str[0]='\0'; sprintf(str,"Изменено значений =%d \n Просмотрено операций в цикле =%d \n программа завершена.",count,obj_count);
//    uc1601(str,1);

/*
    for(i=0,count=0;i<obj_count;i++)
    {
       prg = tags[i]; // идентификатор объекта
       prog_name[0]='\0';
       //UF_OBJ_ask_name(prg, prog_name);// спросим имя обьекта
       UF_OPER_ask_name_from_tag(prg, prog_name);
       UF_OPER_ask_geom_group(prg,&group);

       UF_UI_write_listing_window("\n"); UF_UI_write_listing_window(prog_name); UF_UI_write_listing_window("\n");

       cnt_generate=0;

       //if (generat==1 && cnt_generate>0) { UF_CALL( UF_PARAM_generate (prg,&generated ) ); }

    }
*/

/*
    char *mes1[]={
      "Программа предназначена для установки параметров ---- в операции.",
      "Для этого,Вы должны :",
      "  1) выбрать необходимые операции и нажать кнопку 'Далее..'",
      "  2) в появившемся окне установить необходимые опции ",
      "          если значение = -1 - значение в операции не изменяется",
      NULL
    };
    UF_UI_message_buttons_t buttons1 = { TRUE, FALSE, TRUE, "Далее....", NULL, "Отмена", 1, 0, 2 };
    char *mes2[]={
      "Производить генерацию операции после изменения?",
      NULL
    };
    UF_UI_message_buttons_t buttons2 = { TRUE, FALSE, TRUE, "Генерировать..", NULL, "Нет", 1, 0, 2 };

    response=0;
    UF_UI_message_dialog("Cam.....", UF_UI_MESSAGE_INFORMATION, mes1, 5, TRUE, &buttons1, &response);
    if (response!=1) { return (0) ; }

    int module_id;
    UF_ask_application_module(&module_id);
    if (UF_APP_CAM!=module_id) {
       uc1601("Запуск DLL - производится из модуля обработки\n(ОГТ-ОПУ,КНААПО) - 2005г.",1);
       return (-1);
    }

    if (NULL_TAG==UF_PART_ask_display_part()) {
      uc1601("Cam-часть не активна.....\n программа прервана.",1);
      return (0);
    }

    // выбранные обьекты и их кол-во
    UF_CALL( UF_UI_ONT_ask_selected_nodes(&obj_count, &tags) );
    if (obj_count<=0) {
      uc1601("Не выбрано операций!\n Программа прервана..",1);
      return(0);
    }
*/