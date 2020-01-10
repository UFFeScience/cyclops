package datastructure;

import java.util.NoSuchElementException;

/**
 * @author Rodrigo Prado
 *
 */
public class GraphAsLists extends AbstractGraph
{
	// Properties
	
    protected LinkedList[] adjacencyList;

    // Constructors
    
    public GraphAsLists(int size)
    {
        super(size);
        
        adjacencyList = new LinkedList[size];
        
        for (int i = 0; i < size; ++i)
            adjacencyList[i] = new LinkedList();
    }

    // Methods

    public void purge()
    {
        for(int i = 0; i < super.numberOfVertices; i++)
            adjacencyList[i].purge();

        super.purge();
    }

    protected int compareTo(Comparable comparable)
    {// TODO compareTo GraphAsLists
        throw new MethodNotImplemented();
    }
    
    // Sets

    protected void addEdge(Edge edge)
    {
        int i = edge.getV0().getNumber();
        adjacencyList[i].append(edge);
        super.numberOfEdges++;
    }
    
    // Gets

    protected Enumeration getIncidentEdges(int i)
    {
    	final Vertex arg = getVertex(i);
    	
        return new Enumeration()
                {
                    protected int v = 0;
                    
                    protected LinkedList.Element ptr;
                    
                    { // Replace constructor with an instance initializer
label0:
                    	for (; v < numberOfVertices; v++)
                    	{
                    		ptr = adjacencyList[v].getHead();
                    		
                    		while (ptr != null)
                    		{
                    			if (!((Edge) ptr.getDatum()).getV1().equals(arg))
                    				ptr = ptr.getNext();
                    			else
                    				break label0;
                    		}
                    	}
                    } // Replace constructor with an instance initializer
        
                    public boolean hasMoreElements()
                    {
                    	return ptr != null;
                    }
        
                    public Object nextElement()
                    {
                        if (ptr == null)
                            throw new NoSuchElementException();
                        
                        Object obj = ptr.getDatum();
                        
                        ptr = ptr.getNext();
                        
                        while (ptr != null)
                		{
                        	if (!((Edge) ptr.getDatum()).getV1().equals(arg))
                				ptr = ptr.getNext();
                		}
                        
                        v++;
                        
                        for (; v < numberOfVertices; v++)
                    	{
                    		ptr = adjacencyList[v].getHead();
                    		
                    		while (ptr != null)
                    		{
                    			if (!((Edge) ptr.getDatum()).getV1().equals(arg))
                    				ptr = ptr.getNext();
                    			else
                    				return obj;
                    		}
                    	}
                        
                        return obj;
                    }
                };
    }
    
    protected Enumeration getEmanatingEdges(int i)
    {
        final Vertex arg = getVertex(i);
        
        return new Enumeration() 
                {
                    protected int v;
                    
                    protected LinkedList.Element ptr;
                    
                    {
                        v = arg.getNumber();
                        ptr = adjacencyList[v].getHead();
                    }
                    
                    public boolean hasMoreElements()
                    {
                        return ptr != null;
                    }
        
                    public Object nextElement()
                    {
                        if (ptr == null)
                            throw new NoSuchElementException();
                        
                        Object obj = ptr.getDatum();
                        
                        ptr = ptr.getNext();
                        
                        return obj;
                    }
                };
    }

    public boolean isEdge(int i, int j)
    {
        Bounds.check(i, 0, super.numberOfVertices);
        
        Bounds.check(j, 0, super.numberOfVertices);
        
        for (LinkedList.Element element = adjacencyList[i].getHead(); 
        		element != null; element = element.getNext())
        {
            Edge edge = (Edge)element.getDatum();
            
            if (edge.getV1().getNumber() == j)
                return true;
        }

        return false;
    }

    public Edge getEdge(int i, int j)
    {
        Bounds.check(i, 0, super.numberOfVertices);
        
        Bounds.check(j, 0, super.numberOfVertices);
        
        for (LinkedList.Element element = adjacencyList[i].getHead(); 
        		element != null; element = element.getNext())
        {
            Edge edge = (Edge)element.getDatum();
            if(edge.getV1().getNumber() == j)
                return edge;
        }

        throw new IllegalArgumentException("edge not found");
    }

    public Enumeration getEdges()
    {
        return new Enumeration() 
                {
                    protected int v;
                    
                    LinkedList.Element ptr;
                    
                    {
                        ptr = null;
                        ptr = null;
        
                        for (v = 0; v < numberOfVertices; v++)
                        {
                            ptr = adjacencyList[v].getHead();
                            if (ptr != null)
                                break;
                        }
                    }
                    
                    public boolean hasMoreElements()
                    {
                        return ptr != null;
                    }
        
                    public Object nextElement()
                    {
                        if (ptr == null)
                            throw new NoSuchElementException();
                        
                        Object obj = ptr.getDatum();
                        
                        ptr = ptr.getNext();
                        
                        if (ptr == null)
                            for (v++; v < numberOfVertices; v++)
                            {
                                ptr = adjacencyList[v].getHead();
                                
                                if (ptr != null)
                                    break;
                            }
        
                        return obj;
                    }
                };
    }
}
