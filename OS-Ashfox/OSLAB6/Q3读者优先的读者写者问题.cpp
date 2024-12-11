#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define NUM_READERS 3   //���߽�������
#define NUM_WRITERS 2   //д�߽�������

static int data = 0;   //��ʼ���ٽ绺����Ϊ��
static int readers_count = 0;  //��¼���߽�������
static HANDLE mutex;   //����ʵ�ֶ��߽��̶���readers_count�����Ļ������
static HANDLE rw_semaphore;    //����ʵ�ֶ��ߺ�д�߻�������ٽ绺����

static DWORD WINAPI Reader(LPVOID lpParam) {
    int reader_id = *(int*)lpParam;

    while (1) {
        WaitForSingleObject(mutex, INFINITE);
        readers_count++;
        if (readers_count == 1) {
            WaitForSingleObject(rw_semaphore, INFINITE);
        }
        ReleaseMutex(mutex);

        printf("������%d�������Ķ�: %d\n", reader_id, data);
        Sleep(1000);  // ģ���ȡ���ݵĺ�ʱ

        WaitForSingleObject(mutex, INFINITE);
        readers_count--;
        if (readers_count == 0) {
            ReleaseSemaphore(rw_semaphore, 1, NULL);
        }
        ReleaseMutex(mutex);

        Sleep(2000);  // ������Ϣһ��ʱ����ٴζ�ȡ����
    }

    return 0;
}

DWORD WINAPI Writer(LPVOID lpParam) {
    int writer_id = *(int*)lpParam;

    while (1) {
        WaitForSingleObject(rw_semaphore, INFINITE);

        data++;  // д������
        printf("��д��%d������д��: %d\n", writer_id, data);
        Sleep(2000);  // ģ��д�����ݵĺ�ʱ

        ReleaseSemaphore(rw_semaphore, 1, NULL);

        Sleep(3000);  // д����Ϣһ��ʱ����ٴ�д������
    }

    return 0;
}

int main() {
	printf("==================================================\n"); 
    printf("���� ������Ashyfox \n");
    printf("��ѧ �š���22231044\n");
    printf("��OSLAB�����������ȵĶ���-д������ģ�⴦�������\n");
    printf("==================================================\n"); 

    // �����������Ͷ�д�ź���
    mutex = CreateMutex(NULL, FALSE, NULL);
    rw_semaphore = CreateSemaphore(NULL, 1, 1, NULL);

    // ���������߳�
    HANDLE reader_threads[NUM_READERS];
    int reader_ids[NUM_READERS];
    for (int i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        reader_threads[i] = CreateThread(NULL, 0, Reader, &reader_ids[i], 0, NULL);
    }

    // ����д���߳�
    HANDLE writer_threads[NUM_WRITERS];
    int writer_ids[NUM_WRITERS];
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        writer_threads[i] = CreateThread(NULL, 0, Writer, &writer_ids[i], 0, NULL);
    }

    // �ȴ����ж����̺߳�д���߳̽���
    WaitForMultipleObjects(NUM_READERS, reader_threads, TRUE, INFINITE);
    WaitForMultipleObjects(NUM_WRITERS, writer_threads, TRUE, INFINITE);

    // �رվ��
    CloseHandle(mutex);
    CloseHandle(rw_semaphore);

    return 0;
}


