package datastructure;

public class Chr extends AbstractObject
{
	// Properties
	
    protected char value;

    // Constructors
    
    public Chr(char value)
	{
    	this.value = value;
    }

    // Methods
    
    public char charValue()
	{
    	return value;
    }

    protected int compareTo(Comparable object)
    {
    	Chr arg = (Chr) object;
    	
    	return (int) value - (int) arg.value;
    }

    // toString

    public String toString()
    {
        return String.valueOf(value);
    }

}
