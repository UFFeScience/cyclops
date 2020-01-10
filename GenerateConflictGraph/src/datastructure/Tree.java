package datastructure;


/**
 * @author Rodrigo Prado
 *
 */
public interface Tree extends Container
{
	// Methods

    void depthFirstTraversal(PrePostVisitor visitor);
    
    void breadthFirstTraversal(Visitor visitor);
    
    // Gets
    
    Object getKey();
    
    Tree getSubtree(int i);
    
    boolean isEmpty();
    
    boolean isLeaf();
    
    int getDegree();
    
    int getHeight();
    
}
