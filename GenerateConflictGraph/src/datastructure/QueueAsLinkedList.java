package datastructure;

import java.util.NoSuchElementException;


/**
 * @author Rodrigo Prado
 *
 */
public class QueueAsLinkedList extends AbstractContainer implements Queue
{
	// Properties
	
    protected LinkedList list;
    
    // Constructors
    
    public QueueAsLinkedList()
    {
    	list = new LinkedList();
    }

    // Methods

    public void enqueue(Object object)
    {
        list.append (object);
        ++count;
    }

    public Object dequeue()
    {
        if (count == 0)
            throw new ContainerEmptyException();
        
        Object result = list.getFirst ();
        
        list.extract (result);
        --count;
        
        return result;
    }

    public void accept(Visitor visitor)
    {
        for(LinkedList.Element element = list.getHead(); element != null; element = element.getNext())
        {
            visitor.visit(element.getDatum());
            if(visitor.isDone())
                return;
        }

    }

    public void purge()
    {
        list.purge();
        count = 0;
    }

    protected int compareTo(Comparable comparable)
    {
        throw new MethodNotImplemented();
    }

    // Gets

    public Object getHead()
    {
        if (count == 0)
            throw new ContainerEmptyException();
        
        return list.getFirst ();        	
    }

    public Enumeration getEnumeration()
    {
        return new Enumeration() 
                {
                    LinkedList.Element position;
                    
                    {
                        position = list.getHead();
                    }
                    
                    public boolean hasMoreElements()
                    {
                        return position != null;
                    }
        
                    public Object nextElement()
                    {
                        if (position == null)
                            throw new NoSuchElementException();
                        
                        Object obj = position.getDatum();
                        
                        position = position.getNext();
                        
                        return obj;
                    }
                };
    }

    // toString
    
    public String toString()
    {
        String queueAsLinkedList = "\n\n\t\tFila Final\n\n" 
            + list;
        
        return queueAsLinkedList;
    }
}