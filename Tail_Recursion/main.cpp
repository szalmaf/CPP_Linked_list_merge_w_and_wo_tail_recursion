//
//  main.cpp
//  Tail_Recursion
//
//  Created by Ferenc Szalma on 8/11/16.
//  Copyright © 2016 Ferenc Szalma. All rights reserved.
//
// The goal is to investigate the difference between non-tail recursive and tail recursive code
// applied to (single) linked lists in C++.
// The code merges two lists in a simpler, non-tail recursive way, and
// a more complex, tail recursive way.
//
// To measure the speed difference I use the chrono library.
// On a MacbookPro8,3 Intel Core i7 2.4GHz 256kB L2 Cache/core 6MB L3 Cache 16GB RAM 1600MHz.
// It takes to create 300,000,000 nodes in two lists ~30 secs.
// It takes to merge the two list using tail recursion ~3 secs (on one core, one thread).

#include <iostream>
#include <chrono>
using namespace std;


// Speed measuring utilities ##################################
typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()

template<typename F, typename... Args>
double funcTime(F func, Args&&... args){
    TimeVar t1=timeNow();
    func(std::forward<Args>(args)...);
    return duration(timeNow()-t1);
}
// ###########################################################



// Linked lists ##############################################
struct Node
{
    int data;
    struct Node *next;
};

Node* InsertTail(Node *head,int data)
{
    if(head == NULL)
    {
        head = new Node();
        head->data = data;
    }
    else
    {
        Node * nd = head;
        
        Node * newNode = new Node();
        newNode->data = data;
        
        while(nd->next != NULL)
            nd = nd->next;
        nd->next = newNode;
    }
    return head;
}



Node * MergeListUtil(Node * accumHead, Node * accumTail, Node * headA, Node * headB)
// Merge list with tail recursion
{
    if(headA && headB)
    {
        if(headA->data < headB->data)
        {
            accumTail->next = headA;
            return MergeListUtil(accumHead, headA, headA->next, headB);
        }
        else
        {
            accumTail->next = headB;
            return MergeListUtil(accumHead, headB, headA, headB->next);
        }
    }
    else if (headA)
    {
        accumTail->next = headA;
        return accumHead;
    }
    else
    {
        accumTail->next = headB;
        return accumHead;
    }
}

Node* MergeListTailRec(Node* headA, Node* headB)
// Tail resursion w/ MergeListUtil() w/ accumHead and accumTail. Ugly way of creating accumHead and accumTail
{
    if(headA && headB) // When both A and B contains data; 6 cases to prepare accumHead and accumTail
    {
        if(headA->data < headB->data)
        {
            Node * accumHead = headA;
            headA = headA->next;
            if(headA)
                if(headA->data < headB->data)
                {
                    accumHead->next = headA;
                    return MergeListUtil(accumHead, headA, headA->next, headB);
                }
                else
                {
                    accumHead->next = headB;
                    return MergeListUtil(accumHead, headB, headA, headB->next);
                }
            else
            {
                accumHead->next = headB;
                return MergeListUtil(accumHead, headB, headA, headB->next);
            }
        }
        else
        {
            Node * accumHead = headB;
            headB = headB->next;
            if(headB)
                if(headA->data < headB->data)
                {
                    accumHead->next = headA;
                    return MergeListUtil(accumHead, headA, headA->next, headB);
                }
                else
                {
                    accumHead->next = headB;
                    return MergeListUtil(accumHead, headB, headA, headB->next);
                }
            else
            {
                accumHead->next = headA;
                return MergeListUtil(accumHead, headA, headA->next, headB);
            }
        }
    }
    else if (headA)
        return headA;
    else
        return headB;
}


Node* MergeLists(Node *headA, Node* headB) // No tail call/recursion
{
    if(headA && headB)
    {
        if(headA->data < headB->data)
        {
            headA->next = MergeLists(headA->next, headB);
            return headA;
        }
        else
        {
            headB->next = MergeLists(headA, headB->next);
            return headB;
        }
    }
    else if (headA)
        return headA;
    else
        return headB;
}

unsigned long listLength(Node* list)
{
    if(list)
        return listLength(list->next) + 1;
    else
        return 0;
}

Node * getNth(Node * head, int n)
// Get the n-th node from head in linked list
{
    if(n)
        return getNth(head->next, n - 1);
    else
        return head;
}

void print(Node * head, int first, int n) // Print from "first", "n" Nodes
{
    if(n > 0 && head)
    {
        head = getNth(head, first);
        cout << head->data << endl;
        print(head->next, 0, n - 1);
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    Node * listA = new Node();
    listA->data = 0;
    Node * listB = new Node();
    listB->data = 1;

    Node * oldNdA = listA;
    Node * oldNdB = listB;
    
    // Create long list (e.g. 300,000,000 nodes)
    int nNode = 300000000;
    auto f = [nNode] (Node * oldNdA, Node * oldNdB) {
        for(int i=1; i < nNode; i++)
            // 300,000,000 (9GB mem'y) needs -O1 optimization; 50,000 can be handled by -O0 - i.e. no optimization.
        {
            Node * ndA = new Node();
            ndA->data = 2*i;
            oldNdA->next = ndA;
            oldNdA = ndA;

            Node * ndB = new Node();
            ndB->data = 2*i+1;
            oldNdB->next = ndB;
            oldNdB = ndB;
        }
    };
    std::cout << "List creation time: " << funcTime(f, oldNdA, oldNdB) / 1000000000. << " sec"<< endl;

// Test0
// no nulls in zeroth/first positions
// Test1
//    getNth(listA, 1)->data = 1;
//    getNth(listB, 0)->data = 2;
// Test2
//    getNth(listA, 0)->next = NULL;
// Test3
//    getNth(listA, 0)->data = 1;
//    getNth(listB, 0)->data = 0;
//    getNth(listB, 1)->data = 2;
// Test4
//    getNth(listA, 0)->data = 2;
//    getNth(listB, 0)->data = 0;
//    getNth(listB, 1)->data = 1;
// Test5
//    getNth(listA, 0)->data = 1;
//    getNth(listB, 0)->data = 0;
//    getNth(listB, 0)->next = NULL;
    
    print(listA, 10000, 3); cout << endl;
    print(listB, 0, 3); cout << endl;
    
    std::cout << "MergeListTailRec time: " << funcTime(MergeListTailRec,listA,listB) / 1000000000. << " sec" << endl;
//    std::cout << "MergeList time: " << funcTime(MergeLists,listA,listB) / 1000000000. << " sec" << endl;
    
    // Check out merged list
//    Node * mList = MergeListTailRec(listA, listB);
//    cout << listLength(mList) << endl;
//    print(mList, 0, 10);
    
    int i;
    cin >> i;
    
    return 0;
}
