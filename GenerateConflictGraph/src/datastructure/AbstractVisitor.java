package datastructure;

public abstract class AbstractVisitor implements Visitor
{
	// Methods
	
    public void visit(Object object)
	{	
	}
    
    // Gets
    
    public boolean isDone()
	{
    	return false;
    }
}
