/** 
* @Time    :   2020/12/24 11:16:21
* @FileName:   screen_manager.c
* @Author  :   Wang Hua(whfyzg@gmail.com)
* @Software:   VSCode
* @Descripton: screen管理模块，功能包括创建screen，screen历史管理，关闭screen  
*/

#include "ic_widgets_inc.h"

#include "screen_manager.h"

#if defined(PLATFORM_MT25)
#include "FreeRTOS.h"
#define  MALLOC_API         pvPortMalloc
#define  FREE_API           vPortFree
#else
#define  MALLOC_API         malloc
#define  FREE_API           free
#endif

static screen_list_t screen_stack;


static screen_list_t* ic_screen_get_current(void)
{
    int i;
    screen_list_t *node = screen_stack.next;
    screen_list_t *prev = &screen_stack;

    for(i = 0; i < MAX_SCREEN_HISTORY; i++){
        if(!node){
           if(prev == &screen_stack){
                return NULL;
           }
          
           break;
        }

        prev = node;
        node = node->next;
    }

    return prev;
}

bool ic_screen_create(screen_index_enum screen_index,
        screen_callback_t *cb)
{
    int i = 0;
    screen_list_t *node = screen_stack.next;
    screen_list_t *prev = &screen_stack;
    screen_list_t* current = ic_screen_get_current();

    if(!cb 
            || screen_index < SCREEN_INDEX_IDLE 
            || screen_index >=SCREEN_INDEX_NUM){
        return false;
    }

    //退出当前屏幕
    if (current)current->screen.cb.exit(&current->screen);

    for(i = 0; i < MAX_SCREEN_HISTORY; i++){
        if(!node){
            node = MALLOC_API(sizeof(screen_list_t));
            node->next = NULL;
            node->prev = prev;
            prev->next = node;

            node->screen.cb.init =      cb->init;
            node->screen.cb.deinit =    cb->deinit;
            node->screen.cb.entry =     cb->entry;
            node->screen.cb.exit =      cb->exit;
            node->screen.index = screen_index;

            if(cb->init)cb->init(&node->screen);

            return true;
        }

        //如果当前要创建的屏幕已经存在，则返回
        if (node->screen.index == screen_index) {
            return true;
        }

        prev = node;
        node = node->next;

        
    }

    return false;
}

bool ic_screen_entry(screen_index_enum screen_index)
{
    int i = 0;

    screen_list_t *head = screen_stack.next;
    screen_list_t *node = head;
    screen_list_t* current = ic_screen_get_current();

    if(screen_index < SCREEN_INDEX_IDLE 
            || screen_index >=SCREEN_INDEX_NUM){
        return false;
    }

    //退出当前屏幕
    if(current)current->screen.cb.exit(&current->screen);

    //如目标屏幕是idle或者主菜单，清除链表后面的节点
    if(screen_index == SCREEN_INDEX_IDLE){
        //进屏
        head->screen.cb.entry(&head->screen);

        //设置为尾节点
        node = head->next;
        head->next =NULL;

        //清除后续节点
        while(node){
            node->screen.cb.deinit(&node->screen);
            screen_list_t* temp = node->next;
            FREE_API(node);
            node = temp;
        }

       return true;
    }else if(screen_index == SCREEN_INDEX_MAINMENU){
        //进屏
        head->next->screen.cb.entry(&head->next->screen);

        //设置为尾节点
        node = head->next->next; 
        head->next->next = NULL; 

        //清除后续节点
        while(node){
            node->screen.cb.deinit(&node->screen);
            screen_list_t* temp = node->next;
            
            FREE_API(node);

            node = temp;
        }

       return true;
    }

    while(node){
        if(node->screen.index == screen_index){
            //进屏
            node->screen.cb.entry(&node->screen);

            if (node->next) {
                //取出当前节点
                node->prev->next = node->next;
                node->next->prev = node->prev;

                //放到尾部
                current = ic_screen_get_current();
                current->next = node;
                node->prev = current;
                node->next = NULL;
            }
            
            return true;
        }

        node = node->next;
    }

    return false;

}

bool ic_screen_goback(void)
{
    int i;
    screen_list_t *node = screen_stack.next;
    screen_list_t *prev = NULL;
    screen_list_t* current = ic_screen_get_current();

    if(current){
        if(current->screen.index == SCREEN_INDEX_IDLE){ //当前屏幕已经是root screen（idle），无法返回上级
            return false;
        } 

        //执行当前屏幕exit
        current->screen.cb.exit(&current->screen);

        //deinit
        current->screen.cb.deinit(&current->screen);


        //进入上级屏幕
        current->prev->screen.cb.entry(&current->prev->screen);

        //从链表中去除当前节点
        current->prev->next = NULL;

        //detete
        FREE_API(current);

        return true;
    }

    return false;
}

bool ic_screen_close_active(void)
{
    return ic_screen_goback();
}

bool ic_screen_goto_idle(void)
{
    screen_list_t *head = screen_stack.next;
    screen_list_t *node = head;
    screen_list_t* current = ic_screen_get_current();

    //退出当前屏幕
    if(current)current->screen.cb.exit(&current->screen);

    //进屏
    //head->screen.cb.entry(&head->screen);

    //设置为尾节点
    node = head;

    //清除后续节点
    while(node){
        node->screen.cb.deinit(&node->screen);
        screen_list_t *temp = node->next;
        FREE_API(node);

        node = temp;
    }

    screen_stack.next = NULL;

    main_screen_1();

    return true;
}
