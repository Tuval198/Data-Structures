#ifndef WET1_AVL_HPP
#define WET1_AVL_HPP

//K must have operator<

template <class T, class K = int>
struct AVLNode{
    AVLNode<T,K> *father;
    AVLNode<T,K> *left_son;
    AVLNode<T,K> *right_son;
    int height;
    T data;
    K key;

    AVLNode<T,K>(T data,K key, AVLNode<T,K> *father=NULL): data(data), key(key), father(father),
                                                 left_son(NULL), right_son(NULL), height(0){};
    AVLNode<T,K>(T data, AVLNode<T,K> *father=NULL): data(data), key(data), father(father),
                                                           left_son(NULL), right_son(NULL), height(0){};
    ~AVLNode<T,K>() = default;
};

template <class T, class K = int>
class AVL{
private:
    AVLNode<T,K> *root;
    AVLNode<T,K> *min; //pointer to the most right node

    AVLNode<T,K>* findNode(K key, AVLNode<T,K>* temp_root);
    void insertNode(AVLNode<T,K>* to_insert ,AVLNode<T,K>* temp_root);
    /*argument should be leaf.
 * update balance factor to all ancestors
 * then blances tree*/
    void balance(AVLNode<T,K>* bottom); //update height then scan up to roll**/
    void printInOrderRec(AVLNode<T, K>* temp_root);
    void switchNodes(AVLNode<T, K>* n,AVLNode<T, K>* w);
    //sets height to max(left_height,right_height) + 1
    int updateHeight(AVLNode<T, K>* n);
    void RollIfNeeded(AVLNode<T, K>* current);
    void LLRoll(AVLNode<T, K>* current);
    void LRRoll(AVLNode<T, K>* current);
    void RLRoll(AVLNode<T, K>* current);
    void RRRoll(AVLNode<T, K>* current);
    void deleteAllNodes(AVLNode<T, K>* current_root);
    void updateMinimum();
    void getKInOrderRec(AVLNode<T, K>* current, int k , T* res, K *res_key, int &counter, bool came_from_father);
    AVLNode<T,K>* insertZerosRec(int *arr, int start, int end);
    int getNodesCountRec(AVLNode<T, K>* temproot);

public:
    //create empty AVL:
    AVL<T,K>():root(NULL), min(NULL){};
    //create AVL with data as root (with or without key:
    AVL<T,K>(T data, K key): root(new AVLNode<T,K>(data,key)){ min = root;}
    AVL<T,K>(T data): root(new AVLNode<T,K>(data)){ min = root;}
    ~AVL<T,K>();

    /* inserts data with key
     * if key already in alreadyExistException is thrown */
    bool insert(T data, K key);
    bool remove(K key);

    /*returns true if key exists in the AVL and false otherwise*/
    bool exists(K key);

    T findData(K key);

   /*returns refernce to the data with given key (O(Logn))
    * notFoundException is thrown if key does not exists in avl*/
    T& find(K key);
    void printInOrder();
    int getHeight(){
        if(root != NULL) return root->height ;
        else return 0;}

    /*this function gets number k and fills the array res in (up to) k elements
     * with the smallest key
     * k is the size of the array res
     * return value is the amount of elements filled in the array
     * */
    int getKInOrder(int k, T *res, K *res_key);

    class notFoundException{};
    class alreadyExistException{};
    class NullArgumentException{};

    //this function only works on an <int,int> tree
    void insertZeros(int i);

    bool avlIsEmpty(){return root == NULL;}

    int getNodesCount();

};

template <class T, class K>
void AVL<T,K>::switchNodes(AVLNode<T, K> *n, AVLNode<T, K> *w) {
    if(!n || !w) throw NullArgumentException();
    int w_h = w->height;
    AVLNode<T, K>* w_f = w->father;
    AVLNode<T, K>* w_l = w->left_son;
    AVLNode<T, K>* w_r = w->right_son;

    int n_h = n->height;
    AVLNode<T, K>* n_f = n->father;
    AVLNode<T, K>* n_l = n->left_son;
    AVLNode<T, K>* n_r = n->right_son;

    bool w_son_of_n = false;
    if(w->father == n){
        w->right_son =n;
        n->father = w;
        w_son_of_n = true;
    }

    w->father = n_f;
    w->left_son = n_l;
    if(!w_son_of_n)w->right_son = n_r;

    if(n==root) root =w;
    else {
        if (n_f->left_son == n) n_f->left_son =w;
        else n_f->right_son =w;
    }
    if(n_l != NULL) n_l->father = w;
    if(n_r != NULL && !w_son_of_n) n_r->father =w;

    if(!w_son_of_n)n->father = w_f;
    n->left_son = w_l;
    n->right_son = w_r;
    if (w_f->left_son == w) w_f->left_son =n;
    else if(!w_son_of_n) w_f->right_son =n;

    if(w_l != NULL) w_l->father = n;
    if(w_r != NULL) w_r->father =n;

    n->height = w_h;
    w->height = n_h;

}

