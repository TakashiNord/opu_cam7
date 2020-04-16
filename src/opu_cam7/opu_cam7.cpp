//////////////////////////////////////////////////////////////////////////////
//
//  opu_cam7.cpp
//
//  Description:
//      Contains Unigraphics entry points for the application.
//
//////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE 1

/*  Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

//  Include files
#include <uf.h>
#include <uf_exit.h>
#include <uf_ui.h>
#include <uf_defs.h>
#include <uf_styler.h>
#include <uf_mb.h>

#include <uf_obj.h>
#include <uf_object_types.h>
#include <uf_disp.h>
#include <uf_part.h>

#include <uf_setup.h>
#include <uf_ncgroup.h>
#include <uf_oper.h>
#include <uf_ui_ont.h>

/*
#if ! defined ( __hp9000s800 ) && ! defined ( __sgi ) && ! defined ( __sun )
# include <strstream>
  using std::ostrstream;
  using std::endl;
  using std::ends;
#else
# include <strstream.h>
#endif
#include <iostream.h>
*/
#include "opu_cam7.h"

#include "opu_cam7_dlg.h"


/* The following definition defines the number of callback entries */
/* in the callback structure:                                      */
/* UF_STYLER_callback_info_t GR_cbs */
#define GR_CB_COUNT ( 6 + 1 ) /* Add 1 for the terminator */

/*--------------------------------------------------------------------------
The following structure defines the callback entries used by the
styler file.  This structure MUST be passed into the user function,
UF_STYLER_create_dialog along with GR_CB_COUNT.
--------------------------------------------------------------------------*/
static UF_STYLER_callback_info_t GR_cbs[GR_CB_COUNT] =
{
 {UF_STYLER_DIALOG_INDEX, UF_STYLER_CONSTRUCTOR_CB  , 0, GR_construct_cb},
 {UF_STYLER_DIALOG_INDEX, UF_STYLER_APPLY_CB        , 0, GR_apply_cb},
 {UF_STYLER_DIALOG_INDEX, UF_STYLER_CANCEL_CB       , 0, GR_cancel_cb},
 {GR_RADIO_NAME         , UF_STYLER_VALUE_CHANGED_CB, 0, GR_name_cb},
 {GR_OPTION_METHODS     , UF_STYLER_ACTIVATE_CB     , 0, GR_group_cb},
 {GR_ACTION_LIST        , UF_STYLER_ACTIVATE_CB     , 0, GR_action_list_act_cb},
 {UF_STYLER_NULL_OBJECT, UF_STYLER_NO_CB, 0, 0 }
};



/*--------------------------------------------------------------------------
UF_MB_styler_actions_t contains 4 fields.  These are defined as follows:

Field 1 : the name of your dialog that you wish to display.
Field 2 : any client data you wish to pass to your callbacks.
Field 3 : your callback structure.
Field 4 : flag to inform menubar of your dialog location.  This flag MUST
          match the resource set in your dialog!  Do NOT ASSUME that changing
          this field will update the location of your dialog.  Please use the
          UIStyler to indicate the position of your dialog.
--------------------------------------------------------------------------*/
static UF_MB_styler_actions_t actions[] = {
    { "opu_cam7_dlg.dlg",  NULL,   GR_cbs,  UF_MB_STYLER_IS_NOT_TOP },
    { NULL,  NULL,  NULL,  0 } /* This is a NULL terminated list */
};


/*****************************************************************************
**  Activation Methods
*****************************************************************************/

#define COUNT_GRP 500

struct GRP
{
   int     num;
   tag_t   tag;
   char    name[UF_OPER_MAX_NAME_LEN+1];
} ;

static struct GRP grp_list[COUNT_GRP] ;
int grp_list_count=0;

#define COUNT_PRG 800

struct PRG
{
   int     num;
   tag_t   tag;
   char    name[UF_OPER_MAX_NAME_LEN+1];
} ;

static struct PRG prg_list[COUNT_PRG] ;
int prg_list_count=0;

