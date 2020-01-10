package datastructure;

public interface SearchableContainer extends Container
{
	// Methods

    void insert(Comparable object);
    
    void withdraw(Comparable obj);
    
    Comparable find(Comparable object);
    
    // Gets
    
    boolean isMember(Comparable object);
    
}

