package datastructure;

import java.util.NoSuchElementException;


/**
 * @author Rodrigo Prado
 *
 */
public class StackAsLinkedList extends AbstractContainer implements Stack
{
	// Properties
	
    protected LinkedList list;

    // Constructors
    
    public StackAsLinkedList()
    {
    	list = new LinkedList ();
    }
    
    // Methods

    public void push(Object object)
    {
        list.prepend(object);
        ++count;
    }
    
    public Object pop()
    {
        if (count == 0)
            throw new ContainerEmptyException();
        
        Object result = list.getFirst();
        
        list.extract (result);
        --count;
        
        return result;
    }

    public void accept(Visitor visitor)
    {
        for (LinkedList.Element ptr = list.getHead();
        		ptr != null; ptr = ptr.getNext())
        {
            visitor.visit (ptr.getDatum());
            
            if (visitor.isDone())
                return;
        }
    }
    
    public void purge ()
    {
        list.purge ();
        count = 0;
    }

    protected int compareTo(Comparable comparable)
    {// TODO compareTo StackAsLinkedList
        throw new MethodNotImplemented();
    }

    // Gets
    
    public Object getTop()
    {
        if (count == 0)
            throw new ContainerEmptyException();
        
        return list.getFirst();
    }
    
    public Enumeration getEnumeration()
    {
        return new Enumeration()
        {
            protected LinkedList.Element position = list.getHead();
            
            public boolean hasMoreElements()
            {
            	return position != null;
            }
            
            public Object nextElement()
            {
                if (position == null)
                    throw new NoSuchElementException();
                
                Object result = position.getDatum();
                position = position.getNext();
                
                return result;
            }
        };
    }

    // toString
    
    public String toString()
    {
        String stackAsLinkedList = "\n\n\t\tPilha Final\n\n" 
            + list;
        
        return stackAsLinkedList;
    }
}
