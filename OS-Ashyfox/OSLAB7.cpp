#include<bits/stdc++.h>
using namespace std;

typedef struct PCB{  // ���̿��ƿ� 
    char name[20];
    int running_time; //  ����ʱ��
    int enter_time; //   ����ʱ��
    int priority;  //    ���ȼ�
    int end_time;      //���ʱ��
    int copyRunning_time;  //����ʱ��Ƭ��ת
    int start_time;  //  ���̿�ʼ���е�ʱ��
    struct PCB* next;
} PCB;

typedef struct PCBQueue{
    PCB* firstProg;
    PCB* LastProg;
    int size;
} PCBQueue;

void Queueinit(PCBQueue* queue){
    if(queue==NULL){
        return;
    }
    queue->size = 0;
    queue->LastProg = (PCB*)malloc(sizeof(PCB));
    queue->firstProg = queue->LastProg;
}

void EnterQueue(PCBQueue* queue,PCB* pro){   //������̶���
    queue->LastProg->next = (PCB*)malloc(sizeof(PCB));
    queue->LastProg = queue->LastProg->next;
    queue->LastProg->enter_time = pro->enter_time;
    memcpy(queue->LastProg->name,pro->name,sizeof(pro->name));
    queue->LastProg->priority = pro->priority;
    queue->LastProg->running_time = pro->running_time;
    queue->LastProg->copyRunning_time = pro->copyRunning_time;
    queue->LastProg->start_time = pro->start_time;
    queue->size++;
}
PCB* poll(PCBQueue* queue){ // ������б߽� 
    PCB* temp = queue->firstProg->next;
    if(temp == queue->LastProg){
        queue->LastProg=queue->firstProg;
        queue->size--;
        return temp;
    }
    queue->firstProg->next = queue->firstProg->next->next;
    queue->size--;
    return temp;
}
// ��������˳������ 
void sortWithEnterTime(PCB pro[],int num){
   sort(pro,pro+num,[](const PCB &a,const PCB &b){
   		return a.enter_time < b.enter_time;});
}
// ��������ʱ�䡿����
void sortWithLongth(PCB pro[],int start,int end){
    sort(pro+start,pro+end,[](const PCB &a,const PCB &b){
    	return a.running_time<b.running_time;});
}
//������Ӧ�ȡ���������
void sortWithResponse(PCB pro[],int start,int end){
    sort(pro + start, pro + end, [](const PCB &a, const PCB &b) {
        float responseA = (a.start_time - a.enter_time + a.running_time) / (a.running_time + 0.0);
        float responseB = (b.start_time - b.enter_time + b.running_time) / (b.running_time + 0.0);
        return responseA > responseB;});
}
//����Ȩ�ء�����
void sortWithPriority(PCB pro[],int start,int end){
    sort(pro + start, pro + end, [](const PCB &a, const PCB &b) {
        return a.priority < b.priority;});
}
// �����ȷ����㷨
void FCFS(PCB pro[],int num){ 
    printf("���� ����ʱ��  ����ʱ�� ��ʼʱ�� ���ʱ�� ��תʱ�� ��Ȩ��תʱ��\n");
    sortWithEnterTime(pro,num);    //���ս���˳������
    PCBQueue* queue = (PCBQueue*)malloc(sizeof(PCBQueue));
    Queueinit(queue);
    EnterQueue(queue,&pro[0]);
    int time = pro[0].enter_time;
    int pronum=1;    //��¼��ǰ�Ľ���
    float sum_T_time = 0; //ƽ����תʱ��
    float sum_QT_time = 0 ; // ��Ȩƽ����תʱ��

    while(queue->size>0){
        PCB* curpro = poll(queue);   //�ӽ��̶�����ȡ������
        if(time < curpro->enter_time) time =  curpro->enter_time;
        int end_time = time+curpro->running_time; //���ʱ��
        int T_time = end_time - curpro->enter_time; //��תʱ��
        sum_T_time += T_time;
        float QT_time = T_time / (curpro->running_time+0.0); // ��Ȩ��תʱ��
        sum_QT_time += QT_time;
        for(int tt = time;tt<=end_time && pronum<num;tt++){    //ģ����̵�ִ�й���
            if(tt >= pro[pronum].enter_time){
                EnterQueue(queue,&pro[pronum]);
                pronum++;
            }
        }
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%.2f\n",curpro->name,curpro->enter_time,
		curpro->running_time,time,end_time,T_time,QT_time);
        time += curpro->running_time;
        if(queue->size==0 && pronum < num){   
            EnterQueue(queue,&pro[pronum]);
            pronum++;
        }
    }
    printf("ƽ����תʱ��Ϊ%.2f\tƽ����Ȩ��תʱ��Ϊ%.2f\n",
	sum_T_time/(num+0.0),sum_QT_time/(num+0.0));
}
// �̽��������㷨
void SPF(PCB pro[],int num) {
    printf("���� ����ʱ��  ����ʱ�� ��ʼʱ�� ���ʱ�� ��תʱ�� ��Ȩ��תʱ��\n");
    sortWithEnterTime(pro,num);
    PCBQueue* queue = (PCBQueue*)malloc(sizeof(PCBQueue));;
    Queueinit(queue);
    EnterQueue(queue,&pro[0]);
    int time = pro[0].enter_time;
    int pronum=1; 
    float sum_T_time = 0,sum_QT_time = 0;
    while(queue->size>0){
        PCB* curpro = poll(queue);   //�ӽ��̶�����ȡ������
        if(time <  curpro->enter_time) time =  curpro->enter_time;
        int end_time = time+curpro->running_time;
        int T_time = end_time - curpro->enter_time;
        float QT_time = T_time / (curpro->running_time+0.0) ;
        sum_T_time += T_time;
        sum_QT_time += QT_time;
        int pre = pronum;
        for(int tt = time;tt<=end_time&&pronum<num;tt++)    
            if(tt>=pro[pronum].enter_time) 
                pronum++;
        sortWithLongth(pro,pre,pronum);
        for(int i=pre;i<pronum;i++) EnterQueue(queue,&pro[i]);
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%.2f\n",curpro->name,curpro->enter_time,
		curpro->running_time,time,end_time,T_time,QT_time);
        time+=curpro->running_time;
        if(queue->size==0&&pronum<num){   
            EnterQueue(queue,&pro[pronum]);
            pronum++;
        }
    }
    printf("ƽ����תʱ��Ϊ%.2f\tƽ����Ȩ��תʱ��Ϊ%.2f\n",
	sum_T_time/(num+0.0),sum_QT_time/num);
}

