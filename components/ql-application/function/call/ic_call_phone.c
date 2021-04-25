/******************************************************************************
* @Time    :   2021/4/17 11:16:21
* @FileName:   ic_call_phone.c
* @Author  :   xiongweiming
* @Software:   VSCode
* @Descripton: obj move animation implementation on LVGL  
******************************************************************************/


/******************************************************************************
 * include  head file
 * 
 *
 ******************************************************************************/
#include "ic_call_phone.h"
#include "ql_api_voice_call.h"
#include "image_id_list.h"
#include "ic_mainmenu_calllog.h"

/******************************************************************************
 * Macro definition
 * 
 *
 ******************************************************************************/
#define NUMBER_MAX_LENGTH 16   //电话号码最长16

/******************************************************************************
 * external variable
 * 
 *
 ******************************************************************************/
extern ql_task_t lvgl_gui_task;


/******************************************************************************
 * local variable
 * 
 *
 ******************************************************************************/
static lv_obj_t * num0;
static lv_obj_t * num1;
static lv_obj_t * num2;
static lv_obj_t * num3;
static lv_obj_t * num4;
static lv_obj_t * num5;
static lv_obj_t * num6;
static lv_obj_t * num7;
static lv_obj_t * num8;
static lv_obj_t * num9;
static lv_obj_t * well;
static lv_obj_t * delete;
static lv_obj_t * dial;
static lv_obj_t * plus;
static lv_obj_t * star;
static lv_obj_t * phone_number_dis;

static uint8_t input_number[NUMBER_MAX_LENGTH] = {0};//保存输入的字符
static uint8_t number_len = 0;//输入的数字长度

//通话记录的信息
static ic_call_phone_str number_information[CALL_RECORDS_MAX] = {{{1,1,1,21,4,2021,3},{1,1,1,21,4,2021,3},"111","13632788307",0,1,1},
{{2,2,2,21,4,2021,3},{2,2,2,21,4,2021,3},"222","220",0,1,1},{{3,3,3,21,4,2021,3},{3,3,3,21,4,2021,3},"333","330",0,1,1},
{{4,4,4,21,4,2021,3},{4,4,4,21,4,2021,3},"444","440",0,1,1},{{5,5,5,21,4,2021,3},{5,5,5,21,4,2021,3},"555","550",0,1,1}};

//通话记录链表头指针
static call_list *head=NULL;

/******************************************************************************
 * Local function
 * 
 *
 ******************************************************************************/