/*    */
/*----------------------------------------------------------------------------*/
int _run ( tag_t grp , tag_t prg , int tp)
{
   int  ecode ;
   char  prog_name[UF_OPER_MAX_NAME_LEN+1];
   char  grp_name[UF_OPER_MAX_NAME_LEN+1];
//   tag_t setup_tag ;
//    tag_t group ;
   char str[256+UF_NCGROUP_MAX_REASON_LEN+1];
   logical   answer ;
   char      reason[UF_NCGROUP_MAX_REASON_LEN+1] ;
   int    ret = 0 ;
   int type,subtype;

  ret = 0 ;

  /* type =               subtype =
  //     программа=121              160
  //     операция =100              110 */
  ecode = UF_OBJ_ask_type_and_subtype (prg, &type, &subtype );
  printf(" type=%d subtype=%d ",type,subtype);
  if (type!=UF_machining_operation_type) { return (ret); }

  // ecode = UF_SETUP_ask_setup(&setup_tag);
  // if( ecode != 0 ) { return (ret); }

  ecode = UF_NCGROUP_can_accept_member( grp, prg, &answer, &reason[0] );
  if ( ecode == 0 && answer == TRUE ) {
      ecode = UF_NCGROUP_accept_member( grp, prg );
      ret = 1;
  }
  else
  {
     // if (tp==0) {
     //   ecode = UF_SETUP_ask_geom_null(setup_tag,&group);
     // } else {
     //   ecode = UF_SETUP_ask_mct_null(setup_tag,&group);
     // }
     // ecode = UF_NCGROUP_accept_member( group, prg );

     grp_name[0]='\0';
     UF_OBJ_ask_name(grp, grp_name);// спросим имя обьекта

     prog_name[0]='\0';
     //UF_OBJ_ask_name(prg, prog_name);// спросим имя обьекта
     UF_OPER_ask_name_from_tag(prg, prog_name);

     UF_UI_open_listing_window();
     str[0]='\0';
     sprintf(str,"\n! %s - не могу принять в группу %s \n\t -> т.к. =",prog_name, grp_name );
     UF_UI_write_listing_window(str);
     UF_UI_write_listing_window(reason);
  }

  return (ret);
}

/* */
/*----------------------------------------------------------------------------*/
static logical cycleGeneratePrg( tag_t   tag, void   *data )
{
   char     name[UF_OPER_MAX_NAME_LEN + 1];
   int      ecode;

   name[0]='\0';
   ecode = UF_OBJ_ask_name(tag, name);// спросим имя обьекта
   //UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(name);

   if (prg_list_count>=COUNT_PRG) {
     uc1601("Число Операций-превышает допустимое (>800)\n Уменьшите количество выбора",1);
   	 return( FALSE );
   }
   prg_list[prg_list_count].num=prg_list_count;
   prg_list[prg_list_count].tag=tag;
   prg_list[prg_list_count].name[0]='\0';
   sprintf(prg_list[prg_list_count].name,"%s",name);
   prg_list_count++;

   return( TRUE );
}


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

   if (grp_list_count>=COUNT_GRP) {
   	 uc1601("Число элементов - превышает допустимое (>500)",1);
   	 return( FALSE );
   }
   grp_list[grp_list_count].num=grp_list_count;
   grp_list[grp_list_count].tag=tag;
   grp_list[grp_list_count].name[0]='\0';
   sprintf(grp_list[grp_list_count].name,"%s",name);
   grp_list_count++;

   return( TRUE );
}

/* Methods={0.1.2.3}={prog.tool.geom.meth} */
int _ask_cam_group( int Methods )
{
    tag_t setup_tag, group ;
    int ecode ;
    tag_t tag ;

    ecode = UF_SETUP_ask_setup(&setup_tag);
    if( ecode != 0 ) { ; }

    grp_list_count=0; // обнуляем число элементов в группе
    if (grp_list_count>=COUNT_GRP) return( 1 );

    switch (Methods) {
     case 0:
        ecode = UF_SETUP_ask_program_root(setup_tag,&group);
     break;
     case 1:
        ecode = UF_SETUP_ask_mct_root(setup_tag,&group);
     break;
     case 2:
        ecode = UF_SETUP_ask_geom_root(setup_tag,&group);
        /*Заглушка*/
        ecode = UF_NCGROUP_ask_object_of_name(group,"GEOMETRY",&tag);
        if (tag!=null_tag) {
         grp_list[grp_list_count].num=grp_list_count;
         grp_list[grp_list_count].tag=tag;
         grp_list[grp_list_count].name[0]='\0';
         sprintf(grp_list[grp_list_count].name,"%s","GEOMETRY");
         grp_list_count++;
        }
     break;
     case 3:
        ecode = UF_SETUP_ask_mthd_root(setup_tag,&group);
     break ;
    }

    if( ecode != 0 ) { ; }
    ecode = UF_NCGROUP_cycle_members( group, cycleGenerateCb,NULL );
    if( ecode != 0 ) { ; }

    return (0);
}


/*    */
int _ask_cam_group_print()
{
   char str[133];
   int i ;

   if (grp_list_count<=0) { return (0); }

   UF_UI_open_listing_window();
   str[0]='\0'; sprintf(str,"Всего элементов в группе = %-3d ",grp_list_count);
   UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(str);
   for (i=0;i<grp_list_count;i++) {
    str[0]='\0'; sprintf(str,"%-3d)  %s",i,grp_list[i].name);
    UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(str);
   }

   return (0);
}