template <class T, class K>
void AVL<T,K>::RollIfNeeded(AVLNode<T, K> *current) {
    AVLNode<T, K> *n = current; //we dont wanna change current
    int bf = updateHeight(n->left_son) - updateHeight(n->right_son);

    if(bf >= -1 && bf <= 1) return; //already balanced

    if(bf>1){
        int bf_left = updateHeight(n->left_son->left_son) - updateHeight(n->left_son->right_son);
        if(bf_left >= 0)
            LLRoll(n);
        else
            LRRoll(n);

    }
    else{
        int bf_right = updateHeight(n->right_son->left_son) - updateHeight(n->right_son->right_son);
        if(bf_right <=0)
            RRRoll(n);
        else
            RLRoll(n);
    }
}

template <class T, class K>
int AVL<T,K>::updateHeight(AVLNode<T, K> *n) {
    if(n==NULL)
        return 0;

    int left_height;
    int right_height;

    if(n->left_son == NULL) left_height =0;
    else left_height = n->left_son->height;

    if(n->right_son == NULL) right_height = 0;
    else right_height = n->right_son->height;

    n->height = (right_height > left_height) ? (right_height+1) : (left_height+1);
    return n->height;
}

template <class T, class K>
void AVL<T,K>::balance(AVLNode<T, K>* bottom) {
    AVLNode<T, K>* current = bottom;
    if(current == NULL){
        return;
    }

    while(current != root){
        updateHeight(current);
        current = current->father;
    }
    updateHeight(current);

    current = bottom;

    while(current != root){
        RollIfNeeded(current);
        current= current->father;
    }
       RollIfNeeded(root);
}

template <class T, class K>
AVLNode<T,K>* AVL<T,K>::findNode(K key_to_find, AVLNode<T,K>* temp_root) {
    if(temp_root == NULL)
        return NULL;
    if(temp_root->key == key_to_find)
        return temp_root;

    if(key_to_find < temp_root->key)
         return findNode(key_to_find, temp_root->left_son);

     return findNode(key_to_find, temp_root->right_son);
}

template <class T, class K>
void AVL<T,K>::insertNode(AVLNode<T, K> *to_insert,
                                AVLNode<T, K> *temp_root) {
    //this function assumes that node with such key does not exist in the AVL!
    if(temp_root != NULL &&temp_root->key == to_insert->key)
        throw AVL::alreadyExistException();

    if((temp_root->key < to_insert->key)&&(temp_root->right_son == NULL)) {
        temp_root->right_son = to_insert;
        to_insert->father = temp_root;
        balance(to_insert);
        return;
    }
    if((to_insert->key < temp_root->key)&&(temp_root->left_son == NULL)) {
        temp_root->left_son = to_insert;
        to_insert->father = temp_root;
        balance(to_insert);
        return;
    }

    if(temp_root->key < to_insert->key)
        insertNode(to_insert, temp_root->right_son);
    else
        insertNode(to_insert, temp_root->left_son);

    return;
}

template <class T, class K>
bool AVL<T,K>::exists(K key) {
    AVLNode<T,K>* n = findNode(key, root);
    if(n == NULL)
        return false;
    return true;
}

template <class T, class K>
T& AVL<T,K>::find(K key) {
    AVLNode<T,K>* n = findNode(key, root);
    if(n == NULL)
        throw AVL::notFoundException();
    return n->data;
}

template <class T, class K>
bool AVL<T,K>::insert(T data, K key) {
    if(exists(key))
        return false;
    AVLNode<T,K>* n = new AVLNode<T,K>(data, key);

    if(root == NULL){
        root = n;
        updateHeight(n);
        updateMinimum();
        return true;
    }

    insertNode(n, root);
    while(root->father != NULL) root = root->father;
    updateMinimum();

    return true;
}

