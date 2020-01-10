package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public class BinaryTree extends AbstractTree
{
	// Properties
	
    protected Object key;
    
    protected BinaryTree left;
    
    protected BinaryTree right;
    
    // Constructors
    
    public BinaryTree(Object key, 
    		          BinaryTree left, 
    		          BinaryTree right)
    {
        this.key = key;
        this.left = left;
        this.right = right;
    }

    public BinaryTree()
    {
    	this(null, null, null);
    }

    public BinaryTree(Object key)
    {
    	this(key, new BinaryTree(), new BinaryTree());
    }
    
    // Methods

    public void depthFirstTraversal(PrePostVisitor visitor)
    {
        if (!isEmpty())
        {
            visitor.preVisit(key);
            left.depthFirstTraversal(visitor);
            visitor.inVisit(key);
            right.depthFirstTraversal(visitor);
            visitor.postVisit(key);
        }
    }

    public void purge()
    {
        key = null;
        left = null;
        right = null;
    }

    public Object detachKey()
    {
        if (!isLeaf())
            throw new InvalidOperationException();
        
        Object obj = key;
        
        key = null;
        left = null;
        right = null;
        
        return obj;
    }

    public BinaryTree detachLeft()
    {
        if (isEmpty())
            throw new InvalidOperationException();
        
        BinaryTree binarytree = left;
            
        left = new BinaryTree();
            
        return binarytree;
    }

    public BinaryTree detachRight()
    {
        if (isEmpty())
            throw new InvalidOperationException();
        
        BinaryTree binarytree = right;
            
        right = new BinaryTree();
            
        return binarytree;
    }

    public void attachKey(Object obj)
    {
        if (!isEmpty())
            throw new InvalidOperationException();
        
        key = obj;
        left = new BinaryTree();
        right = new BinaryTree();
    }

    public void attachLeft(BinaryTree binarytree)
    {
        if (isEmpty() || !left.isEmpty())
            throw new InvalidOperationException();
        
        left = binarytree;
    }

    public void attachRight(BinaryTree binarytree)
    {
        if (isEmpty() || !right.isEmpty())
            throw new InvalidOperationException();
        
        right = binarytree;
    }

    protected int compareTo(Comparable object)
    {
        BinaryTree arg = (BinaryTree) object;
        
        if (isEmpty())
            return arg.isEmpty() ? 0 : -1;
        else if (arg.isEmpty())
            return 1;
        else
        {
            int result = ((Comparable) getKey()).compare(
                    (Comparable) arg.getKey());
            
            if (result == 0)
                result = ((BinaryTree) getLeft()).compareTo(arg.getLeft());
            
            if (result == 0)
                result = ((BinaryTree) getRight()).compareTo(arg.getRight());
            
            return result;
        }
    }
    
    public void printingOnlyLeaves(PrePostVisitor visitor)
    {
        if (!isEmpty())
        {
            if (left.isEmpty() && right.isEmpty())
                visitor.preVisit(key);
            
            left.printingOnlyLeaves(visitor);
            right.printingOnlyLeaves(visitor);
        }
    }

    /*
     * Esse metodo eh para ser chamado usando zero como argumento,
     * para que o no pai fique mais a esquerda e para que se a arvore
     * estiver vazia, seja tratada. Pois a excecao de arvore vazia so eh lancada se
     * n for igual a zero.
     */
    public void printingTree(int n) {
        // TODO printingTree. Review this method
        int i;
        
        if (!isEmpty())
        {
            right.printingTree(n + 3);
            
            for(i = 1; i <= n; i++)            
                System.out.print(" ");
            
            System.out.print(this.key);
            
            left.printingTree(n + 3);
        } // End if(isEmpty())
        else
            if (n == 0) throw new ContainerEmptyException();
    } // printingTree (int n)
     
    // Gets

    public Tree getSubtree(int i)
    {
        Bounds.check(i, 0, 2);
        
        if (i == 0)
            return getLeft();
        else
            return getRight();
    }

    public Object getKey()
    {
        if (isEmpty())
            throw new InvalidOperationException();
    
        return key;
    }

    public boolean isEmpty()
	{
    	return key == null;
    }

    public int getDegree()
    {
        return isEmpty() ? 0 : 2;
    }

    public boolean isLeaf()
    {
        return !isEmpty() && left.isEmpty() && right.isEmpty();
    }
    
    public BinaryTree getLeft()
    {
        if (isEmpty())
            throw new InvalidOperationException();
        
        return left;
    }
    
    public BinaryTree getRight()
    {
    	if (isEmpty())
            throw new InvalidOperationException();
    
        return right;
    }

    // toString
    
    
} // public class BinaryTree extends AbstractTree
