#include<iostream>
using namespace std;

struct numNode {
	int num = 0;
	numNode* next = nullptr;
};

void Input(numNode &S)
{
	numNode* current = &S;
	int tempNum;
	cin >> S.num;

	if (S.num == -1) {
		current = nullptr;
	}
	else {
		while (1) {
			cin >> tempNum;
			if (tempNum == -1) {
				current->next = nullptr;
				break;
			}
			current->next = new numNode;
			current->next->num = tempNum;
			current = current->next;
			current->next = nullptr;
		}
	}//先判断next是否为-1，-1则将当前的next指针为空，不为-1则继续将current向下流动
	cin.clear();
	cin.ignore(1024, '\n');
}

numNode *Intersection(numNode &S1, numNode &S2)
{
	numNode* S3 = new numNode;
	numNode* front_current3 = S3, * current2 = &S2, * current1 = &S1;

	if (current1 == nullptr || current2 == nullptr) {
		return nullptr;
	}
	while (1) {
		if (current1->num == current2->num) {
			front_current3->next = new numNode;
			front_current3 = front_current3->next;
			front_current3->num = current1->num;
			/*
			此处为什么不统一命名？而是命名为front_的形式？
			原本“front_current3->next = new numNode;front_current3 = front_current3->next;”两句在赋值语句后，
			但是我们会发现编译器在我们刚赋值完一个S3中的结点后就立刻生成了它的next，并且因为我初始化的原因，它始终存在，无法消去。
			这就会导致我们的输出很头疼，结尾总是多带了一个0，虽然我们可以通过设置条件使“不输出”末尾的0，但这无法改变它仍然存在的事实
			聪明的你立刻想到可以让0出现在“开头”，这样我们返回时从其next开始，于是一个不含0的交集数列可以被正常返回并赋值（地址）
			PS:
			其实结尾也可以，但是需要遍历找到倒数第二个指针，使其next不再指向一个空结点而是nullptr，因此比较麻烦
			*/
			if (current1->next != nullptr && current2->next != nullptr) {
				current1 = current1->next;
				current2 = current2->next;
			}
			else {
				break;
			}
		}
		else if (current1->num > current2->num) {
			if (current2->next != nullptr) {
				current2 = current2->next;
			}
			else {			
				break;
			}
		}
		else {
			if (current1->next != nullptr) {
				current1 = current1->next;
			}
			else {
				break;
			}
		}
	}
	
	if (S3==nullptr) {
		return nullptr;
	}
	else {
		return S3->next;//上文已说明原因，此处不再赘述
	}
}

int main()
{
	numNode S1, S2;//两个非降序链表

	cout << "请输入两个非降序正整数数列(分两行，以-1结束): " << endl;
	Input(S1);
	Input(S2);
	numNode *S3 = Intersection(S1, S2);
	cout << "新链表为: " << endl;
	if (S3 == nullptr) {
		cout << "NULL" << endl;
	}
	else {
		numNode* current = S3;
		while (current != nullptr) {
			cout << current->num << ' ';
			current = current->next;
		}
	}

	numNode* del;
	while (S3 != nullptr) {
		del = S3;
		S3 = S3->next;
		delete del;
	}//释放空间

	return 0;
}