/*****************************************************************************/
int _main_loadDll( void )
{
    int  response   = 0;
    int errorCode;
    char *dialog_name=actions->styler_file ; // "opu_cam7_dlg.dlg\0";
    char env_names[][25]={
    "UGII_USER_DIR" ,
    "UGII_SITE_DIR" ,
    "UGII_VENDOR_DIR" ,
    "USER_UFUN" ,
    "UGII_INITIAL_UFUN_DIR" ,
    "UGII_TMP_DIR" ,
    "HOME" ,
    "TMP" } ;
 int i ;
 char *path , envpath[133] , dlgpath[255] , info[133];
 int status ;

 path = (char *) malloc(133+10);

 errorCode=-1;
  for (i=0;i<7;i++) {

    envpath[0]='\0';
    path=envpath;
    UF_translate_variable(env_names[i], &path);
    if (path!=NULL) {

       /*1*/
       dlgpath[0]='\0';
       strcpy(dlgpath,path); strcat(dlgpath,"\\application\\"); strcat(dlgpath,dialog_name);
       UF_print_syslog(dlgpath,FALSE);

       // работа с файлом
       UF_CFI_ask_file_exist (dlgpath, &status );
       if (!status) { errorCode=0; break ; }

       // работа с файлом
       UF_CFI_ask_file_exist (dlgpath, &status );
       if (!status) { errorCode=0; break ; }

       /*2*/
       dlgpath[0]='\0';
       strcpy(dlgpath,path); strcat(dlgpath,"\\"); strcat(dlgpath,dialog_name);
       UF_print_syslog(dlgpath,FALSE);

     } else { //if (envpath!=NULL)
      info[0]='\0'; sprintf (info,"Переменная %s - не установлена \n ",env_names[i]);
      UF_print_syslog(info,FALSE);
     }
  } // for

 if (errorCode!=0) {
    info[0]='\0'; sprintf (info,"Don't load %s  \n ",dialog_name);
    uc1601 (info, TRUE );
  } else {
       if ( ( errorCode = UF_STYLER_create_dialog ( dlgpath,
           GR_cbs,      /* Callbacks from dialog */
           GR_CB_COUNT, /* number of callbacks*/
           NULL,        /* This is your client data */
           &response ) ) != 0 )
        {
              /* Get the user function fail message based on the fail code.*/
              PrintErrorMessage( errorCode );
         }
  }

 return(errorCode);
}


/*****************************************************************************/
int cam7( void )
{
   int ecode;

   int module_id;
   UF_ask_application_module(&module_id);
   if (UF_APP_CAM!=module_id) {
       uc1601("Запуск DLL - производится из модуля обработки\n - 2005г.",1);
       return (-1);
    }

   if (NULL_TAG==UF_PART_ask_display_part()) {
      uc1601("Cam-часть не активна.....\n программа прервана.",1);
      return (0);
   }

   ecode=_main_loadDll( );

   return(0);
}


int _name_cb ( int dialog_id )
{
  UF_STYLER_item_value_type_t data  ;
  int irc ;
  int i ;
  int type ;
  char *ls[COUNT_GRP];

  data.item_attr=UF_STYLER_VALUE ;

  data.item_id=GR_RADIO_NAME ;
  irc=UF_STYLER_ask_value(dialog_id,&data);     PrintErrorMessage( irc );
  type=data.value.integer;

  _ask_cam_group( type ) ;

  for (i=0;i<grp_list_count;i++)
  {
    ls[i]= (char *) malloc ((strlen(grp_list[i].name)+1)*sizeof(char)) ;
    ls[i][0]='\0';
    sprintf(ls[i],"%s",grp_list[i].name);
  }

  data.item_id=GR_OPTION_METHODS ;
  data.item_attr=UF_STYLER_SUBITEM_VALUES;
  data.value.strings=(char **)NULL;
  data.count=0;
  irc=UF_STYLER_set_value(dialog_id,&data);     PrintErrorMessage( irc );

  data.item_id=GR_OPTION_METHODS ;
  data.item_attr=UF_STYLER_SUBITEM_VALUES;
  data.value.strings=ls;
  data.count=grp_list_count;
  irc=UF_STYLER_set_value(dialog_id,&data);     PrintErrorMessage( irc );

  data.item_attr=UF_STYLER_VALUE ;
  data.subitem_index=0;
  irc=UF_STYLER_set_value(dialog_id,&data);     PrintErrorMessage( irc );

  UF_STYLER_free_value (&data) ;

  free(ls);
  return (0);
}


