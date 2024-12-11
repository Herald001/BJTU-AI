#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <numeric>  // �����ۼ�
#define _CRT_SECURE_NO_WARNINGS 1
using namespace std;

const int maxRightValue = 199; // ��ͷ�ƶ������ұ߽�ֵ
const int minLeftValue = 0;    // ��ͷ�ƶ�������߽�ֵ
vector<int> FSCAN_SHOW;        // ���FSCAN_SHOW�㷨�ķ���˳��

// ����100������������У�posΪ��ͷ��ʼλ��
void getRandomNumber(vector<int>& randValue, int& pos) {
    randValue.clear();  // ���֮ǰ�����ֵ
    for (int i = 1; i <= 100; i++) {
        int r = rand() % 200;  // ����һ�����������Χ��0-199
        randValue.push_back(r); // ���������ӵ�������
    }
    pos = rand() % 200; // ��ͷ�����ʼλ��
}

// ��ӡ���ɵ��������
void printRandomSequence(const vector<int>& randValue) {
    printf("---------------����������������£�---------------\n");
    for (int i = 0; i < randValue.size(); i++) {
        printf("%-3d ", randValue[i]);
        if ((i + 1) % 10 == 0) {  // ÿ10�����ֻ���
            cout << "\n";
        }
    }
    cout << "\n";
}

// ��ӡ��������
void Show(const string& str, const vector<int>& t) {
    cout << str << "�ķ�������Ϊ:";
    for (int i = 0; i < t.size(); i++) {
        if (i != t.size() - 1)
            cout << t[i] << "->";
        else
            cout << t[i] << endl;
    }
}

// FCFS�㷨�������ȷ���
int FCFS(const vector<int>& t, int pos, bool printFlag = true) {
    int sum = 0;
    if (printFlag) {
        Show("FCFS", t);
    }
    for (int i = 0; i < t.size(); i++) {
        sum += abs(pos - t[i]);
        pos = t[i];
    }
    return sum;
}

// Ѱ�Ҿ��뵱ǰλ������Ĵŵ�
int findClose(const vector<int>& t, int pos) {
    int minDistance = INT_MAX;
    int index = -1;
    for (int i = 0; i < t.size(); i++) {
        if (t[i] == -1)
            continue;
        int distance = abs(pos - t[i]);
        if (minDistance > distance) {
            minDistance = distance;
            index = i;
        }
    }
    return index;
}

// SSTF�㷨�����Ѱ��ʱ������
int SSTF(vector<int> t, int pos, bool printFlag = true) {
    vector<int> show;
    int sum = 0;
    for (int i = 0; i < t.size(); i++) {
        int index = findClose(t, pos);
        if (index == -1) {
            break;
        } else {
            show.push_back(t[index]);
            sum += abs(pos - t[index]);
            pos = t[index];
            t[index] = -1;
        }
    }
    if (printFlag) {
        Show("SSTF", show);
    }
    return sum;
}

// �������ָ�����������ƶ���SCAN�㷨
pair<int, int> SCAN(vector<int> t, int pos, int flag = 1, bool printFlag = true) {
    int sum = 0;
    vector<int> left, right;
    vector<int> show;
    for (auto e : t) {
        if (e < pos) {
            left.push_back(e);
        } else {
            right.push_back(e);
        }
    }
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());
    for (auto e : right) {
        show.push_back(e);
    }
    sum += maxRightValue - pos;
    if (!left.empty()) {
        sum += maxRightValue - left[0];
        reverse(left.begin(), left.end());

        for (auto e : left) {
            show.push_back(e);
        }
    }
    if (printFlag) {
        Show("SCAN", show);
    } else {
        for (auto e : show) {
            FSCAN_SHOW.push_back(e);
        }
    }
    pair<int, int> res;
    res.first = sum;
    res.second = pos;
    return res;
}

// CSCAN�㷨��ѭ��ɨ��
int CSCAN(vector<int> t, int pos, bool printFlag = true) {
    int sum = 0;
    vector<int> left, right;
    vector<int> show;
    for (auto e : t) {
        if (e < pos) {
            left.push_back(e);
        } else {
            right.push_back(e);
        }
    }
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());
    for (auto e : right) {
        show.push_back(e);
    }
    sum += maxRightValue - pos + 200;
    if (!left.empty()) {
        for (auto e : left) {
            show.push_back(e);
        }
        sum += left.back();
    }
    if (printFlag) {
        Show("CSCAN", show);
    }
    return sum;
}

