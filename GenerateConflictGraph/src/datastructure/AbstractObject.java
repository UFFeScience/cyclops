package datastructure;

public abstract class AbstractObject implements Comparable
{
	// Methods

    public final boolean equals(Object object)
    {
    	if (object instanceof Comparable)
    	    return isEQ((Comparable) object);
    	else
    	    return false;
    }
    
    public final int compare(Comparable arg)
    {
    	if (getClass() == arg.getClass())
    	    return compareTo(arg);
    	else
    	    return getClass().getName().compareTo(arg.getClass().getName());
    }
	
    protected abstract int compareTo(Comparable arg);
    
	// Gets
	
    public final boolean isLT(Comparable object)
	{
    	return compare(object) <  0;
    }

    public final boolean isLE(Comparable object)
	{
    	return compare(object) <= 0;
    }

    public final boolean isGT(Comparable object)
	{
    	return compare(object) >  0;
    }

    public final boolean isGE(Comparable object)
	{
    	return compare(object) >= 0;
    }

    public final boolean isEQ(Comparable object)
	{
    	return compare(object) == 0;
    }

    public final boolean isNE(Comparable object)
	{
    	return compare(object) != 0;
    }
   
}
