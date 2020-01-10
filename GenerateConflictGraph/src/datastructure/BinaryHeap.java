package datastructure;


public class BinaryHeap extends AbstractContainer implements PriorityQueue
{
	// Properties
	
    protected Comparable[] array;
    
    // Constructors
    
    public BinaryHeap(int length)
	{ 
        array = new Comparable[length + 1]; 
    }

    // Methods
    
    public void purge()
    {
        while (count > 0)
            array [count--] = null;
    }
    
    public void enqueue(Comparable object)
    {
        if (count == array.length - 1)
            throw new ContainerFullException();
        
        ++count;
        
        int i = count;
        
        while (i > 1 && array[i/2].isGT(object))
        {
            array [i] = array[i / 2];
            i /= 2;
        }
        
        array [i] = object;
    }
    
    public Comparable findMin()
    {
        if (count == 0)
            throw new ContainerEmptyException();
        
        return array[1];
    }
    
    public Comparable dequeueMin()
    {
        if (count == 0)
            throw new ContainerEmptyException ();
        
        Comparable result = array[1];
        Comparable last = array[count];
        --count;
        int i = 1;
        while (2 * i < count + 1)
        {
            int child = 2 * i;
            if (child + 1 < count + 1
                    && array [child + 1].isLT (array [child]))
                child += 1;
            if (last.isLE (array [child]))
                break;
            array [i] = array [child];
            i = child;
        }
        array [i] = last;
        return result;
    }

    /* (non-Javadoc)
     * @see fundamentos.AbstractObject#compareTo(fundamentos.Comparable)
     */
    protected int compareTo (Comparable arg)
    {
        // TODO Auto-generated method stub
        return 0;
    }

    /* (non-Javadoc)
     * @see fundamentos.Container#accept(fundamentos.Visitor)
     */
    public void accept (Visitor visitor)
    {
        // TODO Auto-generated method stub
        
    }

    /* (non-Javadoc)
     * @see fundamentos.Container#getEnumeration()
     */
    public Enumeration getEnumeration ()
    {
        // TODO Auto-generated method stub
        return null;
    }
    
    
}
