#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

mutex chopsticks[5]; // �������
mutex cout_mutex; // ���������
int state[5]; // ��ѧ�ҵ�״̬��0 ��ʾ˼����1 ��ʾ������2 ��ʾ�Ͳ�

// ��ѧ���̺߳���������Ϊ��ѧ�ҵı��
void philosopher(int num) {
    int left_chopstick = num; // ��ߵĿ��ӱ��
    int right_chopstick = (num + 1) % 5; // �ұߵĿ��ӱ��

    while (true) {
        // ˼��
        state[num] = 0;
        cout_mutex.lock();
        cout << "����ѧ��" << num << "������˼��" << endl;
        cout_mutex.unlock();
        this_thread::sleep_for(chrono::milliseconds(500));
        // ����
        state[num] = 1;
        cout_mutex.lock();
        cout << "����ѧ��" << num << "���е�����" << endl;
        cout_mutex.unlock();
        // ���Ի�ȡ���� 
        if (num % 2 == 0) {
            // ż������ѧ�ң������ҿ��ӣ����������
            chopsticks[right_chopstick].lock();
            cout_mutex.lock();
            cout << "����ѧ��" << num << "������[�ұ�]����" << right_chopstick << endl;
            cout_mutex.unlock();
            chopsticks[left_chopstick].lock();
            cout_mutex.lock();
            cout << "����ѧ��" << num << "������[���]����" << left_chopstick << endl;
            cout_mutex.unlock();
        } else {
            // ��������ѧ�ң���������ӣ������ҿ���
            chopsticks[left_chopstick].lock();
            cout_mutex.lock();
            cout << "����ѧ��" << num << "������[���]����" << left_chopstick << endl;
            cout_mutex.unlock();
            chopsticks[right_chopstick].lock();
            cout_mutex.lock();
            cout << "����ѧ��" << num << "������[�ұ�]����" << right_chopstick << endl;
            cout_mutex.unlock();
        }
        // �Ͳ�
        state[num] = 2;
        cout_mutex.lock();
        cout << "����ѧ��" << num << "���ڳ�'������Ѽ'" << endl;
        cout_mutex.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
        // ������ߵĿ���
        chopsticks[left_chopstick].unlock();
        cout_mutex.lock();
        cout << "����ѧ��" << num << "������[���]����" << left_chopstick << endl;
        cout_mutex.unlock();
        // �����ұߵĿ���
        chopsticks[right_chopstick].unlock();
        cout_mutex.lock();
        cout << "����ѧ��" << num << "������[�ұ�]����" << right_chopstick << endl;
        cout_mutex.unlock();
    }
}

int main() {
    cout << "============================================"<<endl; 
    cout << "���� ������Ashyfox "<< endl;
    cout << "��ѧ �š���22231044" << endl;
    cout << "��OSLAB������ѧ�ҾͲ�����ģ�⴦�������"<<endl;
    cout << "============================================"<<endl; 

    // ��ʼ����ѧ��״̬
    for (int i = 0; i < 5; i++) {
        state[i] = 0;
    }

    // ���������ѧ���߳�
    thread t0(philosopher, 0);
    thread t1(philosopher, 1);
    thread t2(philosopher, 2);
    thread t3(philosopher, 3);
    thread t4(philosopher, 4);

    // �ȴ������ѧ���߳̽���
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}

