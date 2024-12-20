/*
    ! NGUYEN PHUC NHAN
    * Last update: 2024-09-15
    * Version 1.0
    * This file defines a doubly linked list
*/

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
#define FOR_in_range(i, start, end) for (int i = start; i < end; ++i)
#define FOR_in_range_reverse(i, start, end) for (int i = start; i >= end; --i)
#define push_to_ss(item) ss << (item)
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    // Student's methods
    // BEGIN
    /* Check valid for index of element in list
        * + index: index of element in list
        * + for_extend: if for_extend = 1, list need to add element => index can be equal count
        *                if for_extend = 0, list need to get element => index must be less than count
    */
    void isValidIndex(int index, bool for_extend);
    void init_head_tail(bool in_constructor = true);
    void addAfter(Node* &prevNode, T e);
    // END

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

    class BWDIterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;
    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }

        // Prefix -- overload
        BWDIterator &operator--()
        {
            pNode = pNode->prev;
            return *this;
        }

        // Postfix -- overload
        BWDIterator operator--(int)
        {
            BWDIterator iterator = *this;
            --*this;
            return iterator;
        }

        BWDIterator &operator++()
        {
            pNode = pNode->prev;
            return *this;
        }

        BWDIterator operator++(int)
        {
            BWDIterator iterator = *this;
            ++*this;
            return iterator;
        }

        T &operator*()
        {
            return pNode->data;
        }

        bool operator!=(const BWDIterator &iterator)
        {
            return pNode != iterator.pNode;
        }

        void remove(void (*removeItemData)(T) = 0)
        {
            Node *pPrev = pNode->prev->prev;
            pNode->prev->prev->next = pNode;
            pNode->prev = pNode->prev->prev;
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode->prev;
            pNode->prev = pPrev;
            pList->count -= 1;
        }
    };
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    stringstream ss;
    push_to_ss("[");
    Node *current = head->next;
    FOR_in_range(i, 0, count) {
        if (item2str) {
            push_to_ss(item2str(current->data));
        } else {
            push_to_ss(current->data);
        }
        if (i < count - 1) {
            push_to_ss(", ");
        }
        current = current->next;
    }
    ss << "]";
    return ss.str();
}
template <class T>
void DLinkedList<T>::init_head_tail(bool in_constructor) {
    if (in_constructor) {
        this->head = new Node();
        this->tail = new Node();
    }
    head->next = tail;
    tail->prev = head;
}

template <class T>
void DLinkedList<T>::addAfter(Node* &prevNode, T e) {
    Node *newNode = new Node(e, prevNode->next, prevNode);
    prevNode->next = newNode;
    newNode->next->prev = newNode;
    ++count;
}


template <class T>
void DLinkedList<T>::isValidIndex(int index, bool for_extend){
    if (for_extend && index == count) return;
    if (index < 0 || index >= count) {
        throw "Index is out of range!";
    }
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    if (this->count > 0)
    {
        this->removeInternalData();
    }
    delete head;
    delete tail;
    this->count = 0;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    /*
    * Objectives: add an item to the end of the list
    */
    if (head->next == tail) {           // ? empty list
        addAfter(head, e);
    } else {                            // : non-empty list
        addAfter(tail->prev, e);
    }
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    /*
    * Objectives: add an item to the list at a specific index
    */
    isValidIndex(index, 1);
    if (index == count) {                       // ? add to the end of the list
        add(e);
        return;
    }
    Node *current = getPreviousNodeOf(index);
    addAfter(current, e);
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    Node *current = head;
    if (index < count / 2)
    {
        FOR_in_range(i, 0, index)
        {
            current = current->next;
        }
    }
    else
    {
        current = tail;
        FOR_in_range_reverse(i, count, index)
        {
            current = current->prev;
        }
    }
    return current;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    /*
    * Objectives: remove an item from the list at a specific index
    * Return: the removed item
    * Exception: throw an out_of_range exception if the index is invalid
    */
    isValidIndex(index, 0);
    Node *current = getPreviousNodeOf(index);
    Node *removeNode = current->next;
    T data = removeNode->data;
    current->next = removeNode->next;
    removeNode->next->prev = current;
    delete removeNode;
    count--;
    return data;
}

template <class T>
int DLinkedList<T>::size()
{
    // * Objectives: get the number of items in the list
    return count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // * Objectives: clear the list
    if (this->count > 0) {
        this->removeInternalData();
    }
    count = 0;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    /*
    * Objectives: get an item from the list at a specific index
    * Return: the item at the index
    * Exception: throw an out_of_range exception if the index is invalid
    * Note: index is 0-based
    */
    isValidIndex(index, 0);
    Node *current = getPreviousNodeOf(index);
    return current->next->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    /*
    * Objectives: get the index of an item in the list
    * Return: the index of the item if it exists, otherwise -1
    * Note: if the item is a pointer type, the itemEqual function will be called to compare the item data
    *     if it is not null
    */
    Node *current = head->next;
    FOR_in_range(i, 0, count) {
        if (equals(current->data, item, itemEqual)) {
            return i;
        }
        current = current->next;
    }
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    /*
    * Objectives: remove an item from the list
    * Return: true if the item is removed, otherwise false
    * Note: if the item is a pointer type, the removeItemData function will be called to remove the item data
    *    if it is not null
    */
    Node *current = head->next;
    int idx = indexOf(item);
    if (idx == -1) {
        return false;
    }
    T data = removeAt(idx);
    if (removeItemData) {
        removeItemData(data);
    }
    return true;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    /*
    * Objectives: check if the list contains an item
    * Return: true if the item is in the list, otherwise false
    * Note: if the item is a pointer type, the itemEqual function will be called to compare the item data
    *   if it is not null
    */
    Node *current = head->next;
    FOR_in_range(i, 0, count) {
        if (equals(current->data, item, itemEqual)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    if (this->count > 0) {
        this->removeInternalData();
    }
    this->itemEqual = list.itemEqual;
    // this->deleteUserData = list.deleteUserData;
    
    Node *current = list.head->next;

    FOR_in_range(i, 0, list.count) {
        T copy_data;
        copy_data = current->data;
        this->add(copy_data);
        current = current->next;
    }
    this->count = list.count;
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    if (this->deleteUserData != nullptr)
    {
        this->deleteUserData(this);
    }
    Node* tmp = head->next;
    while (count > 0)
    {
        Node *current = tmp;
        tmp = tmp->next;
        delete current;
        --count;
    }
    init_head_tail(false);
    count = 0;
}


template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
    : deleteUserData(deleteUserData), itemEqual(itemEqual), count(0)
{
    init_head_tail();
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    /*
    * Objectives: copy constructor
    * Parameters: a list to be copied
    */
    this->count = 0;

    init_head_tail();
    this->itemEqual = nullptr;
    this->deleteUserData = nullptr;

    this->copyFrom(list);
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    /*
    * Objectives: assignment operator
    * Parameters: a list to be assigned
    */
    if (this != &list)
    {
        this->copyFrom(list);
    }
    return *this;
}

template <class T>
bool DLinkedList<T>::empty()
{
    // * Objectives: check if the list is empty
    return count == 0;
}

#endif /* DLINKEDLIST_H */