//���ȼ������㷨
void HPF(PCB pro[],int num){
    printf("���� ����ʱ��  ����ʱ�� ��ʼʱ�� ���ʱ�� ��תʱ�� ��Ȩ��תʱ��\n");
    sortWithEnterTime(pro,num);
    PCBQueue* queue = (PCBQueue*)malloc(sizeof(PCBQueue));;
    Queueinit(queue);
    EnterQueue(queue,&pro[0]);
    int time = pro[0].enter_time;
    int pronum=1;    //��¼��ǰ�Ľ���
    float sum_T_time = 0,sum_QT_time = 0;
    while(queue->size>0){
        PCB* curpro = poll(queue);   //�ӽ��̶�����ȡ������
        if(time<curpro->enter_time)
            time =  curpro->enter_time;
        int end_time = time+curpro->running_time;
        int T_time = end_time - curpro->enter_time;
        float QT_time = T_time / (curpro->running_time+0.0) ;
        sum_T_time += T_time;
        sum_QT_time += QT_time;
        int pre = pronum;
        for(int tt = time;tt<=end_time&&pronum<num;tt++)
            if(tt>=pro[pronum].enter_time) 
                pronum++;
        sortWithPriority(pro,pre,pronum);
        for(int i=pre;i<pronum;i++) EnterQueue(queue,&pro[i]);
        pre = pronum;
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%.2f\n",curpro->name,curpro->enter_time,
		curpro->running_time,time,end_time,T_time,QT_time);
        time +=  curpro->running_time;
        if(queue->size==0&&pronum<num){  
            EnterQueue(queue,&pro[pronum]);
            pronum++;
        }
    }
    printf("ƽ����תʱ��Ϊ%.2f\tƽ����Ȩ��תʱ��Ϊ%.2f\n",
	sum_T_time/(num+0.0),sum_QT_time/(num+0.0));
}
//ʱ��Ƭ��ת����
void RR(PCB pro[],int num){
    printf("������ʱ��Ƭ��С��");
    int timeslice;
    scanf("%d",&timeslice);
    printf("���� ����ʱ�� ����ʱ�� ����ʱ�� ���ʱ�� ��תʱ�� ��Ȩ��תʱ��\n");
    sortWithEnterTime(pro,num);
    PCBQueue* queue = (PCBQueue*)malloc(sizeof(PCBQueue));;
    Queueinit(queue);
    pro[0].start_time = pro[0].enter_time;
    EnterQueue(queue,&pro[0]);
    int time = 0;
    int pronum = 1;
    float sum_T_time = 0,sum_QT_time = 0;
    while(queue->size>0){
        PCB* curpro = poll(queue);   
        if(time<curpro->enter_time)
            time = curpro->enter_time;
        if(timeslice >= curpro->running_time){  
            for(int tt = time;tt<=time+curpro->running_time&&pronum<num;tt++){ 
                if(tt>=pro[pronum].enter_time){
                    pro[pronum].start_time = tt;
                    EnterQueue(queue,&pro[pronum]);
                    pronum++;
                }
            }
            time += curpro->running_time;
            curpro->running_time = 0;
            curpro->end_time = time;
            int T_time = curpro->end_time-curpro->start_time;
            float QT_time = T_time / (curpro->copyRunning_time+0.0);
            sum_T_time += T_time;
            sum_QT_time += QT_time;
            printf("%s\t%d\t%d\t  %d\t   %d\t %d\t  %.2f\n",curpro->name,curpro->enter_time,
			curpro->copyRunning_time,curpro->start_time,curpro->end_time,T_time,QT_time);
            if(queue->size==0&&pronum<num){   
                pro[pronum].start_time = pro[pronum].enter_time;
                EnterQueue(queue,&pro[pronum]);
                pronum++;
            }
            continue;
        }
        // ����ʱ�����ʱ��Ƭ
        for(int tt = time;tt<=time+timeslice&&pronum<num;tt++){    
            if(tt>=pro[pronum].enter_time){
                pro[pronum].start_time = tt;
                EnterQueue(queue,&pro[pronum]);
                pronum++;
            }
        }
        time += timeslice;
        curpro->running_time -= timeslice;
        //��ǰ����δ���  ������ӵ�������
        EnterQueue(queue,curpro);
        if(queue->size==0&&pronum<num){ 
            pro[pronum].start_time = pro[pronum].enter_time;
            EnterQueue(queue,&pro[pronum]);
            pronum++;
        }
    }
    printf("ƽ����תʱ��Ϊ%.2f\tƽ����Ȩ��תʱ��Ϊ%.2f\n",
	sum_T_time/(num+0.0),sum_QT_time/(num+0.0));
}
//����Ӧ������
void HRRN(PCB pro[],int num) {
    printf("���� ����ʱ��  ����ʱ�� ��ʼʱ�� ���ʱ�� ��תʱ�� ��Ȩ��תʱ��\n");
    sortWithEnterTime(pro,num);
    PCBQueue* queue = (PCBQueue*)malloc(sizeof(PCBQueue));;
    Queueinit(queue);
    EnterQueue(queue,&pro[0]);
    int time = pro[0].enter_time;
    int pronum=1;    //��¼��ǰ�Ľ���
    float sum_T_time = 0,sum_QT_time = 0;
    while(queue->size>0){
        PCB* curpro = poll(queue);  
        if(time <  curpro->enter_time) time =  curpro->enter_time;
        int end_time = time+curpro->running_time;
        int T_time = end_time - curpro->enter_time;
        float QT_time = T_time / (curpro->running_time+0.0) ;
        sum_T_time += T_time;
        sum_QT_time += QT_time;
        int pre = pronum;
        for(int tt = time;tt<=end_time&&pronum<num;tt++)   
            if(tt>=pro[pronum].enter_time) 
                pronum++;
        sortWithResponse(pro,pre,pronum);
        for(int i=pre;i<pronum;i++) EnterQueue(queue,&pro[i]);
        pre = pronum;
        printf("%s\t%d\t%d\t%d\t%d\t%d\t%.2f\n",curpro->name,curpro->enter_time,
		curpro->running_time,time,end_time,T_time,QT_time);
        time +=  curpro->running_time;
        if(queue->size==0&&pronum<num){  
            EnterQueue(queue,&pro[pronum]);
            pronum++;
        }
    }
    printf("ƽ����תʱ��Ϊ%.2f\tƽ����Ȩ��תʱ��Ϊ%.2f\n",
	sum_T_time/(num+0.0),sum_QT_time/num);
}
int main(){
	printf("======================================\n");
	printf("ʵ�����7-�����������㷨ģ��ʵ����Ƚ�\n");
	printf("������Ϣ��������Ashyfox	ѧ�ţ�22231044\n");
	printf("======================================\n");
    int proNum; 
    printf("��������̵ĸ�����");
    cin >> proNum;
    PCB pro[proNum];
    for(int i=0;i<proNum;i++){
        printf("�������%d�����̵����֡�����ʱ�䡢����ʱ�䡢���ȼ����ո����֣���",i+1);
        cin>>pro[i].name>>pro[i].enter_time>>pro[i].running_time>>pro[i].priority; 
        pro[i].copyRunning_time = pro[i].running_time;
   }
    int choice;
    while(1){
    	printf("---------------------------------------------------------------\n");
		printf("��ѡ����̵����㷨��\n");
    	printf("1.�����ȷ����㷨\n2.�̽��������㷨\n3.���ȼ������㷨\n4.ʱ��Ƭ��ת�㷨\n5.����Ӧ�������㷨\n0.�˳�\n");
    	printf("��������Ҫ���еĲ�����");
        cin >> choice;
        printf("---------------------------------------------------------------\n");
        switch(choice){
            case 1:FCFS(pro,proNum);break;
            case 2:SPF(pro,proNum);break;
            case 3:HPF(pro,proNum);break;
            case 4:RR(pro,proNum);break;
            case 5:HRRN(pro,proNum);break;
            case 0:return 0;
            default:printf("������ڴ�������������!\n");break;
        }
    }
    return 0;
}

