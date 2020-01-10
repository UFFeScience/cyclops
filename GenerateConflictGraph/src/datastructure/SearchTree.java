package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public interface SearchTree	extends Tree, SearchableContainer
{
	// Methods
	
    Comparable findMin();
    
    Comparable findMax();
}
