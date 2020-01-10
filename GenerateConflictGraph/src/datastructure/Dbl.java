package datastructure;

public class Dbl extends AbstractObject
{
	// Properties
	
    protected double value;

    // Constructors
    
    public Dbl(double value)
	{
    	this.value = value;
    }

    public double doubleValue()
	{
    	return value;
    }

    protected int compareTo(Comparable object)
    {
    	Dbl arg = (Dbl) object;
    	
    	if (value < arg.value)
    		return -1;
    	else if (value > arg.value)
    		return +1;
    	else
    		return 0;
    }
    
    public int hashCode ()
    {
        long bits = Double.doubleToLongBits(value);
        
        return (int) (bits >>> 20);
    }

    // toString
    
    public String toString()
    {
        return Double.toString(value);
    }

}
