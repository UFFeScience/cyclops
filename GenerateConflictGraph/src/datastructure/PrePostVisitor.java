package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public interface PrePostVisitor
{
	// Methods
    void preVisit (Object object);
    
    void inVisit (Object object);
    
    void postVisit (Object object);
 
    // Get
    
    boolean isDone ();
}
