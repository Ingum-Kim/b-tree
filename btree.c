#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int degree;

typedef struct _node{
	int* key;
	struct _node** child;
	int size;
	int internal;
}Node;

Node* AllocNode(int* key, Node** child,int size,int internal){
	int i;

	Node* result = (Node*)malloc(sizeof(Node));
	result->key = (int*)malloc(sizeof(int) * (2*degree - 1));
	result->child = (Node**)malloc(sizeof(Node*)*(2*degree));
	
	for(i = 0; i < size; i++){
		result->key[i] = key[i];
	}

	for(i = 0; i < size + 1; i++)
		result->child[i] = child[i];
	for(i = size + 1;  i < 2*degree; i++)
		result->child[i] = NULL;

	result->size = size;
	result->internal = internal;

	return result;
}

Node* split(Node* parent, Node* p){
	int i,j;
	Node* r_node;
	Node* l_node;
	Node* new;
	int* l_key;
	int* r_key;
	Node** l_child;
	Node** r_child;
	Node** new_child;
	int* new_key;
	int r_size,l_size;

	l_size = (p->size)/2;
	l_key = (int*)malloc(sizeof(int) * l_size);
	l_child = (Node**)malloc(sizeof(Node*) * (l_size+1));
	if(p->size % 2 != 0)
		r_size = l_size;
	else
		r_size = l_size - 1;
	r_key = (int*)malloc(sizeof(int) * r_size);
	r_child = (Node**)malloc(sizeof(Node*) * (r_size+1));
	for(i = 0; i < l_size; i++){
		l_key[i] = p->key[i];
		l_child[i] = p->child[i];
	}
	l_child[i] = p->child[i];
	for(i = l_size + 1; i < p->size; i++){
		r_key[i- (l_size + 1)] = p->key[i];
		r_child[i-(l_size + 1)] = p->child[i];
	}
	r_child[i-(l_size + 1)] = p->child[i];
	
	l_node = AllocNode(l_key,l_child,l_size,p->internal);
	r_node = AllocNode(r_key,r_child,r_size,p->internal);

	free(l_key);
	free(r_key);
	free(l_child);
	free(r_child);
		
	if(parent != NULL){
		for(i = 0; i < parent->size; i++){
			if(p->key[l_size] <= parent->key[i]){
				for(j = (parent->size)-1; j >= i; j--){
					parent->key[j+1] = parent->key[j];
					parent->child[j+2] = parent->child[j+1];
				}
				parent->key[i] = p->key[l_size];
				parent->child[i] = l_node;
				parent->child[i+1] = r_node;
				break;
			}
		}
		if(i == (parent->size)){
			parent->key[i] = p->key[l_size];
			parent->child[i] = l_node;
			parent->child[i+1] = r_node;
		}
		(parent->size)++;
		free(p->key);
		free(p->child);
		free(p);
		return parent;
	}else{
		new_key = (int*)malloc(sizeof(int));
		new_key[0] = p->key[l_size];
		new_child = (Node**)malloc(sizeof(Node*) * 2);
		new_child[0] = l_node;
		new_child[1] = r_node;
		new = AllocNode(new_key,new_child,1,1);
		
		free(new_key);
		free(new_child);
		free(p->key);
		free(p->child);
		free(p);
		return new;
	}	
}
		
Node* insert(Node* p,int key){
	int i,j;
	Node* p_parent = NULL;
	Node* root = p;

	if(p == NULL){
		int* key_a = (int*)malloc(sizeof(int));
		Node** child_a = (Node**)malloc(sizeof(Node*) *2);
		key_a[0] = key;
		for(i = 0; i < 2; i++)
			child_a[i] = NULL;
		p = AllocNode(key_a,child_a,1,0);
		
		free(key_a);
		free(child_a);
		return p;
	}

	while(1){
		if(p->size == 2*degree -1){
			p = split(p_parent,p);
			if(p_parent == NULL)
				root = p;
		}
		if(p->internal == 1){
			for(i = 0; i < p->size; i++){
				if(key <= p->key[i])
					break;			
			}
			p_parent = p;
			p = p->child[i];
			
		}else{
			for(i = 0; i < p->size; i++){
				if(key <= p->key[i]){
					for(j = (p->size)-1; j >= i; j--)
						p->key[j+1] = p->key[j];
					p->key[i] = key;
					break;
				}
			}
			if(i == (p->size))
				p->key[p->size] = key;
			(p->size)++;
			break;
		}
	}
	
	return root;
}

Node* merge(Node* p, int left, int right){
	int i;
	Node* result;
	
	int* result_key = (int*)malloc(sizeof(int) * (2*degree-1));
	Node** result_child = (Node**)malloc(sizeof(Node*) * (2*degree));
       	for(i = 0; i < p->child[left]->size; i++){
		result_key[i] = p->child[left]->key[i];
		result_child[i] = p->child[left]->child[i];
	}
	result_child[i] = p->child[left]->child[i];
	result_key[i] = p->key[left];
	for(i = 0; i < p->child[right]->size; i++){
		result_key[p->child[left]->size + 1 + i] = p->child[right]->key[i];
		result_child[p->child[left]->size +1 +i] = p->child[right]->child[i];
	}
	result_child[p->child[left]->size + 1 + i] = p->child[right]->child[i];

	result = AllocNode(result_key,result_child,2*degree-1,p->child[left]->internal);
	
	free(p->child[left]->key);
	free(p->child[left]->child);
	free(p->child[left]);
	free(p->child[right]->key);
	free(p->child[right]->child);
	free(p->child[right]);
	for(i = left + 1; i < p->size; i++){
		p->key[i-1] = p->key[i];
		p->child[i] = p->child[i+1];
	}
	
	p->child[left] = result;
	p->key[i-1] = 0;
	p->child[i] = NULL;
	(p->size)--;

	free(result_key);
	free(result_child);

	return p;
}

