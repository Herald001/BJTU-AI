#include <stdio.h>
#include  <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX 5010
// ҳ����Ϣ�ṹ
struct PageInfo
{
    int pages[MAX];         // ģ���������ҳ����
    int rw[MAX];           // ����ҳ��������ֵ�Ƿ����0.7
    int page_missing_num;   // ȱҳ�жϴ���
    int allocated_page_num; // �����ҳ����
    int visit_list_length;  // ����ҳ�����г���
} pInfo;

// ҳ������Ϣ
typedef  struct
{
    int time;      // ��¼ҳ�������ݵķ��ʴ���
    int isVisit;  // ����λ
    int isModify; // �޸�λ
    int isRW;     // ֧�ֶ�����  0 / ֧��д���� 1
    int pages;     // ҳ��
} MemInfo;
// ȫ�ֱ�������
MemInfo pageList[MAX];          // �����ҳ��
int current_page, replace_page; // ҳ�����ָ�롢ҳ���滻ָ��
int isLoss;                    // ȱҳ��־�� 1 ȱҳ�� 0 ����
int N;                          // �����߼���ַ�ռ�ҳ������
void Init()
{ // ��ʼ���������ṹ�塢ȫ�ֱ����ĳ�ʼ��
    printf("===================================================\n");
    printf("ʵ�������-ҳ����̭�㷨ģ��ʵ����Ƚ�\n");
    printf("������Ϣ������-Ashyfox ѧ��-22231044\n");
    printf("===================================================\n");

    printf("����������߼���ַ�ռ�ҳ��������");
    scanf("%d", &N); // �����߼���ַ�ռ�ҳ������
    getchar();

    isLoss = 0;
    pInfo.page_missing_num = 0;

    printf("����������ҳ������"); // �Զ�������ҳ����
    scanf("%d", &pInfo.allocated_page_num);
    getchar();

    if (pInfo.allocated_page_num > N)
    {
        printf("�ռ䲻�㣡������ҳ����Ϊ%d\n", N);
        pInfo.allocated_page_num = N;
    }

    for (int i = 0; i < N; i++) // ���ҳ������
        pInfo.pages[i] = -1;
    for (int i = 0; i < pInfo.allocated_page_num; i++)
        pageList[i].isRW = rand() % 2 ? 1 : 0; // ��������Ƿ�֧�ֶ�д����
}

// �������ҳ���������
void RandomSequence()
{
    int w, v, l;           // �������а�����ҳ��w���������ƶ�����v
    int s = rand() % N; // �����ʼ����������ʼҳ�� s��[0, N)
    printf("�����빤����������ҳ����");
    scanf("%d", &w);
    getchar();
    printf("�����빤�����ƶ��ʣ�");
    scanf("%d", &v);
    getchar();
    printf("������������еĳ��ȣ�");
    scanf("%d", &l);
    pInfo.visit_list_length = l; 
    getchar();
    srand((unsigned int)time(NULL)); // �������
    double t = rand() % 10 * 0.1;

    int j = 0;
    for (int i = 0; i < l; i++)
    {
        if (j < l)
        {
            pInfo.pages[i] = (s + (rand() % w)) % N; 
            double r_w = (rand() % 10) * 0.1; 
            pInfo.rw[i] = r_w > 0.7 ? 1 : 0; 
            j++;                             
        }
        if (j % v == 0)
        { // ����ȡֵ����Ϊ [0, 1] ��һ������� r
            double r = (rand() % 10) * 0.1;
            if (r < t)          // �Ƚ� r �� t �Ĵ�С
                s = rand() % N; // Ϊ s ����һ����ֵ��s��[0, N)��
            else
                s = (s + 1) % N;
        }
    }
    printf("\n-----------------ҳ���������-----------------\n");
    for (int i = 0; i < l; i++)
    {
        printf("%d ", pInfo.pages[i]);
        if ((i + 1) % 10 == 0)
            printf("\n");   // ÿ����ʾ10��
    }
    if (l % 10 != 0)
        printf("\n");
    printf("----------------------------------------------\n");
}

