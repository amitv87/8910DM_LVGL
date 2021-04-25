#include "Adapter.h"

#define MICRO_SECOND (1)					/*1us*/
#define MILLI_SECOND (MICRO_SECOND * 1000)	/*1ms*/

extern int gettimeofday(struct timeval *tv, void *tz);

#define _COUNT_ (10)
//#define _NO_COND_
#define _COND_
//--------------------------------------------------------------------
static pthread_cond_t cond;
static pthread_mutex_t lock;


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

#ifndef _COND_
static void* consumer(void* _h)
{
    _node_p head=(_node_p) _h;
    for(;;)
    {
        pthread_mutex_lock((pthread_mutex_t *)lock);
        int data=rand()%1000;
        pop_List(head,&data);
        pthread_mutex_unlock((pthread_mutex_t *)lock);
        OSI_LOGI(0x10007615, "line:%d consumer:%d\n",__LINE__, data);
        osiThreadSleep(1000);
    }

	return NULL;
}

static void* producter(void* _h)
{
    _node_p head=(_node_p) _h; 
    for(;;)
    {
        pthread_mutex_lock((pthread_mutex_t *)lock);
        int i=rand()%1000;
        push_List(head,i);
        pthread_mutex_unlock((pthread_mutex_t *)lock);
        OSI_LOGI(0x10007616, "line:%d producter:%d\n",__LINE__, i);
        osiThreadSleep(100);
    }

	return NULL;
}
#endif

#ifdef _COND_
static void* consumer(void* _h)
{
	int num = 0;
    _node_p head=(_node_p) _h;
    for(num = 0; num < _COUNT_; num++)
    {
        pthread_mutex_lock((pthread_mutex_t *)lock);
        int data=rand()%1000;
        pop_List(head,&data);
        pthread_mutex_unlock((pthread_mutex_t *)lock);
		pthread_cond_signal((pthread_cond_t *)cond);
        OSI_LOGI(0x10007617, "zy_test line:%d consumer:%d\n",__LINE__, data);
		usleep(1000 * MILLI_SECOND);
    }

	pthread_cancel(0);			//must!
	return NULL;
}

static void* producter(void* _h)
{
	int num = 0;
    _node_p head=(_node_p) _h; 
	unsigned int iDifferenceValue = 10; 	//10s
	struct timespec stCurrentTime = {0};
	
	clock_gettime(0, &stCurrentTime);
	stCurrentTime.tv_sec += iDifferenceValue;
    for(num = 0; num < _COUNT_; num++)
    {
        pthread_mutex_lock((pthread_mutex_t *)lock);
		while(!is_empty(head))
        {
            pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)lock, &stCurrentTime);
        }
        int i=rand()%1000;
        push_List(head,i);
        pthread_mutex_unlock((pthread_mutex_t *)lock);
        OSI_LOGI(0x10007618, "zy_test line:%d producter:%d\n",__LINE__, i);
		usleep(100 * MILLI_SECOND);
    }

	pthread_cancel(0);			//must!
	return NULL;
}
#endif

int Unit_Test_pthread(int _test_number)
{
	char OutStrings[128] = {0};
    _node_p head = NULL;
    pthread_t tid1;	
    pthread_t tid2;

	switch(_test_number)
	{
	case 0:
	    init_List(&head);

		pthread_mutex_init(&lock, NULL);
		pthread_cond_init(&cond, NULL);

	    pthread_create(&tid1, NULL, producter, head);
	    pthread_create(&tid2, NULL, consumer, head);
		
		OutStrings[0] = '\0';
		sprintf(OutStrings, "zy_test [%s(L:%d)]:consumer tid is 0x%lx, producter tid is 0x%lx.", __func__, __LINE__, (long)tid1, (long)tid2);
		OSI_LOGI(0, OutStrings);

		break;
	case 1:
		pthread_mutex_init(&lock, NULL);
		pthread_mutex_lock((pthread_mutex_t *)lock);
		usleep(1 * 1000 * MILLI_SECOND);
		pthread_mutex_unlock((pthread_mutex_t *)lock);
		usleep(5 * 1000 * MILLI_SECOND);
		pthread_mutex_destroy((pthread_mutex_t *)lock);

		OutStrings[0] = '\0';
		sprintf(OutStrings, "zy_test [%s(L:%d)]", __func__, __LINE__);
		OSI_LOGI(0, OutStrings);

		break;
	default:
		;
	}
	
	return 0;
}

