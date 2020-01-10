package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public abstract class AbstractPrePostVisitor implements PrePostVisitor
{
	// Methods
	
    public void preVisit (Object object)
	{
    }
    
    public void inVisit (Object object)
	{
    }
    
    public void postVisit (Object object)
	{
    }
    
    // Gets
    
    public boolean isDone ()
	{ 
    	return false; 
    }
}
