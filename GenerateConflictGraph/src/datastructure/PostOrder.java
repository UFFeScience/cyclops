package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public class PostOrder extends AbstractPrePostVisitor
{
	// Properties
	
    protected Visitor visitor;

    // Constructors
    
    public PostOrder(Visitor visitor)
	{
    	this.visitor = visitor;
    }

    // Methods
    
    public void postVisit(Object object)
	{
    	visitor.visit(object);
    }
    
    // Get
    
    public boolean isDone()
	{
    	return visitor.isDone();
    }
}
