package datastructure;

public class Str extends AbstractObject
{
	// Constants
    private static final int shift = 6;
    private static final int mask = ~0 << (32 - shift);

    // Properties
    
    protected String value;

    public Str(String value)
	{
    	this.value = value;
    }

    public String stringValue()
	{
    	return value;
    }

    protected int compareTo(Comparable object)
    {
    	Str arg = (Str) object;
    	return value.compareTo(arg.value);
    }
 
    public int hashCode ()
    {
    	int result = 0;
	
    	for (int i = 0; i < value.length(); ++i)
    		result = (result & mask) ^ (result << shift) ^ value.charAt(i);
	
    	return result;
    }
    
    // toString

    public String toString()
    {
        return value;
    }
    
}
