package datastructure;

public interface Comparable
{
	// Methods
	
	int compare(Comparable object);
	
	// Gets
	
    boolean isLT(Comparable object);
    
    boolean isLE(Comparable object);
    
    boolean isGT(Comparable object);
    
    boolean isGE(Comparable object);
    
    boolean isEQ(Comparable object);
    
    boolean isNE(Comparable object);
}
