/****************/
eng_370k_shuffle.txt and heart_darkness.txt is needed.

/***************/
Please use make all, make run, make memchk to run my assignment.
No memory leak in lab machine.

/***************/
I did 3 things.
1. Prettier print, using: make prettier
2. Hashing, using: make hash
3. Threaded tree, using: make thread

/****************/
1. Prettier print:

The prettier.c is the one that I favorite, because I didn’t read any paper or search in the Internet. I think my method is really really unique.

Generally speaking, I used 2 2-D-arrays to store the tree’s structure information and one list(called data) to store the data. The index of these data is stored in the “seque” array. If I want the data of this node, I can use data[seque[i][j]].

The first 2-D-array called “lable” is used to record the node’s child condition and to check if this position has a node. It is for printing the line(dot and arrow). Another one called “seque” is used to record the assess sequence. I used pre-order traversal to store the data in an array and stored the index of these data in the “seque” array. 

By building these two arrays and the data array, the tree can be printed line by line. When I find the the “lable[i][j]” is bigger than 0, I know that there is a node and that the “seque[i][j]” is the data’s index. As a result, it is convenient to assess the data(using data[seque[i][j]]).

The size of these arrays is decided by the depth and width of the tree. I used some mathematical methods to calculate the width instead of searching.

Besides, I also considered to use the queue. But, the limitation of this program is that it is still needed to use DFS to check the shape of this tree. If I have a enough time, I can find some other ways to do this.
/***************/
2. Hashing

The hash function used comes from Brian Kernighan and Dennis Ritchie's book "The C Programming Language". 

This method really saves a lot of time. The time complexity for searching is only O(1), which is much faster the BST(O(log(n)).

/***************/
3. Threaded tree

I get the idea and knowledge from Internet and I also read Joseph’s paper Traversing binary trees simply and cheaply.








