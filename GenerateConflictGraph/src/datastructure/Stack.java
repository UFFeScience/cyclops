package datastructure;


public interface Stack extends Container
{
    // Methods
	
    void push(Object object);
    
    Object pop();
    
    // Gets
    
    Object getTop();
    
}