template <class T, class K>
bool AVL<T,K>::remove(K key) {
    if(!exists(key))
        return false;

    if(root->key == key && root->left_son == NULL && root->right_son == NULL){
        // last one in the tree
        delete root;
        root = NULL;
        updateMinimum();
        return true;
    }

    AVLNode<T,K>* n = findNode(key,root);
    AVLNode<T,K>* w;

    if(n->left_son != NULL && n->right_son != NULL){
        w = n->right_son;
        while(w->left_son != NULL)
            w = w->left_son;
        switchNodes(n,w);
        //no need to update root
    }
    if(n->right_son == NULL && n->left_son == NULL) //leaf, just delete
    {
        if(n->father != NULL) {
            if(n->father->left_son == n) n->father->left_son = NULL;
            else n->father->right_son = NULL;
        }
        else {
            root = NULL; //actualy we should not get inside the if
        }//n was the last one
    }
    if(n->right_son == NULL && n->left_son != NULL){
        if(n->father != NULL) {
            if(n->father->left_son == n) n->father->left_son = n->left_son;
            else n->father->right_son = n->left_son;
        }
        else{
            root = n->left_son;
        }
        n->left_son->father = n->father;
    }
    if(n->left_son == NULL && n->right_son != NULL){
        if(n->father != NULL) {
            if(n->father->left_son == n) n->father->left_son = n->right_son;
            else n->father->right_son = n->right_son;
        }
        else{
            root = n->right_son;
        }
        n->right_son->father = n->father;
    }
    AVLNode<T,K>* new_father_of_n = n->father;
    delete n;
    balance(new_father_of_n);
    updateMinimum();
    return true;
}

template <class T, class K>
void AVL<T,K>::printInOrderRec(AVLNode<T, K> *temp_root) {
    if(temp_root == NULL) return;
    printInOrderRec(temp_root->left_son);
    std::cout << temp_root->key<< ", ";
    printInOrderRec(temp_root->right_son);
}

template <class T, class K>
void AVL<T,K>::printInOrder() {
    if(root == NULL) {
        std::cout << "empty tree" << std::endl;
        return;
    }
    std::cout <<"in order : ";
    printInOrderRec(root);
    std::cout <<std::endl <<">>root : "<< root->key ;
    std::cout <<std::endl <<">>min : "<< min->key ;
    std::cout <<std::endl <<">>height : "<< root->height ;
    if(root->height >2)
        std::cout <<std::endl <<">>root BF : "<< root->left_son->height - root->right_son->height ;
    std::cout <<std::endl<<std::endl ;
}

template<class T, class K>
void AVL<T, K>::LLRoll(AVLNode<T, K> *current) {
    AVLNode<T, K> *a = current->left_son;
    AVLNode<T, K> *a_r = a->right_son;

    a->right_son = current;
    a->father = current->father;

    current->father = a;
    current->left_son = a_r;

    if(a_r != NULL)
        a_r->father = current;

    if(a->father == NULL) root = a;
    else {
        if(a->father->left_son == current) a->father->left_son = a;
        if(a->father->right_son == current) a->father->right_son =a;
    }
    current->height -=2;
}

template<class T, class K>
void AVL<T, K>::RRRoll(AVLNode<T, K> *current) {
    AVLNode<T, K> *a = current->right_son;
    AVLNode<T, K> *a_l = a->left_son;


    a->left_son = current;
    a->father = current->father;

    current->father = a;
    current->right_son = a_l;

    if(a_l != NULL)
        a_l->father = current;

    if(a->father == NULL) root = a;
    else {
        if(a->father->left_son == current) a->father->left_son = a;
        if(a->father->right_son == current) a->father->right_son =a;
    }
    current->height -=2;
}

template<class T, class K>
void AVL<T, K>::LRRoll(AVLNode<T, K> *current) {
    AVLNode<T, K> *c = current;
    AVLNode<T, K> *a = c->left_son;
    AVLNode<T, K> *b = a->right_son;
    AVLNode<T, K> *b_l = b->left_son;
    AVLNode<T, K> *b_r = b->right_son;
    AVLNode<T, K> *c_f_temp = c->father;

    b->left_son = a;
    a->father = b;

    b->right_son =c;
    c->father = b;

    b->father = c_f_temp;
    if(b->father == NULL) root = b;
    else {
        if(b->father->left_son == current) b->father->left_son = b;
        if(b->father->right_son == current) b->father->right_son =b;
    }

    c->left_son = b_r;
    if(b_r != NULL)
        b_r->father = c;

    a->right_son = b_l;
    if(b_l != NULL)
        b_l->father = a;

    a->height --;
    b->height++;
    c->height -=2;
}


