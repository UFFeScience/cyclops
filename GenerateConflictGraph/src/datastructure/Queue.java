package datastructure;


/**
 * @author Rodrigo Prado
 */
public interface Queue extends Container
{
    // Methods
	
    void enqueue(Object object);
    
    Object dequeue();
    
    // Gets
    
    Object getHead();
    
}
