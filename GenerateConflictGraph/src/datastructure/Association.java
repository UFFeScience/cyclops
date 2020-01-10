package datastructure;

public class Association extends AbstractObject
{
	// Properties
	
    protected Comparable key;
    
    protected Object value;
    
    // Constructors
    
    public Association(Comparable key, Object value)
    {
    	this.key = key;
    	this.value = value;
    }

    public Association(Comparable key)
	{
    	this(key, null);
    }
    
    // Methods

    public int hashCode()
    {
    	return key.hashCode();
    }

    protected int compareTo(Comparable object)
    {
    	Association association = (Association) object;
    	
    	return key.compare(association.getKey());
    }

    // Gets
    
    public Comparable getKey()
	{
    	return key;
    }

    public Object getValue()
	{
    	return value;
    }

    // toString
    
    public String toString()
    { // TODO verify toString Method. Did it needs a StringBuffer?
    	String result = "Association {" + key;
    	
    	if (value != null)
    		result += ", " + value;
    	
    	return result + "}";
    }
}
