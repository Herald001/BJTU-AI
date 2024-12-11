#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
using namespace std;

const int DISK_SIZE = 1048576; // 1 MB
const int dirtable_max_size = 80;
const int fatnum = 80;
char Disk[DISK_SIZE];
int dbr[2];
int startindex;

struct fcb {
    short blockindex; // FAT�еĿ�����
    short filesize;   // �ļ��ܴ�С
    short datasize;   // ��ǰ���ݴ�С
};

struct dirunit {
    char filename[80];
    char type;         // 0: Ŀ¼, 1: �ļ�
    short startfat;
    short startfcb;
    short startdir;
};

struct dirtable {
    int dirunitnum;   // Ŀ¼��Ԫ����
    dirunit dirs[dirtable_max_size];
};

short fat[fatnum];          // FAT ��
// free: 0, last: -1, next: index

dirtable* table[10];        // Ŀ¼��
fcb* FCB[fatnum];           // FCB ��

dirtable rootdirtable;      // ��Ŀ¼��
dirtable* currentdirtable;  // ��ǰĿ¼��
char path[100] = "root\\";
int FCBrecord = 0; // ��ǰ������ FCB ����
int TABrecord = 0; // ��ǰ������Ŀ¼������

// ����ԭ��
void init();
void createfile(char filename[], int filesize, int type);
void adddirunit(char fileName[], int type);
int findunit(char filename[]);
void ls();
void deletefile(char filename[]);
void deleteunit(int unitindex);
short findfreefat();
void freefat(char filename[]);
void changedir(char dirname[]);
void read(char filename[], int length);
void write(char filename[], char content[]);
void adjustfat(short num, int unitindex);
void formatdisk();
void savetoimage(const char* imagefile);
void deletedir(char dirname[]);
void renamedir(char oldname[], char newname[]);

void init() {
    memset(Disk, 0, sizeof(Disk));
    memset(fat, 0, sizeof(fat));
    rootdirtable.dirunitnum = 0;
    currentdirtable = &rootdirtable;
    dbr[0] = fatnum;
    dbr[1] = dirtable_max_size;
    startindex = 8 + fatnum; // �������ݿ����ʼ����
    strcpy(path, "root\\");
    cout << "���̳�ʼ����ɡ�" << endl;
}

void savetoimage(const char* imagefile) {
    ofstream file(imagefile, ios::binary);
    if (!file) {
        cerr << "�޷���������ӳ���ļ���" << endl;
        return;
    }

    // ��һ����д�� DBR������������
    char dbr[512] = {0}; // ��ʼ��Ϊ��
    dbr[0] = 0xEB;       // ��תָ��
    dbr[1] = 0x3C;
    dbr[2] = 0x90;
    memcpy(dbr + 3, "FAT16   ", 8); // OEM ����

    // ���� BIOS �����飨BPB��
    *((short *)(dbr + 11)) = 512;      // ÿ�����ֽ���
    dbr[13] = 1;                       // ÿ��������
    *((short *)(dbr + 14)) = 1;        // ����������
    dbr[16] = 2;                       // FAT �����
    *((short *)(dbr + 17)) = 224;      // ��Ŀ¼�����Ŀ��
    *((short *)(dbr + 19)) = 2880;     // ������������׼�� 1.44MB ���̣�
    dbr[21] = 0xF8;                    // ý��������
    *((short *)(dbr + 22)) = 9;        // ÿ�� FAT ���������

    // �ļ�ϵͳǩ��
    memcpy(dbr + 510, "\x55\xAA", 2); // ��������ǩ��

    file.write(dbr, 512); // д����������

    // �ڶ�����д�� FAT ��
    char fat[512 * 9] = {0}; // 9 ���������� FAT ��
    fat[0] = 0xF8;           // ý��������
    fat[1] = 0xFF;           // ������
    fat[2] = 0xFF;           // ������

    // д������ FAT ����
    file.write(fat, sizeof(fat)); // ��һ�� FAT
    file.write(fat, sizeof(fat)); // �ڶ��� FAT

    // ��������д���Ŀ¼��
    int root_dir_size = currentdirtable->dirunitnum * 32;
    char* root_dir = new char[root_dir_size];
    memset(root_dir, 0, root_dir_size); // ��ʼ����Ŀ¼��

    for (int i = 0; i < currentdirtable->dirunitnum; ++i) {
        const dirunit& dir = currentdirtable->dirs[i];
        char entry[32] = {0}; // ÿ����Ŀ 32 �ֽ�

        // �����ļ��������Ϊ 11 ���ַ���
        strncpy(entry, dir.filename, min(11, (int)strlen(dir.filename)));

        entry[11] = dir.type == 0 ? 0x10 : 0x20; // ���ԣ�0x10 ��ʾĿ¼��0x20 ��ʾ�ļ�
        *((short *)(entry + 26)) = dir.startfat; // ��ʼ��
        *((int *)(entry + 28)) = dir.startfcb;  // �ļ���С���򻯣�

        // д���Ŀ¼��Ŀ
        memcpy(root_dir + i * 32, entry, 32);
    }

    file.write(root_dir, root_dir_size); // д���Ŀ¼��
    delete[] root_dir;  // �ͷ��ڴ�

    // ���Ĳ���д��������
    file.write(Disk, sizeof(Disk)); // д�����ݿ�

    file.close();
    cout << "����ӳ���ѱ��浽 " << imagefile << endl;
}