// ������ʾ״̬��Ϣ
void showState()
{
    printf("����%d -->�ڴ�ռ�[", pInfo.pages[current_page]);
    for (int i = 0; i < pInfo.allocated_page_num; i++)
    {
        if (pageList[i].pages >= 0)
            printf("%d ", pageList[i].pages);
        else
            printf("   ");
    }
    printf("]");
    if (isLoss)
    {
        if (!pInfo.page_missing_num)
        {
            printf("--> Ԥ����ҳ��......\n");
        }
        else
        {
            printf(" --> ȱҳ�ж� --> ȱҳ�� = %.2f\n", (float)(pInfo.page_missing_num) * 100.00 / current_page);
        }
    }
    else
        printf("--> ����\n");
}

// ����ҳ���Ƿ��Ѿ����ڴ���
int isExist(){
    for (int i = 0; i < pInfo.allocated_page_num; i++){
        pageList[i].time++;
        if (pageList[i].pages == pInfo.pages[current_page])
        {
            isLoss = 0;
            pageList[i].time = 0;
            pageList[i].isVisit = 1;
            if (pageList[i].isRW && pInfo.rw[current_page])
                pageList[i].isModify = 1;
            return 1;
        }
    }
    isLoss = 1;
    return 0;
}

// �����̭�㷨
void OPT(){
    pInfo.page_missing_num = 0;
    int full, status;
    replace_page = 0;                                  // ҳ���滻ָ���ʼ��Ϊ0
    full = 0;                                          // �Ѿ�ռ�õ�ҳ����
    isLoss = 0;                                        // ȱҳ��־��0Ϊ��ȱҳ��1Ϊȱҳ
    for (int i = 0; i < pInfo.allocated_page_num; i++) // ���ҳ����Ϣ
        pageList[i].pages = -1;
    for (current_page = 0; current_page < pInfo.visit_list_length; current_page++){ // �������еķ���ҳ��
        if (!isExist()){ // ҳ��������װ��ҳ��
            if (full < pInfo.allocated_page_num){                                                                 // ���Ѿ�ռ�õ�ҳ���� < ����Ŀ���ʱ���򲻿��ܷ���ȱҳ
                pageList[replace_page].pages = pInfo.pages[current_page];     // ��ǰλ�÷����µ�ҳ��
                replace_page = (replace_page + 1) % pInfo.allocated_page_num; // ѭ������һλ
                full++;                                                       // �Ѿ�װ���ҳ����+1
            }else{                     // ҳ���Ѿ�����ȱҳ�û�
                int min, max = 0; // min ������¼ҳ����ֵ�����ʱ�䣬max ������¼��Զ��ҳ��
                for (int m = 0; m < pInfo.allocated_page_num; m++){ // ������ǰ�ڴ����ѷ����ҳ��
                    min = 1000;
                    for (int i = current_page; i < pInfo.visit_list_length; i++){ // �����ҷ�������
                        if (pInfo.pages[i] == pageList[m].pages){// ������������е�ĳ��ҳ����ڵ�ǰҳ��
                            min = i; // ��¼��ҳ����һ�γ����ڷ��������е�λ��
                            break;   // �ҵ�֮���������ѭ��
                        }
                    }
                    if (max < min){ // �����ǰҳ���´γ��ֵ�ʱ���Ŀǰ��Զ��ҳ�����ʱ���Զ
                        max = min;        // ������Զʱ��Ϊ��ǰҳ��ĳ���ʱ��
                        replace_page = m; // ����ǰҳ����ΪҪ���滻��ҳ��
                    }
                }
                pageList[replace_page].pages = pInfo.pages[current_page]; // �û�����
                replace_page = (replace_page + 1) % pInfo.allocated_page_num;
                pInfo.page_missing_num++; // �û�������1
            }
        }
        showState();
    }	return;
}

