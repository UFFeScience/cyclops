package datastructure;

/**
 * @author Rodrigo Prado
 *
 */
public class BinarySearchTree extends BinaryTree implements SearchTree
{
	
	// Constructor

    public BinarySearchTree()
    {
    }

    // Methods

    public Comparable find(Comparable object)
    {
        if (isEmpty())
            return null;
        
        int diff = object.compare((Comparable) getKey());
        
        if (diff == 0)
            return (Comparable) getKey();
        else if (diff < 0)
            return getLeftBST().find(object);
        else
            return getRightBST().find(object);
    }
    
    public Comparable findMin()
    {
        if (isEmpty())
            return null;
        else if (getLeftBST().isEmpty())
            return (Comparable) getKey();
        else
            return getLeftBST().findMin();
    }
    
    public void insert(Comparable object)
    {
        if (isEmpty())
            attachKey(object);
        else
        {
            int diff = object.compare((Comparable) getKey());
            
            if (diff == 0)
                throw new IllegalArgumentException("duplicate key");
            
            if (diff < 0)
                getLeftBST ().insert (object);
            else
                getRightBST ().insert (object);
        }
        
        balance();
    }

    public void attachKey(Object object)
    {
        if (!isEmpty())
            throw new InvalidOperationException();
        
        key = object;
        left = new BinarySearchTree();
        right = new BinarySearchTree();
    }

    // AVLTree overwrite this method 
    protected void balance()
    {    	
    }
    
    public void withdraw(Comparable object)
    {
        if (isEmpty())
            throw new IllegalArgumentException("object not found");
        
        int diff = object.compare ((Comparable) getKey ());
        
        if (diff == 0)
        {
            if (!getLeftBST ().isEmpty ())
            {
                Comparable max = getLeftBST ().findMax ();
                key = max;
                getLeftBST ().withdraw (max);
            }
            else if (!getRightBST ().isEmpty ())
            {
                Comparable min = getRightBST ().findMin ();
                key = min;
                getRightBST ().withdraw (min);
            }
            else
                detachKey ();
        }
        else if (diff < 0)
            getLeftBST ().withdraw (object);
        else
            getRightBST ().withdraw (object);
        
        balance();
    } // public void withdraw(Comparable object)
    
    public Object detachKey () throws InvalidOperationException
    {
        if (isEmpty ())
            throw new InvalidOperationException();
        
        Object chave = (Object) key;
        
        key = null;
        left = null;
        right = null;
        
        return chave;
    }

    public Comparable findMax()
    {
        if (isEmpty ())
            return null;
        else if (getRightBST ().isEmpty ())
            return (Comparable) getKey ();
        else
            return getRightBST ().findMax ();
    }

    // Gets
    
    private BinarySearchTree getLeftBST()
	{
    	return (BinarySearchTree) getLeft();
    }
    
    private BinarySearchTree getRightBST()
	{
    	return (BinarySearchTree) getRight();
    }

    public boolean isMember(Comparable comparable)
    {
    	if (isEmpty())
            return false;
    	
//        if (comparable == getKey())
//            return true;
    	if (comparable.equals(getKey()))
            return true;
        
        if (comparable.isLT((Comparable) getKey()))
            return getLeftBST().isMember(comparable);
        else
            return getRightBST().isMember(comparable);
    }
        
}