void createfile(char filename[], int filesize, int type) {
	if (strlen(filename) > 80) {
		cout << "�ļ�������" << endl;
		return;
	}
	// ���Ŀ¼��Ԫ
	adddirunit(filename, type);
	int index = findunit(filename);
	// ���� FCB
	fcb* curfcb = (fcb*)new fcb();
	curfcb->blockindex = startindex++;
	curfcb->filesize = filesize;
	curfcb->datasize = 0; // �ļ�Ϊ��
	FCB[FCBrecord] = curfcb;
	currentdirtable->dirs[index].startfcb = FCBrecord;
	// ���� FAT
	int i = findfreefat();
	currentdirtable->dirs[index].startfat = i;
	fat[i] = -1;
	// ����Ŀ¼
	if (type == 0) {
		dirtable* curdirtable = (dirtable*)new dirtable();
		table[TABrecord] = curdirtable;
		curdirtable->dirunitnum = 0;
		currentdirtable->dirs[index].startdir = TABrecord;
		cout << "Ŀ¼�����ɹ�: " << filename << endl;
		return;
	}
	cout << "�ļ������ɹ�: " << filename << endl;
}

void adddirunit(char filename[], int type) {
	int dirunitnum = currentdirtable->dirunitnum;
	// �Ƿ�����
	if (dirunitnum == dirtable_max_size) {
		cout << "Ŀ¼����������ɾ��һЩ�ļ�������" << endl;
		return;
	}

	// �Ƿ��Ѵ���
	if (findunit(filename) != -1) {
		printf("�ļ��Ѵ���\n");
		return;
	}
	// �����µ�Ŀ¼��Ԫ
	dirunit* newdirunit = &currentdirtable->dirs[dirunitnum];
	currentdirtable->dirunitnum++;
	int i = strlen(filename);
	while (i--)
		newdirunit->filename[i] = filename[i];
	newdirunit->type = type;
	cout << "Ŀ¼��Ԫ��ӳɹ�: " << filename << endl;
	return;
}

int findunit(char filename[]) {
	// ˳�����
	int dirunitnum = currentdirtable->dirunitnum;
	int unitIndex = -1;
	for (int i = 0; i < dirunitnum; i++)
		if (strcmp(filename, currentdirtable->dirs[i].filename) == 0)
			unitIndex = i;
	return unitIndex;
}

void ls() {
	int uninum = currentdirtable->dirunitnum;
	for (int i = 0; i < uninum; i++) {
		dirunit curunit = currentdirtable->dirs[i];
		cout << curunit.filename << " ";
	}
	cout << endl;
	cout << "Ŀ¼�б���ʾ���" << endl;
}

