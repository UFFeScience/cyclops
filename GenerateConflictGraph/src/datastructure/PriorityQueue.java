package datastructure;


public interface PriorityQueue extends Container
{
    void enqueue(Comparable object);
    
    Comparable findMin();
    
    Comparable dequeueMin();
}
