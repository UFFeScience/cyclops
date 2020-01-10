package datastructure;

public abstract class AbstractContainer extends AbstractObject implements Container
{
	// Properties
	
    protected int count;

    // Constructors
    
    public AbstractContainer()
    {
    }
    
    // Methods

    public void accept(Visitor visitor)
    {
        for (Enumeration enumeration = getEnumeration(); 
        		enumeration.hasMoreElements() && !visitor.isDone(); 
        		visitor.visit(enumeration.nextElement()));
    }
    
    public int hashCode()
    {
        Visitor visitor = new AbstractVisitor()
                {
                    private int value;
                    
                    public void visit(Object object)
                    {
                    	value += object.hashCode();
                    }
                    
                    public int hashCode()
                    {
                    	return value;
                    }
                };
        
        accept(visitor);
        
        return getClass().hashCode() + visitor.hashCode();
    }
    
    // Gets
    
    public int getCount()
	{
    	return count;
    }

    public boolean isEmpty()
	{
    	return getCount () == 0;
    }

    public boolean isFull()
	{
    	return false;
    }
    
    // toString

    public String toString()
    {
        final StringBuffer buffer = new StringBuffer();	
        
        Visitor visitor = new AbstractVisitor()
        		{
        			private boolean comma;

            	    public void visit (Object object)
            	    {
            	        if (comma)
            	            buffer.append (", ");
            	        
            	        buffer.append (object);
            	        comma = true;
            	    }
    			};
    	
		accept(visitor);
		
		return getClass().getName() + " {" + buffer + "}";
    }

}