void deletefile(char filename[]) {
	int unitindex = findunit(filename);
	if (unitindex == -1) {
		cout << "�ļ�δ�ҵ�" << endl;
		return;
	}
	// ɾ��Ŀ¼���еĵ�Ԫ
	deleteunit(unitindex);
	freefat(filename);
	cout << "�ļ�ɾ���ɹ�: " << filename << endl;
}

void deleteunit(int unitindex) {
	// ����һ����Ԫ����
	int dirunitnum = currentdirtable->dirunitnum;
	for (int i = unitindex; i < dirunitnum - 1; i++) {
		currentdirtable->dirs[i] = currentdirtable->dirs[i + 1];
	}
	currentdirtable->dirunitnum--;
}

short findfreefat() {
	for (short i = 0; i < fatnum; i++) {
		if (fat[i] == 0) return i;
	}
	return -1; 
}

void freefat(char filename[]) {
	// ���� FAT �����ͷ�
	int i = currentdirtable->dirs[findunit(filename)].startfat;
	if (i == -1) {
		fat[i] = 0;
		return;
	}
	while (i == -1) {
		int temp = i;
		i = fat[i];
		fat[temp] = 0;
	}
	if (i == -1) {
		fat[i] = 0;
		return;
	}
	cout << "FAT�ͷųɹ�: " << filename << endl;
}

void changedir(char dirname[]) {
	// ���Ŀ¼���Ƿ���Ч
	int unitindex = findunit(dirname);
	if (unitindex == -1) {
		cout << "Ŀ¼δ�ҵ�" << endl;
		return;
	}
	if (currentdirtable->dirs[unitindex].type == 1) {
		cout << "����Ŀ¼" << endl;
		return;
	}
	// �л���ǰĿ¼
	int i = currentdirtable->dirs[unitindex].startdir;
	currentdirtable = table[i];
	// �޸�·��
	if (strcmp(dirname, "..") == 0) {
		// �����ϼ�Ŀ¼
		int length = strlen(path);
		for (int i = length - 2; i >= 0; i--) {
			if (path[i] == '\\') {
				path[i + 1] = '\0';
				break;
			}
		}
	}
	else {
		strcat(path, dirname);
		strcat(path, "\\");
	}
	cout << "�л���Ŀ¼ (" << dirname << ") �ɹ�" << endl;
}

void read(char filename[], int length) {
	int unitindex = findunit(filename);
	if (unitindex == -1) {
		cout << "sorry,not found" << endl;
		return;
	}

	//read the data of given length 
	int index = currentdirtable->dirs[unitindex].startfcb;
	fcb* myfcb = FCB[index];
	for (int i = 0; i < length; i++) {
		cout << Disk[i + myfcb->blockindex];
	}
	cout << endl;
	cout << "�ļ����ݶ�ȡ��ɣ�" << endl; 
}

void write(char filename[], char content[]) {
	int unitindex = findunit(filename);
	if (unitindex == -1) {
		cout << "sorry,not found" << endl;
		return;
	}
	int length = sizeof(content);
	//how many clusters need
	int num = (length % 32 == 0) ? length / 32 : length / 32 + 1;
	adjustfat(num, unitindex);

	//renew the filesize
	FCB[currentdirtable->dirs[unitindex].startfcb]->filesize = num;

	//get the start index and write the content in order
	int index = currentdirtable->dirs[unitindex].startfcb;
	fcb* myfcb = FCB[index];
	for (int i = 0; i < length; i++) {
		Disk[i + myfcb->blockindex] = content[i];
	}
	cout << endl;
	cout << "�ļ�����д����ɣ�" << endl; 
}

void adjustfat(short num, int unitindex) {
	int index = currentdirtable->dirs[unitindex].startfat;
	for (int i = 0; i < num - 1; i++) {
		short j = findfreefat();
		fat[index] = j;
		index = j;
	}
	fat[index] = -1;
}

