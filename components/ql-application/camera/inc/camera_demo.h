/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _CAMERADEMO_H
#define _CAMERADEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef int    QlCAMERAStatus;
typedef void * ql_task_t;


/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_camera_demo_thread(void *param);
void ql_camera_app_init(void);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _CAMERADEMO_H */


