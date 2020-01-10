package datastructure;


public class LinkedList
{
	// Properties
	
    protected Element head;
    
    protected Element tail;

    // Constructors
    
	public LinkedList() 
	{	
	}
	
	// Methods

    public void prepend(Object item)
    {
        Element tmp = new Element(item, head);
        
        if (head == null)
            tail = tmp;
        
		head = tmp;
    }

    public void append(Object item)
    {
        Element tmp = new Element(item, null);
        
        if (head == null)
            head = tmp;
        else
            tail.next = tmp;
        
        tail = tmp;
    }
    
    public void assign(LinkedList list)
    {
        if (list != this)
        {
            purge ();
            
            for (Element ptr = list.head; ptr != null; ptr = ptr.next)
            {
                append (ptr.datum);
            }
        }
    }

    public void extract(Object item)
    {
        Element ptr = head;
        
        Element prevPtr = null;
        
        /*
         * Não utiliza o método compareTo() do objeto.
         * Por isso não compara adequadamente o array[i] com o object.
         */
        //while (ptr != null && ptr.datum != item)
        
        while (ptr != null && !ptr.datum.equals(item))
        {
            prevPtr = ptr;
            ptr = ptr.next;
        }
        
        if (ptr == null)
            throw new IllegalArgumentException("item not found");
                    
        if (ptr == head)
            head = ptr.next;
        else
            prevPtr.next = ptr.next;
        
        if (ptr == tail)
            tail = prevPtr;
    }
  
	public void purge()
    {
	    head = null;
	    tail = null;
    }
	
	// Gets
	
    public Element getHead()
	{
    	return head;
    }
    
    public Element getTail()
	{
    	return tail;
    }
    
    public boolean isEmpty()
	{
    	return head == null;
    }
    
	public Object getFirst()
    {
		if (head == null)
		    throw new ContainerEmptyException();
	    return head.datum;
    }
    
    public Object getLast() 
    {
        if (tail == null)
            throw new ContainerEmptyException();
        
        return tail.datum;
    }
    
    // toString
    
    public String toString()
    {
        Element i = head;
        
        String linkedList = "";
        
        if(i == null)
            linkedList = "Lista vazia.";
        else
        {   
            while(i != null)
            {
                linkedList += "" + i;
                i = i.getNext();
            }
        }
        
        return linkedList;
    }
    
    // Inner class

    public final class Element
    {
        Object datum;
        
        Element next;

        Element(Object datum, Element next)
        {
            this.datum = datum;
            this.next = next;
        }

        public Object getDatum()
	    {
        	return datum;
        }

        public Element getNext()
	    {
        	return next;
        }
	
        public void insertAfter(Object item)
        {
            next = new Element (item, next);
            
            if (tail == this)
                tail = next;
        }

        public void insertBefore(Object item)
        {
            Element tmp = new Element (item, this);
            
            if (this == head)
                head = tmp;
            else
            {
                Element prevPtr = head;
                
                while (prevPtr != null && prevPtr.next != this)
                    prevPtr = prevPtr.next;
                
                prevPtr.next = tmp;
            }
        }
        
        // toString
        
        public String toString()
        {
            String element = "" + datum;
            
            return element;
        }
    }
}