/******************************************************************************
 *  Function    -  ic_get_callog_total
 * 
 *  Purpose     -  当前通话记录条数
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
uint8_t ic_get_callog_total(void)
{
	uint8_t count = 0;
	
	for(int i = 0;i < CALL_RECORDS_MAX;i++)
	{
		if(number_information[i].flag == 1)
		{
			count = count + 1;
		}
	}
	
	LOGI("total callog  = %d\n",count);

	return count;
}

/******************************************************************************
 *  Function    -  ic_get_number_information
 * 
 *  Purpose     -  得到通话记录信息
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-23,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_get_number_information(ic_call_phone_str ss[])
{
	uint8_t count = ic_get_callog_total();
	for(int i = 0;i < count;i++)
	{
		memcpy(&ss[i],&number_information[i],sizeof(ic_call_phone_str));		
	}
}

/******************************************************************************
 *  Function    -  ic_list_show
 * 
 *  Purpose     -  打印数据 
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_list_show(call_list *p)
{
	call_list *temp;
	temp=p;
	do
	{
		LOGI("%d-%d-%d-%d-%d-%d  name = %s, number = %s,call_type = %d,call_type =%d,flag = %d\n",temp->data.start.tm_year,
		temp->data.start.tm_mon,temp->data.start.tm_mday,temp->data.start.tm_hour,temp->data.start.tm_min,temp->data.start.tm_sec,
		temp->data.name,temp->data.number,temp->data.call_type,temp->data.call_type2,temp->data.flag);
		temp=temp->next;	
	}
	while(temp!=p);
}

/******************************************************************************
 *  Function    -  ic_node_insert
 * 
 *  Purpose     -  插入节点
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_node_insert(call_list **pNode,int place,ic_call_phone_str num)  //链表的插入  
{
	call_list *temp,*target;//尾节点
	int i;

	//当通话记录条数大于阈值,那么删除最后一条
	if(ic_get_callog_total() >= CALL_RECORDS_MAX)
	{
		ic_node_del(pNode,ic_get_callog_total());
	}
	
	if(place == 1)				//如果输入的数字是1，表示要插入头节点。应该特殊处理
	{						//首先找到尾节点，然后让新节点的next指向头节点，尾节点指向新的头节点，在让头指针指向temp。这要特别注意
		temp=(call_list *)malloc(sizeof(call_list));
		if(!temp)
		{
			LOGI("insert node error\n");
			return;
		}
		//temp->data=num;
		memcpy(&temp->data,&num,sizeof(ic_call_phone_str));
		for(target=*pNode;target->next!=*pNode;target=target->next);
		
		temp->next=*pNode;
		target->next=temp;
		*pNode=temp;//特别注意
	}
	else			//在其他的地方插入节点。  同样先找到要插入的位置，如果位置超出链表的长度，自动插入队尾。						tar  new  原来是2
	{				//找到要插入位置的前一个节点target，让target->next=temp，插入节点的前驱指向新节点，新节点指向target->next的地址  1    2   3  
		for(i=1,target=*pNode;target->next!=*pNode&&i!=place-1;target=target->next,i++);
		temp=(call_list *)malloc(sizeof(call_list));
		//temp->data=num;
		memcpy(&temp->data,&num,sizeof(ic_call_phone_str));
		temp->next=target->next;
		target->next=temp;
	}
 
}

/******************************************************************************
 *  Function    -  ic_node_del
 * 
 *  Purpose     -  删除节点
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_node_del(call_list **pNode,int place)  //删除操作
{
	call_list *temp,*target;
	int i;
	temp=*pNode;
	
	if(temp == NULL)//首先判断链表是否为空
	{
		LOGI("this is NULL\n");
		return;
	}
	
	if(place==1)		//如果删除的是头节点	
	{				//应当特殊处理，找到尾节点，使尾节点的next指向头节点的下一个节点 rear->next=(*head)->next;然后让新节点作为头节点，释放原来的头节点
		for(target=*pNode;target->next!=*pNode;target=target->next);
		temp=*pNode;
		
		*pNode=(*pNode)->next;
		target->next=*pNode;
		free(temp);
	}
	else
	{		//删除其他节点
		for(i=1,target=*pNode;target->next!=*pNode&&i!=place-1;target=target->next,i++); //首先找出尾节点
		if(target->next==*pNode)		//判断要删除的位置是否大于链表长度，若大于链表长度，特殊处理直接删除尾节点
		{
			for(target=*pNode;target->next->next!=*pNode;target=target->next);//找出尾节的前一个节点
			temp=target->next;												 //	尾节点的前一个节点直接指向头节点  释放原来的尾节点									
			target->next=*pNode;
			LOGI("数字太大删除尾巴\n");
			free(temp);
		}
		else
		{
			temp=target->next;//  删除普通节点  找到要删除节点的前一个节点target，使target指向要删除节点的下一个节点  转存删除节点地址
			target->next=temp->next;	//  然后释放这个节点
			free(temp);
		}
	}
	
}

#if 0
int find_list_val(call_list *pNode,int val) //寻找值
{
	int i=1;
	call_list *node;
	node=pNode;
	while(node->data!=val&&node->next!=pNode)
	{
		i++;
		node=node->next;
	}
	if(node->next==pNode&&node->data!=val)//尾节点指向头节点就跳出，因此还要检测一次为节点的data
	{
		return -1;
	}
	return i;
}
#endif

/******************************************************************************
 *  Function    -  get_list_len
 * 
 *  Purpose     -  计算大小
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
int get_list_len(call_list *head)
{
	call_list *ps;
	int count = 1;
	ps = head;
	
	if(ps == NULL)
	{
		return 0;
	}
	else
	{
		while(ps->next != head)
		{
			count++;
			ps = ps->next;
		}
		return count;
	}
}

/******************************************************************************
 *  Function    -  create_link_list
 * 
 *  Purpose     -  创建链表 
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void create_link_list(call_list **p)//如果链表为空，则创建一个链表，指针域指向自己，否则寻找尾节点，将
{						 //将尾节点的指针域指向这个新节点，新节点的指针域指向头结点
	int item;
	call_list *temp;
	call_list *target;
	uint8_t total = ic_get_callog_total();

	for(int i =0;i < total;i++)
	{		
		if(*p == NULL)   //如果输入的链表是空。则创建一个新的节点，使其next指针指向自己  (*head)->next=*head;
		{
			*p=(call_list *)malloc(sizeof(call_list));
			if(!*p)
			{
				return;
			}
			memcpy(&(*p)->data,&number_information[i],sizeof(ic_call_phone_str));
			(*p)->next=*p;
		}
		else		//输入的链表不是空的，寻找链表的尾节点，使尾节点的next=新节点。新节点的next指向头节点
		{
			for(target=*p;target->next!=*p;target=target->next);//寻找尾节点

			temp=(call_list *)malloc(sizeof(call_list));
			if(!temp)
			{
				return;
			}
			memcpy(&temp->data,&number_information[i],sizeof(ic_call_phone_str));
			temp->next=*p;  //新节点指向头节点
			target->next=temp;//尾节点指向新节点
		}
	}

}

/******************************************************************************
 *  Function    -  create_link_list
 * 
 *  Purpose     -  通话记录链表初始化
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-22,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_callog_list_init(void)
{
	ic_call_phone_str num = {{8,8,8,8,8,8,8},{0,0,0,0,0,0,0},"111","13632788307",0,1,1};
	ic_call_phone_str num1 = {{9,9,9,9,9,9,9},{0,0,0,0,0,0,0},"111","13632788307",0,1,1};

	create_link_list(&head);
	ic_list_show(head);
	printf("2222222222222 = %x\n",number_information);

#if 0//测试用的
	ic_node_insert(&head,1,num);
	memcpy(&number_information[4],&num,sizeof(ic_call_phone_str));
	ic_list_show(head);
	printf("2222222222222 = %d\n",get_list_len(head));

	ic_node_insert(&head,1,num1);
	memcpy(&number_information[5],&num1,sizeof(ic_call_phone_str));
	ic_list_show(head);
	printf("2222222222222 = %d\n",get_list_len(head));
#endif	
}

/******************************************************************************
 *  Function    -  ic_vc_call_event
 * 
 *  Purpose     -  语音通话相关事件
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_vc_call_event(ql_event_t event)
{
	LOGI("vc call event = %d",event);

	switch(event.id)
	{
		case QL_VC_INIT_OK_IND:
			break;

		case QL_VC_RING_IND:
			//有新的来电
			LOGI("RING TEL = %d",event.param1);
			break;

		case QL_VC_CONNECT_IND:
			break;
			
		case QL_VC_NOCARRIER_IND:
			break;		

		case QL_VC_ERROR_IND:
			break;
			
		case QL_VC_CCWA_IND:
			break;
			
		default:
			break;
	}
}


/******************************************************************************
 *  Function    -  ic_voice_call_start
 * 
 *  Purpose     -  拨打电话
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-17,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_voice_call_start(char* dial_num)
{
	ql_vc_errcode_e err = QL_VC_ERROR;

#if defined(PLATFORM_EC600)
	err = ql_voice_call_start(dial_num);
#endif
	if(err != QL_VC_SUCCESS)
	{
		LOGI("ic call start fail");
	}
	else
	{
		LOGI("ic call start ok");
	}
}

/******************************************************************************
 *  Function    -  ic_voice_call_end
 * 
 *  Purpose     -  挂断电话
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-19,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_voice_call_end(void)
{
#if defined(PLATFORM_EC600)
	ql_voice_call_end();
#endif
}

/******************************************************************************
 *  Function    -  user_voice_call_event_callback
 * 
 *  Purpose     -  通话的回调
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-21,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void user_voice_call_event_callback(int event_id, void *ctx)
{
	ql_event_t vc_event = {0};
	switch(event_id)
	{
		case QL_VC_RING_IND:
			//ctx will be destroyed when exit.
			LOGI("RING TEL = %s",(char *)ctx);
			break;
		case QL_VC_CCWA_IND:
			//ctx will be destroyed when exit.
			LOGI("QL_VC_CCWA_IND : %s",(char *)ctx);
			break;
		default:
			break;
	}
	vc_event.id = event_id;
	if(NULL != ctx)
	{
		vc_event.param1 = *(uint32 *)ctx;
	}
	
#if defined(PLATFORM_EC600)	
	ql_rtos_event_send(lvgl_gui_task, &vc_event);
#endif
}

static void calling_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* calling = lv_obj_create(NULL, NULL);
	static lv_style_t calling_bg;//must static type
	lv_style_init(&calling_bg);
	screen->data = calling;

	//添加窗口的风格
	lv_style_set_radius(&calling_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&calling_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&calling_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&calling_bg, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(calling,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(calling, LV_OBJ_PART_MAIN, &calling_bg);

	lv_obj_t *touxiang  = lv_img_create(calling, NULL);
	lv_obj_t *label1 = lv_label_create(calling, NULL);
	lv_obj_t *label2 = lv_label_create(calling, NULL);
	lv_obj_t *end  = lv_img_create(calling, NULL);

	//头像图片
	lv_obj_set_pos(touxiang, 86,10);
	lv_img_set_src(touxiang, iclv_get_image_by_id(IMG_CALL_FATHER_ID));

	lv_label_set_text(label1, "father");
	lv_obj_set_pos(label1,94,89);

	lv_label_set_text(label2, "calling....");
	lv_obj_set_pos(label2,76,134);

	//挂断图片
	lv_obj_set_pos(end, 90,170);
	lv_img_set_src(end, iclv_get_image_by_id(IMG_CALL_END_ID));
	
}

static void calling_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void calling_exit(void* arg)
{
    LOGI("calling screen exit");
}

static void calling_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  ic_entry_calling_screen
 * 
 *  Purpose     -  正在拨号界面
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-20,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_entry_calling_screen(void)
{
	screen_callback_t cb;

    cb.init = calling_init;
    cb.deinit = calling_deinit;
    cb.entry = calling_entry;
    cb.exit = calling_exit;

	if(!ic_screen_create(SCREEN_CALLING, &cb)) 
	{
		LOGI("create dial screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_CALLING)) 
	{
        LOGI("enter dial screen fail");
		return;
    }

}


/******************************************************************************
 *  Function    -  ic_save_pb_number
 * 
 *  Purpose     -  保存按键的内容
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-20,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void ic_save_pb_number(lv_obj_t* obj)
{
	if(number_len >= NUMBER_MAX_LENGTH)
	{
		LOGI("number len too long\n");
		if(obj == delete)
		{
			number_len--;
			input_number[number_len] = '\0'; 
		}
		return;
	}

	if(obj == delete)
	{
		if(number_len > 0)
		{
			number_len--;
			input_number[number_len] = '\0'; 
		}
	}
	else if(obj == num1)
	{
		input_number[number_len] = '1'; 
		number_len++;
	}
	else if(obj == num2)
	{
		input_number[number_len] = '2'; 
		number_len++;
	}
	else if(obj == num3)
	{
		input_number[number_len] = '3'; 
		number_len++;
	}
	else if(obj == num4)
	{
		input_number[number_len] = '4'; 
		number_len++;
	}
	else if(obj == num5)
	{
		input_number[number_len] = '5'; 
		number_len++;
	}
	else if(obj == num6)
	{
		input_number[number_len] = '6'; 
		number_len++;
	}
	else if(obj == num7)
	{
		input_number[number_len] = '7'; 
		number_len++;
	}
	else if(obj == num8)
	{
		input_number[number_len] = '8'; 
		number_len++;
	}
	else if(obj == num9)
	{
		input_number[number_len] = '9'; 
		number_len++;
	}
	else if(obj == star)
	{
		input_number[number_len] = '*'; 
		number_len++;
	}
	else if(obj == num0)
	{
		input_number[number_len] = '0'; 
		number_len++;
	}
	else if(obj == well)
	{
		input_number[number_len] = '#'; 
		number_len++;
	}
	else if(obj == plus)
	{
		input_number[number_len] = '+'; 
		number_len++;
	}
}

/******************************************************************************
 *  Function    -  ic_dial_click_event_cb
 * 
 *  Purpose     -  点击拨号键盘的回调
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-20,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
static void ic_dial_click_event_cb(lv_obj_t* obj, lv_event_t event)
{
    static lv_point_t down_pos;
    static lv_point_t last_pos;
    lv_point_t pos;
    static lv_point_t offset = {0};

	//LOGI("777777777777 = %d\n",event);

	switch (event)
	{
		case LV_EVENT_SHORT_CLICKED:
		case LV_EVENT_CLICKED:
		case LV_EVENT_VALUE_CHANGED:
			lv_indev_get_point(lv_indev_get_act(), &pos);
			LOGI("release(%d, %d)\n", pos.x, pos.y);

			offset.x = pos.x - down_pos.x;
			offset.y = pos.y - down_pos.y;

			if(abs(offset.x) > abs(offset.y) && offset.x > 10)
			{
				memset(input_number,0,NUMBER_MAX_LENGTH);
				number_len = 0;
				ic_screen_goback();
			}
			break;
	
		case LV_EVENT_PRESSED:
			lv_indev_get_point(lv_indev_get_act(), &down_pos);
			LOGI("pressed(%d, %d)\n", down_pos.x, down_pos.y);
			offset.x = 0;
			offset.y = 0;
			
			if(obj == delete)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(delete, 199,37);
				lv_img_set_src(delete, iclv_get_image_by_id(IMG_DIAL_H_DELETE_ID));
			}
			else if(obj == num1)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num1, 3,67);
				lv_img_set_src(num1, iclv_get_image_by_id(IMG_DIAL_H_NUM1_ID));
			}
			else if(obj == num2)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num2, 82,67);
				lv_img_set_src(num2, iclv_get_image_by_id(IMG_DIAL_H_NUM2_ID));
			}
			else if(obj == num3)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num3, 161,67);
				lv_img_set_src(num3, iclv_get_image_by_id(IMG_DIAL_H_NUM3_ID));
			}
			else if(obj == num4)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num4, 3,99);
				lv_img_set_src(num4, iclv_get_image_by_id(IMG_DIAL_H_NUM4_ID));
			}
			else if(obj == num5)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num5, 82,99);
				lv_img_set_src(num5, iclv_get_image_by_id(IMG_DIAL_H_NUM5_ID));
			}
			else if(obj == num6)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num6, 161,99);
				lv_img_set_src(num6, iclv_get_image_by_id(IMG_DIAL_H_NUM6_ID));
			}
			else if(obj == num7)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num7, 3,131);
				lv_img_set_src(num7, iclv_get_image_by_id(IMG_DIAL_H_NUM7_ID));
			}
			else if(obj == num8)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num8, 82,131);
				lv_img_set_src(num8, iclv_get_image_by_id(IMG_DIAL_H_NUM8_ID));
			}
			else if(obj == num9)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num9, 161,131);
				lv_img_set_src(num9, iclv_get_image_by_id(IMG_DIAL_H_NUM9_ID));
			}
			else if(obj == star)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(star, 3,163);
				lv_img_set_src(star, iclv_get_image_by_id(IMG_DIAL_H_STAR_ID));
			}
			else if(obj == num0)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(num0, 82,163);
				lv_img_set_src(num0, iclv_get_image_by_id(IMG_DIAL_H_NUM0_ID));
			}
			else if(obj == well)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(well, 161,163);
				lv_img_set_src(well, iclv_get_image_by_id(IMG_DIAL_H_WELL_ID));
			}
			else if(obj == plus)
			{
				ic_save_pb_number(obj);
				lv_textarea_set_text(phone_number_dis, (char *)input_number);
				lv_obj_set_pos(plus, 3,199);
				lv_img_set_src(plus, iclv_get_image_by_id(IMG_DIAL_H_PLUS_ID));
			}
			else if(obj == dial)
			{
				lv_obj_set_pos(dial, 82,199);
				lv_img_set_src(dial, iclv_get_image_by_id(IMG_DIAL_H_DIAL_ID));
				ic_voice_call_start((char *)input_number);
				ic_entry_calling_screen();
			}
			
			break;

		case LV_EVENT_RELEASED:
			lv_indev_get_point(lv_indev_get_act(), &pos);
			LOGI("release(%d, %d)\n", pos.x, pos.y);

			offset.x = pos.x - down_pos.x;
			offset.y = pos.y - down_pos.y;

			if(abs(offset.x) > abs(offset.y) && offset.x > 10)
			{
				memset(input_number,0,NUMBER_MAX_LENGTH);
				number_len = 0;
				ic_screen_goback();
				break;
			}
			
			lv_obj_set_pos(delete, 199,37);
			lv_img_set_src(delete, iclv_get_image_by_id(IMG_DIAL_DELETE_ID));

			lv_obj_set_pos(num1, 3,67);
			lv_img_set_src(num1, iclv_get_image_by_id(IMG_DIAL_NUM1_ID));

			lv_obj_set_pos(num2, 82,67);
			lv_img_set_src(num2, iclv_get_image_by_id(IMG_DIAL_NUM2_ID));

			lv_obj_set_pos(num3, 161,67);
			lv_img_set_src(num3, iclv_get_image_by_id(IMG_DIAL_NUM3_ID));

			lv_obj_set_pos(num4, 3,99);
			lv_img_set_src(num4, iclv_get_image_by_id(IMG_DIAL_NUM4_ID));

			lv_obj_set_pos(num5, 82,99);
			lv_img_set_src(num5, iclv_get_image_by_id(IMG_DIAL_NUM5_ID));

			lv_obj_set_pos(num6, 161,99);
			lv_img_set_src(num6, iclv_get_image_by_id(IMG_DIAL_NUM6_ID));

			lv_obj_set_pos(num7, 3,131);
			lv_img_set_src(num7, iclv_get_image_by_id(IMG_DIAL_NUM7_ID));

			lv_obj_set_pos(num8, 82,131);
			lv_img_set_src(num8, iclv_get_image_by_id(IMG_DIAL_NUM8_ID));

			lv_obj_set_pos(num9, 161,131);
			lv_img_set_src(num9, iclv_get_image_by_id(IMG_DIAL_NUM9_ID));

			lv_obj_set_pos(star, 3,163);
			lv_img_set_src(star, iclv_get_image_by_id(IMG_DIAL_STAR_ID));

			lv_obj_set_pos(num0, 82,163);
			lv_img_set_src(num0, iclv_get_image_by_id(IMG_DIAL_NUM0_ID));

			lv_obj_set_pos(well, 161,163);
			lv_img_set_src(well, iclv_get_image_by_id(IMG_DIAL_WELL_ID));

			lv_obj_set_pos(plus, 3,199);
			lv_img_set_src(plus, iclv_get_image_by_id(IMG_DIAL_PLUS_ID));

			lv_obj_set_pos(dial, 82,199);
			lv_img_set_src(dial, iclv_get_image_by_id(IMG_DIAL_DIAL_ID));
			break;
			
		case LV_EVENT_PRESS_LOST:
			lv_obj_set_pos(delete, 199,37);
			lv_img_set_src(delete, iclv_get_image_by_id(IMG_DIAL_DELETE_ID));

			lv_obj_set_pos(num1, 3,67);
			lv_img_set_src(num1, iclv_get_image_by_id(IMG_DIAL_NUM1_ID));

			lv_obj_set_pos(num2, 82,67);
			lv_img_set_src(num2, iclv_get_image_by_id(IMG_DIAL_NUM2_ID));

			lv_obj_set_pos(num3, 161,67);
			lv_img_set_src(num3, iclv_get_image_by_id(IMG_DIAL_NUM3_ID));

			lv_obj_set_pos(num4, 3,99);
			lv_img_set_src(num4, iclv_get_image_by_id(IMG_DIAL_NUM4_ID));

			lv_obj_set_pos(num5, 82,99);
			lv_img_set_src(num5, iclv_get_image_by_id(IMG_DIAL_NUM5_ID));

			lv_obj_set_pos(num6, 161,99);
			lv_img_set_src(num6, iclv_get_image_by_id(IMG_DIAL_NUM6_ID));

			lv_obj_set_pos(num7, 3,131);
			lv_img_set_src(num7, iclv_get_image_by_id(IMG_DIAL_NUM7_ID));

			lv_obj_set_pos(num8, 82,131);
			lv_img_set_src(num8, iclv_get_image_by_id(IMG_DIAL_NUM8_ID));

			lv_obj_set_pos(num9, 161,131);
			lv_img_set_src(num9, iclv_get_image_by_id(IMG_DIAL_NUM9_ID));

			lv_obj_set_pos(star, 3,163);
			lv_img_set_src(star, iclv_get_image_by_id(IMG_DIAL_STAR_ID));

			lv_obj_set_pos(num0, 82,163);
			lv_img_set_src(num0, iclv_get_image_by_id(IMG_DIAL_NUM0_ID));

			lv_obj_set_pos(well, 161,163);
			lv_img_set_src(well, iclv_get_image_by_id(IMG_DIAL_WELL_ID));

			lv_obj_set_pos(plus, 3,199);
			lv_img_set_src(plus, iclv_get_image_by_id(IMG_DIAL_PLUS_ID));

			lv_obj_set_pos(dial, 82,199);
			lv_img_set_src(dial, iclv_get_image_by_id(IMG_DIAL_DIAL_ID));
			break;
			
		default:
			break;
	}

}

static void dial_init(void *arg)
{
	ic_screen_t* screen = (ic_screen_t*)arg;
	lv_obj_t* dial_num = lv_obj_create(NULL, NULL);
	static lv_style_t dial_bg;//must static type
	lv_style_init(&dial_bg);
	screen->data = dial_num;

	//添加窗口的风格
	lv_style_set_radius(&dial_bg, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&dial_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&dial_bg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&dial_bg, LV_STATE_DEFAULT, 0);
	lv_obj_set_size(dial_num,LV_HOR_RES,LV_VER_RES);
	lv_obj_add_style(dial_num, LV_OBJ_PART_MAIN, &dial_bg);

	num0 = lv_img_create(dial_num, NULL);
	num1 = lv_img_create(dial_num, NULL);
	num2 = lv_img_create(dial_num, NULL);
	num3 = lv_img_create(dial_num, NULL);
	num4 = lv_img_create(dial_num, NULL);
	num5 = lv_img_create(dial_num, NULL);
	num6 = lv_img_create(dial_num, NULL);
	num7 = lv_img_create(dial_num, NULL);
	num8 = lv_img_create(dial_num, NULL);
	num9 = lv_img_create(dial_num, NULL);
	well = lv_img_create(dial_num, NULL);
	delete = lv_img_create(dial_num, NULL);
	dial = lv_img_create(dial_num, NULL);
	plus = lv_img_create(dial_num, NULL);
	star = lv_img_create(dial_num, NULL);
	phone_number_dis = lv_textarea_create(dial_num, NULL);

	//显示号码的显示框
	static lv_style_t phone_dis_pg;
	lv_style_init(&phone_dis_pg);
	//设置字体大小
	//lv_obj_set_style_local_text_font(phone_number_dis,LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,&lv_font_montserrat_14);
	lv_style_set_bg_color(&phone_dis_pg, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&phone_dis_pg, LV_STATE_DEFAULT, LV_COLOR_BLACK);	//添加边界框
	lv_style_set_border_width(&phone_dis_pg, LV_STATE_DEFAULT, 0);
	lv_style_set_text_color(&phone_dis_pg, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_add_style(phone_number_dis, LV_OBJ_PART_MAIN, &phone_dis_pg);
	
	lv_textarea_set_one_line(phone_number_dis, true);
	lv_obj_set_size(phone_number_dis,198,43);
	lv_obj_align(phone_number_dis, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	lv_textarea_set_text(phone_number_dis,"");
	lv_obj_set_pos(phone_number_dis,0,25);
	lv_obj_set_event_cb(phone_number_dis, ic_dial_click_event_cb);

	
	//删除
	lv_obj_set_pos(delete, 199,37);
	lv_img_set_src(delete, iclv_get_image_by_id(IMG_DIAL_DELETE_ID));
	lv_obj_set_click(delete, true);
	lv_obj_set_event_cb(delete, ic_dial_click_event_cb);
	
	//数字键盘
	lv_obj_set_pos(num1, 3,67);
	lv_img_set_src(num1, iclv_get_image_by_id(IMG_DIAL_NUM1_ID));
	lv_obj_set_click(num1, true);
	lv_obj_set_event_cb(num1, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num2, 82,67);
	lv_img_set_src(num2, iclv_get_image_by_id(IMG_DIAL_NUM2_ID));
	lv_obj_set_click(num2, true);
	lv_obj_set_event_cb(num2, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num3, 161,67);
	lv_img_set_src(num3, iclv_get_image_by_id(IMG_DIAL_NUM3_ID));
	lv_obj_set_click(num3, true);
	lv_obj_set_event_cb(num3, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num4, 3,99);
	lv_img_set_src(num4, iclv_get_image_by_id(IMG_DIAL_NUM4_ID));
	lv_obj_set_click(num4, true);
	lv_obj_set_event_cb(num4, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num5, 82,99);
	lv_img_set_src(num5, iclv_get_image_by_id(IMG_DIAL_NUM5_ID));
	lv_obj_set_click(num5, true);
	lv_obj_set_event_cb(num5, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num6, 161,99);
	lv_img_set_src(num6, iclv_get_image_by_id(IMG_DIAL_NUM6_ID));
	lv_obj_set_click(num6, true);
	lv_obj_set_event_cb(num6, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num7, 3,131);
	lv_img_set_src(num7, iclv_get_image_by_id(IMG_DIAL_NUM7_ID));
	lv_obj_set_click(num7, true);
	lv_obj_set_event_cb(num7, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num8, 82,131);
	lv_img_set_src(num8, iclv_get_image_by_id(IMG_DIAL_NUM8_ID));
	lv_obj_set_click(num8, true);
	lv_obj_set_event_cb(num8, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num9, 161,131);
	lv_img_set_src(num9, iclv_get_image_by_id(IMG_DIAL_NUM9_ID));
	lv_obj_set_click(num9, true);
	lv_obj_set_event_cb(num9, ic_dial_click_event_cb);

	lv_obj_set_pos(star, 3,163);
	lv_img_set_src(star, iclv_get_image_by_id(IMG_DIAL_STAR_ID));
	lv_obj_set_click(star, true);
	lv_obj_set_event_cb(star, ic_dial_click_event_cb);
	
	lv_obj_set_pos(num0, 82,163);
	lv_img_set_src(num0, iclv_get_image_by_id(IMG_DIAL_NUM0_ID));
	lv_obj_set_click(num0, true);
	lv_obj_set_event_cb(num0, ic_dial_click_event_cb);
	
	lv_obj_set_pos(well, 161,163);
	lv_img_set_src(well, iclv_get_image_by_id(IMG_DIAL_WELL_ID));
	lv_obj_set_click(well, true);
	lv_obj_set_event_cb(well, ic_dial_click_event_cb);


	lv_obj_set_pos(plus, 3,199);
	lv_img_set_src(plus, iclv_get_image_by_id(IMG_DIAL_PLUS_ID));
	lv_obj_set_click(plus, true);
	lv_obj_set_event_cb(plus, ic_dial_click_event_cb);
	
	lv_obj_set_pos(dial, 82,199);	
	lv_img_set_src(dial, iclv_get_image_by_id(IMG_DIAL_DIAL_ID));
	lv_obj_set_click(dial, true);
	lv_obj_set_event_cb(dial, ic_dial_click_event_cb);


}

static void dial_deinit(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;

    lv_obj_del((lv_obj_t*)screen->data);

}

static void dial_exit(void* arg)
{
    LOGI("mainmenu information screen exit");
}

static void dial_entry(void* arg)
{
    ic_screen_t* screen = (ic_screen_t*)arg;
    lv_scr_load((lv_obj_t*)screen->data);
}

/******************************************************************************
 *  Function    -  ic_entry_dial_screen
 * 
 *  Purpose     -  进入拨号界面
 * 
 *  Description -   
 * 
 * modification history
 * ----------------------------------------
 * v1.0  , 2021-04-19,  xiongweiming  written
 * ----------------------------------------
 ******************************************************************************/
void ic_entry_dial_screen(void)
{
	screen_callback_t cb;

    cb.init = dial_init;
    cb.deinit = dial_deinit;
    cb.entry = dial_entry;
    cb.exit = dial_exit;

	if(!ic_screen_create(SCREEN_DIAL, &cb)) 
	{
		LOGI("create dial screen fail");
		return;
	}

    if(!ic_screen_entry(SCREEN_DIAL)) 
	{
        LOGI("enter dial screen fail");
		return;
    }

}