// FSCAN�㷨��˫����ɨ��
int FSCAN(vector<int> t, int pos, bool printFlag = true) {
    int sum = 0;
    vector<int> t1(t.begin(), t.begin() + 50); // ǰ50������
    vector<int> t2(t.begin() + 50, t.begin() + 100); // ��50������
    const pair<int, int>& temp = SCAN(t1, pos, 0, false);
    sum += temp.first;
    sum += SCAN(t2, temp.second, 0, false).first;
    if (printFlag) {
        Show("FSCAN", FSCAN_SHOW);
    }
    return sum;
}

// ��β��Բ�����ƽ��Ѱ����
void runTests(int numTests) {
    vector<int> FCFS_results, SSTF_results, SCAN_results, CSCAN_results, FSCAN_results;

    for (int i = 0; i < numTests; i++) {
        vector<int> randValue;
        int pos = 0;
        getRandomNumber(randValue, pos);
        FCFS_results.push_back(FCFS(randValue, pos, false));
        SSTF_results.push_back(SSTF(randValue, pos, false));
        SCAN_results.push_back(SCAN(randValue, pos, 1, false).first);
        CSCAN_results.push_back(CSCAN(randValue, pos, false));
        FSCAN_results.push_back(FSCAN(randValue, pos, false));
    }

    // ����ÿ���㷨��ƽ��Ѱ����
    double FCFS_avg = accumulate(FCFS_results.begin(), FCFS_results.end(), 0) / static_cast<double>(numTests);
    double SSTF_avg = accumulate(SSTF_results.begin(), SSTF_results.end(), 0) / static_cast<double>(numTests);
    double SCAN_avg = accumulate(SCAN_results.begin(), SCAN_results.end(), 0) / static_cast<double>(numTests);
    double CSCAN_avg = accumulate(CSCAN_results.begin(), CSCAN_results.end(), 0) / static_cast<double>(numTests);
    double FSCAN_avg = accumulate(FSCAN_results.begin(), FSCAN_results.end(), 0) / static_cast<double>(numTests);

    cout << "\n=================== ���Խ������ ===================\n";
    cout << "FCFS�㷨��ƽ��Ѱ����: " << FCFS_avg / 100 << endl;
    cout << "SSTF�㷨��ƽ��Ѱ����: " << SSTF_avg / 100 << endl;
    cout << "SCAN�㷨��ƽ��Ѱ����: " << SCAN_avg / 100 << endl;
    cout << "CSCAN�㷨��ƽ��Ѱ����: " << CSCAN_avg / 100 << endl;
    cout << "FSCAN�㷨��ƽ��Ѱ����: " << FSCAN_avg / 100 << endl;
}

int main() {
    printf("===================================================\n");
    printf("ʵ�������-�ƶ�ͷ���̵����㷨ģ��ʵ����Ƚ�\n");
    printf("������Ϣ������-Ashyfox ѧ��-22231044\n");
    printf("===================================================\n");

    int pos = 0;
    vector<int> randValue;
    srand((unsigned)time(NULL)); // �����������
    getRandomNumber(randValue, pos);
    printRandomSequence(randValue);

    printf("\n=====�ƶ�ͷ���̵����㷨ģ��ʵ�ֲ˵�=====\n");
    printf("1.�����ȷ����㷨(FCFS)\n");
    printf("2.���Ѱ��ʱ�������㷨(SSTF)\n");
    printf("3.ɨ���㷨(SCAN)\n");
    printf("4.ѭ��ɨ���㷨(CSCAN)\n");
    printf("5.˫��ɨ���㷨(FSCAN)\n");
    printf("0.�˳�\n");

    int numTests = 10000;
    while (1) {
        printf("���������ѡ��(1/2/3/4/5)��");
        int choice;
        cin >> choice;
        getchar(); // ��ȡ�����Ļ��з�

        switch (choice) {
            case 1:
                cout << "FCFS��ƽ��Ѱ����Ϊ:" << FCFS(randValue, pos) / 100 << endl;
                break;
            case 2:
                cout << "SSTF��ƽ��Ѱ����Ϊ:" << SSTF(randValue, pos) / 100 << endl;
                break;
            case 3:
                cout << "SCAN��ƽ��Ѱ����Ϊ:" << SCAN(randValue, pos).first / 100 << endl;
                break;
            case 4:
                cout << "CSCAN��ƽ��Ѱ����Ϊ:" << CSCAN(randValue, pos) / 100 << endl;
                break;
            case 5:
                cout << "FSCAN��ƽ��Ѱ����Ϊ:" << FSCAN(randValue, pos) / 100 << endl;
                break;
            case 0:
                printf("�����˳�\n");
                runTests(numTests); // �������ò��Ժ���
                return 0;
            default:
                printf("����ѡ������\n");
                break;
        }
    }
    return 0;
}