Node* delete(Node* p, int key){
	int i,j,k;
	Node* root = p;
	Node* temp;

	while(1){
		for(i = 0; i < (p->size); i++){
			if(p->key[i] == key)
				break;
		}
		if(i == (p->size)){
			for(i = 0; i < p->size; i++){
				if(key <= p->key[i]){
					temp = p->child[i];
					break;
				}
			}
			if(i == p->size)
				temp = p->child[i];
			if(temp->size == degree -1){
				if((i - 1 >= 0) && (p->child[i-1]->size >= degree)){
					for(j = temp->size - 1; j >= 0; j--){
						temp->key[j+1] = temp->key[j];
						temp->child[j+2] = temp->child[j+1];
					}
					temp->child[j+2] = temp->child[j+1];
					temp->key[0] = p->key[i-1];
					temp->child[0] = p->child[i-1]->child[(p->child[i-1]->size)];
					(temp->size)++;
					
					p->key[i-1] = p->child[i-1]->key[(p->child[i-1]->size)-1];
					p->child[i-1]->key[(p->child[i-1]->size)-1] = 0;
					p->child[i-1]->child[(p->child[i-1]->size)] = NULL;
					(p->child[i-1]->size)--;
					p = temp;
				}else if((i + 1 < p->size) && (p->child[i+1]->size >= degree)){
					temp->key[temp->size] = p->key[i];
					temp->child[(temp->size)+1] = p->child[i+1]->child[0];
					(temp->size)++;

					p->key[i] = p->child[i+1]->key[0];
					for(j = 1; j < p->child[i+1]->size; j++){
						p->child[i+1]->key[j-1] = p->child[i+1]->key[j];
						p->child[i+1]->child[j-1] = p->child[i+1]->child[j];
					}
					p->child[i+1]->child[j-1] = p->child[i+1]->child[j];
					p->child[i+1]->child[j] = NULL;
					p->child[i+1]->key[j-1] = 0;
					(p->child[i+1]->size)--;
					p = temp;
				}else if((i - 1 >= 0) && (p->child[i-1]->size == degree - 1)){
					p = merge(p,i-1,i);
					if(p->size == 0){
						root = p->child[i-1];
						free(p->key);
						free(p->child);
						free(p);
					}else{
						p = p->child[i-1];
					}
				}else if((i + 1 < p->size) && (p->child[i+1]->size == degree - 1)){
					p = merge(p,i,i+1);
					if(p->size == 0){
						root = p->child[i];
						free(p->key);
						free(p->child);
						free(p);
					}else{
						p = p->child[i];
					}
				}
			}else{
				p = temp;
			}
		}else if(p->internal == 1){
			if(p->child[i]->size >= degree){
				temp = p;
				p = p->child[i];
				while(1){
					if(p->child[p->size] != NULL)
						p = p->child[p->size];
					else
						break;
				}
				temp->key[i] = p->key[(p->size) - 1];
				key = temp->key[i];
				p = temp->child[i];
			}else if(p->child[i+1]->size >= degree){
				temp = p;
				p = p->child[i+1];
				while(1){
					if(p->child[0] != NULL)
						p = p->child[0];
					else
						break;
				}
				temp->key[i] = p->key[0];
				key = temp->key[i];
				p = temp->child[i+1];
			}else{
				p = merge(p,i,i+1);
				p = p->child[i];
			}
		}else{
			for(j = i + 1; j <= (p->size - 1); j++)
				p->key[j-1] = p->key[j];
			p->key[j-1] = 0;
			(p->size)--;
			break;
		}
	}

	return root;
}

int main(){
	int i,j;
	Node* root = NULL;
	int num;
	int n;
	int count = 0;
	int arr[1000];
	int a[500];
	struct timespec tstart_insert = {0.0}, tend_insert = {0,0};
	struct timespec tstart_delete = {0,0}, tend_delete = {0,0};
	srand((unsigned)time(NULL));
	
	while(count < 1000){
		n = rand() % 10000;
		for(i = 0; i < count; i++)
			if(arr[i] == n)
				break;

		if(count == i) 
			arr[count++] = n;
	}
	
	
	printf("degree를 입력하시오.: ");
	scanf("%d",&degree);
	
	clock_gettime(CLOCK_MONOTONIC,&tstart_insert);
	for(i = 0; i < 1000; i++){
		root = insert(root,arr[i]);
	}
	clock_gettime(CLOCK_MONOTONIC,&tend_insert);
	
	count = 0;
	while(count < 500){
		n = rand() % 1000;
		for(i = 0; i < count; i++)
			if(a[i] == n)
				break;

		if(count == i)
			a[count++] = n;
	}

	clock_gettime(CLOCK_MONOTONIC,&tstart_delete);
	for(i = 0; i <500; i++){
		root = delete(root,arr[a[i]]);
	}	
	clock_gettime(CLOCK_MONOTONIC,&tend_delete);

	printf("insert time: %.8f seconds\n",((double)tend_insert.tv_sec + 1.0e-9*tend_insert.tv_nsec) - ((double)tstart_insert.tv_sec + 1.0e-9*tstart_insert.tv_nsec));
	printf("delete time: %.8f seconds\n",((double)tend_delete.tv_sec + 1.0e-9*tend_delete.tv_nsec) - ((double)tstart_delete.tv_sec + 1.0e-9*tstart_delete.tv_nsec));
}

