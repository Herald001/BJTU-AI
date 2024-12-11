#include<iostream>
#include<mutex>
#include<chrono>
#include<thread>
using namespace std;

int n = 10;	// ������������С
int produce[4] = { 0,0,0,0 },consume[4] = { 0,0,0,0 }; // ����ָ��,����ָ��
int count[3] = { 0,0,0 }; // �ֱ��Ӧ��������������ŵ����ݸ�������ʼʱ��Ϊ0
int buffer[3][10]; // 3�����10�����ݵĻ�����
mutex mtx[3]; // ������������
mutex display_mtx;	// ���������

//   �����ߺ��� 
void producer(int id)
{
    while(1){
        for (int i = 0; i < 3; i++) // ���α�������������
        {
        if (mtx[i].try_lock()) 
            {
            if (count[i]!=n) 
                {
                buffer[i][produce[i]] = 1;
                display_mtx.lock(); 
                // ���������ڽ���������Ϊ 
                cout << "��Producer" << id << "��:produce {product" ;
                cout << produce[i] <<"}  at  [Buffer"<<i+1 <<"]:";
                // ��ӡ������״�� 
                for (int j = 0; j < n; j++){ cout << buffer[i][j] << " "; }
                cout << endl;
                display_mtx.unlock();
                produce[i] = (produce[i] + 1) % n; 
                count[i]++; 
                mtx[i].unlock(); 
                this_thread::sleep_for(chrono::seconds(1)); 
                break;
                }
            else {mtx[i].unlock(); }
            }
        }
    }
}
// �����ߺ��� 
void consumer(int id)
{
    while(1){
        for (int i = 0; i < 3; i++) 
        {
        if (mtx[i].try_lock()) 
            {
            if (count[i] != 0) 
                {// ���������ڽ���������Ϊ
                buffer[i][consume[i]] = 0; 
                display_mtx.lock();
                cout << "��Consumer" << id << "��:consume {product" ;
			cout << consume[i] <<"} from [Buffer"<<i+1 <<"]:";
			// ��ӡ������״�� 
                for (int j = 0; j < n; j++){cout << buffer[i][j] << " "; }
                cout << endl;
                display_mtx.unlock(); 
                consume[i] = (consume[i] + 1) % n;
                count[i]--;
                mtx[i].unlock(); 

                this_thread::sleep_for(chrono::seconds(5));
                break;
                }
            else {mtx[i].unlock();}}
        }
    };
}

int main() {
	cout << "============================================"<<endl; 
	cout << "���� ������Ashyfox "<< endl;
	cout << "��ѧ �š���22231044" << endl;
	cout << "��OSLAB����������-����������ģ�⴦�������"<<endl;
	cout << "============================================"<<endl; 
    //���������ߺ�������
    thread t1(producer, 1); 
    thread t2(consumer, 1); 
    thread t3(producer, 2);
    thread t4(consumer, 2);
    thread t5(producer, 3);
    thread t6(consumer, 3);
    thread t7(producer, 4);
    thread t8(consumer, 4); 
	//�߳̿�ʼ����
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    return 0;
}

