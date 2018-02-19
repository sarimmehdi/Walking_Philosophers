# Walking_Philosophers

My solution for the class project. Project description, as given by the teacher, is as under:

Implement a solution of a modified version of the dining philosophers, as follows:

* there are 10 philosophers in a room, engaged in the usual think/eat cycle;
* philosophers think while walking in the room, and can only eat while sitting at the table;
* however, at any given time only 5 philosophers at most can sit at a table, and since every two adjacent philosophers are sharing a     chopsticks, only two non-adjacent philosophers can eat at the same time (as in the classic version of the dining philosophers problem);
* therefore, in order to eat, a philosopher should first find a seat at the table, then wait for his/her turn to grab the chopsticks;
* once seated, a philosopher cannot change seat;
* after eating, philosophers get up to make room for other philosophers to eat.

Analyze the problem, design a monitor, implement the monitor and test it. Clearly, deadlock must be avoided. Moreover, the following design criteria are given:

* concurrency should be preserved: two philosophers must be able to eat at the same time;
* starvation should be avoided: in case of resource contention, you should favor the philosophers who have been eating the least.
* Notice that the second bullet is intentionally ambiguous. You are encouraged to provide your own interpretation, devise a method for keeping track of how much each philosopher has eaten, etc.. You are responsible for your design choices, as long as they are clearly explained.