void deletedir(char dirname[]) {
    int index = findunit(dirname);
    if (index == -1) {
        cout << "Ŀ¼δ�ҵ�." << endl;
        return;
    }
    if (currentdirtable->dirs[index].type != 0) {
        cout << "����һ��Ŀ¼." << endl;
        return;
    }
    deleteunit(index);
    cout << "Ŀ¼ " << dirname << " ��ɾ��." << endl;
}
void renamedir(char oldname[], char newname[]) {
	// ����Ŀ¼��������
	int unitindex = findunit(oldname);
	if (unitindex == -1) {
		cout << "Ŀ¼δ�ҵ�" << endl;
		return;
	}
	if (currentdirtable->dirs[unitindex].type == 1) {
		cout << "�����ļ�������������ΪĿ¼" << endl;
	}
	int i = 0;
	while (i < 80) {
		currentdirtable->dirs[unitindex].filename[i] = newname[i];
		if (newname[i] == '\0') break;
		i++;
	}
	cout << "Ŀ¼�������ɹ�: " << oldname << " -> " << newname << endl;
}


int main() {
    init();
    int choice;
    char name[80] = {0};
    char newname[80] = {0};
    char content[100] = {0};
    int length = 0;
	printf("========================================================================================\n");
    printf("|                          \033[1;36mʵ�������-FAT�ļ�ϵͳģ�������ʵ��\033[0m                         |\n");
    printf("|---------------------------------------------------------------------------------------|\n");
    printf("|                          \033[1;32m������Ϣ������-Ashyfox ѧ��-22231044\033[0m                          |\n");
    printf("|---------------------------------------------------------------------------------------|\n");
    printf("|==================================== FAT16 �ļ�ϵͳ ===================================|\n");
    printf("| 1. ��ʽ������ | 12. �������ӳ��  | 0. �˳�                                           |\n");
    printf("|---------------------------------------------------------------------------------------|\n");
    printf("| Ŀ¼����                                                                            |\n");
    printf("| 2. ����Ŀ¼   | 3. �л�Ŀ¼       | 4. ������Ŀ¼   | 5. ��ʾĿ¼  | 6. ɾ��Ŀ¼      |\n");
    printf("|---------------------------------------------------------------------------------------|\n");
    printf("| �ļ�����                                                                            |\n");
    printf("| 7. �����ļ�   | 8. �������ļ�     | 9. д���ļ�     | 10. ��ʾ�ļ�  | 11. ɾ���ļ�    |\n");
    printf("========================================================================================\n");
    while (true) {
    	printf("------------------------------------------------------\n");
        printf("��ǰ���̣�%s   ��ǰ·����%s\n","DHHOS", path);        
        printf("��ѡ�����: ");
        scanf("%d", &choice);
        getchar(); // Clear input buffer

        switch (choice) {
            case 0:
                printf("�˳�ϵͳ����лʹ�ã�\n");
                return 0;
            case 1:
                init();
                break;
            case 2:
                printf("����Ŀ¼��: ");
                scanf("%s", name);
                createfile(name, 1, 0);
                break;
            case 3:
                printf("����Ŀ¼��: ");
                scanf("%s", name);
                changedir(name);
                break;
            case 4:
                printf("����ԭĿ¼��: ");
                scanf("%s", name);
                printf("������Ŀ¼��: ");
                scanf("%s", newname);
                renamedir(name, newname);
                break;
            case 5:
                ls();
                break;
            case 6:
                printf("����Ŀ¼��: ");
                scanf("%s", name);
                deletedir(name);
                break;
            case 7:
                printf("�����ļ���: ");
                scanf("%s", name);
                createfile(name, 1, 1);
                break;
            case 9:
                printf("�����ļ���: ");
                scanf("%s", name);
                printf("��������: ");
                scanf("%s", content);
                write(name, content);
                break;
            case 10:
                printf("�����ļ���: ");
                scanf("%s", name);
                printf("�����ȡ����: ");
                scanf("%d", &length);
                read(name, length);
                break;
            case 11:
                printf("�����ļ���: ");
                scanf("%s", name);
                deletefile(name);
                break;
            case 12:
                savetoimage("mydisk.img");
                break;
            default:
                printf("��Чѡ�������ѡ��\n");
        }
    }
}






