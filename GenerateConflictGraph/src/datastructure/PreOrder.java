package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public class PreOrder extends AbstractPrePostVisitor
{
	// Properties
	
    protected Visitor visitor;

    // Constructors
    
    public PreOrder(Visitor visitor)
	{
    	this.visitor = visitor;
    }

    // Methods
    
    public void preVisit(Object object)
	{
    	visitor.visit(object);
    }
    
    // Get
    
    public boolean isDone()
	{
    	return visitor.isDone();
    }
}
