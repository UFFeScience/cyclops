package datastructure;

public class Array
{
	// Properties
	
	protected Object[] data;
	
    protected int base;
    
    // Constructors
    
    public Array(int n, int m)
    {
    	data = new Object[n];
    	base = m;
    }

    public Array()
	{
    	this(0, 0);
    }

    public Array(int n)
	{
    	this(n, 0);
    }
    
    // Methods
    
    public void assign(Array array)
    {
    	if (array != this)
    	{
    	    if (data.length != array.data.length)
    	    	data = new Object [array.data.length];
    	    
    	    for (int i = 0; i < data.length; ++i)
    	    	data [i] = array.data [i];
    	    
    	    base = array.base;
    	}
    }
    
    // Set
    
    public void put(int position, Object object)
	{
    	data[position - base] = object;
    }
    
    public void setBase(int base)
   	{
    	this.base = base;
    }

    public void setLength(int newLength)
    {
    	if (data.length != newLength)
	   	{
    		Object[] newData = new Object[newLength];
    		
    		int min = data.length < newLength ? data.length : newLength;
   	    
    		for (int i = 0; i < min; ++i)
    			newData [i] = data [i];
    		
    		data = newData;
   		}
	}
    
    // Gets

    public Object get(int position)
	{
    	return data[position - base];
    }

    public Object[] getData()
	{
    	return data;
    }

    public int getBase()
	{
    	return base;
    }

    public int getLength()
	{
    	return data.length;
    }
    
    // toString
    

    public String toString()
    {
        StringBuffer stringbuffer = new StringBuffer();
        
        stringbuffer.append("Array {\nbase = " + base + "\n");
        
        for(int i = 0; i < data.length; i++)
            stringbuffer.append(i + ": " + data[i] + "\n");

        stringbuffer.append("}\n");
        
        return stringbuffer.toString();
    }

}
