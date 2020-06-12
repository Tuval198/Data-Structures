#ifndef _DList_
#define _DList_
#include <iostream>

//NOTE: this list may only contain pointers to data, it does not make copy of the data and
//doesnt free the data at destruction

template <class T>
class DLNode{
private:
    DLNode<T> *next;
    DLNode<T> *prev;
    T* data_ptr;
public:
    DLNode<T>(T* data_ptr ,DLNode<T> *next = NULL,DLNode<T> *prev = NULL):
            data_ptr(data_ptr), next(next), prev(prev){};
    ~DLNode<T>() = default;

    T* getData(){return data_ptr;}
    DLNode<T>* getNext(){return next;}
    DLNode<T>* getPrev(){return prev;}
    void setNext(DLNode<T>* n){next = n;}
    void setPrev(DLNode<T>* p){prev = p;}
};

template <class T>
class DList{
private:
    DLNode<T> *first;
    DLNode<T> *last;
public:
    DList<T>():first(new DLNode<T>(NULL,NULL,NULL)){last = first;}

    ~DList<T>(){
        DLNode<T> *current = last;
        DLNode<T> *prv = current->getPrev();

        while(prv != NULL){
            removeNode(current);
            current=prv;
            prv = current->getPrev();
        }
        delete current; //deletes the first dummy node
    }

    /*inserts new node with given data in the end
     * return value : pointer to the new node*/
    DLNode<T>* insert(T* data_ptr);

    /*inserts new node with given data after given node
     * return value : pointer to the new node*/
    DLNode<T>* insertAfter(DLNode<T>* current, T* data_ptr);

    /*removes given Node*/
    void removeNode(DLNode<T>* to_remove);

    DLNode<T>* getFirst();
    DLNode<T>* getLast();

    void printList();

};

template <class T>
DLNode<T>* DList<T>::insertAfter(DLNode<T>* current, T* data_ptr){
    DLNode<T>* new_node = new DLNode<T>(data_ptr, current->getNext(),current);
    DLNode<T>* nxt = new_node->getNext();
    current->setNext(new_node);
    if(nxt != NULL) //its not the last node
        nxt->setPrev(new_node);
    else //the new node is the last one
        last = new_node;
    return new_node;

}

template <class T>
void DList<T>::removeNode(DLNode<T>* to_remove){
    DLNode<T>* nxt = to_remove->getNext();
    to_remove->getPrev()->setNext(nxt);

    if(nxt != NULL){
        nxt->setPrev(to_remove->getPrev());
    }
    else{
        last = to_remove->getPrev();
    }
    delete to_remove; //also deletes data!
}

template <class T>
void DList<T>::printList() {
    DLNode<T> *current = first;
    DLNode<T> *nxt = current->getNext();

    while(current != last){
        if(current->getData() == NULL){
            std::cout<< "Dummy"<< "--->";
        }
        else {
            std::cout << *current->getData() << "--->";
        }
        current=nxt;
        nxt = current->getNext();
    }

    if(current->getData() == NULL){
        std::cout<< "Dummy"<< "--->"<<std::endl;
    }
    else {
        std::cout << *current->getData() << "--->"<<std::endl;
    }

    std::cout << "|||||| last: "<< *last->getData() <<std::endl;
    std::cout << "|||||| first: "<< *first->getNext()->getData() <<std::endl;
}

template <class T>
DLNode<T>* DList<T>::insert(T* data_ptr){
    return insertAfter(last,data_ptr);

}

template<class T>
DLNode<T> *DList<T>::getFirst() {
    return first;
}

template<class T>
DLNode<T> *DList<T>::getLast() {
    return last;
}


#endif