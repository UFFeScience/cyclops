package datastructure;

public interface Container extends Comparable
{
	// Methods

    void purge();
    
    void accept(Visitor visitor);
    
    // Gets
    
    int getCount();
    
    boolean isEmpty();
    
    boolean isFull();
    
    Enumeration getEnumeration();
}
