package datastructure;

import java.util.NoSuchElementException;


/**
 * @author Rodrigo Prado
 *
 */
public abstract class AbstractTree extends AbstractContainer implements Tree
{
	// Constructors

    public AbstractTree()
    {
    }
	
	// Methods
	
    public void depthFirstTraversal(PrePostVisitor visitor)
    {
        if (visitor.isDone())
            return;
        
        if (!isEmpty())
        {
            visitor.preVisit(getKey());
            
            for (int i = 0; i < getDegree(); ++i)
                getSubtree(i).depthFirstTraversal(visitor);
            
            visitor.postVisit(getKey());
        }
    }
    
    public void breadthFirstTraversal(Visitor visitor)
    {
        Queue queue = new QueueAsLinkedList();
        
        if (!isEmpty())
            queue.enqueue(this);
        
        while (!queue.isEmpty() && !visitor.isDone())
        {
            Tree head = (Tree) queue.dequeue();
            
            visitor.visit(head.getKey());
            
            for (int i = 0; i < head.getDegree(); ++i)
            {
                Tree child = head.getSubtree(i);
                
                if (!child.isEmpty())
                    queue.enqueue(child);
            }
        }
    }
    
    public void accept(Visitor visitor)
	{
    	depthFirstTraversal(new PreOrder(visitor));
    }
    
    // Get

    public int getHeight()
    {
        if (isEmpty())
            return -1;
        
        int i = -1;
        
        for (int j = 0; j < getDegree(); j++)
            i = Math.max(i, getSubtree(j).getHeight());

        return i + 1;
    }

    public int getCount()
    {
        if(isEmpty())
            return 0;
        
        int i = 1;
        
        for(int j = 0; j < getDegree(); j++)
            i += getSubtree(j).getCount();

        return i;
    }

    public Enumeration getEnumeration()
	{
    	return new TreeEnumeration();
    }

    // Inner classes
    
    protected class TreeEnumeration implements Enumeration
    {
        protected Stack stack;

        public TreeEnumeration()
    	{
    	    stack = new StackAsLinkedList();
    	    
    	    if (!isEmpty())
    	        stack.push(AbstractTree.this);
    	}
        
        public boolean hasMoreElements()
	    { 
        	return !stack.isEmpty();
        }

        public Object nextElement()
        {
            if (stack.isEmpty())
                throw new NoSuchElementException();
            
            Tree top = (Tree) stack.pop();
            
            for (int i = top.getDegree() - 1; i >= 0; --i)
            {
                Tree subtree = (Tree) top.getSubtree(i);
                
                if (!subtree.isEmpty())
                    stack.push(subtree);
            }
            
            return top.getKey();
        }
    } // protected class TreeEnumeration	implements Enumeration
} // public abstract class AbstractTree extends AbstractContainer implements Tree