template<class T, class K>
void AVL<T, K>::RLRoll(AVLNode<T, K> *current) {
    AVLNode<T, K> *c = current;
    AVLNode<T, K> *a = c->right_son;
    AVLNode<T, K> *b = a->left_son;
    AVLNode<T, K> *b_l = b->left_son;
    AVLNode<T, K> *b_r = b->right_son;
    AVLNode<T, K> *c_f_temp = c->father;

    b->left_son = c;
    c->father = b;

    b->right_son =a;
    a->father = b;

    b->father = c_f_temp;
    if(b->father == NULL) root = b;
    else {
        if(b->father->left_son == current) b->father->left_son = b;
        if(b->father->right_son == current) b->father->right_son =b;
    }

    c->right_son = b_l;
    if(b_l != NULL)
        b_l->father = c;

    a->left_son = b_r;
    if(b_r != NULL)
        b_r->father = a;

    a->height --;
    b->height++;
    c->height -=2;
}

template<class T, class K>
AVL<T, K>::~AVL<T, K>() {
    deleteAllNodes(root);
}

template<class T, class K>
void AVL<T, K>::deleteAllNodes(AVLNode<T, K> *current_root) {
    if(current_root == NULL) return;
    deleteAllNodes(current_root->left_son);
    deleteAllNodes(current_root->right_son);
    delete current_root;
}

template<class T, class K>
void AVL<T, K>::updateMinimum() {
    if(root == NULL) {
        min = NULL;
        return;
    }
    min = root;
    while(min->left_son != NULL) min = min->left_son;
}



template<class T, class K>
int AVL<T, K>::getKInOrder(int k, T *res, K *res_key) {
    if(root == NULL) return 0;
    int counter = 0;
    getKInOrderRec(min,k,res,res_key,counter, false);
    return counter;
}

template<class T, class K>
void AVL<T, K>::getKInOrderRec(AVLNode<T, K> *current, int k, T *res, K *res_key,
                               int &counter, bool came_from_father) {
    //in all cases we stop when counter = k

    if(came_from_father && current->left_son != NULL && counter != k)//left son
        getKInOrderRec(current->left_son, k, res, res_key, counter, true);

    //self:
    if(counter != k) {
        res[counter] = current->data;
        res_key[counter] = current->key;
        counter++;
    }

    if(current->right_son != NULL && counter != k)
        getKInOrderRec(current->right_son, k, res,res_key, counter, true);

    if(!came_from_father && current->father != NULL && counter != k)
        getKInOrderRec(current->father, k, res, res_key , counter, false);

    return;
}

template<class T, class K>
T AVL<T, K>::findData(K key) {
    return findNode(key,root)->data;
}


template<class T, class K>
void AVL<T, K>::insertZeros(int i) {
    int *arr = new int[i];
    for(int j = 0; j<i; j++)
        arr[j] = j;

    root = insertZerosRec(arr,0,i-1);
    updateMinimum();
    delete[] arr;

}

template<class T, class K>
AVLNode<T,K>* AVL<T, K>::insertZerosRec(int *arr, int start, int end) {
    if(start>end)
        return NULL;

    int mid = (start + end) /2;
    AVLNode<T,K>* new_root = new AVLNode<T,K>(arr[mid],arr[mid]);
    new_root->height =1;

    int left_height = 0;
    int right_height = 0;

    new_root->left_son = insertZerosRec(arr,start, mid-1);
    if(new_root->left_son != NULL) {
        new_root->left_son->father = new_root;
        left_height = new_root->left_son->height;
    }

    new_root->right_son = insertZerosRec(arr, mid+1, end);

    if(new_root->right_son != NULL) {
        new_root->right_son->father = new_root;
        right_height = new_root->right_son->height;
    }

    if(left_height > right_height)
        new_root->height = left_height +1;
    else
        new_root->height = right_height +1;

    return new_root;
}

template<class T, class K>
int AVL<T, K>::getNodesCountRec(AVLNode<T, K> *temproot) {
    if(temproot == NULL) return 0;
    return 1 + getNodesCountRec(temproot->left_son) + getNodesCountRec(temproot->right_son);

}

template<class T, class K>
int AVL<T, K>::getNodesCount() {
    return getNodesCountRec(root);
}

#endif //WET1_AVL_HPP
