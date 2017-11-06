#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "doubly_linked_list.h"

int main()
{
    DB_node_t *head = NULL;             /* Head node in the linked list */
    uint32_t data_val = 10;
    uint32_t index_num = 1;
    uint32_t data_val2 = 11;
    uint32_t index_num2 = 2;
    uint32_t data_val3 = 12;
    uint32_t index_num3 = 3;
    uint32_t data_val4 = 13;
    uint32_t index_num4 = 1;
    uint32_t data_val5 = 14;
    uint32_t index_num5 = 2;
    uint32_t ret_val = 0;
    uint32_t ret_ind = 0;
    
    
    if((add_node(&head,data_val,index_num)) == LIST_ERROR)          /* Add node into the linked list */
    {
        printf("Error\n");
    }
    if((add_node(&head,data_val2,index_num2)) == LIST_ERROR)        /* Add node into the linked list */
    {
        printf("Error\n");
    }
    if((add_node(&head,data_val3,index_num3)) == LIST_ERROR)        /* Add node into the linked list */
    {
        printf("Error\n");
    }
    if((add_node(&head,data_val4,index_num4)) == LIST_ERROR)        /* Add node into the linked list */
    {
        printf("Error\n");
    }
    if((add_node(&head,data_val5,index_num5)) == LIST_ERROR)        /* Add node into the linked list */
    {
        printf("Error\n");
    }
    print_nodes(head);                                              /* Print all the nodes in the linked list */
    remove_node(&head, &ret_val, index_num2);                       /* Remove node at a given index */
    printf("After removing\n");                 
    print_nodes(head);                                              /* Print all the nodes in the linked list */
    search(head,&ret_ind, data_val3);                               /* Search the node with the given data value */
    printf("The searched value's index is %d \n",ret_ind);
    printf("The number of nodes in the lists are %d\n",size(head));
    destroy(&head);                                                 /* Remove all the nodes in the linked list */
    print_nodes(head);                                              /* Print all the nodes in the linked list */
    printf("The number of nodes in the lists are %d\n",size(head));
}