// �Ƚ��ȳ�����̭�㷨
void FIFO(){
    pInfo.page_missing_num = 0;
    int full, status;
    replace_page = 0;                                  // ҳ���滻ָ���ʼ��Ϊ0
    full = 0;                                          // �Ƿ�װ�������е�ҳ��
    for (int i = 0; i < pInfo.allocated_page_num; i++) // ���ҳ����Ϣ
        pageList[i].pages = -1;
    isLoss = 0; // ȱҳ��־��0Ϊ��ȱҳ��1Ϊȱҳ
    for (current_page = 0; current_page < pInfo.visit_list_length; current_page++){
        // status = isExist(); // ����ҳ���Ƿ����ڴ�
        if (!isExist()){
            if (full < pInfo.allocated_page_num){ // װ��ҳ��
                pageList[replace_page].pages = pInfo.pages[current_page];
                replace_page = (replace_page + 1) % pInfo.allocated_page_num;
                full++;
            }else{ // ֱ���û�
                pageList[replace_page].pages = pInfo.pages[current_page];
                replace_page = (replace_page + 1) % pInfo.allocated_page_num;
                pInfo.page_missing_num++; // ȱҳ������1
            }
        }
        showState(); // ��ʾ��ǰ״̬
    }
    return;
}

// ������δʹ����̭�㷨
void LRU(void){
    pInfo.page_missing_num = 0;
    int full, status, max;
    replace_page = 0; // ҳ���滻ָ��
    full = 0;         // �Ƿ�װ�����е�ҳ��
    for (int i = 0; i < pInfo.allocated_page_num; i++){
        pageList[i].pages = -1; // ���ҳ����Ϣ
        pageList[i].time = 0;   // ���ҳ����ʷ
    }
    isLoss = 0; // ȱҳ��־��0Ϊ��ȱҳ��1Ϊȱҳ
    for (current_page = 0; current_page < pInfo.visit_list_length; current_page++){
        if (!isExist()){
            if (full < pInfo.allocated_page_num){                                                             // ��ʼʱ������ȱҳ, ҳ��������װ��ҳ��
                pageList[replace_page].pages = pInfo.pages[current_page]; 
                replace_page = (replace_page + 1) % pInfo.allocated_page_num;
                full++;
            }else{ // ����ȱҳ�û�, ҳ���������û�ҳ��
                max = 0;
                for (int i = 1; i < pInfo.allocated_page_num; i++)
                    if (pageList[i].time > pageList[max].time) max = i;           
                replace_page = max;
                pageList[replace_page].pages = pInfo.pages[current_page];
                pageList[replace_page].time = 0;
                pInfo.page_missing_num++; // ȱҳ������1
            }
        }
        showState(); // ��ʾ��ǰ״̬
    }
    return;
}

