#include "../../paho.mqtt.c-master/src/Thread.h"

#define MICRO_SECOND (1)					/*1us*/
#define MILLI_SECOND (MICRO_SECOND * 1000)	/*1ms*/

extern int gettimeofday(struct timeval *tv, void *tz);

//#define _MUTEX_
#define _COND_
//--------------------------------------------------------------------
#ifdef _MUTEX_
static mutex_type g_pMutex = NULL;
#endif
#ifdef _COND_
static cond_type g_pstCond = NULL;
#endif

typedef struct _node
{
    struct _node*  _next;
    int data;
}_node,*_node_p,**_node_pp;

static _node_p alloc(int data)
{
    _node_p _node=(_node_p)malloc(sizeof(struct _node));
    if(_node==NULL)
    {
        exit (0);
    }
    _node->data=data;
    _node->_next=NULL;
    return _node;
}

static void delete_node(_node_p temp)
{
    if(!temp)
    free(temp);
}

static int is_empty(_node_p _h)
{
    if(NULL==_h->_next)
    {
        return 1;
    }
    return 0;
}

#if 0
static void show_List(_node_p _h)
{
    //assert(_h);
    _node_p temp=_h->_next;
    while(NULL!=temp)
    {
        OSI_LOGI(0x10007614, "line:%d %d ",__LINE__, temp->data);
        temp=temp->_next;
    }
    OSI_LOGI(0x08000161, "\n");
}
#endif

static void pop_List(_node_p _h,int* _o)
{
    if(!is_empty(_h))
    {
        _node_p temp=_h->_next;
        _h->_next=temp->_next;
        *_o=temp->data;
        delete_node(temp);
    }
}

static void init_List(_node_pp _h)
{
    *_h=alloc(0);
}

#if 0
static void destory_List(_node_p _h)
{
    int data;
    while(is_empty(_h))
    {
        pop_List(_h,&data);
    }
    delete_node(_h);
}
#endif

static void push_List(_node_p _h,int data)
{
    _node_p Newnode=alloc(data);
    Newnode->_next=_h->_next;   
    _h->_next=Newnode;
}

static void* consumer(void* _h)
{
    _node_p head=(_node_p) _h;
    for(;;)
    {
    #ifdef _MUTEX_
        Thread_lock_mutex(g_pMutex);
	#endif
        int data=rand()%1000;
        pop_List(head,&data);
    #ifdef _MUTEX_
        Thread_unlock_mutex(g_pMutex);
	#endif
	#ifdef _COND_
		Thread_signal_cond(g_pstCond);
	#endif
        OSI_LOGI(0x10007617, "zy_test line:%d consumer:%d\n",__LINE__, data);
		usleep(1000 * MILLI_SECOND);
    }

	return NULL;
}

static void* producter(void* _h)
{
    _node_p head=(_node_p) _h; 
#ifdef _COND_
	unsigned int iDifferenceValue = 10; 	//10s
#endif
    for(;;)
    {
    #ifdef _MUTEX_
        Thread_lock_mutex(g_pMutex);
	#endif
	#ifdef _COND_
		while(!is_empty(head))
        {
        	Thread_wait_cond(g_pstCond, iDifferenceValue);
        }
	#endif
        int i=rand()%1000;
        push_List(head,i);
    #ifdef _MUTEX_
        Thread_unlock_mutex(g_pMutex);
	#endif
        OSI_LOGI(0x10007618, "zy_test line:%d producter:%d\n",__LINE__, i);
		usleep(100 * MILLI_SECOND);
    }

	return NULL;
}

int Unit_Test_Thread(void)
{
	char OutStrings[128] = {0};
    _node_p head = NULL;
    thread_id_type tid1;	
    thread_id_type tid2;

    init_List(&head);

#ifdef _MUTEX_
	g_pMutex = Thread_create_mutex();
#endif
#ifdef _COND_
	g_pstCond = Thread_create_cond();
#endif

    tid1 = Thread_start(producter, head);
    tid2 = Thread_start(consumer, head);
	
	OutStrings[0] = '\0';
	sprintf(OutStrings, "zy_test [%s(L:%d)]:consumer tid is 0x%lx, producter tid is 0x%lx.", __func__, __LINE__, (long)tid1, (long)tid2);
	OSI_LOGI(0, OutStrings);
	
	return 0;
}
