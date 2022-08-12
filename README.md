# DSA2-Project-Implementing-PieceTable

Om Khare <br/>
112003066 <br/>
DSA2 - Project <br/>
Division - 1 <br/>

## Includes : <br/>
1. Fully functioning text editor implementation of Piece Table using Linked List. <br/>
2. Splay tree implementation of Piece Table supporting Insertion in a text editor as of now.

## Test Results <br/>
1. Using Linked List: <br/>
a. Insertion of 10000 characters in a 1,50,000 line text took 7.31 seconds:<br/>
2. Using Splay tree:<br/>
a. Insertion of 10000 characters in a 1,50,000 line text took < 1 second.<br/>

## Graph of time required per insertion using Splay Tree
![Graph](https://github.com/OmKhare/DSA2-Project-Implementing-PieceTable/blob/master/Screenshot%202022-06-12%20225915.png)

## Analysis from graph
* Spikes are observed in the graph
* These sipkes are due to the splaying function of the splay tree.
* Once we edit something, the edited piece of text is splayed to the root of the tree and hence the sudden drop in time required.
* Although time required in some cases is more, the average time required is very less as compared to linked list implementation.

## Conclusion
* Splay tree method is efficient for normal file editing limiting the size of file to 1000 lines
* Beyond that, the text editor should switch back to its linked list implemetation of piece table and auto save feature should be turned off.
* A combination of splay tree and linked list will be the optimal solution.

## References: <br/>
[Build Your Own Text Editor](https://viewsourcecode.org/snaptoken/kilo/index.html) <br/>
https://darrenburns.net/posts/piece-table/ <br/>
https://www.cs.unm.edu/~crowley/papers/sds/node15.html <br/>
https://www.javatpoint.com/splay-tree