// Clock��̭�㷨 : 0 �� 1 �Ľ�
// ���㷨�û�����
int replace_page_clock(int num)
{
    int j;
    // ��һ�֣� �����λ����Ϊ0
    for (j = 0; j < pInfo.allocated_page_num; j++)
    {
        if (pageList[(j + num) % pInfo.allocated_page_num].isVisit == 0)
            return (j + num) % pInfo.allocated_page_num;
        pageList[(j + num) % pInfo.allocated_page_num].isVisit = 0;
    }
    // �ڶ��֣������λ
    for (j = 0; j < pInfo.allocated_page_num; j++)
        if (pageList[(j + num) % pInfo.allocated_page_num].isVisit == 0)
            return (j + num) % pInfo.allocated_page_num;
    return 0;
}
// �Ľ��㷨�û�����
int replace_page_pro(int num){
	int len = pInfo.allocated_page_num;
    // ��һ�֣��ң�0,0��
    for (int j = 0; j < len; j++)
        if (pageList[(j + num) % len].isVisit == 0 && pageList[(j + num) % len].isModify == 0)
            return (j + num) % len;
    // �ڶ��֣��ң�0,1��������λȫ��Ϊ0
    for (int j = 0; j < len; j++)
    {
        if (pageList[(j + num) % len].isVisit == 0 && pageList[(j + num) % len].isModify == 1)
            return (j + num) % len;
        pageList[(j + num) % len].isVisit = 0;
    }
    // �����֣��ң�0,0��
    for (int j = 0; j < len; j++)
        if (pageList[(j + num) % len].isVisit == 0 && pageList[(j + num) % len].isModify == 0)
            return (j + num) % len;
    // �����֣��ң�0,1��
    for (int j = 0; j < len; j++)
        if (pageList[(j + num) % len].isVisit == 0 && pageList[(j + num) % len].isModify == 1)
            return (j + num) % len;
    return 0;
}
void CLOCK_pro(int choose)
{
    pInfo.page_missing_num = 0;
    int n, full, status;
    int num = -1;
    replace_page = 0; // ҳ���滻ָ���ʼ��Ϊ0
    full = 0;         // �Ƿ�װ�������е�ҳ��
    for (n = 0; n < pInfo.allocated_page_num; n++){ // ���ҳ����Ϣ
        pageList[n].pages = -1;
        pageList[n].isModify = 0;
        pageList[n].isVisit = 0;
        pageList[n].time = 0;
    }
    isLoss = 0; // ȱҳ��־��0Ϊ��ȱҳ��1Ϊȱҳ
    for (current_page = 0; current_page < pInfo.visit_list_length; current_page++){ // ִ���㷨
        if (!isExist()){
            if (full < pInfo.allocated_page_num){ // ��ʼʱ������ȱҳ
                // ҳ��������װ��ҳ��
                pageList[replace_page].pages = pInfo.pages[current_page];
                replace_page = (replace_page + 1) % pInfo.allocated_page_num;
                pageList[n].isVisit = 1;
                full++;
            }else{   // ����ȱҳ�û�, ҳ���������û�ҳ��
                if (choose == 1)
                    replace_page = replace_page_pro(++num); // ��choose = 1ʱ���øĽ���clock�㷨
                else if (choose == 0)
                    replace_page = replace_page_clock(++num); // ��choose = 0ʱ���û�����clock�㷨
                pageList[replace_page].pages = pInfo.pages[current_page];
                pageList[replace_page].isVisit = 1;
                pInfo.page_missing_num++; // ȱҳ������1
            }
        }
        showState(); // ��ʾ��ǰ״̬
    } // �û��㷨ѭ������
    return;
}

int main()
{
    char ch;
    Init();
    RandomSequence();
    clock_t start, finish;
    double totaltime;
    while (1)
    {
        printf("=======================�˵�=======================\n");
        printf("1. �����̭�㷨(OPT)\n");
        printf("2. �Ƚ��ȳ���̭�㷨(FIFO)\n");
        printf("3. ������δʹ����̭�㷨(LRU)\n");
        printf("4. �� Clock ��̭�㷨\n");
        printf("5. �Ľ��� Clock ��̭�㷨\n");
        printf("0. �˳��˵�\n");
        printf("=================================================\n");
        printf("������ѡ����㷨: ");
        ch = getchar();
        switch (ch)
        {
        case '1':
            printf("\n��------------1. �����̭�㷨(OPT)--------------��\n");
            start = clock();
            OPT();
            finish = clock();
            totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
            printf("\n������ʱ�� = %lf ��\n", totaltime);
            getchar();
            break;
        case '2':
            printf("\n\n��----------2. �Ƚ��ȳ���̭�㷨(FIFO)------------��\n");
            start = clock();
            FIFO();
            finish = clock();
            totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
            printf("\n������ʱ�� = %lf ��\n", totaltime);
            getchar();
            break;
        case '3':
            printf("\n\n��--------3. ������δʹ����̭�㷨(LRU)-----------��\n");
            start = clock();
            LRU();
            finish = clock();
            totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
            printf("\n������ʱ�� = %lf ��\n", totaltime);
            getchar();
            break;
        case '4':
            printf("\n\n��------------ 4. �� Clock ��̭�㷨--------------��\n");
            start = clock();
            CLOCK_pro(0);
            finish = clock();
            totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
            printf("\n������ʱ�� = %lf ��\n", totaltime);
            getchar();
            break;
        case '5':
            printf("\n\n��------------ 5. �Ľ���Clock ��̭�㷨--------------��\n");
            start = clock();
            CLOCK_pro(1);
            finish = clock();
            totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
            printf("\n������ʱ�� = %lf ��\n", totaltime);
            getchar();
            break;
        case '0':
            printf("���˳���\n");
            return 0;
        default:
            printf("����������⣬���������룡\n");
            break;
        }
    }
    return 0;
}