int _apply_cb ( int dialog_id )
{
    char str[133];
    int ecode ;
    int type , index;
    tag_t   grp = NULL_TAG , prg = NULL_TAG;
    int i , j , count = 0 ;
    int obj_count = 0;
    tag_t *tags = NULL ;

/********************************/
    UF_STYLER_item_value_type_t data  ;
    int irc ;
    data.item_attr=UF_STYLER_VALUE;
    data.item_id=GR_OPTION_METHODS ;
    irc=UF_STYLER_ask_value(dialog_id,&data);
    index=data.value.integer;
    data.item_id=GR_RADIO_NAME ;
    irc=UF_STYLER_ask_value(dialog_id,&data);
    type=data.value.integer;
    UF_STYLER_free_value (&data) ;
/********************************/

  // выбранные обьекты и их кол-во
  ecode = UF_UI_ONT_ask_selected_nodes(&obj_count, &tags) ;
  if (obj_count<=0) {
    uc1601("Не выбрано операций или программ!\n ....",1);
    return(0);
  }

  UF_UI_toggle_stoplight(1);

  grp = grp_list[index].tag ; // таг-группы куда вносим обьекты

  for(i=0,count=0;i<obj_count;i++)
  {
    prg = tags[i]; // идентификатор объекта

    prg_list_count=0;// заполняем структуру
    ecode = UF_NCGROUP_cycle_members( prg, cycleGeneratePrg,NULL ) ;

    if (prg_list_count==0) {
  	  count+=_run( grp, prg , type);
    } else for (j=0;j<prg_list_count;j++) {
             count+=_run( grp, prg_list[j].tag , type);
           }

  }

  UF_free(tags);

  UF_UI_ONT_refresh();

  UF_DISP_refresh ();

  UF_UI_toggle_stoplight(0);

  str[0]='\0'; sprintf(str,"Изменено операций=%d \n ....",count);
  uc1601(str,1);

  return (0);
}



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
        cam7();

        /* Terminate the API environment */
        errorCode = UF_terminate();
    }

    /* Print out any error messages */
    PrintErrorMessage( errorCode );
    *returnCode=0;
}

//----------------------------------------------------------------------------
//  Utilities
//----------------------------------------------------------------------------

// Unload Handler
//     This function specifies when to unload your application from Unigraphics.
//     If your application registers a callback (from a MenuScript item or a
//     User Defined Object for example), this function MUST return
//     "UF_UNLOAD_UG_TERMINATE".
extern "C" int ufusr_ask_unload (void)
{
     /* unload immediately after application exits*/
     return ( UF_UNLOAD_IMMEDIATELY );

     /*via the unload selection dialog... */
     /*return ( UF_UNLOAD_SEL_DIALOG );   */
     /*when UG terminates...              */
     /*return ( UF_UNLOAD_UG_TERMINATE ); */
}

/*--------------------------------------------------------------------------
You have the option of coding the cleanup routine to perform any housekeeping
chores that may need to be performed.  If you code the cleanup routine, it is
automatically called by Unigraphics.
--------------------------------------------------------------------------*/
extern void ufusr_cleanup (void)
{
    return;
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




/*-------------------------------------------------------------------------*/
/*---------------------- UIStyler Callback Functions ----------------------*/
/*-------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
 * Callback Name: GR_construct_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_construct_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */
     _name_cb ( dialog_id );

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG);
    /* A return value of UF_UI_CB_EXIT_DIALOG will not be accepted    */
    /* for this callback type.  You must continue dialog construction.*/

}


/* -------------------------------------------------------------------------
 * Callback Name: GR_apply_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_apply_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */
     _apply_cb ( dialog_id ) ;

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog                 */
    /* A return value of UF_UI_CB_EXIT_DIALOG will not be accepted    */
    /* for this callback type.  You must respond to your apply button.*/
    return (UF_UI_CB_CONTINUE_DIALOG);

}


/* -------------------------------------------------------------------------
 * Callback Name: GR_cancel_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_cancel_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, terminate dialog             */
    /* It is STRONGLY recommended that you exit your       */
    /* callback with UF_UI_CB_EXIT_DIALOG in a cancel call */
    /* back rather than UF_UI_CB_CONTINUE_DIALOG.          */
    return ( UF_UI_CB_EXIT_DIALOG );

}


/* -------------------------------------------------------------------------
 * Callback Name: GR_name_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_name_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */
     _name_cb ( dialog_id );

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG);

    /* or Callback acknowledged, terminate dialog.  */
    /* return ( UF_UI_CB_EXIT_DIALOG );             */

}


/* -------------------------------------------------------------------------
 * Callback Name: GR_group_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_group_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG);

    /* or Callback acknowledged, terminate dialog.    */
    /* return ( UF_UI_CB_EXIT_DIALOG );               */

}


/* -------------------------------------------------------------------------
 * Callback Name: GR_action_list_act_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object.
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s)
 *                               UF_STYLER_set_value
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int GR_action_list_act_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */
     if ( UF_initialize() != 0)
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */
     _ask_cam_group_print();

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG);

    /* or Callback acknowledged, terminate dialog.    */
    /* return ( UF_UI_CB_EXIT_DIALOG );               */

}



